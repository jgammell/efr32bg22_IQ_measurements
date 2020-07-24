/***************************************************************************//**
 * @file
 * @brief Simple RAIL application which includes hal
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// Standard C libraries
#include <stdio.h>

// SI Labs libraries
#include "em_chip.h"
#include "hal-config.h"
#include "bsp.h"
#include "retargetserial.h"
#include "gpiointerrupt.h"
#include "rail.h"
#include "hal_common.h"
#include "rail_config.h"

#include "em_prs.h"

// Constants
#define TX_BUFFER_LENGTH 256
#define RX_BUFFER_LENGTH 4096
#define PAYLOAD_LENGTH   16
#define CHANNEL          0

// Event handlers
void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);
void gpioCallback(uint8_t);

// Buffers
static uint8_t rx_buffer[RX_BUFFER_LENGTH];
static uint8_t tx_buffer[TX_BUFFER_LENGTH];

void assert(bool condition)
{
	if(condition == false)
	{
		BSP_LedSet(1);
		while(1);
	}
}

#define ONESHOT_TIMER TIMER0
#define ONESHOT_TIMER_IRQn TIMER0_IRQn
#define ONESHOT_TIMER_IRQHandler TIMER0_IRQHandler



void init_timer_oneshot(void)
{
	ONESHOT_TIMER->CMD = TIMER_CMD_STOP;
	ONESHOT_TIMER->CFG = 0;
	ONESHOT_TIMER->CFG |= TIMER_CFG_MODE_DOWN;
	ONESHOT_TIMER->CFG |= TIMER_CFG_OSMEN;
	ONESHOT_TIMER->CTRL = 0;
	ONESHOT_TIMER->IEN = 0;
	ONESHOT_TIMER->IEN |= TIMER_IEN_UF;
	ONESHOT_TIMER->CC[0].CFG = 0;
	ONESHOT_TIMER->CC[1].CFG = 0;
	ONESHOT_TIMER->CC[2].CFG = 0;
	NVIC_EnableIRQ(ONESHOT_TIMER_IRQn);
}

void peripheralInit(void)
{
	// Misc. initializations
	halInit();

	// Initialize buttons
	typedef struct ButtonArray
	{
		GPIO_Port_TypeDef port;
		unsigned int pin;
	} ButtonArray_t;
	const ButtonArray_t buttonArray[BSP_BUTTON_COUNT] = BSP_BUTTON_INIT;
	RETARGET_SerialCrLf(1);
	GPIOINT_Init();
	for(int i=0; i<BSP_BUTTON_COUNT; ++i)
	{
		GPIO_PinModeSet(buttonArray[i].port, buttonArray[i].pin, gpioModeInputPull, 1);
		GPIOINT_CallbackRegister(buttonArray[i].pin, gpioCallback);
		GPIO_IntConfig(buttonArray[i].port, buttonArray[i].pin, false, true, true);
	}
}

RAIL_Handle_t railHandle;
static RAIL_Config_t railCfg = {
  .eventsCallback = &RAILCb_Generic,
};
void radioInit(void)
{
  railHandle = RAIL_Init(&railCfg, NULL);
  assert(railHandle != NULL);
  RAIL_ConfigCal(railHandle, RAIL_CAL_ALL);

  // Set us to a valid channel for this config and force an update in the main
  // loop to restart whatever action was going on
  RAIL_ConfigChannels(railHandle, channelConfigs[0], NULL);

  // Initialize the PA now that the HFXO is up and the timing is correct
  RAIL_TxPowerConfig_t txPowerConfig = \
		  {.mode = RAIL_TX_POWER_MODE_2P4_HP,
           .voltage = BSP_PA_VOLTAGE,
           .rampTime = HAL_PA_RAMP};
  RAIL_Status_t status = RAIL_ConfigTxPower(railHandle, &txPowerConfig);
  assert(status == RAIL_STATUS_NO_ERROR);

  // Power settings modified - see what this is doing
  //RAIL_SetTxPower(railHandle, HAL_PA_POWER);
  RAIL_EnablePaAutoMode(railHandle, true);
  RAIL_SetTxPowerDbm(railHandle, 30);

  RAIL_StateTransitions_t transitions = {RAIL_RF_STATE_IDLE, RAIL_RF_STATE_IDLE};
  RAIL_SetRxTransitions(railHandle, &transitions);
  RAIL_SetTxTransitions(railHandle, &transitions);
  RAIL_ConfigEvents(railHandle, RAIL_EVENTS_ALL, RAIL_EVENTS_TX_COMPLETION|RAIL_EVENTS_RX_COMPLETION|RAIL_EVENT_CAL_NEEDED);

  // Code to enable FIFOs
  RAIL_SetTxFifo(railHandle, tx_buffer, 0, TX_BUFFER_LENGTH);
  static const RAIL_DataConfig_t railDataConfig = \
		  {.txSource = TX_PACKET_DATA,
	       .rxSource = RX_IQDATA_FILTLSB,
	       .txMethod = FIFO_MODE,
	       .rxMethod = FIFO_MODE};
  status = RAIL_ConfigData(railHandle, &railDataConfig);
  assert(status == RAIL_STATUS_NO_ERROR);
  uint16_t rx_fifo_size = RX_BUFFER_LENGTH;
  status = RAIL_SetRxFifo(railHandle, rx_buffer, &rx_fifo_size);
  assert(status == RAIL_STATUS_NO_ERROR);
  assert(rx_fifo_size == RX_BUFFER_LENGTH);
  RAIL_SetTxFifoThreshold(railHandle, (int)(.9*TX_BUFFER_LENGTH));
  RAIL_SetRxFifoThreshold(railHandle, (int)(.9*RX_BUFFER_LENGTH));
  RAIL_ConfigEvents(railHandle, RAIL_EVENTS_ALL, RAIL_EVENT_TX_FIFO_ALMOST_EMPTY | RAIL_EVENT_RX_FIFO_ALMOST_FULL);
}

typedef enum
{
	tone,
	receive,
	idle
} modes;

volatile static modes current_mode;
volatile static uint16_t iq_available;

void reset(void)
{
	memset(tx_buffer, 0, TX_BUFFER_LENGTH);
	memset(rx_buffer, 0, RX_BUFFER_LENGTH);
	current_mode = idle;
	iq_available = 0;
	RAIL_ResetFifo(railHandle, false, true);
}

int main(void)
{
  current_mode = idle;
  iq_available = 0;
  memset(tx_buffer, 0, TX_BUFFER_LENGTH);
  memset(rx_buffer, 0, RX_BUFFER_LENGTH);


  CHIP_Init();
  radioInit();
  peripheralInit();
  RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);

  while (1)
  {
	  int8_t c = RETARGET_ReadChar();
	  if(c == (int8_t)'t')
	  {
		  assert(current_mode == idle);
		  current_mode = tone;
		  RAIL_Status_t status = RAIL_StartTxStream(railHandle, CHANNEL, RAIL_STREAM_CARRIER_WAVE);
		  assert(status == RAIL_STATUS_NO_ERROR);
	  }
	  else if(c == (int8_t)'i')
	  {
		  assert(current_mode == tone);
		  current_mode = idle;
		  //reset();
		  RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);
	  }
	  else if(c == (int8_t)'r')
	  {
		  assert(current_mode == idle);
		  current_mode = receive;
		  RAIL_Status_t status = RAIL_StartRx(railHandle, CHANNEL, NULL);
		  assert(status == RAIL_STATUS_NO_ERROR);
	  }
	  else
		  assert(c == -1);
	  if(iq_available != 0)
	  {
		  assert(current_mode == idle);
		  printf("%d\n", iq_available/4);
		  for(int i=0; i<iq_available/2; i+=2)
		  {
			  int16_t I = ((int16_t *)rx_buffer)[i];
			  int16_t Q = ((int16_t *)rx_buffer)[i+1];
			  printf("%d;%d\n", I, Q);
		  }
		  iq_available = 0;
		  reset();
	  }
  }
}

void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  if(events & RAIL_EVENT_RX_FIFO_ALMOST_FULL)
  {
	  assert(current_mode == receive);
	  iq_available = RAIL_GetRxFifoBytesAvailable(railHandle);
	  //RAIL_ReadRxFifo(railHandle, rx_buffer, iq_available);
	  RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);
	  RAIL_ResetFifo(railHandle, false, true);
	  current_mode = idle;
  }
  if(events & RAIL_EVENTS_RX_COMPLETION)
	assert(false);
  if(events & RAIL_EVENT_CAL_NEEDED)
	  RAIL_Calibrate(railHandle, NULL, RAIL_CAL_ALL_PENDING);
  if(events & RAIL_EVENTS_TX_COMPLETION);
}

void gpioCallback(uint8_t pin)
{
}

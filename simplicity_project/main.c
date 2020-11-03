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
#include "rx.h"

// Project-specific libraries
#include "led_assert.h"
#include "oneshot_timer.h"

// Constants
#define TX_BUFFER_LENGTH 256
#define RX_BUFFER_LENGTH 2048
#define PAYLOAD_LENGTH   16
#define CHANNEL          0
#define MAX_NUM_TRIALS   10

// Event handlers
void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);

// Buffers
static uint8_t rx_buffer[RX_BUFFER_LENGTH];
static uint8_t tx_buffer[TX_BUFFER_LENGTH];
static uint32_t data[MAX_NUM_TRIALS][RX_BUFFER_LENGTH/4];

// State variables
typedef enum
{
	tone,
	receive,
	idle
} modes;
static volatile modes current_mode;
static volatile uint16_t iq_available;
RAIL_Handle_t railHandle;
static RAIL_Config_t railCfg = {
  .eventsCallback = &RAILCb_Generic,
};

// Project-specific functions
void peripheralInit(void)
{
	halInit();
	ONESHOT_init();
	RETARGET_SerialCrLf(1);
}

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
  RAIL_SetTxPowerDbm(railHandle, 0);

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
  RAIL_SetTxFifoThreshold(railHandle, (int)(.45*TX_BUFFER_LENGTH));
  RAIL_SetRxFifoThreshold(railHandle, RX_BUFFER_LENGTH-256);
  RAIL_ConfigEvents(railHandle, RAIL_EVENTS_ALL, RAIL_EVENT_TX_FIFO_ALMOST_EMPTY | RAIL_EVENT_RX_FIFO_ALMOST_FULL);
}

void reset(void)
{
	memset(tx_buffer, 0, TX_BUFFER_LENGTH);
	memset(rx_buffer, 0, RX_BUFFER_LENGTH);
	current_mode = idle;
	iq_available = 0;
	RAIL_ResetFifo(railHandle, false, true);
}

void set_mode_tone(void)
{
	assert(current_mode == idle);
	current_mode = tone;
	RAIL_Status_t status = RAIL_StartTxStream(railHandle, CHANNEL, RAIL_STREAM_CARRIER_WAVE);
	assert(status == RAIL_STATUS_NO_ERROR);
}

void set_mode_receive(void)
{
	assert(current_mode == idle);
	current_mode = receive;
	RAIL_ResetFifo(railHandle, false, true);
	RAIL_Status_t status = RAIL_StartRx(railHandle, CHANNEL, NULL);
	assert(status == RAIL_STATUS_NO_ERROR);
}

void set_mode_idle(void)
{
	assert(current_mode == tone);
	current_mode = idle;
	RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);
}

void print_results(int num_trials, uint16_t * n_samples)
{
	assert(current_mode == idle);
	for(uint16_t trial_idx=0; trial_idx<num_trials; ++trial_idx)
	{
		printf("%d\n", n_samples[trial_idx]/4);
		for(int i=0; i<n_samples[trial_idx]/2; i+=2)
		{
			int16_t I = ((int16_t *)data[trial_idx])[i];
			int16_t Q = ((int16_t *)data[trial_idx])[i+1];
			printf("%d;%d\n", I, Q);
		}
	}
}

void parse_delay(int * initial_delay_us, int * num_trials, int * delay_us)
{
	typedef enum
	{
		fieldNone,
		fieldInitialDelay,
		fieldNumTrials,
		fieldDelay
	} field;
	field currently_parsing = fieldNone;
	int8_t initial_delay_raw[5];
	int initial_delay_idx = 0;
	int8_t num_trials_raw[2];
	int num_trials_idx = 0;
	int8_t delay_raw[5];
	int delay_idx = 0;
	while(1)
	{
		int8_t c = RETARGET_ReadChar();
		if(c == -1)
			continue;
		else if(c == (int8_t)'i')
		{
			assert(currently_parsing == fieldNone);
			currently_parsing = fieldInitialDelay;
		} else if(c == (int8_t)'n')
		{
			assert(currently_parsing == fieldInitialDelay);
			assert(initial_delay_idx != 0);
			currently_parsing = fieldNumTrials;
		} else if(c == (int8_t)'d')
		{
			assert(currently_parsing == fieldNumTrials);
			assert(num_trials_idx != 0);
			currently_parsing = fieldDelay;
		} else if(c == (int8_t)'\n')
		{
			assert(currently_parsing == fieldDelay);
			assert(delay_idx != 0);
			break;
		} else
		{
			assert('0'<=c && c<='9');
			if(currently_parsing == fieldNone)
				assert(false);
			else if(currently_parsing == fieldInitialDelay)
			{
				assert((0 <= initial_delay_idx) && (initial_delay_idx < 5));
				initial_delay_raw[initial_delay_idx] = c - (int8_t)'0';
				initial_delay_idx += 1;
			} else if(currently_parsing == fieldNumTrials)
			{
				assert((0 <= num_trials_idx) && (num_trials_idx < 2));
				num_trials_raw[num_trials_idx] = c - (int8_t)'0';
				num_trials_idx += 1;
			} else if(currently_parsing == fieldDelay)
			{
				assert((0 <= delay_idx) && (delay_idx < 5));
				delay_raw[delay_idx] = c - (int8_t)'0';
				delay_idx += 1;
			}
		}
	}
	*initial_delay_us = 0;
	for(int i=initial_delay_idx-1, j=1; i>=0; --i, j*=10)
		*initial_delay_us += j * ((int)initial_delay_raw[i]);
	assert((0 <= *initial_delay_us) && (*initial_delay_us <= 100000));
	*num_trials = 0;
	for(int i=num_trials_idx-1, j=1; i>=0; --i, j*=10)
		*num_trials += j * ((int)num_trials_raw[i]);
	assert((0 <= *num_trials) && (*num_trials <= 10));
	*delay_us = 0;
	for(int i=delay_idx-1, j=1; i>=0; --i, j*=10)
		*delay_us += j * ((int)delay_raw[i]);
	assert((0 <= *delay_us) && (*delay_us <= 100000));
}

void receive_multiple_trials(int initial_delay_us, int num_trials, int delay_us)
{
	int trial_number = 0;
	uint16_t sample_counts[MAX_NUM_TRIALS];
	if(initial_delay_us > 0)
		ONESHOT_delay(initial_delay_us);
	ONESHOT_configure(delay_us);
	while(trial_number < num_trials)
	{
		ONESHOT_set();
		set_mode_receive();
		while(current_mode == receive);
		sample_counts[trial_number] = iq_available;
		memcpy(data[trial_number], rx_buffer, iq_available);
		iq_available = 0;
		trial_number += 1;
		reset();
		assert(ONESHOT_is_set_coarse() || ONESHOT_is_set_fine());
		while(ONESHOT_is_set_coarse());
		while(ONESHOT_is_set_fine());
	}
	print_results(num_trials, sample_counts);
}

int main(void)
{

  current_mode = idle;
  iq_available = 0;
  memset(tx_buffer, 0, TX_BUFFER_LENGTH);
  memset(rx_buffer, 0, RX_BUFFER_LENGTH);

  CHIP_Init();
  peripheralInit();
  radioInit();
  RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);

  while (1)
  {
	  int8_t c = RETARGET_ReadChar();
	  if(c == (int8_t)'t')
	  {
		  assert(RAIL_IsInitialized());
		  RAIL_Status_t status = RAIL_StartTxStream(railHandle, 0, RAIL_STREAM_10_STREAM);
		  assert(status == RAIL_STATUS_NO_ERROR);
		  while(RETARGET_ReadChar() != (uint8_t)'t');
		  RAIL_StopTxStream(railHandle);
	  }
	  else if(c == (int8_t)'r')
	  {
		  RX_loop(railHandle, rx_buffer);
	  }
	  else
		  assert(c == -1);
  }
}

void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  if(events & RAIL_EVENT_RX_FIFO_ALMOST_FULL)
  {
	  assert(current_mode == receive);
	  iq_available = 4*(RAIL_GetRxFifoBytesAvailable(railHandle)/4);
	  RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);
	  RAIL_ResetFifo(railHandle, false, true);
	  current_mode = idle;
  }
  if(events & RAIL_EVENTS_RX_COMPLETION)
	assert(false);
  if(events & RAIL_EVENT_CAL_NEEDED)
	  RAIL_Calibrate(railHandle, NULL, RAIL_CAL_ALL_PENDING);
  if(events & RAIL_EVENTS_TX_COMPLETION){}
}

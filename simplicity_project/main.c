
// Standard C libraries
#include <stdio.h>

// SI labs libraries
#include "em_chip.h"
#include "hal-config.h"
#include "bsp.h"
#include "retargetserial.h"
#include "gpiointerrupt.h"
#include "rail.h"
#include "hal_common.h"
#include "rail_config.h"

// Project-specific libraries
#include "led_assert.h"
#include "tx.h"
#include "rx.h"

#define TX_FIFO_SIZE 256
#define RX_FIFO_SIZE 4096

static char next_char(void)
{
	while(1)
	{
		int8_t c = RETARGET_ReadChar();
		if(c != -1)
			return (char)c;
	}
}

static void peripheral_init(void)
{
	halInit();
	RETARGET_SerialInit();
}

uint8_t tx_fifo[TX_FIFO_SIZE];
uint8_t rx_fifo[RX_FIFO_SIZE];

typedef enum
{
	undefined,
	receiver,
	transmitter
} role_t;
static role_t role;

RAIL_Status_t RAILCb_SetupRxFifo(RAIL_Handle_t rail_handle)
{
	uint16_t size = RX_FIFO_SIZE;
	RAIL_Status_t status = RAIL_SetRxFifo(rail_handle, rx_fifo, &size);
	assert(size == RX_FIFO_SIZE);
	assert(status == RAIL_STATUS_NO_ERROR);
	return RAIL_STATUS_NO_ERROR;
}

RAIL_Handle_t rail_handle = NULL;
void event_handler(RAIL_Handle_t, RAIL_Events_t);
RAILSched_Config_t rail_scheduler;
RAIL_Config_t rail_config =
{
		.eventsCallback = &event_handler,
		.protocol = NULL,
		.scheduler = &rail_scheduler
};
static void rail_global_init(void)
{
	memset(&rail_scheduler, 0, sizeof(RAILSched_Config_t));
	memset(&(rail_config.buffer), 0, sizeof(RAIL_StateBuffer_t));
	rail_handle = RAIL_Init(&rail_config, NULL);
	assert(rail_handle != NULL);
	RAIL_Status_t status = RAIL_ConfigCal(rail_handle, RAIL_CAL_ALL);
	assert(status == RAIL_STATUS_NO_ERROR);
	RAIL_ConfigChannels(rail_handle, *channelConfigs, NULL);
	RAIL_TxPowerConfig_t tx_power =
	{
			.mode = RAIL_TX_POWER_MODE_2P4_HP,
			.voltage = BSP_PA_VOLTAGE,
			.rampTime = HAL_PA_RAMP
	};
	status = RAIL_ConfigTxPower(rail_handle, &tx_power);
	assert(status == RAIL_STATUS_NO_ERROR);
	status = RAIL_EnablePaAutoMode(rail_handle, true);
	assert(status == RAIL_STATUS_NO_ERROR);
	status = RAIL_SetTxPowerDbm(rail_handle, 0);
	assert(status == RAIL_STATUS_NO_ERROR);
	status = RAIL_ConfigEvents(rail_handle, RAIL_EVENTS_ALL, RAIL_EVENT_CAL_NEEDED);
	assert(status == RAIL_STATUS_NO_ERROR);
	RAIL_DataConfig_t data_config =
	{
			.txSource = TX_PACKET_DATA,
			.rxSource = RX_IQDATA_FILTLSB,
			.txMethod = FIFO_MODE,
			.rxMethod = FIFO_MODE
	};
	status = RAIL_ConfigData(rail_handle, &data_config);
	assert(status == RAIL_STATUS_NO_ERROR);
	uint16_t size = RAIL_SetTxFifo(rail_handle, tx_fifo, 0, TX_FIFO_SIZE);
	assert(size == TX_FIFO_SIZE);
	RAIL_ResetFifo(rail_handle, true, true);
}

static void parse_txconfig(uint16_t * period_us, uint16_t * time_trans_us, uint8_t * channel)
{
	assert(role == transmitter);
	const int txconfig_length = 5;
	uint8_t config[txconfig_length];
	for(int i=0; i<txconfig_length; ++i)
		config[i] = (uint8_t)next_char();
	assert(next_char() == '\n');
	*period_us = ((uint16_t *)config)[0];
	*time_trans_us = ((uint16_t *)config)[1];
	*channel = ((uint8_t *)config)[4];
	RETARGET_WriteChar('\n');
}

static void parse_rxconfig(uint16_t * delay_us, uint16_t * duration_us, uint8_t * channel)
{
	assert(role == receiver);
	const int rxconfig_length = 5;
	uint8_t config[rxconfig_length];
	for(int i=0; i<rxconfig_length; ++i)
		config[i] = next_char();
	assert(next_char() == '\n');
	*delay_us = ((uint16_t *)config)[0];
	*duration_us = ((uint16_t *)config)[1];
	*channel = ((uint8_t *)config)[4];
	RETARGET_WriteChar('\n');
}

int main(void)
{
	memset(tx_fifo, 0, TX_FIFO_SIZE);
	memset(rx_fifo, 0, RX_FIFO_SIZE);
	CHIP_Init();
	peripheral_init();
	rail_global_init();

	while(1)
	{
		char c = next_char();
		if (c == 't')
		{
			assert(RAIL_IsInitialized());
			RAIL_Status_t status = RAIL_StartTxStream(rail_handle, 0, RAIL_STREAM_10_STREAM);
			assert(status == RAIL_STATUS_NO_ERROR);
			while(1);
		}
		else if(c == 'r')
		{
			RX_loop(rail_handle, rx_fifo);
		}
		/*if(c == 't')
		{
			//assert(role == undefined);
			role = transmitter;
			uint16_t period_us, time_trans_us;
			uint8_t channel;
			parse_txconfig(&period_us, &time_trans_us, &channel);
			TX_config(rail_handle, period_us, time_trans_us, channel);
			//TX_loop(rail_handle);
			RAIL_Status_t status = RAIL_StartTxStream(rail_handle, channel, RAIL_STREAM_10_STREAM);
			assert(status == RAIL_STATUS_NO_ERROR);
			while(1);
		}
		else if(c == 'r')
		{
			//assert(role == undefined);
			role = receiver;
			uint16_t delay_us, duration_us;
			uint8_t channel;
			parse_rxconfig(&delay_us, &duration_us, &channel);
			RX_config(rail_handle, delay_us, duration_us, channel);
			RX_loop(rail_handle, rx_fifo);
		}*/
		else
			assert(false);
	}
}

void event_handler(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
	if(events & RAIL_EVENT_CAL_NEEDED)
	{
		RAIL_Status_t status = RAIL_Calibrate(rail_handle, NULL, RAIL_CAL_ALL_PENDING);
		assert(status == RAIL_STATUS_NO_ERROR);
	}
	if(events & (~RAIL_EVENT_CAL_NEEDED))
	{
		if(role == transmitter)
			TX_event_handler(rail_handle, events);
		else if(role == receiver)
			RX_event_handler(rail_handle, events);
		else
			assert(false);
	}
}

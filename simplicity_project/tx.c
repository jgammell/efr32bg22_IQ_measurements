/*
 * tx.c
 *
 *  Created on: Aug 14, 2020
 *      Author: jgamm
 */

#include "tx.h"
#include "led_assert.h"
#include "retargetserial.h"

#define TX_EVENTS (RAIL_EVENT_TX_FIFO_ALMOST_EMPTY | RAIL_EVENT_TX_PACKET_SENT |\
			RAIL_EVENT_TX_ABORTED | RAIL_EVENT_TX_BLOCKED | RAIL_EVENT_TX_UNDERFLOW |\
			RAIL_EVENT_TX_CHANNEL_BUSY | RAIL_EVENT_TX_SCHEDULED_TX_MISSED)
#define DEFAULT_TRANS_TIME 100
#define PACKET_DATA 0xFF
#define PACKET_LEN  1
#define TX_THRESH 4

static volatile bool tx_pending = true;
static volatile RAIL_Time_t next_transition_time;
static uint16_t period_us;
static uint8_t channel;

void TX_config(RAIL_Handle_t rail_handle, uint16_t _period_us, uint16_t time_trans_us, uint8_t _channel)
{
	assert(RAIL_IsInitialized());
	period_us = _period_us;
	channel = _channel;
	RAIL_StateTransitions_t transitions =
	{
			.success = RAIL_RF_STATE_IDLE,
			.error = RAIL_RF_STATE_IDLE
	};
	RAIL_Status_t status = RAIL_SetTxTransitions(rail_handle, &transitions);
	assert(status == RAIL_STATUS_NO_ERROR);
	status = RAIL_ConfigEvents(rail_handle, TX_EVENTS, TX_EVENTS);
	assert(status == RAIL_STATUS_NO_ERROR);
	RAIL_StateTiming_t timings =
	{
			.idleToRx = DEFAULT_TRANS_TIME,
			.txToRx = DEFAULT_TRANS_TIME,
			.idleToTx = time_trans_us,
			.rxToTx = DEFAULT_TRANS_TIME,
			.rxSearchTimeout = DEFAULT_TRANS_TIME,
			.txToRxSearchTimeout = DEFAULT_TRANS_TIME
	};
	status = RAIL_SetStateTiming(rail_handle, &timings);
	assert(status == RAIL_STATUS_NO_ERROR);
	uint16_t thresh = RAIL_SetTxFifoThreshold(rail_handle, TX_THRESH);
	assert(thresh == TX_THRESH);
	uint16_t size = RAIL_SetFixedLength(rail_handle, PACKET_LEN);
	assert(size == PACKET_LEN);
}

static void refill_tx_buffer(RAIL_Handle_t rail_handle)
{
	uint8_t packet[PACKET_LEN];
	memset(packet, PACKET_DATA, PACKET_LEN);
	uint16_t size = RAIL_WriteTxFifo(rail_handle, packet, PACKET_LEN, false);
	assert(size == PACKET_LEN);
}
static void schedule_next_tx(RAIL_Handle_t rail_handle)
{
	RAIL_ScheduleTxConfig_t txconfig =
	{
			.when = next_transition_time,
			.mode = RAIL_TIME_ABSOLUTE,
			.txDuringRx = RAIL_SCHEDULED_TX_DURING_RX_ABORT_TX
	};
	RAIL_Status_t status = RAIL_StartScheduledTx(rail_handle, channel, 0, &txconfig, NULL);
	assert(status == RAIL_STATUS_NO_ERROR);
	next_transition_time += period_us;
	tx_pending = true;
}

void TX_loop(RAIL_Handle_t rail_handle)
{
	RAIL_ResetFifo(rail_handle, true, false);
	next_transition_time = RAIL_GetTime() + 5*period_us;
	for(int i=0; i<TX_THRESH+5; ++i)
		refill_tx_buffer(rail_handle);
	schedule_next_tx(rail_handle);
	while(1)
	{
		int space = RAIL_GetTxFifoSpaceAvailable(rail_handle);
		assert(RAIL_GetTime() < next_transition_time);
		while(tx_pending == true);
		refill_tx_buffer(rail_handle);
		schedule_next_tx(rail_handle);
		int8_t c = RETARGET_ReadChar();
		if(c == (int8_t)'i')
			break;
		else
			assert(c == -1);
	}
}

void TX_event_handler(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
	if(events & RAIL_EVENT_TX_PACKET_SENT)
	{
		assert(tx_pending == true);
		tx_pending = false;
	}
	else if(events & (TX_EVENTS & (~RAIL_EVENT_TX_PACKET_SENT)))
	{
		if(events & RAIL_EVENT_TX_FIFO_ALMOST_EMPTY)
			assert(false);
		if(events & RAIL_EVENT_TX_ABORTED)
			assert(false);
		if(events & RAIL_EVENT_TX_BLOCKED)
			assert(false);
		if(events & RAIL_EVENT_TX_UNDERFLOW)
			assert(false);
		if(events & RAIL_EVENT_TX_CHANNEL_BUSY)
			assert(false);
		if(events & RAIL_EVENT_TX_SCHEDULED_TX_MISSED)
			assert(false);
	}
	else
		assert(false);
}

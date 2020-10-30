/*
 * rx.c
 *
 *  Created on: Aug 14, 2020
 *      Author: jgamm
 */

#include <stdio.h>
#include "rx.h"
#include "rail.h"
#include "led_assert.h"

#define RX_EVENTS (RAIL_EVENT_RX_FIFO_FULL | RAIL_EVENT_RX_FIFO_OVERFLOW |\
	RAIL_EVENT_RX_SCHEDULED_RX_END | RAIL_EVENT_RX_SCHEDULED_RX_MISSED)

static volatile bool rx_pending = false;
static volatile int iq_available = 0;
static uint16_t delay_us, duration_us;
static uint8_t channel;

void RX_config(RAIL_Handle_t rail_handle, uint16_t _delay_us, uint16_t _duration_us, uint8_t _channel)
{
	assert(RAIL_IsInitialized());
	delay_us = _delay_us;
	duration_us = _duration_us;
	channel = _channel;
	RAIL_Status_t status = RAIL_ConfigEvents(rail_handle, RX_EVENTS, RX_EVENTS);
	assert(status == RAIL_STATUS_NO_ERROR);
}

/*void RX_loop(RAIL_Handle_t rail_handle, uint8_t * rx_fifo)
{
	const RAIL_ScheduleRxConfig_t rxconfig =
	{
			.start = delay_us,
			.startMode = RAIL_TIME_DELAY,
			.end = duration_us,
			.endMode = RAIL_TIME_DELAY,
			.rxTransitionEndSchedule = 0,
			.hardWindowEnd = 1
	};
	RAIL_ResetFifo(rail_handle, false, true);
	RAIL_Status_t status = RAIL_ScheduleRx(rail_handle, channel, &rxconfig, NULL);
	assert(status == RAIL_STATUS_NO_ERROR);
	rx_pending = true;
	while(rx_pending);
	assert(iq_available > 0);
	printf("%d\n", iq_available/4);
	for(int i=0; i<iq_available/2; i+=2)
	{
		int16_t I = ((int16_t *)rx_fifo)[i];
		int16_t Q = ((int16_t *)rx_fifo)[i+1];
		printf("%d;%d\n", I, Q);
	}
}*/

void RX_loop(RAIL_Handle_t rail_handle, uint8_t * rx_fifo)
{
	RAIL_ResetFifo(rail_handle, false, true);
	RAIL_Status_t status = RAIL_StartRx(rail_handle, 0, NULL);
	assert(status == RAIL_STATUS_NO_ERROR);
	rx_pending = true;
	while(RAIL_GetRxFifoBytesAvailable(rail_handle) < 1000);
	iq_available = 4*(RAIL_GetRxFifoBytesAvailable(rail_handle)/4);
	RAIL_Idle(rail_handle, RAIL_IDLE_ABORT, true);
	RAIL_ResetFifo(rail_handle, false, true);
	printf("%d\n", iq_available/4);
	for(int i=0; i<iq_available/2; i+=2)
	{
		int16_t I = ((int16_t *)rx_fifo)[i];
		int16_t Q = ((int16_t *)rx_fifo)[i+1];
		printf("%d;%d\n", I, Q);
	}
}

void RX_event_handler(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
	if(events & RAIL_EVENT_RX_SCHEDULED_RX_END)
	{
		iq_available = 4*(RAIL_GetRxFifoBytesAvailable(rail_handle)/4);
		RAIL_Idle(rail_handle, RAIL_IDLE_ABORT, true);
		RAIL_ResetFifo(rail_handle, false, true);
		assert(rx_pending == true);
		rx_pending = false;
	}
	else if(events & (RX_EVENTS & (~RAIL_EVENT_RX_SCHEDULED_RX_END)))
	{
		if(events & RAIL_EVENT_RX_FIFO_FULL)
			assert(false);
		if(events & RAIL_EVENT_RX_FIFO_OVERFLOW)
			assert(false);
		if(events & RAIL_EVENT_RX_SCHEDULED_RX_MISSED)
			assert(false);
	}
	else
		assert(false);
}

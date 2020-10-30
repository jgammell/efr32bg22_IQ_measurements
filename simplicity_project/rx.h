/*
 * rx.h
 *
 *  Created on: Aug 14, 2020
 *      Author: jgamm
 */

#ifndef RX_H_
#define RX_H_

#include <stdint.h>
#include "rail.h"

void RX_config(RAIL_Handle_t rail_handle, uint16_t delay_us, uint16_t duration_us, uint8_t channel);

void RX_loop(RAIL_Handle_t rail_handle, uint8_t * rx_fifo);

void RX_event_handler(RAIL_Handle_t rail_handle, RAIL_Events_t events);

#endif /* RX_H_ */

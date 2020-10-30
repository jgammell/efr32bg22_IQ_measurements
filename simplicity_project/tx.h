/*
 * tx.h
 *
 *  Created on: Aug 14, 2020
 *      Author: jgamm
 */

#ifndef TX_H_
#define TX_H_

#include <stdint.h>
#include "rail.h"

void TX_config(RAIL_Handle_t rail_handle, uint16_t period_us, uint16_t time_trans_us, uint8_t channel);

void TX_loop(RAIL_Handle_t rail_handle);

void TX_event_handler(RAIL_Handle_t rail_handle, RAIL_Events_t events);

#endif /* TX_H_ */

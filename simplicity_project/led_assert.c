/*
 * led_assert.c
 *
 *  Created on: Jul 24, 2020
 *      Author: jgamm
 */

#include "led_assert.h"
#include "bsp.h"

void assert(bool condition)
{
	if(condition == false)
	{
		BSP_LedSet(0);
		while(1);
	}
}

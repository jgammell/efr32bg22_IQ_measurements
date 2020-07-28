/*
 * timer.c
 *
 *  Created on: Jul 24, 2020
 *      Author: jgamm
 */


#include "oneshot_timer.h"
#include "em_timer.h"
#include "em_cmu.h"
#include "led_assert.h"

static uint32_t repetitions_setting = 0;
static uint16_t initial_count = 0;
volatile uint32_t repetitions = 0;

void ONESHOT_init(void)
{
	assert(TIMER_Valid(ONESHOT_TIMER));
	repetitions = 0;
	repetitions_setting = 0;
	initial_count = 0;
	CMU_ClockEnable(cmuClock_ONESHOT, true);
	//TIMER_Reset(ONESHOT_TIMER);
	TIMER_Init_TypeDef init_settings;
	init_settings.enable     = false;
	init_settings.debugRun   = false;
	init_settings.prescale   = timerPrescale1;
	init_settings.clkSel     = timerClkSelHFPerClk;
	init_settings.count2x    = false;
	init_settings.ati        = false;
	init_settings.fallAction = timerInputActionNone;
	init_settings.riseAction = timerInputActionNone;
	init_settings.mode       = timerModeUp;
	init_settings.dmaClrAct  = false;
	init_settings.quadModeX4 = false;
	init_settings.oneShot    = false;
	init_settings.sync       = false;
	TIMER_Init(ONESHOT_TIMER, &init_settings);
	TIMER_InitCC_TypeDef cc_settings;
	cc_settings.eventCtrl = timerEventEveryEdge;
	cc_settings.edge = timerEdgeRising;
	cc_settings.prsSel = 0; // not used
	cc_settings.cufoa = timerOutputActionNone;
	cc_settings.cofoa = timerOutputActionNone;
	cc_settings.cmoa = timerOutputActionSet;
	cc_settings.mode = timerCCModeCompare;
	cc_settings.filter = 0;
	cc_settings.prsInput = false;
	cc_settings.coist = false;
	cc_settings.outInvert = false;
	cc_settings.prsOutput = timerPrsOutputDefault;
	cc_settings.prsInputType = timerPrsInputNone;
	TIMER_InitCC(ONESHOT_TIMER, ONESHOT_CC_NUM, &cc_settings);
	TIMER_TopSet(ONESHOT_TIMER, TIMER_MaxCount(ONESHOT_TIMER));
	TIMER_IntDisable(ONESHOT_TIMER, _TIMER_IEN_MASK);
	NVIC_EnableIRQ(ONESHOT_IRQn);
}

void ONESHOT_configure(uint32_t time_us)
{
	uint32_t freq_mhz = CMU_ClockFreqGet(cmuClock_ONESHOT)/1000000;
	uint32_t counts_needed = time_us*freq_mhz;
	repetitions_setting = counts_needed/TIMER_MaxCount(ONESHOT_TIMER);
	if((repetitions_setting != 0) && (counts_needed%TIMER_MaxCount(ONESHOT_TIMER) < 0x8000))
	{
		TIMER_CompareSet(ONESHOT_TIMER, ONESHOT_CC_NUM, (counts_needed%TIMER_MaxCount(ONESHOT_TIMER))+0x8000);
		initial_count = 0x8000;
	} else
	{
		TIMER_CompareSet(ONESHOT_TIMER, ONESHOT_CC_NUM, counts_needed%TIMER_MaxCount(ONESHOT_TIMER));
		initial_count = 0;
	}
}

void ONESHOT_set(void)
{
	ONESHOT_TIMER->IF_CLR = _TIMER_IF_MASK;
	ONESHOT_TIMER->CNT_SET = initial_count;
	ONESHOT_TIMER->CMD = TIMER_CMD_START;
	repetitions = repetitions_setting;
	ONESHOT_TIMER->IEN_SET = (repetitions != 0) * TIMER_IEN_OF;
}

bool ONESHOT_is_set_coarse(void)
{
	return repetitions > 0;
}

void ONESHOT_delay(int time_us)
{
	ONESHOT_configure(time_us);
	ONESHOT_set();
	while(ONESHOT_is_set_coarse());
	while(ONESHOT_is_set_fine());
}

void ONESHOT_clear(void)
{
	TIMER_Enable(ONESHOT_TIMER, false);
	TIMER_IntDisable(ONESHOT_TIMER, TIMER_IEN_UF);
}

void ONESHOT_IRQHandler(void)
{
	uint32_t flags = ONESHOT_TIMER->IF & ONESHOT_TIMER->IEN;
	ONESHOT_TIMER->IF_CLR = flags;
	if(flags & TIMER_IF_OF)
	{
		assert(repetitions > 0);
		repetitions -= 1;
		if(repetitions == 0)
		{
			TIMER_IntDisable(ONESHOT_TIMER, TIMER_IEN_OF);
			ONESHOT_TIMER->IF_CLR = ONESHOT_IF_CC;
		}
	}
	if(flags & ~TIMER_IF_OF)
		assert(false);
}

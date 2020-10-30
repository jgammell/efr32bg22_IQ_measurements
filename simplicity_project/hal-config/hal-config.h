#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

#include "em_device.h"
#include "hal-config-types.h"

// This file is auto-generated by Hardware Configurator in Simplicity Studio.
// Any content between $[ and ]$ will be replaced whenever the file is regenerated.
// Content outside these regions will be preserved.

// $[ANTDIV]
// [ANTDIV]$

// $[BTL_BUTTON]
// [BTL_BUTTON]$

// $[BUTTON]
#define BSP_BUTTON_PRESENT                   (1)

#define BSP_BUTTON0_PIN                      (1U)
#define BSP_BUTTON0_PORT                     (gpioPortB)

#define BSP_BUTTON_COUNT                     (1U)
#define BSP_BUTTON_INIT                      { { BSP_BUTTON0_PORT, BSP_BUTTON0_PIN } }
#define BSP_BUTTON_GPIO_DOUT                 (HAL_GPIO_DOUT_LOW)
#define BSP_BUTTON_GPIO_MODE                 (HAL_GPIO_MODE_INPUT)
// [BUTTON]$

// $[CMU]
#define HAL_CLK_HFCLK_SOURCE                 (HAL_CLK_HFCLK_SOURCE_HFXO)
#define HAL_CLK_PLL_CONFIGURATION            (HAL_CLK_PLL_CONFIGURATION_76_8MHZ)
#define HAL_CLK_EM01CLK_SOURCE               (HAL_CLK_HFCLK_SOURCE_HFRCODPLL)
#define HAL_CLK_EM23CLK_SOURCE               (HAL_CLK_LFCLK_SOURCE_LFRCO)
#define HAL_CLK_EM4CLK_SOURCE                (HAL_CLK_LFCLK_SOURCE_LFRCO)
#define HAL_CLK_RTCCCLK_SOURCE               (HAL_CLK_LFCLK_SOURCE_LFRCO)
#define HAL_CLK_WDOGCLK_SOURCE               (HAL_CLK_LFCLK_SOURCE_LFRCO)
#define BSP_CLK_HFXO_PRESENT                 (0)
#define BSP_CLK_HFXO_FREQ                    (38400000UL)
#define BSP_CLK_HFXO_INIT                     CMU_HFXOINIT_DEFAULT
#define BSP_CLK_HFXO_CTUNE                   (-1)
#define BSP_CLK_LFXO_PRESENT                 (0)
#define BSP_CLK_LFXO_INIT                     CMU_LFXOINIT_DEFAULT
#define BSP_CLK_LFXO_FREQ                    (32768U)
#define BSP_CLK_LFXO_CTUNE                   (0U)
#define HAL_CLK_LFRCO_PRECISION              (0)
// [CMU]$

// $[COEX]
// [COEX]$

// $[DCDC]
#define BSP_DCDC_PRESENT                     (1)

#define BSP_DCDC_INIT                         EMU_DCDCINIT_DEFAULT
#define HAL_DCDC_BYPASS                      (0)
// [DCDC]$

// $[EMU]
// [EMU]$

// $[EUART0]
// [EUART0]$

// $[EXTFLASH]
// [EXTFLASH]$

// $[EZRADIOPRO]
// [EZRADIOPRO]$

// $[FEM]
// [FEM]$

// $[GPIO]
// [GPIO]$

// $[I2C0]
// [I2C0]$

// $[I2C1]
// [I2C1]$

// $[I2CSENSOR]
// [I2CSENSOR]$

// $[IADC0]
// [IADC0]$

// $[IOEXP]
// [IOEXP]$

// $[LED]
#define BSP_LED_PRESENT                      (1)

#define BSP_LED0_PIN                         (0U)
#define BSP_LED0_PORT                        (gpioPortB)

#define BSP_LED_COUNT                        (1U)
#define BSP_LED_INIT                         { { BSP_LED0_PORT, BSP_LED0_PIN } }
// #define BSP_LED_COUNT                        (2U)
// #define BSP_LED_INIT                         { { BSP_LED0_PORT, BSP_LED0_PIN }, { BSP_LED1_PORT, BSP_LED1_PIN } }
#define HAL_LED_COUNT                        (0U)
#define HAL_LED_ENABLE                       { 0, 1 }
#define BSP_LED_POLARITY                     (1)
// [LED]$

// $[LETIMER0]
// [LETIMER0]$

// $[LFXO]
// [LFXO]$

// $[MODEM]
// [MODEM]$

// $[PA]
#define HAL_PA_ENABLE                        (1)

#define HAL_PA_CURVE_HEADER                   "pa_curves_efr32.h"
#define HAL_PA_POWER                         (252U)
#define HAL_PA_RAMP                          (10UL)
#define BSP_PA_VOLTAGE                       (3300U)
#define HAL_PA_SELECTION                     (HAL_PA_SELECTION_2P4_HP)
// [PA]$

// $[PDM]
// [PDM]$

// $[PORTIO]
// [PORTIO]$

// $[PRS]
// [PRS]$

// $[PTI]
#define PORTIO_PTI_DFRAME_PIN                (5U)
#define PORTIO_PTI_DFRAME_PORT               (gpioPortC)

#define PORTIO_PTI_DOUT_PIN                  (4U)
#define PORTIO_PTI_DOUT_PORT                 (gpioPortC)

#define BSP_PTI_DFRAME_PIN                   (5U)
#define BSP_PTI_DFRAME_PORT                  (gpioPortC)

#define BSP_PTI_DOUT_PIN                     (4U)
#define BSP_PTI_DOUT_PORT                    (gpioPortC)

#define HAL_PTI_ENABLE                       (1)

#define HAL_PTI_MODE                         (HAL_PTI_MODE_UART)
#define HAL_PTI_BAUD_RATE                    (1600000UL)
// [PTI]$

// $[SERIAL]

#define BSP_SERIAL_APP_PORT                  (HAL_SERIAL_PORT_USART1)
#define BSP_SERIAL_APP_TX_PIN                (5U)
#define BSP_SERIAL_APP_TX_PORT               (gpioPortA)

#define BSP_SERIAL_APP_RX_PIN                (6U)
#define BSP_SERIAL_APP_RX_PORT               (gpioPortA)

#define BSP_SERIAL_APP_CTS_PIN               (8U)
#define BSP_SERIAL_APP_CTS_PORT              (gpioPortA)

#define BSP_SERIAL_APP_RTS_PIN               (7U)
#define BSP_SERIAL_APP_RTS_PORT              (gpioPortA)

#define HAL_SERIAL_RXWAKE_ENABLE             (0)
#define HAL_SERIAL_IDLE_WAKE_ENABLE          (1)
#define HAL_SERIAL_USART0_ENABLE             (0)
#define HAL_SERIAL_USART1_ENABLE             (0)
// [SERIAL]$

// $[SPIDISPLAY]
// [SPIDISPLAY]$

// $[SPINCP]
// [SPINCP]$

// $[TIMER0]
// [TIMER0]$

// $[TIMER1]
// [TIMER1]$

// $[TIMER2]
// [TIMER2]$

// $[TIMER3]
// [TIMER3]$

// $[TIMER4]
// [TIMER4]$

// $[UARTNCP]
// [UARTNCP]$

// $[USART0]
// [USART0]$

// $[USART1]
// [USART1]$

// $[VCOM]
#define HAL_VCOM_ENABLE                      (1)

// [VCOM]$

// $[VUART]
// [VUART]$

// $[WDOG]
// [WDOG]$

#if defined(_SILICON_LABS_MODULE)
#include "sl_module.h"
#endif


#endif /* HAL_CONFIG_H */

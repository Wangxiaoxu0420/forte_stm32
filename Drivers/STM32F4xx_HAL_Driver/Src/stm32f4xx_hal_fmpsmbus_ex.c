

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup FMPSMBUSEx FMPSMBUSEx
  * @brief FMPSMBUS Extended HAL module driver
  * @{
  */

#ifdef HAL_FMPSMBUS_MODULE_ENABLED
#if defined(FMPI2C_CR1_PE)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup FMPSMBUSEx_Exported_Functions FMPSMBUS Extended Exported Functions
  * @{
  */

/** @defgroup FMPSMBUSEx_Exported_Functions_Group1 Extended features functions
  * @brief    Extended features functions
 *
@verbatim
 ===============================================================================
                      ##### Extended features functions #####
 ===============================================================================
    [..] This section provides functions allowing to:

      (+) Configure Fast Mode Plus

@endverbatim
  * @{
  */

/**
  * @brief Enable the FMPSMBUS fast mode plus driving capability.
  * @param ConfigFastModePlus Selects the pin.
  *   This parameter can be one of the @ref FMPSMBUSEx_FastModePlus values
  * @note  For FMPI2C1, fast mode plus driving capability can be enabled on all selected
  *        FMPI2C1 pins using FMPI2C_FASTMODEPLUS_FMPI2C1 parameter or independently
  *        on each one of the following pins PB6, PB7, PB8 and PB9.
  * @note  For remaining FMPI2C1 pins (PA14, PA15...) fast mode plus driving capability
  *        can be enabled only by using FMPI2C_FASTMODEPLUS_FMPI2C1 parameter.
  * @retval None
  */
void HAL_FMPSMBUSEx_EnableFastModePlus(uint32_t ConfigFastModePlus)
{
  /* Check the parameter */
  assert_param(IS_FMPSMBUS_FASTMODEPLUS(ConfigFastModePlus));

  /* Enable SYSCFG clock */
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* Enable fast mode plus driving capability for selected pin */
  SET_BIT(SYSCFG->CFGR, (uint32_t)ConfigFastModePlus);
}

/**
  * @brief Disable the FMPSMBUS fast mode plus driving capability.
  * @param ConfigFastModePlus Selects the pin.
  *   This parameter can be one of the @ref FMPSMBUSEx_FastModePlus values
  * @note  For FMPI2C1, fast mode plus driving capability can be disabled on all selected
  *        FMPI2C1 pins using FMPI2C_FASTMODEPLUS_FMPI2C1 parameter or independently
  *        on each one of the following pins PB6, PB7, PB8 and PB9.
  * @note  For remaining FMPI2C1 pins (PA14, PA15...) fast mode plus driving capability
  *        can be disabled only by using FMPI2C_FASTMODEPLUS_FMPI2C1 parameter.
  * @retval None
  */
void HAL_FMPSMBUSEx_DisableFastModePlus(uint32_t ConfigFastModePlus)
{
  /* Check the parameter */
  assert_param(IS_FMPSMBUS_FASTMODEPLUS(ConfigFastModePlus));

  /* Enable SYSCFG clock */
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* Disable fast mode plus driving capability for selected pin */
  CLEAR_BIT(SYSCFG->CFGR, (uint32_t)ConfigFastModePlus);
}


/**
  * @}
  */

/**
  * @}
  */

#endif /* FMPI2C_CR1_PE */
#endif /* HAL_FMPSMBUS_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */



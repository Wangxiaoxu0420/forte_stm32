 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

#ifdef HAL_PCCARD_MODULE_ENABLED
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) ||\
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
/** @defgroup PCCARD PCCARD  
  * @brief PCCARD HAL module driver
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
    
/** @defgroup PCCARD_Private_Defines PCCARD Private Defines
  * @{
  */
#define PCCARD_TIMEOUT_READ_ID                 0x0000FFFFU
#define PCCARD_TIMEOUT_READ_WRITE_SECTOR       0x0000FFFFU
#define PCCARD_TIMEOUT_ERASE_SECTOR            0x00000400U
#define PCCARD_TIMEOUT_STATUS                  0x01000000U

#define PCCARD_STATUS_OK                       (uint8_t)0x58
#define PCCARD_STATUS_WRITE_OK                 (uint8_t)0x50
/**
  * @}
  */ 
    
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @defgroup PCCARD_Exported_Functions PCCARD Exported Functions
  * @{
  */

/** @defgroup PCCARD_Exported_Functions_Group1 Initialization and de-initialization functions 
  * @brief    Initialization and Configuration functions 
  *
  @verbatim    
  ==============================================================================
          ##### PCCARD Initialization and de-initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to initialize/de-initialize
    the PCCARD memory
  
@endverbatim
  * @{
  */
    
/**
  * @brief  Perform the PCCARD memory Initialization sequence
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.
  * @param  ComSpaceTiming Common space timing structure
  * @param  AttSpaceTiming Attribute space timing structure
  * @param  IOSpaceTiming IO space timing structure     
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCCARD_Init(PCCARD_HandleTypeDef *hpccard, FMC_NAND_PCC_TimingTypeDef *ComSpaceTiming, FMC_NAND_PCC_TimingTypeDef *AttSpaceTiming, FMC_NAND_PCC_TimingTypeDef *IOSpaceTiming)
{
  /* Check the PCCARD controller state */
  if(hpccard == NULL)
  {
     return HAL_ERROR;
  }
  
  if(hpccard->State == HAL_PCCARD_STATE_RESET)
  {  
    /* Allocate lock resource and initialize it */
    hpccard->Lock = HAL_UNLOCKED;
#if (USE_HAL_PCCARD_REGISTER_CALLBACKS == 1)
    if(hpccard->MspInitCallback == NULL)
    {
      hpccard->MspInitCallback = HAL_PCCARD_MspInit;
    }
    hpccard->ItCallback = HAL_PCCARD_ITCallback;

    /* Init the low level hardware */
    hpccard->MspInitCallback(hpccard);
#else
    /* Initialize the low level hardware (MSP) */
    HAL_PCCARD_MspInit(hpccard);
#endif
  }
  
  /* Initialize the PCCARD state */
  hpccard->State = HAL_PCCARD_STATE_BUSY;    

  /* Initialize PCCARD control Interface */
  FMC_PCCARD_Init(hpccard->Instance, &(hpccard->Init));
  
  /* Init PCCARD common space timing Interface */
  FMC_PCCARD_CommonSpace_Timing_Init(hpccard->Instance, ComSpaceTiming);
  
  /* Init PCCARD attribute space timing Interface */  
  FMC_PCCARD_AttributeSpace_Timing_Init(hpccard->Instance, AttSpaceTiming);
  
  /* Init PCCARD IO space timing Interface */  
  FMC_PCCARD_IOSpace_Timing_Init(hpccard->Instance, IOSpaceTiming);
  
  /* Enable the PCCARD device */
  __FMC_PCCARD_ENABLE(hpccard->Instance); 
  
  /* Update the PCCARD state */
  hpccard->State = HAL_PCCARD_STATE_READY;  
  
  return HAL_OK;

}

/**
  * @brief  Perform the PCCARD memory De-initialization sequence
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.
  * @retval HAL status
  */
HAL_StatusTypeDef  HAL_PCCARD_DeInit(PCCARD_HandleTypeDef *hpccard)
{
#if (USE_HAL_PCCARD_REGISTER_CALLBACKS == 1)
  if(hpccard->MspDeInitCallback == NULL)
  {
    hpccard->MspDeInitCallback = HAL_PCCARD_MspDeInit;
  }

  /* DeInit the low level hardware */
  hpccard->MspDeInitCallback(hpccard);
#else
  /* De-Initialize the low level hardware (MSP) */
  HAL_PCCARD_MspDeInit(hpccard);
#endif

  /* Configure the PCCARD registers with their reset values */
  FMC_PCCARD_DeInit(hpccard->Instance);
  
  /* Update the PCCARD controller state */
  hpccard->State = HAL_PCCARD_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hpccard);

  return HAL_OK; 
}

/**
  * @brief  PCCARD MSP Init
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.
  * @retval None
  */
__weak void HAL_PCCARD_MspInit(PCCARD_HandleTypeDef *hpccard)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpccard);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCCARD_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief  PCCARD MSP DeInit
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.
  * @retval None
  */
__weak void HAL_PCCARD_MspDeInit(PCCARD_HandleTypeDef *hpccard)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpccard);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCCARD_MspDeInit could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup PCCARD_Exported_Functions_Group2 Input and Output functions 
  * @brief    Input Output and memory control functions 
  *
  @verbatim    
  ==============================================================================
                    ##### PCCARD Input and Output functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to use and control the PCCARD memory
  
@endverbatim
  * @{
  */
  
/**
  * @brief  Read Compact Flash's ID.
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.
  * @param  CompactFlash_ID Compact flash ID structure.  
  * @param  pStatus pointer to compact flash status         
  * @retval HAL status
  *   
  */ 
HAL_StatusTypeDef HAL_PCCARD_Read_ID(PCCARD_HandleTypeDef *hpccard, uint8_t CompactFlash_ID[], uint8_t *pStatus)
{
  uint32_t timeout = PCCARD_TIMEOUT_READ_ID, index = 0U;
  uint8_t status = 0;
  
  /* Process Locked */
  __HAL_LOCK(hpccard);  
  
  /* Check the PCCARD controller state */
  if(hpccard->State == HAL_PCCARD_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Update the PCCARD controller state */
  hpccard->State = HAL_PCCARD_STATE_BUSY;
  
  /* Initialize the PCCARD status */
  *pStatus = PCCARD_READY;  
  
  /* Send the Identify Command */
  *(__IO uint16_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD)  = 0xECEC;
    
  /* Read PCCARD IDs and timeout treatment */
  do 
  {
     /* Read the PCCARD status */
     status = *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD_ALTERNATE);
     
     timeout--;
  }while((status != PCCARD_STATUS_OK) && timeout); 
  
  if(timeout == 0U)
  {
    *pStatus = PCCARD_TIMEOUT_ERROR;
  }
  else
  {
     /* Read PCCARD ID bytes */
    for(index = 0U; index < 16U; index++)
    {
      CompactFlash_ID[index] = *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_DATA);
    }    
  }
  
  /* Update the PCCARD controller state */
  hpccard->State = HAL_PCCARD_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hpccard);  
  
  return HAL_OK;
}
   
/**
  * @brief  Read sector from PCCARD memory
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer pointer to destination read buffer
  * @param  SectorAddress Sector address to read
  * @param  pStatus pointer to PCCARD status
  * @retval HAL status
  */    
HAL_StatusTypeDef HAL_PCCARD_Read_Sector(PCCARD_HandleTypeDef *hpccard, uint16_t *pBuffer, uint16_t SectorAddress, uint8_t *pStatus)
{
  uint32_t timeout = PCCARD_TIMEOUT_READ_WRITE_SECTOR, index = 0U;
  uint8_t status = 0;

  /* Process Locked */
  __HAL_LOCK(hpccard);
  
  /* Check the PCCARD controller state */
  if(hpccard->State == HAL_PCCARD_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Update the PCCARD controller state */
  hpccard->State = HAL_PCCARD_STATE_BUSY;

  /* Initialize PCCARD status */
  *pStatus = PCCARD_READY;

  /* Set the parameters to write a sector */
  *(__IO uint16_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_CYLINDER_HIGH) = (uint16_t)0x00;
  *(__IO uint16_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_SECTOR_COUNT)  = ((uint16_t)0x0100) | ((uint16_t)SectorAddress);
  *(__IO uint16_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD)    = (uint16_t)0xE4A0;  

  do
  {
    /* wait till the Status = 0x80 */
    status =  *(__IO uint16_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD_ALTERNATE);
    timeout--;
  }while((status == 0x80) && timeout);
  
  if(timeout == 0U)
  {
    *pStatus = PCCARD_TIMEOUT_ERROR;
  }
  
  timeout = PCCARD_TIMEOUT_READ_WRITE_SECTOR;

  do
  {
    /* wait till the Status = PCCARD_STATUS_OK */
    status =  *(__IO uint16_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD_ALTERNATE);
    timeout--;
  }while((status != PCCARD_STATUS_OK) && timeout);
  
  if(timeout == 0U)
  {
    *pStatus = PCCARD_TIMEOUT_ERROR;
  }
  
  /* Read bytes */
  for(; index < PCCARD_SECTOR_SIZE; index++)
  {
    *(uint16_t *)pBuffer++ = *(uint16_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR);
  } 

  /* Update the PCCARD controller state */
  hpccard->State = HAL_PCCARD_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hpccard);
      
  return HAL_OK;
}


/**
  * @brief  Write sector to PCCARD memory
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.
  * @param  pBuffer pointer to source write buffer
  * @param  SectorAddress Sector address to write
  * @param  pStatus pointer to PCCARD status
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCCARD_Write_Sector(PCCARD_HandleTypeDef *hpccard, uint16_t *pBuffer, uint16_t SectorAddress,  uint8_t *pStatus)
{
  uint32_t timeout = PCCARD_TIMEOUT_READ_WRITE_SECTOR, index = 0U;
  uint8_t status = 0;

  /* Process Locked */
  __HAL_LOCK(hpccard);  
  
  /* Check the PCCARD controller state */
  if(hpccard->State == HAL_PCCARD_STATE_BUSY)
  {
     return HAL_BUSY;
  }
   
  /* Update the PCCARD controller state */
  hpccard->State = HAL_PCCARD_STATE_BUSY;
    
  /* Initialize PCCARD status */
  *pStatus = PCCARD_READY;  
    
  /* Set the parameters to write a sector */
  *(__IO uint16_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_CYLINDER_HIGH) = (uint16_t)0x00;
  *(__IO uint16_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_SECTOR_COUNT)  = ((uint16_t)0x0100) | ((uint16_t)SectorAddress);
  *(__IO uint16_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD)    = (uint16_t)0x30A0;
  
  do
  {
    /* Wait till the Status = PCCARD_STATUS_OK */
    status =  *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD_ALTERNATE);
    timeout--;
  }while((status != PCCARD_STATUS_OK) && timeout);
  
  if(timeout == 0U)
  {
    *pStatus = PCCARD_TIMEOUT_ERROR;
  }
  
  /* Write bytes */
  for(; index < PCCARD_SECTOR_SIZE; index++)
  {
    *(uint16_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR) = *(uint16_t *)pBuffer++;
  }

  do
  {
    /* Wait till the Status = PCCARD_STATUS_WRITE_OK */
    status =  *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD_ALTERNATE);
    timeout--;
  }while((status != PCCARD_STATUS_WRITE_OK) && timeout);

  if(timeout == 0U)
  {
    *pStatus = PCCARD_TIMEOUT_ERROR;
  }  

  /* Update the PCCARD controller state */
  hpccard->State = HAL_PCCARD_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hpccard);  
  
  return HAL_OK;
}


/**
  * @brief  Erase sector from PCCARD memory 
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.
  * @param  SectorAddress Sector address to erase
  * @param  pStatus pointer to PCCARD status
  * @retval HAL status
  */
HAL_StatusTypeDef  HAL_PCCARD_Erase_Sector(PCCARD_HandleTypeDef *hpccard, uint16_t SectorAddress, uint8_t *pStatus)
{
  uint32_t timeout = PCCARD_TIMEOUT_ERASE_SECTOR;
  uint8_t status = 0;
  
  /* Process Locked */
  __HAL_LOCK(hpccard);  
  
  /* Check the PCCARD controller state */
  if(hpccard->State == HAL_PCCARD_STATE_BUSY)
  {
     return HAL_BUSY;
  }

  /* Update the PCCARD controller state */
  hpccard->State = HAL_PCCARD_STATE_BUSY;
  
  /* Initialize PCCARD status */ 
  *pStatus = PCCARD_READY;
    
  /* Set the parameters to write a sector */
  *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_CYLINDER_LOW)  = 0x00;
  *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_CYLINDER_HIGH) = 0x00;
  *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_SECTOR_NUMBER) = SectorAddress;
  *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_SECTOR_COUNT)  = 0x01;
  *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_CARD_HEAD)     = 0xA0;
  *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD)    = ATA_ERASE_SECTOR_CMD;
  
  /* wait till the PCCARD is ready */
  status =  *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD_ALTERNATE);
  
  while((status != PCCARD_STATUS_WRITE_OK) && timeout)
  {
    status =  *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD_ALTERNATE);
    timeout--;
  } 
  
  if(timeout == 0U)
  {
    *pStatus = PCCARD_TIMEOUT_ERROR;
  }
  
  /* Check the PCCARD controller state */
  hpccard->State = HAL_PCCARD_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hpccard);   
  
  return HAL_OK;
}

/**
  * @brief  Reset the PCCARD memory 
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCCARD_Reset(PCCARD_HandleTypeDef *hpccard)
{
  /* Process Locked */
  __HAL_LOCK(hpccard);  
  
  /* Check the PCCARD controller state */
  if(hpccard->State == HAL_PCCARD_STATE_BUSY)
  {
     return HAL_BUSY;
  }

  /* Provide a SW reset and Read and verify the:
   - PCCard Configuration Option Register at address 0x98000200 --> 0x80
   - Card Configuration and Status Register at address 0x98000202 --> 0x00
   - Pin Replacement Register at address 0x98000204 --> 0x0C
   - Socket and Copy Register at address 0x98000206 --> 0x00
  */

  /* Check the PCCARD controller state */
  hpccard->State = HAL_PCCARD_STATE_BUSY;
  
  *(__IO uint8_t *)(PCCARD_ATTRIBUTE_SPACE_ADDRESS | ATA_CARD_CONFIGURATION ) = 0x01;
    
  /* Check the PCCARD controller state */
  hpccard->State = HAL_PCCARD_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hpccard);  
  
  return HAL_OK;
}

/**
  * @brief  This function handles PCCARD device interrupt request.
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.
  * @retval HAL status
*/
void HAL_PCCARD_IRQHandler(PCCARD_HandleTypeDef *hpccard)
{
  /* Check PCCARD interrupt Rising edge flag */
  if(__FMC_PCCARD_GET_FLAG(hpccard->Instance, FMC_FLAG_RISING_EDGE))
  {
    /* PCCARD interrupt callback*/
#if (USE_HAL_PCCARD_REGISTER_CALLBACKS == 1)
    hpccard->ItCallback(hpccard);
#else
    HAL_PCCARD_ITCallback(hpccard);
#endif

    /* Clear PCCARD interrupt Rising edge pending bit */
    __FMC_PCCARD_CLEAR_FLAG(hpccard->Instance, FMC_FLAG_RISING_EDGE);
  }
  
  /* Check PCCARD interrupt Level flag */
  if(__FMC_PCCARD_GET_FLAG(hpccard->Instance, FMC_FLAG_LEVEL))
  {
    /* PCCARD interrupt callback*/
#if (USE_HAL_PCCARD_REGISTER_CALLBACKS == 1)
    hpccard->ItCallback(hpccard);
#else
    HAL_PCCARD_ITCallback(hpccard);
#endif

    /* Clear PCCARD interrupt Level pending bit */
    __FMC_PCCARD_CLEAR_FLAG(hpccard->Instance, FMC_FLAG_LEVEL);
  }

  /* Check PCCARD interrupt Falling edge flag */
  if(__FMC_PCCARD_GET_FLAG(hpccard->Instance, FMC_FLAG_FALLING_EDGE))
  {
    /* PCCARD interrupt callback*/
#if (USE_HAL_PCCARD_REGISTER_CALLBACKS == 1)
    hpccard->ItCallback(hpccard);
#else
    HAL_PCCARD_ITCallback(hpccard);
#endif

    /* Clear PCCARD interrupt Falling edge pending bit */
    __FMC_PCCARD_CLEAR_FLAG(hpccard->Instance, FMC_FLAG_FALLING_EDGE);
  }
  
  /* Check PCCARD interrupt FIFO empty flag */
  if(__FMC_PCCARD_GET_FLAG(hpccard->Instance, FMC_FLAG_FEMPT))
  {
    /* PCCARD interrupt callback*/
#if (USE_HAL_PCCARD_REGISTER_CALLBACKS == 1)
    hpccard->ItCallback(hpccard);
#else
    HAL_PCCARD_ITCallback(hpccard);
#endif

    /* Clear PCCARD interrupt FIFO empty pending bit */
    __FMC_PCCARD_CLEAR_FLAG(hpccard->Instance, FMC_FLAG_FEMPT);
  }  
}

/**
  * @brief  PCCARD interrupt feature callback
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.
  * @retval None
  */
__weak void HAL_PCCARD_ITCallback(PCCARD_HandleTypeDef *hpccard)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpccard);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCCARD_ITCallback could be implemented in the user file
   */
}

#if (USE_HAL_PCCARD_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a User PCCARD Callback
  *         To be used instead of the weak (surcharged) predefined callback
  * @param hpccard : PCCARD handle
  * @param CallbackId : ID of the callback to be registered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_PCCARD_MSP_INIT_CB_ID       PCCARD MspInit callback ID
  *          @arg @ref HAL_PCCARD_MSP_DEINIT_CB_ID     PCCARD MspDeInit callback ID
  *          @arg @ref HAL_PCCARD_IT_CB_ID             PCCARD IT callback ID
  * @param pCallback : pointer to the Callback function
  * @retval status
  */
HAL_StatusTypeDef HAL_PCCARD_RegisterCallback (PCCARD_HandleTypeDef *hpccard, HAL_PCCARD_CallbackIDTypeDef CallbackId, pPCCARD_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if(pCallback == NULL)
  {
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hpccard);

  if(hpccard->State == HAL_PCCARD_STATE_READY)
  {
    switch (CallbackId)
    {
    case HAL_PCCARD_MSP_INIT_CB_ID :
      hpccard->MspInitCallback = pCallback;
      break;
    case HAL_PCCARD_MSP_DEINIT_CB_ID :
      hpccard->MspDeInitCallback = pCallback;
      break;
    case HAL_PCCARD_IT_CB_ID :
      hpccard->ItCallback = pCallback;
      break;
    default :
      /* update return status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if(hpccard->State == HAL_PCCARD_STATE_RESET)
  {
    switch (CallbackId)
    {
    case HAL_PCCARD_MSP_INIT_CB_ID :
      hpccard->MspInitCallback = pCallback;
      break;
    case HAL_PCCARD_MSP_DEINIT_CB_ID :
      hpccard->MspDeInitCallback = pCallback;
      break;
    default :
      /* update return status */
      status =  HAL_ERROR;
      break;
    }
  }
  else
  {
    /* update return status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hpccard);
  return status;
}

/**
  * @brief  Unregister a User PCCARD Callback
  *         PCCARD Callback is redirected to the weak (surcharged) predefined callback
  * @param hpccard : PCCARD handle
  * @param CallbackId : ID of the callback to be unregistered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_PCCARD_MSP_INIT_CB_ID       PCCARD MspInit callback ID
  *          @arg @ref HAL_PCCARD_MSP_DEINIT_CB_ID     PCCARD MspDeInit callback ID
  *          @arg @ref HAL_PCCARD_IT_CB_ID             PCCARD IT callback ID
  * @retval status
  */
HAL_StatusTypeDef HAL_PCCARD_UnRegisterCallback (PCCARD_HandleTypeDef *hpccard, HAL_PCCARD_CallbackIDTypeDef CallbackId)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Process locked */
  __HAL_LOCK(hpccard);

  if(hpccard->State == HAL_PCCARD_STATE_READY)
  {
    switch (CallbackId)
    {
    case HAL_PCCARD_MSP_INIT_CB_ID :
      hpccard->MspInitCallback = HAL_PCCARD_MspInit;
      break;
    case HAL_PCCARD_MSP_DEINIT_CB_ID :
      hpccard->MspDeInitCallback = HAL_PCCARD_MspDeInit;
      break;
    case HAL_PCCARD_IT_CB_ID :
      hpccard->ItCallback = HAL_PCCARD_ITCallback;
      break;
    default :
      /* update return status */
      status =  HAL_ERROR;
      break;
    }
  }
  else if(hpccard->State == HAL_PCCARD_STATE_RESET)
  {
    switch (CallbackId)
    {
    case HAL_PCCARD_MSP_INIT_CB_ID :
      hpccard->MspInitCallback = HAL_PCCARD_MspInit;
      break;
    case HAL_PCCARD_MSP_DEINIT_CB_ID :
      hpccard->MspDeInitCallback = HAL_PCCARD_MspDeInit;
      break;
    default :
      /* update return status */
      status =  HAL_ERROR;
      break;
    }
  }
  else
  {
    /* update return status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hpccard);
  return status;
}
#endif

/**
  * @}
  */

/** @defgroup PCCARD_Exported_Functions_Group3 State functions 
 *  @brief   Peripheral State functions 
 *
@verbatim   
  ==============================================================================
                      ##### PCCARD State functions #####
  ==============================================================================  
  [..]
    This subsection permits to get in run-time the status of the PCCARD controller 
    and the data flow.

@endverbatim
  * @{
  */ 
  
/**
  * @brief  return the PCCARD controller state
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.
  * @retval HAL state
  */
HAL_PCCARD_StateTypeDef HAL_PCCARD_GetState(PCCARD_HandleTypeDef *hpccard)
{
  return hpccard->State;
}  
 
/**
  * @brief  Get the compact flash memory status
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.       
  * @retval New status of the PCCARD operation. This parameter can be:
  *          - CompactFlash_TIMEOUT_ERROR: when the previous operation generate 
  *            a Timeout error
  *          - CompactFlash_READY: when memory is ready for the next operation          
  */
HAL_PCCARD_StatusTypeDef HAL_PCCARD_GetStatus(PCCARD_HandleTypeDef *hpccard)
{
  uint32_t timeout = PCCARD_TIMEOUT_STATUS, status_pccard = 0U;  
  
  /* Check the PCCARD controller state */
  if(hpccard->State == HAL_PCCARD_STATE_BUSY)
  {
     return HAL_PCCARD_STATUS_ONGOING;
  }

  status_pccard =  *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD_ALTERNATE);
  
  while((status_pccard == PCCARD_BUSY) && timeout)
  {
    status_pccard =  *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD_ALTERNATE);
    timeout--;
  }

  if(timeout == 0U)
  {          
    status_pccard =  PCCARD_TIMEOUT_ERROR;      
  }   

  /* Return the operation status */
  return (HAL_PCCARD_StatusTypeDef) status_pccard;      
}
  
/**
  * @brief  Reads the Compact Flash memory status using the Read status command
  * @param  hpccard pointer to a PCCARD_HandleTypeDef structure that contains
  *                the configuration information for PCCARD module.      
  * @retval The status of the Compact Flash memory. This parameter can be:
  *          - CompactFlash_BUSY: when memory is busy
  *          - CompactFlash_READY: when memory is ready for the next operation    
  *          - CompactFlash_ERROR: when the previous operation generates error                
  */
HAL_PCCARD_StatusTypeDef HAL_PCCARD_ReadStatus(PCCARD_HandleTypeDef *hpccard)
{
  uint8_t data = 0U, status_pccard = PCCARD_BUSY;
  
  /* Check the PCCARD controller state */
  if(hpccard->State == HAL_PCCARD_STATE_BUSY)
  {
     return HAL_PCCARD_STATUS_ONGOING;
  } 

  /* Read status operation */
  data =  *(__IO uint8_t *)(PCCARD_IO_SPACE_PRIMARY_ADDR | ATA_STATUS_CMD_ALTERNATE);

  if((data & PCCARD_TIMEOUT_ERROR) == PCCARD_TIMEOUT_ERROR)
  {
    status_pccard = PCCARD_TIMEOUT_ERROR;
  } 
  else if((data & PCCARD_READY) == PCCARD_READY)
  {
    status_pccard = PCCARD_READY;
  }
  
  return (HAL_PCCARD_StatusTypeDef) status_pccard;
}  
 
/**
  * @}
  */
  
/**
  * @}
  */
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx ||\
          STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
#endif /* HAL_PCCARD_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */



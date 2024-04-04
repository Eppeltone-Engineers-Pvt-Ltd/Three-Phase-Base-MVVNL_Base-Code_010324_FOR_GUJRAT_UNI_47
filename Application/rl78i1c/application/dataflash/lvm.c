/****************************************************************************** 
* File Name    : lvm.c
* Version      : 1.00
* Device(s)    : RL78
* Tool-Chain   : CubeSuite Version 1.5
* H/W Platform : Unified Energy Meter Platform
* Description  : Logical Volume Manager APIs
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* MD Macro Driver */

/* Code Standard */
#include "typedef.h"            /* GSCE Standard Typedef */

/* Application */
#include "lvm.h"           /* Logical Volume Manager */

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define LVM_BANK1_START_ADDR                    (PDEV_DEVICE_START_ADDR + PDEV_LVM_FLG_SIZE)
#define LVM_BANK2_START_ADDR                    (PDEV_DEVICE_START_ADDR +  PDEV_BANK_SIZE + PDEV_LVM_FLG_SIZE)

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Sub-functions (internal used for module) */
static uint8_t  LVM_Flag_Restore(lvm_flag_t *p_flg);
static uint8_t  LVM_Flag_Backup(lvm_flag_t flg);
static uint8_t  LVM_Format(void);                                       /* Logical Volume Manager Format, use internal only */
static uint8_t  LVM_Bank_Copy(uint8_t dest_bank, uint8_t src_bank);     /* Logical Volume Manager Bank Copy*/
static uint8_t  LVM_BankCheck(void);                                    /* Logical Volume Manager Startup Check */

/******************************************************************************
* Function Name: LVM_Flag_Restore()
* Description  : LVM Flag Restore
* Arguments    : lvm_flag_t *p_flg
* Return Value : Execution status
*              :    PDEV_OK             Normal end
*              :    PDEV_ERROR          Error
******************************************************************************/
static uint8_t  LVM_Flag_Restore(lvm_flag_t *p_flg)
{
    if(p_flg == NULL)
    {
        return PDEV_ERROR;  /* Data error */
    }
    if (PDEV_READ(PDEV_LVM_FLG_ADDR, (uint8_t*) p_flg, PDEV_LVM_FLG_SIZE) != PDEV_OK)
    {
        return PDEV_ERROR;  /* Data error */
    }

    return LDEV_OK;
}

/******************************************************************************
* Function Name: LVM_Flag_Backup()
* Description  : LVM Flag Backup
* Arguments    : lvm_flag_t flg
* Return Value : uint8_t, execution status
*              :    PDEV_OK             Normal end
*              :    PDEV_ERROR          Error
******************************************************************************/
static uint8_t  LVM_Flag_Backup(lvm_flag_t flg)
{
    if (PDEV_WRITE(PDEV_LVM_FLG_ADDR, (uint8_t*)&flg, PDEV_LVM_FLG_SIZE) != PDEV_OK)
    {
        return PDEV_ERROR;  /* Data error */
    }

    return LDEV_OK;
}

/******************************************************************************
* Function Name: LVM_Bank_Copy()
* Description  : Copy whole bank dataf from src_bank_addr to des_bank_addr
* Arguments    : uint32_t src_bank_addr, 
*              : uint32_t des_bank_addr,
*              : uint16_t size
* Return Value : uint8_t, execution status
******************************************************************************/
static uint8_t LVM_Bank_Copy(uint8_t dest_bank, uint8_t src_bank)
{
    /* Procedure,  clear the dest bank 
     * Direct write to dest bank    
    */
    uint8_t page_buffer[PDEV_DEVICE_BANK_SIZE];
    uint32_t src_addr = src_bank * PDEV_DEVICE_BANK_SIZE;
    
    /* Read from whole src block */
    if (PDEV_READ(src_addr, page_buffer, PDEV_DEVICE_BANK_SIZE) != PDEV_OK)
    {
        return LDEV_ERROR_NO_RESPOND;   /* Data error */
    }
    
    /* Write to whole dest block */
    if (PDEV_WRITEBANK(dest_bank, page_buffer) != PDEV_OK)
    {
        return LDEV_ERROR_NO_RESPOND;   /* Data error */
    }
    
    return LDEV_OK;
}

/******************************************************************************
* Function Name: void LVM_Init();
* Description  : LVM Initialization
* Arguments    : None
* Return Value : uint8_t, execution status
******************************************************************************/
uint8_t LVM_Init(void)
{   
    /* Init Physical volume */
    if (PDEV_INIT() != PDEV_OK)
    {
        return LDEV_ERROR;
    }

    /* Checking before start */
    if (LVM_BankCheck() != PDEV_OK)
    {
        return LDEV_ERROR;
    }

    return LDEV_OK;
}

/******************************************************************************
* Function Name: uint8_t LVM_Read(uint32_t addr, uint8_t* buf, uint16_t size)
* Description  : Read from logical volume
*              : NOTE: logical volume is split into 2 bank
* Arguments    : addr: Local address in logical volume
*              : buf : Buffer to store the result
*              : size: Expected size want to read
* Return Value : Execution status
*              :    LDEV_OK             Normal end
*              :    LDEV_ERROR_NO_RESPOND       Device does not respond
*              :    LDEV_ERROR          Expected size and address are not suitable
******************************************************************************/
uint8_t LVM_Read(uint32_t addr, uint8_t* buf, uint16_t size)
{
    uint32_t local_addr;

    /* Check the Bank status */
    LVM_BankCheck();

    /* After checking, state become normal */
    local_addr = addr + LVM_BANK1_START_ADDR;

    /* Now check the address valid range */
    if ((local_addr + size) > PDEV_DEVICE_BANK_SIZE)
    {
        return LDEV_ERROR_SIZE;
    }
    
    /* Start reading */
    if (PDEV_READ(local_addr, buf, size) != PDEV_OK)
    {
        return LDEV_ERROR_NO_RESPOND;   /* Data error */
    }

    return LDEV_OK; /* Read successful */
}

/******************************************************************************
* Function Name: uint8_t LVM_Write(uint32_t addr, uint8_t* buf, uint16_t size)
* Description  : Write to logical volume
*              : NOTE: logical volume is split into 2 bank
* Arguments    : addr: Local address in logical volume
*              : buf : Buffer to write to logical volume
*              : size: Expected size (in buf) want to write
* Return Value : Execution status
*              :    LDEV_OK             Normal end
*              :    LDEV_ERROR_NO_RESPOND       Device does not respond
*              :    LDEV_ERROR          Expected size and address are not suitable
******************************************************************************/
uint8_t LVM_Write(uint32_t addr, uint8_t* buf, uint16_t size)
{
    lvm_flag_t flag;
    uint32_t local_addr;

    /* Check the Bank status */
    LVM_BankCheck();

    /* After checking, state become normal */
    /* Get the flag */
    if (LVM_Flag_Restore(&flag) != PDEV_OK)
    {
        return LDEV_ERROR;
    }
    
    /* Start for write session */
    flag.is_bank1_in_use = 1;
    flag.is_updating = 1;

    if (LVM_Flag_Backup(flag) != PDEV_OK)
    {
        return LDEV_ERROR_NO_RESPOND;   /* Data error */
    }

    /* Write data to first bank */
    local_addr = addr + LVM_BANK1_START_ADDR;
    
    /* Now check the address valid range */
    if ((local_addr + size) > PDEV_DEVICE_BANK_SIZE)
    {
        return LDEV_ERROR_SIZE;
    }
    
    /* Start writing */
    if (PDEV_WRITE(local_addr, buf, size) != PDEV_OK)
    {
        return LDEV_ERROR_NO_RESPOND;   /* Data error */
    }

    /* ACK: Write data to first bank DONE - Start to write to 2nd bank */
    flag.is_bank1_in_use = 0;
    flag.is_bank2_in_use = 1;

    if (LVM_Flag_Backup(flag) != PDEV_OK)
    {
        return LDEV_ERROR_NO_RESPOND;   /* Data error */
    }

    /* Start for copy whole bank 2 to bank 1 */
    if (LVM_Bank_Copy(PDEV_BANK2, PDEV_BANK1) != PDEV_OK)
    {
        return LDEV_ERROR_NO_RESPOND;   /* Data error */
    }

    /* ACK: Done */
    flag.is_bank2_in_use = 0;
    flag.is_updating = 0;
    if (LVM_Flag_Backup(flag) != PDEV_OK)
    {
        return LDEV_ERROR_NO_RESPOND;   /* Data error */
    }
    
    /* Start for copy whole bank 2 to bank 1 */
    if (LVM_Bank_Copy(PDEV_BANK2, PDEV_BANK1) != PDEV_OK)
    {
        return LDEV_ERROR_NO_RESPOND;   /* Data error */
    }

    return LDEV_OK; /* Write successful */
    
}

/******************************************************************************
* Function Name: uint8_t LVM_Format(void)
* Description  : Format storage module & reset LVM
* Arguments    : None
* Return Value : Execution status
*              :    LDEV_OK                     Normal end
*              :    LDEV_ERROR_NO_RESPOND       Device does not respond
*              :    LDEV_ERROR                  Expected size and address are not suitable
******************************************************************************/
uint8_t LVM_Format(void)
{
    lvm_flag_t flag = { 0, 0, 0, 0, PDEV_CODE_CHECK};
    
    if (PDEV_FORMAT(DATAFLASH_BLOCK1) != PDEV_OK)
    {
        return LDEV_ERROR_NO_RESPOND;   /* Data error */
    }
    
    if (PDEV_FORMAT(DATAFLASH_BLOCK2) != PDEV_OK)
    {
        return LDEV_ERROR_NO_RESPOND;   /* Data error */
    }
    
    if (LVM_Flag_Backup(flag) != PDEV_OK)
    {
        return LDEV_ERROR_NO_RESPOND;   /* Data error */
    }

    return LDEV_OK; /* Successful */

}

/******************************************************************************
* Function Name: static uint8_t LVM_BankCheck(void)
* Description  : Polling processing for storage module
* Arguments    : None
* Return Value : None
******************************************************************************/
static uint8_t LVM_BankCheck(void)
{
    lvm_flag_t flag;
    uint32_t local_addr;

    /* Get the flag */
    if (LVM_Flag_Restore(&flag) != PDEV_OK)
    {
        return LDEV_ERROR;
    }
    
    /* Previous not updating, no bank is in use --> no error */
    if (flag.is_bank1_in_use == 0 && flag.is_bank2_in_use == 0)
    {
        return LDEV_OK; // OK
    }

    /* Both bank in use */
    if (((flag.is_bank1_in_use == 1) && (flag.is_bank2_in_use == 1)))
    {
        /* Bank1 flag may failed, copy from bank2 to try again */
        /* Start for copy whole bank 2 to bank 1 */
        if (LVM_Bank_Copy(PDEV_BANK1, PDEV_BANK2) != PDEV_OK)
        {
            return LDEV_ERROR;  /* Data error */
        }
        
        /* Get the flag again */
        if (LVM_Flag_Restore(&flag) != PDEV_OK)
        {
            return LDEV_ERROR;
        }
        
        /* Data on bank2 ok? */
        if (flag.is_bank1_in_use == 0 && flag.is_bank2_in_use == 0)
        {
            return LDEV_OK; // OK
        }    
        
        /* Still error, then both bank corrupted, format device */
        if (((flag.is_bank1_in_use == 1) && (flag.is_bank2_in_use == 1)) ||
            ((flag.is_bank1_in_use == 0) && (flag.is_bank2_in_use == 1)))
        {
            if (LVM_Format() != PDEV_OK)
            {
                return LDEV_ERROR;  /* Data error */
            }
            /* Return: storage blank */
            return LDEV_OK;
        }
    }
    
    /* Check which bank has problem */
    if (flag.is_bank1_in_use == 1)
    {
        /* Upto here, of course, if is_bank1_in_use==1 --> is_bank2_in_use==0 */
        
        /* Start for copy whole bank 2 to bank 1 */
        if (LVM_Bank_Copy(PDEV_BANK1, PDEV_BANK2) != PDEV_OK)
        {
            return LDEV_ERROR;  /* Data error */
        }
        
        /* ACK: Update Bank 1 : Done */
        flag.is_bank1_in_use = 0;

    }
    else
    {
        /* Upto here, of course, if is_bank1_in_use==0 --> is_bank2_in_use==1 */

        /* Start for copy whole bank 1 to bank 2 */
        if (LVM_Bank_Copy(PDEV_BANK2, PDEV_BANK1) != PDEV_OK)
        {
            return LDEV_ERROR; /* Data error */
        }
        
        /* ACK: Update Bank 1 : Done */
        flag.is_bank2_in_use = 0;
    }
    
    /* Backup the flag */
    flag.is_updating = 0;
    if (LVM_Flag_Backup(flag) != PDEV_OK)
    {
        return LDEV_ERROR;  /* Data error */
    }    
    
    return LDEV_OK;
}
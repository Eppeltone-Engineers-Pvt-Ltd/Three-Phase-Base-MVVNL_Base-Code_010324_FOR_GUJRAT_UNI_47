/***********************************************************************************************************************
* File Name    : key.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : KEY processing source File
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* CG Macro Driver */

/* MW/Core */
#include "em_core.h"        /* EM Core APIs */

/* Application */
#include "key.h"            /* KEY Interface Header File */
#include "debug.h"          /* DEBUG Interface Header File */
//#include "em_display.h"     /* LCD Display Header File */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
// Key flag 
/*
typedef struct tagEventFlag
{
    uint8_t is_key_down_pressed  :1;    // Key down flag 
    uint8_t is_key_mid_pressed   :1;    // Key mid flag 
    uint8_t is_key_up_pressed    :1;    //Key up flag 
    uint8_t reserved             :5;    // (NO USE) Reserved 
    
} KEY_FLAG;
*/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/


//#define     KEY_UP      (P15.2)

/*
static KEY_FLAG g_key_flag = {0, 0, 0};

void KEY_DownPressed(void)
{
    g_key_flag.is_key_down_pressed = 1;
}

void KEY_MidPressed(void)
{
    g_key_flag.is_key_mid_pressed = 1;
}

void KEY_UpPressed(void)
{
    g_key_flag.is_key_up_pressed = 1;
}
*/

/*
void KEY_PollingProcessing(void)
{
	

    // Detection on port here
    #if 0
    static int8_t key_down_last = (!KEY_PRESSED);
    static int8_t key_mid_last = (!KEY_PRESSED);
    static int8_t key_up_last = (!KEY_PRESSED);
    
    
    if (KEY_DOWN == (!KEY_PRESSED) &&
        key_down_last == KEY_PRESSED)
    {
        KEY_DownPressed();
    }
    key_down_last = KEY_DOWN;   
    
    if (KEY_MID == (!KEY_PRESSED) &&
        key_mid_last == KEY_PRESSED)
    {
        KEY_MidPressed();
    }
    key_mid_last = KEY_MID;
    
    if (KEY_UP == (!KEY_PRESSED) &&
        key_up_last == KEY_PRESSED)
    {
        KEY_UpPressed();
    }
    key_up_last = KEY_UP;
    #endif
    
    // When key DOWN is pressed 
    if (g_key_flag.is_key_down_pressed == 1)
    {
        //DEBUG_Printf((uint8_t *)"\n\rKEY: Key DOWN pressed! Switch the auto scroll\n\r");
        DEBUG_AckNewLine();
        
        // Switch the auto scroll of LCD 
        LCD_SwitchAutoRoll();

        // ACK 
        g_key_flag.is_key_down_pressed = 0;
    }
    
    // When key MID is pressed 
    if (g_key_flag.is_key_mid_pressed == 1)
    {
        //DEBUG_Printf((uint8_t *)"\n\rKEY: Key MID pressed! Do nothing\n\r");
        DEBUG_AckNewLine();
        
        // ACK
        g_key_flag.is_key_mid_pressed = 0;
    }
    
    // When key UP is pressed
    if (g_key_flag.is_key_up_pressed == 1)
    {
//        //DEBUG_Printf((uint8_t *)"\n\rKEY: Key Up pressed! Change to the next display\n\r");
        DEBUG_AckNewLine();
        
        // Change to the next display item of LCD 
        LCD_ChangeNext();
        
        // ACK 
        g_key_flag.is_key_up_pressed = 0;
    }
		
}
*/



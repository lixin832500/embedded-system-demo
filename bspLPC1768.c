/* Copyright 2017 Ali Haidous
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


/* -- COMPILER DIRECTIVES -- */


/* -- INCLUDES -- */
#include "LPC17xx.H"
#include "bspDataTypes.h"
#include "bspHardwareAbstractionLayer.h"
#include "GLCD.h"

/* -- DEFINES and ENUMS -- */
/* External Interrupt 0 defines */
#define PINSEL_EINT0    20
#define SBIT_EINT0      0
#define SBIT_EXTMODE0   0
#define SBIT_EXTPOLAR0  0

 #define SET_PIN_1_25   (1UL<<25)
 #define SET_PIN_1_26   (1UL<<26)
 #define SET_PIN_1_27   (1UL<<27)
 #define SET_PIN_1_28   (1UL<<28)
 #define SET_PIN_1_29   (1UL<<29)

 #define JS_SEL         (1UL<<0)
 #define JS_DOWN        (1UL<<1)
 #define JS_LEFT        (1UL<<2)
 #define JS_RIGHT       (1UL<<3)
 #define JS_UP          (1UL<<4)

/* -- TYPEDEFS and STRUCTURES -- */


/* -- STATIC AND GLOBAL VARIABLES -- */
QWORD gqw10msTicks;

pfnEventCallback pfnGrenadeEvent;
pfnEventCallback pfnUFORightCommandEvent;
pfnEventCallback pfnUFOLeftCommandEvent;
pfnEventCallback pfnRefreshLCDEvent;

/* -- STATIC FUNCTION PROTOTYPES -- */
/* Display functions */
static void scDisplaySetup (void);

/* LPC1768 specific interrupt routines */


/*----------------------------------------------------------------------------
  EINT0 IRQ: Executed on Grenade Event
 *----------------------------------------------------------------------------*/
void EINT0_IRQHandler(void)  // SW2 Interrupt
{
    LPC_SC->EXTINT = (1<<SBIT_EINT0);  /* Clear Interrupt Flag */

    /* Call Callback Grenade Event*/
    if (pfnGrenadeEvent != NULL_PTR)
    {
        pfnGrenadeEvent();
    }
}


void TIMER0_IRQHandler(void)
{
  if ( (LPC_TIM0->IR & 0x01) == 0x01 ) // if MR0 interrupt (this is a sanity check);
  {
    LPC_TIM0->IR |= 1 << 0; // Clear MR0 interrupt flag

        /* Call Callback Refresh LCD Event*/
    if (pfnRefreshLCDEvent != NULL_PTR)
    {
        pfnRefreshLCDEvent();
    }
  }
}


/*----------------------------------------------------------------------------
  SysTick IRQ: Executed periodically every 10ms
 *----------------------------------------------------------------------------*/
void SysTick_Handler (void) // SysTick Interrupt Handler (10ms);
{
    gqw10msTicks++;
}
/* End LPC1768 specific interrupt routines */


/*----------------------------------------------------------------------------

    @Prototype: BOOL fnfHALSetup (void)

    @Description: Hardware setup routine for abstraction.

    @Parameters: void

    @Returns: BOOL Exit code flag
                        TRUE - Success
                        FALSE - Failure

    @Revision History:
        DATE             NAME               REVISION COMMENT
        03/04/2017       Ali Haidous        Initial Revision

 *----------------------------------------------------------------------------*/
BOOL fHALSetup (void)
{
    BOOL fExitCode = TRUE;

    // Initialize LPC1768 system
    SystemInit();
    scDisplaySetup();

    // Timer 0 configuration
    LPC_SC->PCONP |= 1 << 1; // Power up Timer 0
    LPC_SC->PCLKSEL0 |= 1 << 2; // Clock for timer = CCLK, i.e., CPU Clock
    LPC_TIM0->MR0 = 1 << 23; // give a value suitable for the refresh rate
                                                   // frequency based on the clock frequency
    LPC_TIM0->MCR |= 1 << 0; // Interrupt on Match 0 compare
    LPC_TIM0->MCR |= 1 << 1; // Reset timer on Match 0
    LPC_TIM0->TCR |= 1 << 1; // Manually Reset Timer 0 (forced)
    LPC_TIM0->TCR &= ~(1 << 1); // Stop resetting the timer

    NVIC_EnableIRQ(TIMER0_IRQn);

    LPC_TIM0->TCR |= 1 << 0; // Start timer

    // Configure SW2 as interrupt
    LPC_SC->EXTINT      = (1<<SBIT_EINT0);       /* Clear Pending interrupts EINT0 */
    LPC_PINCON->PINSEL4 = (1<<PINSEL_EINT0);   /* Configure P2_10 as EINT0 */
    LPC_SC->EXTMODE     = (1<<SBIT_EXTMODE0);  /* Configure EINT0 as Edge Triggered*/
    LPC_SC->EXTPOLAR    = (1<<SBIT_EXTPOLAR0); /* Configure EINT0 as Falling Edge */

    /* Configure joystick */
    /* Configure P 1.25 ... P 1.29 as input */
    LPC_GPIO1->FIODIR &= ~(SET_PIN_1_25 |
                           SET_PIN_1_26 |
                           SET_PIN_1_27 |
                           SET_PIN_1_28 |
                           SET_PIN_1_29);

    /* Enable the ENT1 and EINT0 interrupts */
    NVIC_EnableIRQ(EINT0_IRQn);

    /* Generate interrupt each 10 ms */
    SysTick_Config(SystemCoreClock/100);

    gqw10msTicks = 0;

    pfnGrenadeEvent = NULL_PTR;
    pfnUFORightCommandEvent = NULL_PTR;
    pfnUFOLeftCommandEvent = NULL_PTR;
    pfnRefreshLCDEvent = NULL_PTR;

    return fExitCode;
}

/*----------------------------------------------------------------------------

    @Prototype: BOOL fPollJoyStick(void)

    @Description: Poll joystick

    @Parameters: void

    @Returns: BOOL - Was the joystick triggered?

    @Revision History:
        DATE             NAME               REVISION COMMENT
        04/08/2017       Ali Haidous        Initial Revision

 *----------------------------------------------------------------------------*/
BOOL fPollJoyStick(void)
{
    BOOL fRet = FALSE;
    BYTE byJSval = (~((LPC_GPIO1->FIOPIN>>25)&0x1F))&0x1F;

    /* Joystick select was pressed */
    if( byJSval == JS_SEL )
    {
        /* Call Callback Grenade Event*/
        if (pfnGrenadeEvent != NULL_PTR)
        {
            pfnGrenadeEvent();
        }
        fRet = TRUE;
    }
    /* Joystick down was pressed */
    if( byJSval == JS_DOWN )
    {
        if (pfnUFORightCommandEvent != NULL_PTR)
        {
            pfnUFORightCommandEvent();
        }
        fRet = TRUE;
    }
    /* Joystick left was pressed */
    if( byJSval == JS_LEFT )
    {
        if (pfnUFOLeftCommandEvent != NULL_PTR)
        {
            pfnUFOLeftCommandEvent();
        }
        fRet = TRUE;
    }
    /* Joystick right was pressed */
    if( byJSval == JS_RIGHT )
    {
        if (pfnUFORightCommandEvent != NULL_PTR)
        {
            pfnUFORightCommandEvent();
        }
        fRet = TRUE;
    }
    /* Joystick up was pressed */
    if( byJSval == JS_UP )
    {
        if (pfnUFOLeftCommandEvent != NULL_PTR)
        {
            pfnUFOLeftCommandEvent();
        }
        fRet = TRUE;
    }

    return fRet;
}


/*----------------------------------------------------------------------------

    @Prototype: static void scDisplaySetup (void)

    @Description: Display setup routine

    @Parameters: void

    @Returns: void

    @Revision History:
        DATE             NAME               REVISION COMMENT
        04/08/2017       Ali Haidous        Initial Revision

 *----------------------------------------------------------------------------*/
static void scDisplaySetup (void)
{
    LCD_Initialization(); // Initialize graphical LCD
    LCD_Clear(Black);  // Clear graphical LCD display
}


/*----------------------------------------------------------------------------

    @Prototype: void SetPoint(WORD wX, WORD wY, BYTE byColor)

    @Description: Display a point on the display

    @Parameters: void

    @Returns: void

    @Revision History:
        DATE             NAME               REVISION COMMENT
        04/08/2017       Ali Haidous        Initial Revision

 *----------------------------------------------------------------------------*/
void SetPoint(WORD wX, WORD wY, WORD wColor)
{
    LCD_SetPoint(wX, wY, wColor);
}


/*----------------------------------------------------------------------------

    @Prototype: void ClearLCD(void)

    @Description: Clear LCD display

    @Parameters: void

    @Returns: void

    @Revision History:
        DATE             NAME               REVISION COMMENT
        04/08/2017       Ali Haidous        Initial Revision

 *----------------------------------------------------------------------------*/
void ClearLCD(void)
{
    LCD_Clear(Black);  // Clear graphical LCD display
}

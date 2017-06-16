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
#include "bspHardwareAbstractionLayer.h"
#include "bspDataTypes.h"
#include "apUFO.h"

/* -- DEFINES and ENUMS -- */


/* -- TYPEDEFS and STRUCTURES -- */


/* -- STATIC AND GLOBAL VARIABLES -- */
static WORD scwUFOx;
static WORD scwUFOy;
static WORD scwGrenade1x;
static WORD scwGrenade1y;
static WORD scwGrenade2x;
static WORD scwGrenade2y;
static WORD scwGrenade3x;
static WORD scwGrenade3y;
static BOOL scfGrenade1Ready;
static BOOL scfGrenade2Ready;
static BOOL scfGrenade3Ready;

/* -- STATIC FUNCTION PROTOTYPES -- */

/* Callback registration function that registers functions that are called on UFO_EVENT_E events in hardware */
static BOOL scfRegisterCallback (const pfnEventCallback pfnCallback, const UFO_EVENT_E keEvent);
static void scDisplayUFO (WORD wXPos, WORD wYPos);
static void scDisplayGrenade (WORD wXPos, WORD wYPos);

static void scRefreshLCDCallback(void);
static void scGrenadeCallback(void);
static void scUFOLeftCallback(void);
static void scUFORightCallback(void);


/*----------------------------------------------------------------------------

    @Prototype: void InitUFOApp (void)

    @Description: Initialize Intelligent Human Flow Monitor Application
                  main entry point.

    @Parameters: void

    @Returns: void

    @Revision History:
        DATE             NAME               REVISION COMMENT
        04/08/2017       Ali Haidous        Initial Revision

 *----------------------------------------------------------------------------*/
void InitUFOApp (void)
{

    /* Initialize static and globals */
    scwUFOx = X_MAX/2;
    scwUFOy = 8*3;
    scwGrenade1x = X_MAX/2;
    scwGrenade1y = 7*8;
    scwGrenade2x = X_MAX/2;
    scwGrenade2y = 7*8;
    scwGrenade3x = X_MAX/2;
    scwGrenade3y = 7*8;
    scfGrenade1Ready = TRUE;
    scfGrenade2Ready = TRUE;
    scfGrenade3Ready = TRUE;

    /* Setup the hardware */
    if (!fHALSetup())
    {
        // Error condition
    }

    /* Register the callbacks */
    if (!scfRegisterCallback (*scGrenadeCallback, GRENADE_EVENT) ||
        !scfRegisterCallback (*scUFORightCallback, UFO_RIGHT_EVENT) ||
        !scfRegisterCallback (*scUFOLeftCallback, UFO_LEFT_EVENT))// ||
        //!scfRegisterCallback (*scRefreshLCDCallback, REFRESH_LCD))
    {
        // Error condition
    }
}


/*----------------------------------------------------------------------------

    @Prototype: void ExecuteUFOApp (void)

    @Description: Execute UFO Application

    @Parameters: void

    @Returns: Should never return.

    @Revision History:
        DATE             NAME               REVISION COMMENT
        04/08/2017      Ali Haidous        Initial Revision
 *----------------------------------------------------------------------------*/
void ExecuteUFOApp (void)
{
    for (;;)
    {
        /* 10ms task */
        if (gqw10msTicks >= 1)
        {
            (void)fPollJoyStick();
            gqw10msTicks = 0;  // Reset Ticker
            scRefreshLCDCallback();
        }
    }
}


/*----------------------------------------------------------------------------

    @Prototype: static void RefreshLCDCallback(void)

    @Description: Register a callback function that gets called on REFRESH_LCD event

    @Parameters:  void

    @Returns: void

    @Revision History:
        DATE             NAME               REVISION COMMENT
        04/08/2017       Ali Haidous        Initial Revision

 *----------------------------------------------------------------------------*/
static void scRefreshLCDCallback(void)
{
    ClearLCD();

    scDisplayUFO(scwUFOx, scwUFOy);

    scwGrenade1y += 8;
    scwGrenade2y += 8;
    scwGrenade3y += 8;
    scDisplayGrenade(scwGrenade1x, scwGrenade1y);
    scDisplayGrenade(scwGrenade2x, scwGrenade2y);
    scDisplayGrenade(scwGrenade3x, scwGrenade3y);

    if (scwGrenade1y >= (8*24))
    {
        scfGrenade1Ready = TRUE;
    }
    else if (scwGrenade2y >= (8*24))
    {
        scfGrenade2Ready = TRUE;
    }
    else if (scwGrenade3y >= (8*24))
    {
        scfGrenade3Ready = TRUE;
    }
}


/*----------------------------------------------------------------------------

    @Prototype: static void scGrenadeCallback(void)

    @Description: Register a callback function that gets called on GRENADE_EVENT event

    @Parameters:  void

    @Returns: void

    @Revision History:
        DATE             NAME               REVISION COMMENT
        04/08/2017       Ali Haidous        Initial Revision

 *----------------------------------------------------------------------------*/
static void scGrenadeCallback(void)
{
    if (scfGrenade1Ready)
    {
        scwGrenade1x = scwUFOx;
        scwGrenade1y = 7*8;
        scfGrenade1Ready = FALSE;
    }
    else if (scfGrenade2Ready)
    {
        scwGrenade2x = scwUFOx;
        scwGrenade2y = 7*8;
        scfGrenade2Ready = FALSE;
    }
    else if (scfGrenade3Ready)
    {
        scwGrenade3x = scwUFOx;
        scwGrenade3y = 7*8;
        scfGrenade3Ready = FALSE;
    }
}


/*----------------------------------------------------------------------------

    @Prototype: static void scUFOLeftCallback(void)

    @Description: Register a callback function that gets called on UFO_LEFT_EVENT event

    @Parameters:  void

    @Returns: void

    @Revision History:
        DATE             NAME               REVISION COMMENT
        04/08/2017       Ali Haidous        Initial Revision

 *----------------------------------------------------------------------------*/
static void scUFOLeftCallback(void)
{
    if (scwUFOx > (8*6))
    {
        scwUFOx -= 8;
    }
}


/*----------------------------------------------------------------------------

    @Prototype: static void scUFORightCallback(void)

    @Description: Register a callback function that gets called on UFO_Right_EVENT event

    @Parameters:  void

    @Returns: void

    @Revision History:
        DATE             NAME               REVISION COMMENT
        04/08/2017       Ali Haidous        Initial Revision

 *----------------------------------------------------------------------------*/
static void scUFORightCallback(void)
{
    if (scwUFOx < (X_MAX - (8*7)))
    {
        scwUFOx += 8;
    }
}


/*----------------------------------------------------------------------------

    @Prototype: static BOOL scfRegisterCallback (const pfnEventCallback pfnCallback,
                                                 const UFO_EVENT_E keEvent)

    @Description: Register a callback function that gets called on UFO_EVENT_E type of event

    @Parameters:  const pfnEventCallback pfnCallback
                  const UFO_EVENT_E keEvent

    @Returns: BOOL Exit code flag
                            TRUE - Success
                            FALSE - Failure

    @Revision History:
        DATE             NAME               REVISION COMMENT
        04/08/2017       Ali Haidous        Initial Revision

 *----------------------------------------------------------------------------*/
static BOOL scfRegisterCallback (const pfnEventCallback pfnCallback,
                                 const UFO_EVENT_E keEvent)
{
    BOOL fExitCode = TRUE;

    switch (keEvent)
    {
        case GRENADE_EVENT:
            pfnGrenadeEvent = pfnCallback;
            break;

        case UFO_RIGHT_EVENT:
            pfnUFORightCommandEvent = pfnCallback;
            break;

        case UFO_LEFT_EVENT:
            pfnUFOLeftCommandEvent = pfnCallback;
            break;

        case REFRESH_LCD:
            pfnRefreshLCDEvent = pfnCallback;
            break;

        default:
            fExitCode = FALSE;
            break;
    }

    return fExitCode;
}


/*----------------------------------------------------------------------------

    @Prototype: static void scDisplayUFO (WORD wXPos, WORD wYPos)

    @Description: Display the UFO at given coordinates

    @Parameters:  WORD wXPos
                                    WORD wYPos

    @Returns: void

    @Revision History:
        DATE             NAME               REVISION COMMENT
        04/08/2017       Ali Haidous        Initial Revision

 *----------------------------------------------------------------------------*/
static void scDisplayUFO (WORD wXPos, WORD wYPos)
{
    WORD wX;
    WORD wY;

    for (wX = (wXPos - (6 * 8)); wX < (wXPos + (7 * 8)); wX++)
    {
        for (wY = (wYPos - (2 * 8)); wY < (wYPos + (3 * 8)); wY++)
        {
            if ((((wX >= (wXPos - (6 * 8))) && (wX < (wXPos - (5 * 8)))) ||
                 ((wX >= (wXPos - (4 * 8))) && (wX < (wXPos - (3 * 8)))) ||
                 ((wX >= (wXPos - (2 * 8))) && (wX < (wXPos - (1 * 8)))) ||
                 ((wX >= (wXPos + (0 * 8))) && (wX < (wXPos + (1 * 8)))) ||
                 ((wX >= (wXPos + (2 * 8))) && (wX < (wXPos + (3 * 8)))) ||
                 ((wX >= (wXPos + (4 * 8))) && (wX < (wXPos + (5 * 8)))) ||
                 ((wX >= (wXPos + (6 * 8))) && (wX < (wXPos + (7 * 8))))) &&
                 ((wY >= (wYPos + (0 * 8))) && (wY < (wYPos + (1 * 8)))))
            {
                SetPoint(wY, wX, RED);
            }
            if ((((wX >= (wXPos - (5 * 8))) && (wX < (wXPos - (4 * 8)))) ||
                 ((wX >= (wXPos - (3 * 8))) && (wX < (wXPos - (2 * 8)))) ||
                 ((wX >= (wXPos - (1 * 8))) && (wX < (wXPos - (0 * 8)))) ||
                 ((wX >= (wXPos + (1 * 8))) && (wX < (wXPos + (2 * 8)))) ||
                 ((wX >= (wXPos + (3 * 8))) && (wX < (wXPos + (4 * 8)))) ||
                 ((wX >= (wXPos + (5 * 8))) && (wX < (wXPos + (6 * 8))))) &&
                 ((wY >= (wYPos + (0 * 8))) && (wY < (wYPos + (1 * 8)))))
            {
                SetPoint(wY, wX, YELLOW);
            }
            if ((((wX >= (wXPos - (4 * 8))) && (wX < (wXPos - (3 * 8)))) ||
                 ((wX >= (wXPos - (3 * 8))) && (wX < (wXPos - (2 * 8)))) ||
                 ((wX >= (wXPos - (2 * 8))) && (wX < (wXPos - (1 * 8)))) ||
                 ((wX >= (wXPos - (1 * 8))) && (wX < (wXPos - (0 * 8)))) ||
                 ((wX >= (wXPos + (0 * 8))) && (wX < (wXPos + (1 * 8)))) ||
                 ((wX >= (wXPos + (1 * 8))) && (wX < (wXPos + (2 * 8)))) ||
                 ((wX >= (wXPos + (2 * 8))) && (wX < (wXPos + (3 * 8)))) ||
                 ((wX >= (wXPos + (3 * 8))) && (wX < (wXPos + (4 * 8)))) ||
                 ((wX >= (wXPos + (4 * 8))) && (wX < (wXPos + (5 * 8))))) &&
                (((wY >= (wYPos + (1 * 8))) && (wY < (wYPos + (2 * 8)))) ||
                 ((wY >= (wYPos - (1 * 8))) && (wY < (wYPos - (0 * 8))))))
            {
                SetPoint(wY, wX, BLUE);
            }
            if ((((wX >= (wXPos - (3 * 8))) && (wX < (wXPos - (2 * 8)))) ||
                 ((wX >= (wXPos - (2 * 8))) && (wX < (wXPos - (1 * 8)))) ||
                 ((wX >= (wXPos - (1 * 8))) && (wX < (wXPos - (0 * 8)))) ||
                 ((wX >= (wXPos + (0 * 8))) && (wX < (wXPos + (1 * 8)))) ||
                 ((wX >= (wXPos + (1 * 8))) && (wX < (wXPos + (2 * 8)))) ||
                 ((wX >= (wXPos + (2 * 8))) && (wX < (wXPos + (3 * 8)))) ||
                 ((wX >= (wXPos + (3 * 8))) && (wX < (wXPos + (4 * 8))))) &&
                (((wY >= (wYPos + (2 * 8))) && (wY < (wYPos + (3 * 8)))) ||
                 ((wY >= (wYPos - (2 * 8))) && (wY < (wYPos - (1 * 8))))))
            {
                SetPoint(wY, wX, BLUE);
            }
        }
    }
}


/*----------------------------------------------------------------------------

    @Prototype: static void scDisplayGrenade (WORD wXPos, WORD wYPos)

    @Description: Display the Grenade at given coordinates

    @Parameters:  WORD wXPos
                                    WORD wYPos

    @Returns: void

    @Revision History:
        DATE             NAME               REVISION COMMENT
        04/08/2017       Ali Haidous        Initial Revision

 *----------------------------------------------------------------------------*/
static void scDisplayGrenade (WORD wXPos, WORD wYPos)
{
  WORD wX;
    WORD wY;

    for (wX = (wXPos - (1 * 8)); wX < (wXPos + (2 * 8)); wX++)
    {
        for (wY = (wYPos - (1 * 8)); wY < (wYPos + (4 * 8)); wY++)
        {
            if ((((wX >= (wXPos - (1 * 8))) && (wX < (wXPos - (0 * 8)))) ||
                 ((wX >= (wXPos + (1 * 8))) && (wX < (wXPos + (2 * 8))))) &&
                (((wY >= (wYPos + (2 * 8))) && (wY < (wYPos + (3 * 8)))) ||
                 ((wY >= (wYPos + (3 * 8))) && (wY < (wYPos + (4 * 8))))))
            {
                continue;
            }
            else
            {
                SetPoint(wY, wX, MAGENTA);
            }
        }
    }
}


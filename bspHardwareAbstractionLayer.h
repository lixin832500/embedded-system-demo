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
#ifndef __BSP_HAL_H__
#define __BSP_HAL_H__

/* -- INCLUDES -- */
#include "bspDataTypes.h"

/* -- DEFINES and ENUMS -- */
typedef enum
{
    GRENADE_EVENT,
    UFO_RIGHT_EVENT,
    UFO_LEFT_EVENT,
    REFRESH_LCD
} UFO_EVENT_E;

// LCD color
#define BLUE 0x001F
#define RED 0xF800
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0

#define Y_MAX 240
#define X_MAX 320

/* -- TYPEDEFS and STRUCTURES -- */
typedef void (*pfnEventCallback)(void);


/* -- GLOBAL VARIABLES -- */
extern QWORD gqw10msTicks;

extern pfnEventCallback pfnGrenadeEvent;
extern pfnEventCallback pfnUFORightCommandEvent;
extern pfnEventCallback pfnUFOLeftCommandEvent;
extern pfnEventCallback pfnRefreshLCDEvent;


/* -- EXTERNAL FUNCTIONS -- */
extern BOOL fHALSetup (void);  /* Generic HAL Setup */
extern void SetPoint(WORD wX, WORD wY, WORD wColor);
extern void ClearLCD(void);
extern BOOL fPollJoyStick(void);

#endif /* __BSP_HAL_H__ */

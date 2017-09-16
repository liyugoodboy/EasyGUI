/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2017 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#define GUI_INTERNAL
#include "gui_led.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/
#define __GL(x)             ((GUI_LED_t *)(x))

#define CFG_TOGGLE          0x01
#define CFG_SET             0x02
#define CFG_TYPE            0x03

static
uint8_t GUI_LED_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);
    
/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static const GUI_Color_t Colors[] = {
    GUI_COLOR_LIGHTBLUE,                            /*!< Default color when led is on */
    GUI_COLOR_DARKBLUE,                             /*!< Default color when led is off */
    GUI_COLOR_GRAY,                                 /*!< Default border color when led is on */
    GUI_COLOR_BLACK,                                /*!< Default border color when led is off */
};

static const GUI_WIDGET_t Widget = {
    .Name = _GT("LED"),                             /*!< Widget name */ 
    .Size = sizeof(GUI_LED_t),                      /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = GUI_LED_Callback,                   /*!< Control function */
    .Colors = Colors,                               /*!< List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define l           ((GUI_LED_t *)h)
static
uint8_t GUI_LED_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_SetParam: {                     /* Set parameter for widget */
            GUI_WIDGET_Param_t* p = (GUI_WIDGET_Param_t *)param;
            switch (p->Type) {
                case CFG_SET:
                    if (*(uint8_t *)p->Data) {
                        l->Flags |= GUI_LED_FLAG_ON;
                    } else {
                        l->Flags &= ~GUI_LED_FLAG_ON;
                    }
                    break;
                case CFG_TOGGLE: 
                    l->Flags ^= GUI_LED_FLAG_ON;    /* Toggle flag */
                    break;
                case CFG_TYPE: 
                    l->Type = *(GUI_LED_TYPE_t *)p->Data;   /* Toggle flag */
                    break;
                default: break;
            }
            *(uint8_t *)result = 1;                 /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Color_t c1, c2;
            GUI_Dim_t x, y, width, height;
            
            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute position on screen */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute position on screen */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
            
            /* Get drawing colors */
            if (l->Flags & GUI_LED_FLAG_ON) {       /* If LED is on */
                c1 = __GUI_WIDGET_GetColor(h, GUI_LED_COLOR_ON);
                c2 = __GUI_WIDGET_GetColor(h, GUI_LED_COLOR_ON_BORDER);
            } else {
                c1 = __GUI_WIDGET_GetColor(h, GUI_LED_COLOR_OFF);
                c2 = __GUI_WIDGET_GetColor(h, GUI_LED_COLOR_OFF_BORDER);
            }
            
            if (l->Type == GUI_LED_TYPE_RECT) {     /* When led has rectangle shape */
                GUI_DRAW_FilledRectangle(disp, x + 1, y + 1, width - 2, height - 2, c1);
                GUI_DRAW_Rectangle(disp, x, y, width, height, c2);
            } else {
                GUI_DRAW_FilledCircle(disp, x + width / 2, y + height / 2, width / 2, c1);
                GUI_DRAW_Circle(disp, x + width / 2, y + height / 2, width / 2, c2);
            }
            return 1;                               /* */
        }
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef l

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_LED_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

uint8_t GUI_LED_SetColor(GUI_HANDLE_p h, GUI_LED_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return __GUI_WIDGET_SetColor(h, (uint8_t)index, color); /* Set color */
}

uint8_t GUI_LED_SetType(GUI_HANDLE_p h, GUI_LED_TYPE_t type) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return __GUI_WIDGET_SetParam(h, CFG_TYPE, &type, 1, 1); /* Set parameter */
}

uint8_t GUI_LED_Toggle(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return __GUI_WIDGET_SetParam(h, CFG_TOGGLE, NULL, 1, 0);/* Set parameter */
}

uint8_t GUI_LED_Set(GUI_HANDLE_p h, uint8_t state) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return __GUI_WIDGET_SetParam(h, CFG_SET, &state, 1, 0); /* Set parameter */
}

uint8_t GUI_LED_IsOn(GUI_HANDLE_p h) {
    uint8_t ret;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = !!(__GL(h)->Flags & GUI_LED_FLAG_ON);
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

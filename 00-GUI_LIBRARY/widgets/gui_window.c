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
#include "gui_window.h"

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
#define __GW(x)             ((GUI_WINDOW_t *)(x))

static
uint8_t GUI_WINDOW_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);
    
/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_Color_t Colors[] = {
    GUI_COLOR_WIN_LIGHTGRAY,                        /*!< Default background color */
    GUI_COLOR_WIN_TEXT_TITLE,                       /*!< Default text color */
    GUI_COLOR_WIN_SEL_FOC_BG,                       /*!< Default top background color when widget in focus */
    GUI_COLOR_WIN_SEL_NOFOC_BG,                     /*!< Default top background color when widget not in focus */
};

const static GUI_WIDGET_t Widget = {
    .Name = _T("Window"),                           /*!< Widget name */
    .Size = sizeof(GUI_WINDOW_t),                   /*!< Size of widget for memory allocation */
    .Flags = GUI_FLAG_WIDGET_ALLOW_CHILDREN,        /*!< List of widget flags */
    .Callback = GUI_WINDOW_Callback,                /*!< Control function */
    .Colors = Colors,                               /*!< Pointer to colors array */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define w          ((GUI_WINDOW_t *)h)
static
uint8_t GUI_WINDOW_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
#if GUI_USE_TOUCH
   static GUI_iDim_t tX, tY, Mode = 0;
#endif /* GUI_USE_TOUCH */
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_iDim_t x, y, wi, hi, pt, topH;
            uint8_t inFocus;
            
            pt = __GUI_WIDGET_GetPaddingTop(h);
            
            x = __GUI_WIDGET_GetAbsoluteX(h);
            y = __GUI_WIDGET_GetAbsoluteY(h);
            wi = __GUI_WIDGET_GetWidth(h);
            hi = __GUI_WIDGET_GetHeight(h);
            
            GUI_DRAW_FilledRectangle(disp, x, y, wi, hi, __GUI_WIDGET_GetColor(h, GUI_WINDOW_COLOR_BG));
            if (__GH(h)->Flags & GUI_FLAG_CHILD) {
                GUI_DRAW_Rectangle3D(disp, x, y, wi, hi, GUI_DRAW_3D_State_Lowered);
                
                x += 2;
                y += 2;
                wi -= 4;
                hi -= 4;
                topH = pt - 4;
                
                inFocus = __GUI_WIDGET_IsFocusedOrChildren(h);  /* Check if window is in focus or any children widget */
                
                GUI_DRAW_FilledRectangle(disp, x + 1, y + 1, wi - 2, topH, inFocus ? __GUI_WIDGET_GetColor(h, GUI_WINDOW_COLOR_TOP_BG_FOC) : __GUI_WIDGET_GetColor(h, GUI_WINDOW_COLOR_TOP_BG_NOFOC));
                
                /* Draw "close" button */
                GUI_DRAW_Rectangle3D(disp, x + wi - topH, y + 2, topH - 2, topH - 2, GUI_DRAW_3D_State_Raised);
                GUI_DRAW_FilledRectangle(disp, x + wi - topH + 2, y + 4, topH - 6, topH - 6, GUI_COLOR_GRAY);
                
                if (__GUI_WIDGET_IsFontAndTextSet(h)) {
                    GUI_DRAW_FONT_t f;
                    GUI_DRAW_FONT_Init(&f);         /* Init structure */
                    
                    f.X = x + 3;
                    f.Y = y + 3;
                    f.Width = wi - topH - 5;
                    f.Height = topH - 3;
                    f.Align = GUI_HALIGN_LEFT | GUI_VALIGN_CENTER;
                    f.Color1Width = f.Width;
                    f.Color1 = __GUI_WIDGET_GetColor(h, GUI_WINDOW_COLOR_TEXT);
                    GUI_DRAW_WriteText(disp, __GH(h)->Font, __GH(h)->Text, &f);
                }
            }
            
//            GUI_DRAW_Rectangle3D(disp, x, y, wi, hi, GUI_DRAW_3D_State_Lowered);
//            GUI_DRAW_FilledRectangle(disp, x + pl, y + pt, wi - pl - pr, hi - pt - pb, __GUI_WIDGET_GetColor(h, GUI_WINDOW_COLOR_BG));
            
            return 1;
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param; /* Get touch parameters */

            if (ts->TS.Count == 1 && __GH(h)->Flags & GUI_FLAG_CHILD) { /* For children widgets only on single touch */
                GUI_iDim_t pt, wi;
                pt = __GUI_WIDGET_GetPaddingTop(h); /* Get top padding */
                wi = __GUI_WIDGET_GetWidth(h);      /* Get widget width */
                
                if (ts->RelY[0] < pt && ts->RelX[0] < wi - pt) {
                    Mode = 1;
                    tX = ts->RelX[0];
                    tY = ts->RelY[0];
                }
                *(__GUI_TouchStatus_t *)result = touchHANDLED;  /* Set handled status */
            } else {
                *(__GUI_TouchStatus_t *)result = touchHANDLEDNOFOCUS;   /* Set handled status */
            }
            return 1;
        }
        case GUI_WC_TouchMove: {
            __GUI_TouchData_t* ts = (__GUI_TouchData_t *)param; /* Get touch parameters */
            
            if (Mode == 1) {
                GUI_iDim_t pX, pY;
                pX = __GUI_WIDGET_GetParentAbsoluteX(__GH(h));
                pY = __GUI_WIDGET_GetParentAbsoluteY(__GH(h));
                __GUI_WIDGET_SetXY(h,
                    ts->TS.X[0] - pX - tX,
                    ts->TS.Y[0] - pY - tY);
            }
            
            return 1;
        }
        case GUI_WC_TouchEnd: {
            Mode = 0;
            return 1;
        }
#endif /* GUI_USE_TOUCH */
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef w

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_WINDOW_Create(GUI_ID_t id) {
    GUI_WINDOW_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_WINDOW_t *)__GUI_WIDGET_Create(&Widget, id, 0, 0, GUI.LCD.Width, GUI.LCD.Height, 0, GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP);  /* Allocate memory for basic widget */
    if (ptr) {
        GUI_WINDOW_SetActive(__GH(ptr));            /* Set active window */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    return __GH(ptr);
}

GUI_HANDLE_p GUI_WINDOW_CreateChild(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, uint16_t flags) {
    GUI_WINDOW_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_WINDOW_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, flags); /* Allocate memory for basic widget */
    if (ptr) {
        /* Control setup */
        __GH(ptr)->Flags |= GUI_FLAG_CHILD;         /* This window is child window */
        
        __GUI_WIDGET_SetPaddingTop(ptr, 26);
        __GUI_WIDGET_SetPaddingRight(ptr, 2);
        __GUI_WIDGET_SetPaddingBottom(ptr, 2);
        __GUI_WIDGET_SetPaddingLeft(ptr, 2);
        
        GUI_WINDOW_SetActive(__GH(ptr));            /* Set active window */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    return __GH(ptr);
}

uint8_t GUI_WINDOW_SetActive(GUI_HANDLE_p h) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    GUI.WindowActive = h;                           /* Set new active window */
    __GUI_WIDGET_MoveDownTree(h);                   /* Move widget down on tree */
    
    __GUI_FOCUS_CLEAR();                            /* Clear focus on widget */
    __GUI_ACTIVE_CLEAR();                           /* Clear active on widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return 1;
}

uint8_t GUI_WINDOW_SetColor(GUI_HANDLE_p h, GUI_WINDOW_COLOR_t index, GUI_Color_t color) {
    uint8_t ret;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ret = __GUI_WIDGET_SetColor(h, (uint8_t)index, color);  /* Set desired color */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}

GUI_HANDLE_p GUI_WINDOW_GetDesktop(void) {
    return (GUI_HANDLE_p)__GUI_LINKEDLIST_GETNEXT_GEN(&GUI.Root, NULL); /* Return desktop window */
}

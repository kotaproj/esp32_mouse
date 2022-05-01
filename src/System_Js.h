/**
 * @file System_Js.h
 * @brief JoyStick定義ヘッダ
 */

#ifndef _SYSTEM_JS_H_
#define _SYSTEM_JS_H_

#include "System.h"

/* DECLARATIONS ***************************************************************/

#define USE_JS 1
#if USE_JS
#define JS_ASGN_AXIS_X (25) //(27)
#define JS_ASGN_AXIS_Y (27) //(25)
#endif                      // USE_JS
#define JS_TH 500

/* DECLARATIONS - enum ********************************************************/

/**
 * @enum JsCode_e
 * @brief JoyStick識別子
 * @note -
 */
typedef enum
{
    JS_CODE_AXIS = (0),
    JS_CODE_ILL = (0xFF),
} JsCode_e;

/* DECLARATIONS - struct ******************************************************/

/**
 * @struct JsMessage_t
 * @brief メッセージ - JoyStick用メッセージデータ
 * @note
 */
typedef struct
{
    uint32_t ulCode;
    int32_t slAxisX;
    int32_t slAxisY;
    uint8_t ucJsserve[20];
} JsMessage_t;

/**
 * @struct MessageCtlJs_t
 * @brief メッセージ - JoyStick用メッセージ
 * @note
 */
typedef struct
{
    MessageHead_t Head_t;
    JsMessage_t Data_t;
} MessageCtlJs_t;

#endif /* _SYSTEM_JS_H_ */

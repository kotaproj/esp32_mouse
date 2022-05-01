/**
 * @file System_Mouse.h
 * @brief Mouse定義ヘッダ
 */

#ifndef _SYSTEM_MOUSE_H_
#define _SYSTEM_MOUSE_H_

#include "System.h"

/* DECLARATIONS ***************************************************************/

/* DECLARATIONS - enum ********************************************************/

/**
 * @enum MouseCode_e
 * @brief Mouse識別子
 * @note -
 */
typedef enum
{
    MOUSE_CODE_CLICK = (0),
    MOUSE_CODE_MOVE,
    MOUSE_CODE_PRESS,
    MOUSE_CODE_RELEASE,
    MOUSE_CODE_SP,
    MOUSE_CODE_NUM,
    MOUSE_CODE_MIN = MOUSE_CODE_CLICK,
    MOUSE_CODE_MAX = MOUSE_CODE_SP,
} MouseCode_e;

/* DECLARATIONS - struct ******************************************************/

/**
 * @struct MouseMessage_t
 * @brief メッセージ - Mouse用メッセージデータ
 * @note
 */
typedef struct
{
    uint8_t u8Code;
    uint8_t u8Type;
    int8_t s8x;
    int8_t s8y;
    int8_t wheel;
    int8_t hWheel;
    uint8_t u8SpType;
    uint8_t ucReserve[25];
} MouseMessage_t;

/**
 * @struct MessageCtlMouse_t
 * @brief メッセージ - Mouse用メッセージ
 * @note
 */
typedef struct
{
    MessageHead_t Head_t;
    MouseMessage_t Data_t;
} MessageCtlMouse_t;

#endif /* _SYSTEM_MOUSE_H_ */

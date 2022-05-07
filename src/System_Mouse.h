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
    MOUSE_CODE_ERR = 0xFF,
} MouseCode_e;

/* DECLARATIONS - struct ******************************************************/

/**
 * @struct MouseMessage_t
 * @brief メッセージ - Mouse用メッセージデータ
 * @note
 */
typedef struct
{
    uint8_t u8Code;        // 1 1
    uint8_t u8Type;        // 1 2
    uint8_t u8SpType;      // 1 3
    uint8_t u8Linear;      // 1 4
    int32_t s32x;          // 4 8
    int32_t s32y;          // 4 12
    int32_t s32wheel;      // 4 16
    int32_t s32hWheel;     // 4 20
    uint8_t ucReserve[12]; // 12 32
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

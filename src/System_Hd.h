/**
 * @file System_Hd.h
 * @brief HTTPd定義ヘッダ
 * @author kotatuneco
 * @date 2019/11/25
 */

#ifndef _SYSTEM_HD_H_
#define _SYSTEM_HD_H_

#include "System.h"

/* DECLARATIONS ***************************************************************/

/* DECLARATIONS - enum ********************************************************/

/**
 * @enum HdCode_e
 * @brief Hd識別子
 * @note -
 */
typedef enum
{
    HD_CODE_CLICK = (0),
    HD_CODE_MOVE,
    HD_CODE_PRESS,
    HD_CODE_RELEASE,
    HD_CODE_SP,
    HD_CODE_NUM,
    HD_CODE_MIN = HD_CODE_CLICK,
    HD_CODE_MAX = HD_CODE_SP,
    HD_CODE_ERR = 0xFF,
} HdCode_e;

/**
 * @enum HdType_e
 * @brief Hd識別子
 * @note -
 */
typedef enum
{
    HD_TYPE_MOUSE_LEFT = 1,
    HD_TYPE_MOUSE_RIGHT = 2,
    HD_TYPE_MOUSE_MIDDLE = 4,
    HD_TYPE_MOUSE_BACK = 8,
    HD_TYPE_MOUSE_FORWARD = 16,
} HdType_e;

/* DECLARATIONS - struct ******************************************************/

/* DECLARATIONS - struct ******************************************************/

/**
 * @struct HdMessage_t
 * @brief メッセージ - Httpd用メッセージ
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
} HdMessage_t;

/**
 * @struct MessageCtlFHd_t
 * @brief メッセージ - Httpd用メッセージ
 * @note
 */
typedef struct
{
    MessageHead_t Head_t;
    HdMessage_t Data_t;
} MessageCtlHd_t;

#endif /* _SYSTEM_HD_H_ */
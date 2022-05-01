/**
 * @file System_Sw.h
 * @brief スイッチ定義ヘッダ
 */

#ifndef _SYSTEM_SW_H_
#define _SYSTEM_SW_H_

#include "System.h"

/* DECLARATIONS ***************************************************************/

#define USE_SW_NO1 1
#if USE_SW_NO1
#define SW_GPIO_ASGN_NO1 (21)
#endif // USE_SW_NO1
#define USE_SW_NO2 1
#if USE_SW_NO2
#define SW_GPIO_ASGN_NO2 (23)
#endif // USE_SW_NO2
#define USE_SW_NO3 1
#if USE_SW_NO3
#define SW_GPIO_ASGN_NO3 (19)
#endif // USE_SW_NO3
#define USE_SW_NO4 1
#if USE_SW_NO4
#define SW_GPIO_ASGN_NO4 (18)
#endif // USE_SW_NO4
#define USE_SW_NO5 0
#if USE_SW_NO5
#define SW_GPIO_ASGN_NO5 (33)
#endif // USE_SW_NO5

/* DECLARATIONS - enum ********************************************************/

/**
 * @enum SwCode_t
 * @brief スイッチ識別子
 * @note -
 */
typedef enum
{
    SW_CODE_NON = 0x00000000,
    SW_CODE_NO1 = 0x00000001,
    SW_CODE_NO2 = 0x00000002,
    SW_CODE_NO3 = 0x00000004,
    SW_CODE_NO4 = 0x00000008,
    SW_CODE_NO5 = 0x00000010,
    SW_CODE_NO6 = 0x00000020,
    SW_CODE_NO7 = 0x00000040,
    SW_CODE_NO8 = 0x00000080,
    SW_CODE_NO9 = 0x00000100,
    SW_CODE_NO10 = 0x00000200,
    SW_CODE_NO11 = 0x00000400,
    SW_CODE_NO12 = 0x00000800,
    SW_CODE_NO13 = 0x00001000,
    SW_CODE_NO14 = 0x00002000,
    SW_CODE_NO15 = 0x00004000,
    SW_CODE_NO16 = 0x00008000,
} SwCode_t;

/**
 * @enum SwType_t
 * @brief スイッチタイプ
 * @note -
 */
typedef enum
{
    SW_TYPE_NONE = 0x01,
    SW_TYPE_DOWN = 0x02,
    SW_TYPE_UP = 0x04,
} SwType_t;

/**
 * @enum SwSts_t
 * @brief スイッチプッシュ論理
 * @note -
 */
typedef enum
{
    SW_STS_PRESSED = 0,
    SW_STS_RELEASED = 1,
} SwSts_t;

/* DECLARATIONS - struct ******************************************************/

/**
 * @struct SwMessage_t
 * @brief メッセージ - スイッチ用メッセージ
 * @note
 */
typedef struct
{
    uint32_t ulCode;
    uint32_t ulType;
    uint8_t ucReserve[24];
} SwMessage_t;

/**
 * @struct MessageCtlFSw_t
 * @brief メッセージ - スイッチ用メッセージ
 * @note
 */
typedef struct
{
    MessageHead_t Head_t;
    SwMessage_t Data_t;
} MessageCtlSw_t;

#endif /* _SYSTEM_SW_H_ */

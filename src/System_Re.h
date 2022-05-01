/**
 * @file System_Re.h
 * @brief ロータリーエンコーダ定義ヘッダ
 * @author kotaproj
 */

#ifndef _SYSTEM_RE_H_
#define _SYSTEM_RE_H_

#include "System.h"

/* DECLARATIONS ***************************************************************/

#define USE_RE 1
#if USE_RE
#define RE_ASGN_SIGNAL_A (18) //(4)
#define RE_ASGN_SIGNAL_B (16) //(2)
#endif                        // USE_RE

/* DECLARATIONS - enum ********************************************************/

/**
 * @enum ReCode_t
 * @brief ロータリーエンコーダ識別子
 * @note -
 */
typedef enum
{
    RE_CODE_DUMMY = 0x00000000,
} ReCode_t;

/* DECLARATIONS - struct ******************************************************/

/**
 * @struct ReMessage_t
 * @brief メッセージ - ロータリーエンコーダ用メッセージデータ
 * @note
 */
typedef struct
{
    uint32_t ulCode;
    int32_t slCount;
    uint8_t ucReserve[24];
} ReMessage_t;

/**
 * @struct MessageCtlRe_t
 * @brief メッセージ - ロータリーエンコーダ用メッセージ
 * @note
 */
typedef struct
{
    MessageHead_t Head_t; ///< 全体ソース指定
    ReMessage_t Data_t;   ///< 全体ソース指定
} MessageCtlRe_t;

#endif /* _SYSTEM_RE_H_ */

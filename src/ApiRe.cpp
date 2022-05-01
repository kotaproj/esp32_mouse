/**
 * @file ApiRe.c
 * @brief ロータリーエンコーダ管理
 */

#include "ApiRe.h"
#include "CtlTaskRe.h"
#include <RotaryEncoder.h>

/// @cond
#define _API_RE_C_
/// @endcond

/* DECLARATIONS ***************************************************************/
#define DEBUG_API_RE (0)   // 0:release, 1:debug
#define RE_CHECK_TIME (50) //ロータリーエンコーダチェック間隔:50msec

/* VARIABLES ******************************************************************/
static RotaryEncoder s_xRotaryEncoder(RE_ASGN_SIGNAL_A, RE_ASGN_SIGNAL_B);
static int32_t s_slBakRePosition;
static TimerHandle_t s_xTimerRe = NULL;

/* PRIVATE PROTOTYPES *********************************************************/
static void IRAM_ATTR vIsrCallbackRe();
static void vTimerCallbackRe(TimerHandle_t xTimer);
static void vSendRe(uint32_t ulCode, int32_t slCount, uint32_t ulGain);

/* TABLES ***************************************************************/

/**
 * @brief ロータリーエンコーダ処理初期化関数
 */
ErType_t xInitRe(void)
{
    Serial.printf("%s : run\n", __func__);

    // 管理データの初期化
    s_slBakRePosition = 0;

    // 割り込み関連の設定
    attachInterrupt(digitalPinToInterrupt(RE_ASGN_SIGNAL_A), vIsrCallbackRe, CHANGE);
    attachInterrupt(digitalPinToInterrupt(RE_ASGN_SIGNAL_B), vIsrCallbackRe, CHANGE);

    //               [0],[1],[2],[3],[4],[5], [6], [7],
    char cName[8] = {'T', 'm', 'r', 'R', 'e', 0x00, 0x00, 0x00};

    // text name, timer period, auto-reload, number of times, callback
    s_xTimerRe = xTimerCreate(cName, RE_CHECK_TIME, pdTRUE, (void *)0, vTimerCallbackRe);
    if (NULL == s_xTimerRe)
    {
        Serial.printf("%s : error - xTimerCreate\n", __func__);
        return ER_FAIL;
    }

    if (pdTRUE != xTimerReset(s_xTimerRe, RE_CHECK_TIME))
    {
        Serial.printf("%s : error - xTimerReset\n", __func__);
        return ER_FAIL;
    }

    Serial.printf("%s : over\n", __func__);
    return ER_OK;
}

/**
 * @brief ロータリーエンコーダのタイマーコールバック関数
 */
static void vTimerCallbackRe(TimerHandle_t xTimer)
{
    static int32_t pos;
#if DEBUG_API_RE
    Serial.printf("%s - run\n", __func__);
#endif

    pos = s_xRotaryEncoder.getPosition();
    if (pos != s_slBakRePosition)
    {
        vSendRe(RE_CODE_DUMMY, (pos - s_slBakRePosition), 2);
#if DEBUG_API_RE
        Serial.printf("%s - (%d), (%d)\n", __func__, pos, (pos - s_slBakRePosition));
#endif
        s_slBakRePosition = pos;
    }

#if DEBUG_API_RE
    Serial.printf("%s - over\n", __func__);
#endif

    return;
}

/**
 * @brief ロータリーエンコーダ割り込み処理
 */
static void IRAM_ATTR vIsrCallbackRe()
{
    s_xRotaryEncoder.tick(); // just call tick() to check the state.
}

/**
 * @brief 管理タスクへロータリーエンコーダイベントを通知
 */
static void vSendRe(uint32_t ulCode, int32_t slCount, uint32_t ulGain)
{
    static ReMessage_t s_xMessage;

    s_xMessage.ulCode = ulCode;
    if (ulGain > 0)
    {
        s_xMessage.slCount = slCount * ulGain;
    }
    else
    {
        s_xMessage.slCount = slCount;
    }

    vCallbackCtlMsgRe(UID_RE, UID_CTL, &s_xMessage);
}

#undef _API_RE_C_
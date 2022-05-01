/**
 * @file ApiJs.cpp
 * @brief JoyStick管理
 */

#include "ApiJs.h"
#include "CtlTaskJs.h"

/// @cond
#define _API_JS_C_
/// @endcond

/* DECLARATIONS ***************************************************************/
#define DEBUG_API_JS (0) // 0:release, 1:debug
#if DEBUG_API_JS
#endif

#define JS_CHECK_TIME (25) // JoyStickチェック間隔:25msec

typedef struct
{
    JsCode_e eJsCode;
    int32_t x;
    int32_t y;
} JsCrd_t;

/* VARIABLES ******************************************************************/
static TimerHandle_t s_xTimerJs = NULL;

/* PRIVATE PROTOTYPES *********************************************************/
static void vTimerCallbackJs(TimerHandle_t xTimer);

/* TABLES ***************************************************************/

/**
 * @brief JoyStick処理初期化関数
 */
ErType_t xInitJs(void)
{
    Serial.printf("%s : run\n", __func__);

    // PIN設定
    pinMode(JS_ASGN_AXIS_X, INPUT);
    pinMode(JS_ASGN_AXIS_Y, INPUT);

    //               [0],[1],[2],[3],[4],[5], [6], [7],
    char cName[8] = {'T', 'm', 'r', 'J', 's', 0x00, 0x00, 0x00};

    // text name, timer period, auto-reload, number of times, callback
    s_xTimerJs = xTimerCreate(cName, JS_CHECK_TIME, pdTRUE, (void *)0, vTimerCallbackJs);
    if (NULL == s_xTimerJs)
    {
        Serial.printf("%s : error - xTimerCreate\n", __func__);
        return ER_FAIL;
    }

    if (pdTRUE != xTimerReset(s_xTimerJs, JS_CHECK_TIME))
    {
        Serial.printf("%s : error - xTimerReset\n", __func__);
        return ER_FAIL;
    }

    Serial.printf("%s : over\n", __func__);
    return ER_OK;
}

/**
 * @brief JoyStickのタイマーコールバック関数
 */
static void vTimerCallbackJs(TimerHandle_t xTimer)
{
#if DEBUG_API_JS
    Serial.printf("%s - run\n", __func__);
#endif

    int32_t x = analogRead(JS_ASGN_AXIS_X);
    int32_t y = analogRead(JS_ASGN_AXIS_Y);

#if DEBUG_API_JS
    Serial.printf("%s - x : %d, y : %d\n", __func__, x, y);
#endif

    if (abs(x - 2048) < JS_TH && abs(y - 2048) < JS_TH)
    {
        return;
    }

    JsMessage_t xMessage;

    xMessage.ulCode = JS_CODE_AXIS;
    xMessage.slAxisX = x;
    xMessage.slAxisY = y;

    vCallbackCtlMsgJs(UID_JS, UID_CTL, &xMessage);

#if DEBUG_API_JS
    Serial.printf("%s - over\n", __func__);
#endif

    return;
}

#undef _API_JS_C_

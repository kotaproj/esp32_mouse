/**
 * @file CtlTaskJs.c
 * @brief 全体管理タスク - ジョイスティック処理
 */
#include "CtlTaskJs.h"
#include "TaskMouse.h"

/// @cond
#define _CTLTASK_JS_C_
/// @endcond

/* DECLARATIONS ***************************************************************/
#define DEBUG_CTL_TASK_JS 0
#if DEBUG_CTL_TASK_JS
void vPrintCtlJs(const char *msg, const char *func, const char *file) { Serial.printf("%s - %s() : %s\n", file, func, msg); }
#else                            // DEBUG_CTL_TASK
void vPrintCtlJs(const char *msg, const char *func, const char *file) {}
#endif                           // DEBUG_CTL_TASK_JS
#define CTL_QUEUE_LENGTH_JS (1U) ///< フロントジョイスティックキューバッファ数

/* VARIABLES ******************************************************************/
static xQueueHandle s_xQueueJs; ///< フロントジョイスティックキューハンドラ

/* PRIVATE PROTOTYPES *********************************************************/
static void vProcCtlCmdJs(JsMessage_t *pxData_t);
static void vProcCtlCmdJs_NONE(uint32_t ulCode);
static void vProcCtlCmdJs_DOWN(uint32_t ulCode);
static void vProcCtlCmdJs_UP(uint32_t ulCode);

/* CODE ***********************************************************************/

/**
 * @brief 初期化-タスク生成前
 */
ErType_t xInitCtlJs(QueueSetHandle_t *pxQueSetHandle)
{
    s_xQueueJs = xQueueCreate(CTL_QUEUE_LENGTH_JS, sizeof(MessageCtlJs_t));
    if (NULL == s_xQueueJs)
    {
        return ER_FAIL;
    }

    if (pdPASS != xQueueAddToSet(s_xQueueJs, *pxQueSetHandle))
    {
        return ER_FAIL;
    }

    return ER_OK;
}

/**
 * @brief ワークエリア初期化-タスク生成後
 */
ErType_t xInitWorkCtlJs(void)
{
    return ER_OK;
}

/**
 * @brief キューの深さ取得
 */
uint32_t ulGetQueLengthCtlJs(void)
{
    return CTL_QUEUE_LENGTH_JS;
}

/**
 * @brief キュー送信処理(Js->Ctl)
 */
void vCallbackCtlMsgJs(UniId_t xSrcId, UniId_t xDstId, void *pvParam)
{
    MessageCtlJs_t xMessage;

    //ヘッダ情報
    xMessage.Head_t.xSrcId = xSrcId;
    xMessage.Head_t.xDstId = xDstId;

    //データ情報
    memcpy(&xMessage.Data_t, (JsMessage_t *)pvParam, sizeof(JsMessage_t));

    //キューの送信
    if (pdTRUE != xQueueSend(s_xQueueJs, &xMessage, 0))
    {
        MessageCtlJs_t xDummy;

        // 送信できないときは、古いデータを削除
        // キューの空読み
        xQueueReceive(s_xQueueJs, &xDummy, 0);
        // キューへ再送信
        xQueueSend(s_xQueueJs, &xMessage, 0);
    }

    return;
}

/**
 * @brief キュー受信処理(Js->Ctl)
 */
ErType_t xReciveQueJs(QueueSetMemberHandle_t *pxQueSetMember, MessageCtlJs_t *pxMessage)
{
    if (s_xQueueJs == *pxQueSetMember)
    {
        xQueueReceive(*pxQueSetMember, pxMessage, 0);
        if (UID_CTL == pxMessage->Head_t.xDstId)
        {
            vProcCtlCmdJs(&(pxMessage->Data_t));
            return ER_OK;
        }
        else
        {
            return ER_PARAM;
        }
    }

    return ER_STATUS;
}

/**
 * @brief ジョイスティック処理(Ctl->Mouse)
 */
static void vProcCtlCmdJs(JsMessage_t *pxData_t)
{
    vPrintCtlJs("run", __func__, __FILE__);

    // +0 - +4095 => -128 - +127    : /16
    // +0 - +4095 => -64 - +63      : /32           oo
    // +0 - +4095 => -32 - +31      : /64           ooo
    // +0 - +4095 => -16 - +15      : /128          o
    // +0 - +4095 => -8 - +7      : /256            x
    // +0 - +4095 => -4 - +3      : /512            xxx
    // +0 - +4095 => -2 - +1      : /1024
    int32_t slConvX, slConvY;

    slConvX = (int32_t)((pxData_t->slAxisX - 2048) / 128);
    slConvY = (-1) * (int32_t)((pxData_t->slAxisY - 2048) / 128);

    xSendMouseQueue_MoveXy(UID_CTL, (int8_t)slConvX, (int8_t)slConvY);

    return;
}

#undef _CTLTASK_JS_C_
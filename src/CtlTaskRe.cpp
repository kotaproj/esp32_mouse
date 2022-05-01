/**
 * @file CtlTaskRe.c
 * @brief 全体管理タスク - ロータリーエンコーダ処理
 */
#include "CtlTaskRe.h"
#include "TaskMouse.h"

/// @cond
#define _CTLTASK_RE_C_
/// @endcond

/* DECLARATIONS ***************************************************************/
#define DEBUG_CTL_TASK_RE 1
#if DEBUG_CTL_TASK_RE
void vPrintCtlRe(const char *msg, const char *func, const char *file) { Serial.printf("%s - %s() : %s\n", file, func, msg); }
#else
void vPrintCtlRe(const char *msg, const char *func, const char *file) {}
#endif
#define CTL_QUEUE_LENGTH_RE (4U) ///< ロータリーエンコーダキューバッファ数

/* VARIABLES ******************************************************************/
static xQueueHandle s_xQueueRe; ///< ロータリーエンコーダキューハンドラ

/* PRIVATE PROTOTYPES *********************************************************/
static void vProcCtlCmdRe(ReMessage_t *pxData_t);

/* CODE ***********************************************************************/

/**
 * @brief 初期化-タスク生成前
 */
ErType_t xInitCtlRe(QueueSetHandle_t *pxQueSetHandle)
{
    s_xQueueRe = xQueueCreate(CTL_QUEUE_LENGTH_RE, sizeof(MessageCtlRe_t));
    if (NULL == s_xQueueRe)
    {
        return ER_FAIL;
    }

    if (pdPASS != xQueueAddToSet(s_xQueueRe, *pxQueSetHandle))
    {
        return ER_FAIL;
    }

    return ER_OK;
}

/**
 * @brief ワークエリア初期化-タスク生成後
 */
ErType_t xInitWorkCtlRe(void)
{
    return ER_OK;
}

/**
 * @brief キューの深さ取得
 */
uint32_t ulGetQueLengthCtlRe(void)
{
    return CTL_QUEUE_LENGTH_RE;
}

/**
 * @brief キュー送信処理(Js->Ctl)
 */
void vCallbackCtlMsgRe(UniId_t xSrcId, UniId_t xDstId, void *pvParam)
{
    MessageCtlRe_t xMessage;

    //ヘッダ情報
    xMessage.Head_t.xSrcId = xSrcId;
    xMessage.Head_t.xDstId = xDstId;

    //データ情報
    memcpy(&xMessage.Data_t, (ReMessage_t *)pvParam, sizeof(ReMessage_t));

    //キューの送信
    if (pdTRUE != xQueueSend(s_xQueueRe, &xMessage, 0))
    {
        MessageCtlRe_t xDummy;

        // 送信できないときは、古いデータを削除
        // キューの空読み
        xQueueReceive(s_xQueueRe, &xDummy, 0);
        // キューへ再送信
        xQueueSend(s_xQueueRe, &xMessage, 0);
    }

    return;
}

/**
 * @brief キュー受信処理(Re->Ctl)
 */
ErType_t xReciveQueRe(QueueSetMemberHandle_t *pxQueSetMember, MessageCtlRe_t *pxMessage)
{
    if (s_xQueueRe == *pxQueSetMember)
    {
        xQueueReceive(*pxQueSetMember, pxMessage, 0);
        if (UID_CTL == pxMessage->Head_t.xDstId)
        {
            vProcCtlCmdRe(&(pxMessage->Data_t));
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
 * @brief ロータリーエンコーダ処理(Ctl->Mouse)
 */
static void vProcCtlCmdRe(ReMessage_t *pxData_t)
{
    vPrintCtlRe("run", __func__, __FILE__);
    xSendMouseQueue_Wheel(UID_CTL, (int8_t)pxData_t->slCount);
    return;
}

#undef _CTLTASK_RE_C_
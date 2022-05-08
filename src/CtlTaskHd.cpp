/**
 * @file CtlTaskHd.c
 * @brief 全体管理タスク - HD処理
 */
#include "CtlTaskHd.h"
#include "TaskMouse.h"

/// @cond
#define _CTLTASK_HD_C_
/// @endcond

/* DECLARATIONS ***************************************************************/
#define DEBUG_CTL_TASK_HD 0
#if DEBUG_CTL_TASK_HD
void vPrintCtlHd(const char *msg, const char *func, const char *file) { Serial.printf("%s - %s() : %s\n", file, func, msg); }
#else                            // DEBUG_CTL_TASK
void vPrintCtlHd(const char *msg, const char *func, const char *file) {}
#endif                           // DEBUG_CTL_TASK_HD
#define CTL_QUEUE_LENGTH_HD (1U) ///< フロントジョイスティックキューバッファ数

/* VARIABLES ******************************************************************/
static xQueueHandle s_xQueueHd; ///< フロントジョイスティックキューハンドラ

/* PRIVATE PROTOTYPES *********************************************************/
static void vProcCtlCmdHd(HdMessage_t *pxData_t);

/* CODE ***********************************************************************/

/**
 * @brief 初期化-タスク生成前
 */
ErType_t xInitCtlHd(QueueSetHandle_t *pxQueSetHandle)
{
    s_xQueueHd = xQueueCreate(CTL_QUEUE_LENGTH_HD, sizeof(MessageCtlHd_t));
    if (NULL == s_xQueueHd)
    {
        return ER_FAIL;
    }

    if (pdPASS != xQueueAddToSet(s_xQueueHd, *pxQueSetHandle))
    {
        return ER_FAIL;
    }

    return ER_OK;
}

/**
 * @brief ワークエリア初期化-タスク生成後
 */
ErType_t xInitWorkCtlHd(void)
{
    return ER_OK;
}

/**
 * @brief キューの深さ取得
 */
uint32_t ulGetQueLengthCtlHd(void)
{
    return CTL_QUEUE_LENGTH_HD;
}

/**
 * @brief キュー送信処理(Hd->Ctl)
 */
void vCallbackCtlMsgHd(UniId_t xSrcId, UniId_t xDstId, void *pvParam)
{
    MessageCtlHd_t xMessage;

    //ヘッダ情報
    xMessage.Head_t.xSrcId = xSrcId;
    xMessage.Head_t.xDstId = xDstId;

    //データ情報
    memcpy(&xMessage.Data_t, (HdMessage_t *)pvParam, sizeof(HdMessage_t));

    //キューの送信
    if (pdTRUE != xQueueSend(s_xQueueHd, &xMessage, 0))
    {
        MessageCtlHd_t xDummy;

        // 送信できないときは、古いデータを削除
        // キューの空読み
        xQueueReceive(s_xQueueHd, &xDummy, 0);
        // キューへ再送信
        xQueueSend(s_xQueueHd, &xMessage, 0);
    }

    return;
}

/**
 * @brief キュー受信処理(Hd->Ctl)
 */
ErType_t xReciveQueHd(QueueSetMemberHandle_t *pxQueSetMember, MessageCtlHd_t *pxMessage)
{
    if (s_xQueueHd == *pxQueSetMember)
    {
        xQueueReceive(*pxQueSetMember, pxMessage, 0);
        if (UID_CTL == pxMessage->Head_t.xDstId)
        {
            vProcCtlCmdHd(&(pxMessage->Data_t));
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
 * @brief Hd処理(Ctl->Mouse)
 */
static void vProcCtlCmdHd(HdMessage_t *pxData_t)
{
    vPrintCtlHd("run", __func__, __FILE__);

    switch (pxData_t->u8Code)
    {
    case MOUSE_CODE_CLICK:
    case MOUSE_CODE_PRESS:
    case MOUSE_CODE_RELEASE:
        xSendMouseQueue_Code(UID_CTL, pxData_t->u8Code, pxData_t->u8Type);
        break;
    case MOUSE_CODE_MOVE:
        if (0 != pxData_t->s32wheel)
        {
            xSendMouseQueue_Wheel(UID_CTL, pxData_t->s32wheel);
        }
        else
        {
            xSendMouseQueue_MoveXy(UID_CTL, pxData_t->s32x, pxData_t->s32y, pxData_t->u8Linear, pxData_t->s32Step, pxData_t->s32Delay);
        }
        break;
    case MOUSE_CODE_SP:
        break;
    default:
        break;
    }

    return;
}

#undef _CTLTASK_HD_C_
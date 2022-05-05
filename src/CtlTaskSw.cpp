/**
 * @file CtlTaskSw.c
 * @brief 全体管理タスク - スイッチ処理
 */
#include "CtlTaskSw.h"
#include "TaskMouse.h"

/// @cond
#define _CTLTASK_SW_C_
/// @endcond

/* DECLARATIONS ***************************************************************/
#define DEBUG_CTL_TASK_SW 1
#if DEBUG_CTL_TASK_SW
void vPrintCtlSw(const char *msg, const char *func, const char *file) { Serial.printf("%s - %s() : %s\n", file, func, msg); }
#else                            // DEBUG_CTL_TASK
void vPrintCtlSw(const char *msg, const char *func, const char *file) {}
#endif                           // DEBUG_CTL_TASK_SW
#define CTL_QUEUE_LENGTH_SW (1U) ///< フロントスイッチキューバッファ数

/* VARIABLES ******************************************************************/
static xQueueHandle s_xQueueSw; ///< フロントスイッチキューハンドラ

/* PRIVATE PROTOTYPES *********************************************************/
static void vProcCtlCmdSw(SwMessage_t *pxData_t);
static void vProcCtlCmdSw_NONE(uint32_t ulCode);
static void vProcCtlCmdSw_DOWN(uint32_t ulCode);
static void vProcCtlCmdSw_UP(uint32_t ulCode);

/* CODE ***********************************************************************/

/**
 * @brief 初期化-タスク生成前
 */
ErType_t xInitCtlSw(QueueSetHandle_t *pxQueSetHandle)
{
    s_xQueueSw = xQueueCreate(CTL_QUEUE_LENGTH_SW, sizeof(MessageCtlSw_t));
    if (NULL == s_xQueueSw)
    {
        return ER_FAIL;
    }

    if (pdPASS != xQueueAddToSet(s_xQueueSw, *pxQueSetHandle))
    {
        return ER_FAIL;
    }

    return ER_OK;
}

/**
 * @brief ワークエリア初期化-タスク生成後
 */
ErType_t xInitWorkCtlSw(void)
{
    return ER_OK;
}

/**
 * @brief キューの深さ取得
 */
uint32_t ulGetQueLengthCtlSw(void)
{
    return CTL_QUEUE_LENGTH_SW;
}

/**
 * @brief キュー送信処理(Sw->Ctl)
 */
void vCallbackCtlMsgSw(UniId_t xSrcId, UniId_t xDstId, void *pvParam)
{
    MessageCtlSw_t xMessage;

    //ヘッダ情報
    xMessage.Head_t.xSrcId = xSrcId;
    xMessage.Head_t.xDstId = xDstId;

    //データ情報
    memcpy(&xMessage.Data_t, (SwMessage_t *)pvParam, sizeof(SwMessage_t));

    //キューの送信
    if (pdTRUE != xQueueSend(s_xQueueSw, &xMessage, 0))
    {
        MessageCtlSw_t xDummy;

        // 送信できないときは、古いデータを削除
        // キューの空読み
        xQueueReceive(s_xQueueSw, &xDummy, 0);
        // キューへ再送信
        xQueueSend(s_xQueueSw, &xMessage, 0);
    }

    return;
}

/**
 * @brief キュー受信処理(Sw->Ctl)
 */
ErType_t xReciveQueSw(QueueSetMemberHandle_t *pxQueSetMember, MessageCtlSw_t *pxMessage)
{
    if (s_xQueueSw == *pxQueSetMember)
    {
        xQueueReceive(*pxQueSetMember, pxMessage, 0);
        if (UID_CTL == pxMessage->Head_t.xDstId)
        {
            vProcCtlCmdSw(&(pxMessage->Data_t));
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
 * @brief スイッチ処理
 */
static void vProcCtlCmdSw(SwMessage_t *pxData_t)
{
    vPrintCtlSw("run", __func__, __FILE__);

    switch (pxData_t->ulType)
    {
    case SW_TYPE_NONE:
        vProcCtlCmdSw_NONE(pxData_t->ulCode);
        break;

    case SW_TYPE_DOWN:
        vProcCtlCmdSw_DOWN(pxData_t->ulCode);
        break;

    case SW_TYPE_UP:
        vProcCtlCmdSw_UP(pxData_t->ulCode);
        break;

    default:
        vPrintCtlSw("vProcCtlCmdSw_DEFAULT", __func__, __FILE__);
        break;
    }

    return;
}

/**
 * @brief スイッチ処理 - None
 */
static void vProcCtlCmdSw_NONE(uint32_t ulCode)
{
    switch (ulCode)
    {
    case SW_CODE_NO1:
        vPrintCtlSw("vProcCtlCmdSw_NONE_CODE_NO1", __func__, __FILE__);
        break;

    case SW_CODE_NO2:
        vPrintCtlSw("vProcCtlCmdSw_NONE_CODE_NO2", __func__, __FILE__);
        break;

    case SW_CODE_NO3:
        vPrintCtlSw("vProcCtlCmdSw_NONE_CODE_NO3", __func__, __FILE__);
        break;

    case SW_CODE_NO4:
        vPrintCtlSw("vProcCtlCmdSw_NONE_CODE_NO4", __func__, __FILE__);
        break;

    case SW_CODE_NO5:
        vPrintCtlSw("vProcCtlCmdSw_NONE_CODE_NO5", __func__, __FILE__);
        break;

    case SW_CODE_NO6:
        vPrintCtlSw("vProcCtlCmdSw_NONE_CODE_NO6", __func__, __FILE__);
        break;

    case SW_CODE_NO7:
        vPrintCtlSw("vProcCtlCmdSw_NONE_CODE_NO7", __func__, __FILE__);
        break;

    case SW_CODE_NO8:
        vPrintCtlSw("vProcCtlCmdSw_NONE_CODE_NO8", __func__, __FILE__);
        break;

    default:
        vPrintCtlSw("vProcCtlCmdSw_NONE_CODE_DEFAULT", __func__, __FILE__);
        break;
    }

    return;
}

/**
 * @brief スイッチ処理 - Down
 */
static void vProcCtlCmdSw_DOWN(uint32_t ulCode)
{
    switch (ulCode)
    {
    case SW_CODE_NO1:
        vPrintCtlSw("vProcCtlCmdSw_DOWN_CODE_NO1", __func__, __FILE__);
        xSendMouseQueue_Code(UID_CTL, MOUSE_CODE_PRESS, MOUSE_LEFT);
        break;

    case SW_CODE_NO2:
        vPrintCtlSw("vProcCtlCmdSw_DOWN_CODE_NO2", __func__, __FILE__);
        xSendMouseQueue_Wheel(UID_CTL, 8);
        break;

    case SW_CODE_NO3:
        vPrintCtlSw("vProcCtlCmdSw_DOWN_CODE_NO3", __func__, __FILE__);
        xSendMouseQueue_Wheel(UID_CTL, -8);
        break;

    case SW_CODE_NO4:
        vPrintCtlSw("vProcCtlCmdSw_DOWN_CODE_NO4", __func__, __FILE__);
        xSendMouseQueue_Code(UID_CTL, MOUSE_CODE_PRESS, MOUSE_RIGHT);
        break;

    case SW_CODE_NO5:
        vPrintCtlSw("vProcCtlCmdSw_DOWN_CODE_NO5", __func__, __FILE__);
        break;

    case SW_CODE_NO6:
        vPrintCtlSw("vProcCtlCmdSw_DOWN_CODE_NO6", __func__, __FILE__);
        break;

    case SW_CODE_NO7:
        vPrintCtlSw("vProcCtlCmdSw_DOWN_CODE_NO7", __func__, __FILE__);
        break;

    case SW_CODE_NO8:
        vPrintCtlSw("vProcCtlCmdSw_DOWN_CODE_NO8", __func__, __FILE__);
        break;

    default:
        vPrintCtlSw("vProcCtlCmdSw_DOWN_CODE_DEFAULT", __func__, __FILE__);
        break;
    }

    return;
}

/**
 * @brief スイッチ処理 - Up
 */
static void vProcCtlCmdSw_UP(uint32_t ulCode)
{
    switch (ulCode)
    {
    case SW_CODE_NO1:
        vPrintCtlSw("vProcCtlCmdSw_UP_CODE_NO1", __func__, __FILE__);
        xSendMouseQueue_Code(UID_CTL, MOUSE_CODE_RELEASE, MOUSE_LEFT);
        break;

    case SW_CODE_NO2:
        vPrintCtlSw("vProcCtlCmdSw_UP_CODE_NO2", __func__, __FILE__);
        break;

    case SW_CODE_NO3:
        vPrintCtlSw("vProcCtlCmdSw_UP_CODE_NO3", __func__, __FILE__);
        break;

    case SW_CODE_NO4:
        vPrintCtlSw("vProcCtlCmdSw_UP_CODE_NO4", __func__, __FILE__);
        xSendMouseQueue_Code(UID_CTL, MOUSE_CODE_RELEASE, MOUSE_RIGHT);
        break;

    case SW_CODE_NO5:
        vPrintCtlSw("vProcCtlCmdSw_UP_CODE_NO5", __func__, __FILE__);
        break;

    case SW_CODE_NO6:
        vPrintCtlSw("vProcCtlCmdSw_UP_CODE_NO6", __func__, __FILE__);
        break;

    case SW_CODE_NO7:
        vPrintCtlSw("vProcCtlCmdSw_UP_CODE_NO7", __func__, __FILE__);
        break;

    case SW_CODE_NO8:
        vPrintCtlSw("vProcCtlCmdSw_UP_CODE_NO8", __func__, __FILE__);
        break;

    default:
        vPrintCtlSw("vProcCtlCmdSw_UP_CODE_DEFAULT", __func__, __FILE__);
        break;
    }

    return;
}

#undef _CTLTASK_SW_C_
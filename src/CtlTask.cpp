/**
 * @file CtlTask.c
 * @brief 全体管理タスク
 */
#include "CtlTask.h"
#include "ApiJs.h"
#include "ApiRe.h"
#include "ApiSw.h"
#include "CtlTaskJs.h"
#include "CtlTaskRe.h"
#include "CtlTaskSw.h"

/// @cond
#define _CTLTASK_C_
/// @endcond

/* DECLARATIONS ***************************************************************/
#define DEBUG_CTL_TASK (1)
#if DEBUG_CTL_TASK
void vPrintCtl(const char *msg, const char *func, const char *file) { Serial.printf("%s - %s : %s\n", file, func, msg); }
#else  // DEBUG_CTL_TASK
void vPrintCtl(const char *msg, const char *func, const char *file) {}
#endif // DEBUG_CTL_TASK

#define CTL_TASK_STACK (8192U)                       ///< コントロールタスクに割り当てるスタック容量
#define CTL_TASK_PRIORITY (configMAX_PRIORITIES - 3) ///< コントロールタスクの優先順位

/* VARIABLES ******************************************************************/
static QueueSetHandle_t s_xQueueSetCtl; ///< 受信管理用キューセット

/* PRIVATE PROTOTYPES *********************************************************/
static ErType_t xInitWorkCtl(void);
static void vCtlTask(void *pvParameters);

/**
 * @brief 全体管理タスク初期化関数
 */
ErType_t xInitCtl(void)
{
    uint32_t ulQueSetLength = 0;

    // キューセットレングスの取得
    ulQueSetLength += ulGetQueLengthCtlSw();
    ulQueSetLength += ulGetQueLengthCtlRe();
    ulQueSetLength += ulGetQueLengthCtlJs();

    // キューセットの作成
    s_xQueueSetCtl = xQueueCreateSet(ulQueSetLength);
    if (NULL == s_xQueueSetCtl)
    {
        return ER_FAIL;
    }

    // 各キューの生成, キューセットへ登録
    ErType_t xErType = ER_OK;

    //キュー - トリガー - SWの生成と登録
    xErType = xInitCtlSw(&s_xQueueSetCtl);
    if (ER_OK != xErType)
    {
        return xErType;
    }

    //キュー - REの生成と登録
    xErType = xInitCtlRe(&s_xQueueSetCtl);
    if (ER_OK != xErType)
    {
        return xErType;
    }

    //キュー - JSの生成と登録
    xErType = xInitCtlJs(&s_xQueueSetCtl);
    if (ER_OK != xErType)
    {
        return xErType;
    }

    // 全体制御タスクの生成
    xTaskCreatePinnedToCore(vCtlTask, "vCtlTask", CTL_TASK_STACK, NULL, CTL_TASK_PRIORITY, NULL, ARDUINO_RUNNING_CORE);

    return xErType;
}

/**
 * @brief ワークエリアの初期化(タスク生成後)
 */
static ErType_t xInitWorkCtl(void)
{
    // 各キューの生成, キューセットへ登録
    ErType_t xErType = ER_OK;

    //ワークエリアの初期化, 送信コールバック関数の登録 - フロントスイッチキュー
    xErType = xInitWorkCtlSw();
    if (ER_OK != xErType)
    {
        return xErType;
    }
    //ワークエリアの初期化, 送信コールバック関数の登録 - ロータリーエンコーダキュー
    xErType = xInitWorkCtlRe();
    if (ER_OK != xErType)
    {
        return xErType;
    }
    //ワークエリアの初期化, 送信コールバック関数の登録 - ジョイスティックキュー
    xErType = xInitWorkCtlJs();
    if (ER_OK != xErType)
    {
        return xErType;
    }

    return xErType;
}

/**
 * @brief 全体管理タスク
 */
static void vCtlTask(void *pvParameters)
{
    static QueueSetMemberHandle_t s_xActivatedMember;
    static MessageCtl_t s_xMessage;
    ErType_t xErType = ER_OK;

    // ワークエリア初期化処理
    xErType = xInitWorkCtl();

    //メインループ
    while (1)
    {
        vPrintCtl("xQueueSelectFromSet", __func__, __FILE__);
        // キュー待ち
        s_xActivatedMember = xQueueSelectFromSet(s_xQueueSetCtl, portMAX_DELAY);

        // トリガースイッチイベント動作
        xErType = xReciveQueSw(&s_xActivatedMember, (MessageCtlSw_t *)&s_xMessage);
        if (ER_OK == xErType)
        {
            vPrintCtl("xReciveQueSw", __func__, __FILE__);
            continue;
        }

        // トリガージョイスティックイベント動作
        xErType = xReciveQueJs(&s_xActivatedMember, (MessageCtlJs_t *)&s_xMessage);
        if (ER_OK == xErType)
        {
            vPrintCtl("xReciveQueJs", __func__, __FILE__);
            continue;
        }

        // トリガースイッチイベント動作
        xErType = xReciveQueRe(&s_xActivatedMember, (MessageCtlRe_t *)&s_xMessage);
        if (ER_OK == xErType)
        {
            vPrintCtl("xReciveQueRe", __func__, __FILE__);
            continue;
        }
    }
}

#undef _CTLTASK_C_
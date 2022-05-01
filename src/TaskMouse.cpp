/**
 * @file TaskMouse.cpp
 * @brief マウス管理タスク
 * @author kotaproj
 */
#include "TaskMouse.h"

/// @cond
#define _TASKMOUSE_C_
/// @endcond

/* DECLARATIONS ***************************************************************/
#define DEBUG_MOUSE_TASK (1)
#define ROUTE_MOUSE_TASK (1)

#define MOUSE_TASK_STACK (8192U)                       ///< タスクに割り当てるスタック容量
#define MOUSE_TASK_PRIORITY (configMAX_PRIORITIES - 6) ///< タスクの優先順位
#define MOUSE_QUEUE_LENGTH (4)                         ///< キューの深さ

/* VARIABLES ******************************************************************/
static BleMouse s_xBleMouse;
static QueueHandle_t s_xQueueMouse; ///< 受信管理用キューセット

/* PRIVATE PROTOTYPES *********************************************************/
static ErType_t xInitWorkMouse(void);
static void vTaskMouse(void *pvParameters);
static ErType_t xProcMouseCode(MouseMessage_t *pxMessage);
static ErType_t xProcMouseCode_CLICK(MouseMessage_t *pxMessage);
static ErType_t xProcMouseCode_MOVE(MouseMessage_t *pxMessage);
static ErType_t xProcMouseCode_PRESS(MouseMessage_t *pxMessage);
static ErType_t xProcMouseCode_RELEASE(MouseMessage_t *pxMessage);
static ErType_t xProcMouseCode_SP(MouseMessage_t *pxMessage);

/* TABLES ***************************************************************/
typedef struct
{
    MouseCode_e eMouseCode;
    ErType_t (*xFunc)(MouseMessage_t *);
} MouseCodeFunt_t;

const MouseCodeFunt_t c_xaMouseCodeFunt_t[] = {
    {MOUSE_CODE_CLICK, xProcMouseCode_CLICK},
    {MOUSE_CODE_MOVE, xProcMouseCode_MOVE},
    {MOUSE_CODE_PRESS, xProcMouseCode_PRESS},
    {MOUSE_CODE_RELEASE, xProcMouseCode_RELEASE},
    {MOUSE_CODE_SP, xProcMouseCode_SP},
};

/**
 * @brief タスク初期化関数
 */
ErType_t xInitMouse(void)
{
    // begin - Mouse
    s_xBleMouse.begin();

    // キューセットの作成
    s_xQueueMouse = xQueueCreate(MOUSE_QUEUE_LENGTH, sizeof(MessageCtlMouse_t));
    if (NULL == s_xQueueMouse)
    {
        return ER_FAIL;
    }

    // タスクの生成
    xTaskCreatePinnedToCore(
        vTaskMouse,
        "vTaskMouse",
        MOUSE_TASK_STACK,
        NULL,
        MOUSE_TASK_PRIORITY,
        NULL,
        ARDUINO_RUNNING_CORE);

    return ER_OK;
}

/**
 * @brief ワークエリアの初期化(タスク生成後)
 */
static ErType_t xInitWorkMouse(void)
{
    return ER_OK;
}

/**
 * @brief タスク処理
 */
static void vTaskMouse(void *pvParameters)
{
    static MessageCtlMouse_t s_xMessage;
    ErType_t xErType = ER_OK;

    // ワークエリア初期化処理
    xErType = xInitWorkMouse();

    //メインループ
    while (1)
    {
        if (pdPASS == xQueueReceive(s_xQueueMouse, &s_xMessage, portMAX_DELAY))
        {
            if (s_xBleMouse.isConnected())
            {
                xErType = xProcMouseCode(&s_xMessage.Data_t);
                if (ER_OK != xErType)
                {
                    Serial.print(__func__);
                    Serial.print("MOUSE_ERROR:");
                    Serial.println(xErType);
                }
            }
        }
    }

    return;
}

/**
 * @brief マウス処理
 */
static ErType_t xProcMouseCode(MouseMessage_t *pxMessage)
{
    for (uint16_t usCnt = 0; usCnt < sizeof(c_xaMouseCodeFunt_t) / sizeof(MouseCodeFunt_t); usCnt++)
    {
        if (c_xaMouseCodeFunt_t[usCnt].eMouseCode == pxMessage->u8Code)
        {
            return c_xaMouseCodeFunt_t[usCnt].xFunc(pxMessage);
        }
    }

    return ER_FAIL;
}

/**
 * @brief コードイベント - CLICK
 */
static ErType_t xProcMouseCode_CLICK(MouseMessage_t *pxMessage)
{
#if ROUTE_MOUSE_TASK
    Serial.printf("%s - run\n", __func__);
#endif

    if (0 == (MOUSE_ALL & pxMessage->u8Type))
    {
        Serial.printf("%s - error type : %d\n", __func__, pxMessage->u8Type);
        return ER_PARAM;
    }

    s_xBleMouse.click(pxMessage->u8Type);

    return ER_OK;
}

/**
 * @brief コードイベント - MOVE
 */
static ErType_t xProcMouseCode_MOVE(MouseMessage_t *pxMessage)
{
#if ROUTE_MOUSE_TASK
    Serial.printf("%s - run - (%d, %d)\n", __func__, pxMessage->s8x, pxMessage->s8y);
#endif

    int32_t max_count;
    int8_t step;
    if (pxMessage->wheel > 0)
    {
        max_count = pxMessage->wheel;
        step = 1;
    }
    else
    {
        max_count = (-1) * pxMessage->wheel;
        step = -1;
    }

    if (0 != pxMessage->wheel || 0 != pxMessage->hWheel)
    {
        for (int32_t i = 0; i < max_count; i++)
        {
            s_xBleMouse.move(0, 0, step, step);
            delay(5);
        }
    }
    else
    {
        s_xBleMouse.move(pxMessage->s8x, pxMessage->s8y, pxMessage->wheel, pxMessage->hWheel);
    }

    return ER_OK;
}

/**
 * @brief コードイベント - PRESS
 */
static ErType_t xProcMouseCode_PRESS(MouseMessage_t *pxMessage)
{
#if ROUTE_MOUSE_TASK
    Serial.printf("%s - run\n", __func__);
#endif

    if (0 == (MOUSE_ALL & pxMessage->u8Type))
    {
        Serial.printf("%s - error type : %d\n", __func__, pxMessage->u8Type);
        return ER_PARAM;
    }

    s_xBleMouse.press(pxMessage->u8Type);

    return ER_OK;
}

/**
 * @brief コードイベント - RELEASE
 */
static ErType_t xProcMouseCode_RELEASE(MouseMessage_t *pxMessage)
{
#if ROUTE_MOUSE_TASK
    Serial.printf("%s - run\n", __func__);
#endif

    if (0 == (MOUSE_ALL & pxMessage->u8Type))
    {
        Serial.printf("%s - error type : %d\n", __func__, pxMessage->u8Type);
        return ER_PARAM;
    }

    s_xBleMouse.release(pxMessage->u8Type);

    return ER_OK;
}

/**
 * @brief コードイベント - SP
 */
static ErType_t xProcMouseCode_SP(MouseMessage_t *pxMessage)
{
#if ROUTE_MOUSE_TASK
    Serial.printf("%s - run\n", __func__);
#endif

    ErType_t eRet = ER_OK;

    return eRet;
}

/**
 * @brief キュー送信処理(Ctl->Mouse)-for CLICK/PRESS/RELEASE
 */
ErType_t xSendMouseQueue_Code(UniId_t xSrcId, uint8_t u8Code, uint8_t u8Type)
{
    MouseMessage_t xData;

    xData.u8Code = u8Code;
    xData.u8Type = u8Type;
    xData.s8x = 0;
    xData.s8y = 0;
    xData.wheel = 0;
    xData.hWheel = 0;
    xData.u8SpType = 0;

    return xSendMouseQueue(xSrcId, UID_MOUSE, &xData);
}

/**
 * @brief キュー送信処理(Ctl->Mouse)-for MOVE
 */
ErType_t xSendMouseQueue_MoveXy(UniId_t xSrcId, int8_t s8x, int8_t s8y)
{
    static MouseMessage_t xData;

    xData.u8Code = MOUSE_CODE_MOVE;
    xData.u8Type = 0;
    xData.s8x = s8x;
    xData.s8y = s8y;
    xData.wheel = 0;
    xData.hWheel = 0;
    xData.u8SpType = 0;

    return xSendMouseQueue(xSrcId, UID_MOUSE, &xData);
}

/**
 * @brief キュー送信処理(Ctl->Mouse)-for wheel
 */
ErType_t xSendMouseQueue_Wheel(UniId_t xSrcId, int8_t wheel)
{
    static MouseMessage_t xData;

    xData.u8Code = MOUSE_CODE_MOVE;
    xData.u8Type = 0;
    xData.s8x = 0;
    xData.s8y = 0;
    // xData.wheel = wheel;
    // xData.hWheel = 0;
    // xData.wheel = 0;
    // xData.hWheel = wheel;
    xData.wheel = wheel;
    xData.hWheel = wheel;
    xData.u8SpType = 0;

    return xSendMouseQueue(xSrcId, UID_MOUSE, &xData);
}

/**
 * @brief キュー送信処理(Ctl->Mouse)-for SP
 */
ErType_t xSendMouseQueue_Sp(UniId_t xSrcId, uint8_t u8Type)
{
    MouseMessage_t xData;

    xData.u8Code = MOUSE_CODE_SP;
    xData.u8Type = u8Type;
    xData.s8x = 0;
    xData.s8y = 0;
    xData.wheel = 0;
    xData.hWheel = 0;
    xData.u8SpType = 0;

    return xSendMouseQueue(xSrcId, UID_MOUSE, &xData);
}

/**
 * @brief キュー送信処理(Ctl->Mouse)
 */
ErType_t xSendMouseQueue(UniId_t xSrcId, UniId_t xDstId, void *pvParam)
{
    MessageCtlMouse_t xMessage;

    xMessage.Head_t.xSrcId = xSrcId;
    xMessage.Head_t.xDstId = xDstId;

    memcpy(&xMessage.Data_t, pvParam, sizeof(xMessage.Data_t));

    if (pdPASS == xQueueSend(s_xQueueMouse, (void *)&xMessage, 0))
    {
        return ER_OK;
    }

    return ER_FAIL;
}

#if DEBUG_MOUSE_TASK
static void vDummyTask(void *pvParameters)
{
    Serial.println("run");
    vTaskDelay(1000 / portTICK_RATE_MS);

    //メインループ
    static MouseMessage_t xData;

    xData.u8Code = MOUSE_CODE_CLICK;
    xData.u8Type = MOUSE_LEFT;
    xData.s8x = 0;
    xData.s8y = 0;
    xData.wheel = 0;
    xData.hWheel = 0;
    memset(&xData.ucReserve[0], 0x00, sizeof(xData.ucReserve));

    while (1)
    {
        xSendMouseQueue(UID_CTL, UID_MOUSE, &xData);
        vTaskDelay(5000 / portTICK_RATE_MS);

        xData.u8Code = MOUSE_CODE_MOVE;
        xData.u8Type = MOUSE_LEFT;
        for (int i = 0; i < 100; i += 10)
        {
            xData.s8x = i;
            xData.s8y = i;
            xSendMouseQueue(UID_CTL, UID_MOUSE, &xData);
            vTaskDelay(50 / portTICK_RATE_MS);
        }
        for (int i = 0; i < 100; i += 10)
        {
            xData.s8x = (-1) * i;
            xData.s8y = (-1) * i;
            xSendMouseQueue(UID_CTL, UID_MOUSE, &xData);
            vTaskDelay(50 / portTICK_RATE_MS);
        }
    }

    return;
}

/**
 * @brief テスト処理
 */
void setup_for_TaskMouse(void)
{
    // Serial.begin(115200);

    Serial.printf("%s - run\n", __func__);
    xTaskCreatePinnedToCore(
        vDummyTask, "vDummyTask" // A name just for humans
        ,
        8192 // This stack size can be checked & adjusted by reading the Stack Highwater
        ,
        NULL, 2 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        NULL, ARDUINO_RUNNING_CORE);

    xInitMouse();
}
#endif // DEBUG_MOUSE_TASK

#undef _TASKMOUSE_C_

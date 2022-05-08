/**
 * @file TaskHd.c
 * @brief HTTPd通信タスク
 */

#include "TaskHd.h"
#include "CtlTaskHd.h"
#include <WebServer.h>
#include <WiFi.h>

/// @cond
#define _INTASK_HD_C_
/// @endcond

/* DECLARATIONS ***************************************************************/
#define DEBUG_TASK_HD (0)
#define TASK_HD_STACK (8192U)                       ///< コントロールタスクに割り当てるスタック容量
#define TASK_HD_PRIORITY (configMAX_PRIORITIES - 6) ///< コントロールタスクの優先順位
#define TASK_HD_WAIT_INTERVAL (1)

/* Put your SSID & Password */
const char *ssid = "xxxxxxxxxxx";      // Enter SSID here
const char *password = "yyyyyyyyyyyy"; // Enter Password here

/* VARIABLES ******************************************************************/

/* Put IP Address details */
WebServer server(80);

/* PRIVATE PROTOTYPES *********************************************************/
static void vTaskHd(void *pvParameters);
void vHandleHdOnConnect(void);
void vHandleHdOnConnectMouse(void);
void vHandleHdNotFound(void);
static String sSendHdHTML(void);

/* TABLES ***************************************************************/

/**
 * @brief 初期化関数
 */
ErType_t xInitHd(void)
{
    ErType_t xErType = ER_OK;

    // 全体制御タスクの起動
    xTaskCreatePinnedToCore(
        vTaskHd,
        "vTaskHd",
        TASK_HD_STACK,
        NULL,
        TASK_HD_PRIORITY,
        NULL,
        ARDUINO_RUNNING_CORE);

    return xErType;
}

/**
 * @brief ワークエリアの初期化(タスク生成後)
 */
static ErType_t xInitWorkHd(void)
{
    WiFi.begin(ssid, password);

    vTaskDelay(1000 / portTICK_RATE_MS);

    while (WiFi.status() != WL_CONNECTED)
    {
        // WiFiに接続できているか
        vTaskDelay(1000 / portTICK_RATE_MS);
        Serial.print(".");
    }

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", vHandleHdOnConnect);
    server.on("/mouse", vHandleHdOnConnectMouse);
    server.onNotFound(vHandleHdNotFound);
    server.begin();

    Serial.println("HTTP server started");

    return ER_OK;
}

/**
 * @brief タスク処理
 */
static void vTaskHd(void *pvParameters)
{
    Serial.printf("%s : run\n", __func__);

    // ワークエリア初期化処理
    ErType_t xErType = xInitWorkHd();
    if (ER_OK != xErType)
    {
        Serial.printf("%s : xErType=%d\n", __func__, (int)xErType);
        while (1)
        {
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    }

    // 十分な待ち時間
    vTaskDelay(1000 / portTICK_RATE_MS);

    Serial.println("pre:vTaskHd_loop");
    //メインループ
    while (1)
    {
        // server
        server.handleClient();

        // ディスパッチ
        vTaskDelay(TASK_HD_WAIT_INTERVAL / portTICK_RATE_MS);
    }

    return;
}

/**
 * @brief [GET]http://ipadr:port/ - 接続時のレスポンス処理
 */
void vHandleHdOnConnect(void)
{
    Serial.println("run:vHandleHdOnConnect");
    server.send(200, "text/html", sSendHdHTML());
    Serial.println("over:vHandleHdOnConnect");
    return;
}

/**
 * @brief query-codeのパース
 */
static HdCode_e eParseHdCode(String msg)
{
    if (msg.equals("click"))
    {
        return HD_CODE_CLICK;
    }
    else if (msg.equals("move"))
    {
        return HD_CODE_MOVE;
    }
    else if (msg.equals("press"))
    {
        return HD_CODE_PRESS;
    }
    else if (msg.equals("release"))
    {
        return HD_CODE_RELEASE;
    }
    else if (msg.equals("sp"))
    {
        return HD_CODE_SP;
    }

    return HD_CODE_ERR;
}

/**
 * @brief query-typeのパース
 */
static HdType_e eParseHdType(String msg)
{
    if (msg.equals("left"))
    {
        return HD_TYPE_MOUSE_LEFT;
    }
    else if (msg.equals("right"))
    {
        return HD_TYPE_MOUSE_RIGHT;
    }
    else if (msg.equals("middle"))
    {
        return HD_TYPE_MOUSE_MIDDLE;
    }
    else if (msg.equals("back"))
    {
        return HD_TYPE_MOUSE_BACK;
    }
    else if (msg.equals("forward"))
    {
        return HD_TYPE_MOUSE_FORWARD;
    }
    else if (msg.equals("0x20"))
    {
        return HD_TYPE_MOUSE_0x20;
    }
    else if (msg.equals("0x40"))
    {
        return HD_TYPE_MOUSE_0x40;
    }
    else if (msg.equals("0x80"))
    {
        return HD_TYPE_MOUSE_0x80;
    }

    return HD_TYPE_MOUSE_LEFT;
}

/**
 * @brief query-数値のパース
 */
static int32_t slParseHdInt(String msg)
{
    if (0 == msg.length())
    {
        return 0;
    }

    return msg.toInt();
}

/**
 * @brief [GET]http://ipadr:port/mouse? - 接続時のレスポンス処理
 */
void vHandleHdOnConnectMouse(void)
{
    Serial.println("run:vHandleHdOnConnectMouse");

    static HdMessage_t s_xMessage;

    s_xMessage.u8Code = (uint8_t)eParseHdCode(server.arg("code"));
    s_xMessage.u8Type = (uint8_t)eParseHdType(server.arg("type"));
    s_xMessage.u8Linear = (uint8_t)slParseHdInt(server.arg("linear"));
    s_xMessage.s32x = slParseHdInt(server.arg("x"));
    s_xMessage.s32y = slParseHdInt(server.arg("y"));
    s_xMessage.s32wheel = slParseHdInt(server.arg("wheel"));
    s_xMessage.s32hWheel = s_xMessage.s32wheel;
    s_xMessage.u8SpType = 0;
    s_xMessage.s32Step = slParseHdInt(server.arg("step"));
    if (0 == s_xMessage.s32Step)
    {
        s_xMessage.s32Step = 10;
    }
    s_xMessage.s32Delay = slParseHdInt(server.arg("delay"));
    if (0 == s_xMessage.s32Delay)
    {
        s_xMessage.s32Delay = 10;
    }

    if (HD_CODE_ERR == (HdCode_e)s_xMessage.u8Code)
    {
        server.send(200, "text/html", "error : code\n\n");
        Serial.println("over:vHandleHdOnConnectMouse");
        return;
    }

    vCallbackCtlMsgHd(UID_HD, UID_CTL, &s_xMessage);

    // Serial.printf("code:%d, type:%d, x:%d, y:%d, wheel:%d\n", s_xMessage.u8Code, s_xMessage.u8Type, s_xMessage.s32x, s_xMessage.s32y, s_xMessage.s32wheel);

    String ptr = "";
    ptr += "u8Code:";
    ptr += String(s_xMessage.u8Code);
    ptr += "\n";
    ptr += "u8Type:";
    ptr += String(s_xMessage.u8Type);
    ptr += "\n";
    ptr += "u8Linear:";
    ptr += String(s_xMessage.u8Linear);
    ptr += "\n";
    ptr += "s32x:";
    ptr += String(s_xMessage.s32x);
    ptr += "\n";
    ptr += "s32y:";
    ptr += String(s_xMessage.s32y);
    ptr += "\n";
    ptr += "s32wheel:";
    ptr += String(s_xMessage.s32wheel);
    ptr += "\n";
    ptr += "s32hWheel:";
    ptr += String(s_xMessage.s32hWheel);
    ptr += "\n";
    ptr += "u8SpType:";
    ptr += String(s_xMessage.u8SpType);
    ptr += "\n";

    server.send(200, "text/html", ptr);
    Serial.println("over:vHandleHdOnConnectMouse");
    return;
}

/**
 * @brief 404のレスポンス処理
 */
void vHandleHdNotFound(void)
{
    Serial.println("run:vHandleHdNotFound");
    server.send(404, "text/plain", "Not found");
    Serial.println("over:vHandleHdNotFound");
    return;
}

/**
 * @brief HTML送信処理
 */
static String sSendHdHTML(void)
{
    String ptr = "<!DOCTYPE html> <html>\n";
    ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    ptr += "<title>Webhooks Board</title>\n";
    ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
    ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
    ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
    ptr += ".button-on {background-color: #3498db;}\n";
    ptr += ".button-on:active {background-color: #2980b9;}\n";
    ptr += ".button-off {background-color: #34495e;}\n";
    ptr += ".button-off:active {background-color: #2c3e50;}\n";
    ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
    ptr += "</style>\n";
    ptr += "</head>\n";
    ptr += "<body>\n";
    ptr += "<h1>Usage</h1>\n";

    ptr += "<h2>Examples</h2>\n";
    ptr += "<ul>\n";
    ptr += "  <li>Perform single tap : IP Address/mouse?code=click&type=left</li>\n";
    ptr += "  <li>movement : IP Address/mouse?code=move&x=-100&y=100</li>\n";
    ptr += "</ul>\n";

    ptr += "<h2>Note</h2>\n";
    ptr += "<a href=\"https:__zenn.dev/kotaproj/articles/esp32_mouse_httpd\">zenn-kotaproj</a>\n";

    ptr += "</body>\n";
    ptr += "</html>\n";

    return ptr;
}

#if DEBUG_TASK_HD
static void vDummyTask(void *pvParameters)
{
    Serial.println("run:vDummyTask");

    vTaskDelay(1000 / portTICK_RATE_MS);

    while (1)
    {
        // xSendHcQueue(UID_CTL, UID_HC, &xData);
        vTaskDelay(5000 / portTICK_RATE_MS);
    }

    return;
}

void setup_for_httpd(void)
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    delay(1000);

    xTaskCreatePinnedToCore(
        vDummyTask, "vDummyTask" // A name just for humans
        ,
        8192 // This stack size can be checked & adjusted by reading the Stack Highwater
        ,
        NULL, 2 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        NULL, ARDUINO_RUNNING_CORE);

    xInitHd();
}

#endif // DEBUG_TASK_HD

#undef _INTASK_HD_C_
/**
 * @file TaskHd.c
 * @brief HTTPd通信タスク
 * @author kotatuneco
 * @date 2019/11/25
 */
// #include "Debug.h"

#include <WebServer.h>
#include <WiFi.h>

#include "TaskHd.h"
// #include "ApiFs.h"
#include "CtlTaskHd.h"

/// @cond
#define _INTASK_HD_C_
/// @endcond

/* DECLARATIONS ***************************************************************/
#define DEBUG_TASK_HD (1)
#define TASK_HD_STACK (8192U)                       ///< コントロールタスクに割り当てるスタック容量
#define TASK_HD_PRIORITY (configMAX_PRIORITIES - 6) ///< コントロールタスクの優先順位
#define TASK_HD_WAIT_INTERVAL (1)

/* Put your SSID & Password */
const char *ssid = "Buffalo-G-6B00";    // Enter SSID here
const char *password = "a5auc4dehctdx"; // Enter Password here

/* VARIABLES ******************************************************************/

/* Put IP Address details */
WebServer server(80);

/* PRIVATE PROTOTYPES *********************************************************/
static void vTaskHd(void *pvParameters);
void vHandleHdOnConnect(void);
void vHandleHdOnConnectMouse(void);
void vHandleHdOnConnectForm(void);
void vHandleHdNotFound(void);
static String sSendHdHTML(void);
static String sSendHdHTML_Result(void);
static String sSendHdHTML_Result_Error(void);

/* TABLES ***************************************************************/

/**
 * @brief 全体管理タスク初期化関数
 * @param void
 * @return ErType_t
 * @author kotatuneco
 * @date 2019/11/25
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
 * @param void
 * @return ErType_t
 * @author kotatuneco
 * @date 2019/11/25
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
    server.on("/form", vHandleHdOnConnectForm);
    server.onNotFound(vHandleHdNotFound);
    server.begin();

    Serial.println("HTTP server started");

    return ER_OK;
}

/**
 * @brief 割り込みからのキュー送信処理
 * @param[in] pvParam : パラメータ
 * @return void
 * @author kotatuneco
 * @date 2019/11/25
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
 * @brief [GET]http://ipadr:port/接続時のレスポンス処理
 */
void vHandleHdOnConnect(void)
{
    Serial.println("run:vHandleHdOnConnect");
    server.send(200, "text/html", sSendHdHTML());
    Serial.println("over:vHandleHdOnConnect");
    return;
}

/**
 * @brief eParseHdCode
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
 * @brief eParseHdType
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

    return HD_TYPE_MOUSE_LEFT;
}

/**
 * @brief scParseHdInt
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
 * @brief [GET]http://ipadr:port/接続時のレスポンス処理
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
 * @brief [POST]http://ipadr:port/form接続時のレスポンス処理
 */
void vHandleHdOnConnectForm(void)
{
    Serial.println("run:vHandleHdOnConnectForm");
    // Serial.println(server.arg("ssid"));
    // Serial.println(server.arg("pass"));
    // Serial.println(server.arg("acskey"));

    // if (0 == server.arg("ssid").length() || 0 == server.arg("pass").length() || 0 == server.arg("acskey").length())
    // {
    //     Serial.println("error:vHandleHdOnConnectForm");
    //     server.send(200, "text/html", sSendHdHTML_Result_Error());
    //     Serial.println("over:vHandleHdOnConnectForm");
    //     return;
    // }

    // StrSsid = server.arg("ssid");
    // StrPass = server.arg("pass");
    // StrAcsKey = server.arg("acskey");

    Serial.println("<<<<<<<<");
    // Serial.println(StrSsid);
    // Serial.println(StrPass);
    // Serial.println(StrAcsKey);

    // ファイルシステム経由で書き込み
    // vSetFsSet_Ssid(StrSsid);
    // vSetFsSet_Pass(StrPass);
    // vSetFsSet_Acskey(StrAcsKey);

    // ファイルシステム書き込み
    // xWriteFsFile();

    // debug
    // xReadFsFile();
    // StrSsid = "Ssid";     // String((const char *)pcGetFsSet_Ssid());
    // StrPass = "Pass";     // String((const char *)pcGetFsSet_Pass());
    // StrAcsKey = "AcsKey"; // String((const char *)pcGetFsSet_Acskey());
    Serial.println("<-------");
    // Serial.println(StrSsid);
    // Serial.println(StrPass);
    // Serial.println(StrAcsKey);
    Serial.println("------->");

    // server.send(200, "text/html", sSendHdHTML());
    server.send(200, "text/html", sSendHdHTML_Result());
    Serial.println("over:vHandleHdOnConnectForm");
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
    ptr += "<h1>ESP32 Web Server</h1>\n";
    ptr += "</body>\n";
    ptr += "</html>\n";

    return ptr;
}

/**
 * @brief HTML送信処理
 */
static String sSendHdHTML_Result(void)
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
    ptr += "<h1>ESP32 Web Server</h1>\n";
    ptr += "<h3>Saved.</h3>\n";

    ptr += "</body>\n";
    ptr += "</html>\n";

    return ptr;
}

/**
 * @brief HTML送信処理 - Result - Error
 */
static String sSendHdHTML_Result_Error(void)
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
    ptr += "<h1>ESP32 Web Server</h1>\n";
    ptr += "<h3>Error.</h3>\n";
    ptr += "<h3><a href=\"http://192.168.1.1\">top page</a><h3>\n";
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

// void setup()
// {
//     BeginDebugPrint();
//     delay(1000);
//     DebugPrint("power on");

//     // ApiFs
//     // xInitFs();
//     // xResetFsFile();

//     xTaskCreatePinnedToCore(
//         vDummyTask, "vDummyTask" // A name just for humans
//         ,
//         8192 // This stack size can be checked & adjusted by reading the Stack Highwater
//         ,
//         NULL, 2 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
//         ,
//         NULL, ARDUINO_RUNNING_CORE);

//     xInitHd();
// }

// void loop()
// {
//     // put your main code here, to run repeatedly:
// }

#endif // DEBUG_TASK_HD

#undef _INTASK_HD_C_
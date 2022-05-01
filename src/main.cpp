#include <Arduino.h>

#include "ApiJs.h"
// #include "ApiRe.h"
#include "ApiSw.h"
#include "CtlTask.h"
#include "TaskMouse.h"

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);

    ErType_t eEr;

    eEr = xInitMouse();
    if (ER_OK != eEr)
    {
        Serial.printf("Error! : xInitMouse - %d\n", eEr);
    }
    eEr = xInitCtl();
    if (ER_OK != eEr)
    {
        Serial.printf("Error! : xInitCtl - %d\n", eEr);
    }
    eEr = xInitSw();
    if (ER_OK != eEr)
    {
        Serial.printf("Error! : xInitSw - %d\n", eEr);
    }
    eEr = xInitJs();
    if (ER_OK != eEr)
    {
        Serial.printf("Error! : xInitJs - %d\n", eEr);
    }
    // eEr = xInitRe();
    // if (ER_OK != eEr)
    // {
    //     Serial.printf("Error! : xInitRe - %d\n", eEr);
    // }
}

void loop()
{
    // put your main code here, to run repeatedly:
}

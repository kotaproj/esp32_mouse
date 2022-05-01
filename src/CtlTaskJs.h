/**
 * @file CtlTaskJs.h
 * @brief 全体管理タスク - ジョイスティックヘッダ
 */

#ifndef _CTLTASK_JS_H_
#define _CTLTASK_JS_H_

/* INCLUDES *******************************************************************/
#include "System.h"
#include "System_Js.h"

/******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

///	@cond
#ifdef _CTLTASK_JS_C_
#define EXTERN
#else
#define EXTERN extern
#endif
    ///	@endcond

    /* DECLARATIONS ***************************************************************/

    /* VARIABLES ******************************************************************/

    /* PROTOTYPES *****************************************************************/
    EXTERN ErType_t xInitCtlJs(QueueSetHandle_t *pxQueSetHandle);
    EXTERN ErType_t xInitWorkCtlJs(void);
    EXTERN uint32_t ulGetQueLengthCtlJs(void);
    EXTERN void vCallbackCtlMsgJs(UniId_t xSrcId, UniId_t xDstId, void *pvParam);
    EXTERN ErType_t xReciveQueJs(QueueSetMemberHandle_t *pxQueSetMember, MessageCtlJs_t *pxMessage);

/******************************************************************************/
#undef EXTERN

#ifdef __cplusplus
}
#endif
#endif /* _CTLTASK_JS_H_ */
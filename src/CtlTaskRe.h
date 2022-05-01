/**
 * @file CtlTaskRe.h
 * @brief 全体管理タスク - ロータリーエンコーダヘッダ
 */

#ifndef _CTLTASK_RE_H_
#define _CTLTASK_RE_H_

/* INCLUDES *******************************************************************/
#include "System.h"
#include "System_Re.h"

/******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

///	@cond
#ifdef _CTLTASK_RE_C_
#define EXTERN
#else
#define EXTERN extern
#endif
    ///	@endcond

    /* DECLARATIONS ***************************************************************/

    /* VARIABLES ******************************************************************/

    /* PROTOTYPES *****************************************************************/
    EXTERN ErType_t xInitCtlRe(QueueSetHandle_t *pxQueSetHandle);
    EXTERN ErType_t xInitWorkCtlRe(void);
    EXTERN uint32_t ulGetQueLengthCtlRe(void);
    EXTERN void vCallbackCtlMsgRe(UniId_t xSrcId, UniId_t xDstId, void *pvParam);
    EXTERN ErType_t xReciveQueRe(QueueSetMemberHandle_t *pxQueSetMember, MessageCtlRe_t *pxMessage);

/******************************************************************************/
#undef EXTERN

#ifdef __cplusplus
}
#endif
#endif /* _CTLTASK_RE_H_ */
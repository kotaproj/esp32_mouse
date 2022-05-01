/**
 * @file CtlTaskSw.h
 * @brief 全体管理タスク - スイッチヘッダ
 */

#ifndef _CTLTASK_SW_H_
#define _CTLTASK_SW_H_

/* INCLUDES */
#include "System.h"
#include "System_Sw.h"

#ifdef __cplusplus
extern "C"
{
#endif

///	@cond
#ifdef _CTLTASK_SW_C_
#define EXTERN
#else
#define EXTERN extern
#endif
    ///	@endcond

    /* DECLARATIONS */

    /* VARIABLES */

    /* PROTOTYPES */
    EXTERN ErType_t xInitCtlSw(QueueSetHandle_t *pxQueSetHandle);
    EXTERN ErType_t xInitWorkCtlSw(void);
    EXTERN uint32_t ulGetQueLengthCtlSw(void);
    EXTERN void vCallbackCtlMsgSw(UniId_t xSrcId, UniId_t xDstId, void *pvParam);
    EXTERN ErType_t xReciveQueSw(QueueSetMemberHandle_t *pxQueSetMember, MessageCtlSw_t *pxMessage);

#undef EXTERN

#ifdef __cplusplus
}
#endif
#endif /* _CTLTASK_SW_H_ */
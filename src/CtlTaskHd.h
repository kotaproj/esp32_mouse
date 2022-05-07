/**
 * @file CtlTaskHd.h
 * @brief 全体管理タスク - HDヘッダ
 */

#ifndef _CTLTASK_HD_H_
#define _CTLTASK_HD_H_

/* INCLUDES *******************************************************************/
#include "System.h"
#include "System_Hd.h"

/******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

///	@cond
#ifdef _CTLTASK_HD_C_
#define EXTERN
#else
#define EXTERN extern
#endif
    ///	@endcond

    /* DECLARATIONS ***************************************************************/

    /* VARIABLES ******************************************************************/

    /* PROTOTYPES *****************************************************************/
    EXTERN ErType_t xInitCtlHd(QueueSetHandle_t *pxQueSetHandle);
    EXTERN ErType_t xInitWorkCtlHd(void);
    EXTERN uint32_t ulGetQueLengthCtlHd(void);
    EXTERN void vCallbackCtlMsgHd(UniId_t xSrcId, UniId_t xDstId, void *pvParam);
    EXTERN ErType_t xReciveQueHd(QueueSetMemberHandle_t *pxQueSetMember, MessageCtlHd_t *pxMessage);

/******************************************************************************/
#undef EXTERN

#ifdef __cplusplus
}
#endif
#endif /* _CTLTASK_HD_H_ */
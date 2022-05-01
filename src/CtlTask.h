/**
 * @file CtlTask.h
 * @brief 全体管理タスクヘッダ
 */

#ifndef _CTLTASK_H_
#define _CTLTASK_H_

/* INCLUDES *******************************************************************/
#include "System.h"

/******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

///	@cond
#ifdef _CTLTASK_C_
#define EXTERN
#else
#define EXTERN extern
#endif
    ///	@endcond

    /* DECLARATIONS ***************************************************************/

    /**
     * @struct MessageDataCtl_t
     * @brief メッセージ共通ヘッダ
     * @note
     */
    typedef struct
    {
        uint8_t ucReserve[32];
    } MessageDataCtl_t;

    /**
     * @struct MessageCtl_t
     * @brief メッセージ共通ヘッダ+データ
     * @note
     */
    typedef struct
    {
        MessageHead_t Head_t;
        MessageDataCtl_t Data_t;
    } MessageCtl_t;

    /* VARIABLES ******************************************************************/

    /* PROTOTYPES *****************************************************************/
    EXTERN ErType_t xInitCtl(void);

/******************************************************************************/
#undef EXTERN

#ifdef __cplusplus
}
#endif
#endif /* _CTLTASK_H_ */
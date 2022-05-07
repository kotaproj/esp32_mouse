/**
 * @file TaskHd.h
 * @brief HTTPd通信タスク
 * @author kotatuneco
 * @date 2019/11/25
 */

#ifndef _TASKHD_H_
#define _TASKHD_H_

/* INCLUDES *******************************************************************/
#include "System.h"
#include "System_Hd.h"

/******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

///	@cond
#ifdef _TASKHD_C_
#define EXTERN
#else
#define EXTERN extern
#endif
    ///	@endcond

    /* DECLARATIONS ***************************************************************/

    /* VARIABLES ******************************************************************/

    /* PROTOTYPES *****************************************************************/
    EXTERN ErType_t xInitHd(void);

/******************************************************************************/
#undef EXTERN

#ifdef __cplusplus
}
#endif
#endif /* _TASKHD_H_ */
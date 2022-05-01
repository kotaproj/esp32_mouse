/**
 * @file ApiSw.h
 * @brief スイッチ管理
 */

#ifndef _API_SW_H_
#define _API_SW_H_

/* INCLUDES *******************************************************************/
#include "System.h"
#include "System_Sw.h"

/******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

///	@cond
#ifdef _API_SW_C_
#define EXTERN
#else
#define EXTERN extern
#endif
    ///	@endcond

    /* DECLARATIONS ***************************************************************/

    /* VARIABLES ******************************************************************/

    /* PROTOTYPES *****************************************************************/
    EXTERN ErType_t xInitSw(void);

/******************************************************************************/
#undef EXTERN

#ifdef __cplusplus
}
#endif
#endif /* _API_SW_H_ */
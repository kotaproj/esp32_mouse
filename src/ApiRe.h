/**
 * @file ApiRe.h
 * @brief ロータリーエンコーダ管理
 */

#ifndef _API_RE_H_
#define _API_RE_H_

/* INCLUDES *******************************************************************/
#include "System.h"
#include "System_Re.h"

/******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

///	@cond
#ifdef _API_RE_C_
#define EXTERN
#else
#define EXTERN extern
#endif
    ///	@endcond

    /* DECLARATIONS ***************************************************************/

    /* VARIABLES ******************************************************************/

    /* PROTOTYPES *****************************************************************/
    EXTERN ErType_t xInitRe(void);

/******************************************************************************/
#undef EXTERN

#ifdef __cplusplus
}
#endif
#endif /* _API_RE_H_ */
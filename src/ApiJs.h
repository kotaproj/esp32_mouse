/**
 * @file ApiJs.h
 * @brief JoyStick管理
 */

#ifndef _API_JS_H_
#define _API_JS_H_

/* INCLUDES *******************************************************************/
#include "System.h"
#include "System_Js.h"

/******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

///	@cond
#ifdef _API_JS_C_
#define EXTERN
#else
#define EXTERN extern
#endif
    ///	@endcond

    /* DECLARATIONS ***************************************************************/

    /* VARIABLES ******************************************************************/

    /* PROTOTYPES *****************************************************************/
    EXTERN ErType_t xInitJs(void);

/******************************************************************************/
#undef EXTERN

#ifdef __cplusplus
}
#endif
#endif /* _API_JS_H_ */

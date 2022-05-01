/**
 * @file TaskMouse.h
 * @brief Mouseタスク
 * @author kotaproj
 */

#ifndef _TASKMOUSE_H_
#define _TASKMOUSE_H_

/* INCLUDES *******************************************************************/
#include "System.h"
#include "System_Mouse.h"
#include <BleMouse.h>

/******************************************************************************/
#ifdef	__cplusplus
extern "C" {
#endif

///	@cond
#ifdef _TASKMOUSE_C_
#define EXTERN
#else
#define EXTERN extern
#endif
///	@endcond

/* DECLARATIONS ***************************************************************/

/* VARIABLES ******************************************************************/

/* PROTOTYPES *****************************************************************/
EXTERN ErType_t xInitMouse( void);
EXTERN ErType_t xSendMouseQueue_Code(UniId_t xSrcId, uint8_t u8Code, uint8_t u8Type);
EXTERN ErType_t xSendMouseQueue_MoveXy(UniId_t xSrcId, int8_t s8x, int8_t s8y);
EXTERN ErType_t xSendMouseQueue_Wheel(UniId_t xSrcId, int8_t wheel);
EXTERN ErType_t xSendMouseQueue_Sp(UniId_t xSrcId, uint8_t u8Type);
EXTERN ErType_t xSendMouseQueue(UniId_t xSrcId, UniId_t xDstId, void* pvParam);
EXTERN void setup_for_TaskMouse( void);


/******************************************************************************/
#undef EXTERN

#ifdef	__cplusplus
	}
#endif
#endif	/* _TASKMOUSE_H_ */

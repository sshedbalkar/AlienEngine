#pragma once
//
#define MCR_REGISTER_RETURN_CODE(x) x,
typedef enum
{
	RET_SUCCESS = 0,
	#include "ReturnNames.h"
	RET_FAIL
} ReturnVal;
#undef MCR_REGISTER_RETURN_CODE
//
#define MCR_REGISTER_RETURN_CODE(x) #x,
static const char* ReturnCodeText[] =
{
	"RET_SUCCESS",
	#include "ReturnNames.h"
	"RET_FAIL"
};
#undef MCR_REGISTER_RETURN_CODE
//
inline const char* GetRetCodeName( const unsigned int ret )
{
	return ReturnCodeText[ ret ];
}
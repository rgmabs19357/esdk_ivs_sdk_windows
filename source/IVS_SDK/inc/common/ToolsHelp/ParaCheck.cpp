#include "ParaCheck.h"
#include "ivs_error.h"

IVS_INT32 CParaCheck::CheckIsIvsBool(IVS_BOOL bIvsBool)
{
	if (0 == bIvsBool || 1 == bIvsBool)
	{
		return IVS_SUCCEED;
	}
	else
	{
		return IVS_PARA_INVALID;
	}
}


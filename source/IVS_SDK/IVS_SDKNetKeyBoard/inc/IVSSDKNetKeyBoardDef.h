/********************************************************************
	filename	: 	IVSSDKNetKeyBoardDef.h
	author		:	
	created		:	2011/10/29	
	description	:	提供IVSSDKNetKeyBoardDef中需要暴露给第三方使用的数据类型（IVSSDKNetKeyBoardDef接口中需要用到的数据类型）
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/29 初始版本
*********************************************************************/
#ifndef IVS_SDK_NET_KEY_BOARD_DEF_H
#define IVS_SDK_NET_KEY_BOARD_DEF_H



typedef int (__stdcall *LPCallBack)(int iKeyValue,int iSpeedFirst,int iSpeedSecond, bool bTvWall);


#endif //IVS_SDK_NET_KEY_BOARD_DEF_H
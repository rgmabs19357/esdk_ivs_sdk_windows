/*****************************************************************
 filename    :    IVS_OCXPlayer.h
 author      :    ykf75928
 created     :    2013/01/16
 description :    ocx程序头文件
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/16 初始版本
*****************************************************************/
#pragma once

// IVS_OCXPlayer.h : main header file for IVS_OCXPlayer.DLL

#if !defined( __AFXCTL_H__ )
#error "include 'afxctl.h' before including this file"
#endif

// CIVS_OCXPlayerApp : See IVS_OCXPlayer.cpp for implementation.

class CIVS_OCXPlayerApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};
//lint -e526
extern const GUID CDECL _tlid;//lint !e526
extern const WORD _wVerMajor;//lint !e526
extern const WORD _wVerMinor;//lint !e526


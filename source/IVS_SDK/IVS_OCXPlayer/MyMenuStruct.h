/*****************************************************************
 filename    :    MyMenuStruct.h
 author      :    ykf75928
 created     :    2013/01/16
 description :    菜单结构体
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2013/01/16 初始版本
*****************************************************************/
#ifndef MENUGROUP_STRUCT
#define MENUGROUP_STRUCT

#include "IVSButton.h"
#include "IVSMenuHeader.h"

#define		MENU_CLASSNAME				_T("IVSMenuCtrl")
#define		FRAME_CLASSNAME				_T("IVSMenuFrameCtrl")
#define		HEADER_CLASSNAME			_T("IVSMenuHeader")
#define		ITEM_CLASSNAME				_T("IVSMenuItem")

#define		MENUTITLE_HEIGHT			48
#define		MARGIN						4
#define		MENUICON_WIDTH				32
#define		MENUICON_HEIGHT				32
#define		BUTTON_MAGIN				8
#define		BTNICON_WIDTH				16
#define		BTNICON_HEIGHT				16
#define		BUTTON_HEIGHT				24  //BTNICON_HEIGHT

//子项目文本颜色和字体大小
#define		BTN_OVER_COL				RGB(255,255,255)
#define		BTN_NORMAL_COL				RGB(204,204,204)
#define		BTN_FONT_SIZE				14

//组合菜单文本颜色和字体大小
#define		MENU_OVER_COL				RGB(0,0,0)
#define		MENU_NORMAL_COL				RGB(164,164,164)
#define		MENU_FONT_SIZE				16

//组合菜单打开的时候边框颜色
#define		MENU_WIDGET_COL				RGB(225,221,98)

//组合菜单顶部文本标题背景渐变颜色：从左到右
#define		MENUTITLE_LEFT_COL			RGB(220,215,61)
#define		MENUTITLE_RIGHT_COL			RGB(255,255,255)

//组合菜单打开时内部背景渐变颜色：从上到下
#define		MENU_TOP_COL					RGB(29, 37, 10)

//组合框头部打开关闭窗体ID
#define	IDC_MENUHEADER				1111111
#define	IDC_MENU	 100
#define IDC_SUBITEM  132		
#define IDC_SUBITEMMAX 398
//用户更新窗体位置消息，可以不处理
//#define		WM_UPDATEPOS				WM_USER+1

typedef struct STRU_BUTTONINFO
{
	CString			strTitle;				//GroupMenu中子项的标题
	UINT			nIcon;					//GroupMenu中子项的图标
	bool			isCreatedResource;		// true is created by Resource; false is Created by filePath;
	IVS_INT32			nNorBitmap;				//GroupMenu中子项的位图
	IVS_INT32			nSelBitmap;				//GroupMenu中子项的位图
	IVS_INT32			nDisBitmap;				//GroupMenu中子项的位图
	std::string         strNorBitmap;
	std::string         strSelBitmap;
	std::string         strDisBitmap;
	UINT			nBtnID;					//GroupMenu中子项的ID
	BOOL			bSubMenu;
	CString			strTip;
	CWnd*			pChild;
	BOOL			bIsBordLine;				//
	BOOL			bFontBold;
	BOOL			bCheckBox;
	CIVSButton	btn;					//GroupMenu中子项
}MenuItemInfo;

typedef CTypedPtrList<CPtrList, MenuItemInfo*> TButtonList;

typedef struct STRU_GROUPMENU
{
	CString			strTitle;			//标题
	UINT			nIcon;				//图标
	UINT			nBitmap;			//位图
	BOOL			bStatus;			//
	CIVSHeader	    btnHeader;			//头部
	TButtonList		BtnList;			//子项列表
}IVSMenu;

#endif

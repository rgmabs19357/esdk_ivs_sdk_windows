/********************************************************************
filename 	: OCXDrawXMLProcess.h
author   	: z00227377
created 	: 2012/12/25
description : 画图XML
copyright 	: Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history 	: 2012/12/25 初始版本
*********************************************************************/


#ifndef __OCX_DRAW_XML_PROCESS_H__ 
#define __OCX_DRAW_XML_PROCESS_H__

#include "SDKDef.h"
#include "IVS_IADrawAPI.h"




class COCXDrawXMLProcess
{
private:
	COCXDrawXMLProcess();
public:
	~COCXDrawXMLProcess();

public:
	// 解析设置图形的XML
	static IVS_INT32 SetGraphicsPraseXML(IA_GRAPHICS *pGraphicsArr, ULONG &ulNum, LPCTSTR pGraphicsXml, const SIZE &szGraphic, BOOL bPersent);

	// 构建获取图形的XML
	static IVS_INT32 GetGraphicsGetXML(const IA_GRAPHICS *pGraphicsArr, IVS_INT32 iResultCode, ULONG ulNum, CXml& xmlGraphics, const SIZE &szGraphic);

	// 构建获取图形失败的XML
	static IVS_INT32 GetGraphicsGetErrXML(IVS_INT32 iResultCode, CXml& xmlGraphics);
	
	// 解析画图前图形属性设置XML
	static IVS_INT32 DrawGraphicsPraseXML(IA_GRAPHICS &GraphicAttribute, ULONG &ulGroupDrawMaxNum, LPCTSTR pGraphicsXml);

	// 构建选中图形消息XML
	static IVS_INT32 SelectGraphicGetXML(CString &xmlToCU, IVS_ULONG ulGraphicGroup);
	
	// 构建去选中图形消息XML
	static IVS_INT32 UnSelectGraphicGetXML(CString &xmlToCU);

};
#endif //__OCX_DRAW_XML_PROCESS_H__

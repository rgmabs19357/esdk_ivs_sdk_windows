#include "stdafx.h"
#include "OCXRealPlayXMLProcess.h"
#include "ToolsHelp.h"
#include "SDKDef.h"
#include "IVS_Trace.h"

#define CHECK_SUCCESS(s)\
	do { if(!(s)) return IVS_FAIL;} while(0)

COCXRealPlayXMLProcess::COCXRealPlayXMLProcess()
{
}

COCXRealPlayXMLProcess::~COCXRealPlayXMLProcess()
{
}

//获取媒体参数XML
LONG COCXRealPlayXMLProcess::GetMediaParamXML(LPCTSTR pRealPlayParamXml, IVS_REALPLAY_PARAM &realPlayParam)
{
    if (NULL == pRealPlayParamXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get Realplay Param", "get realplay param error, xml is null");//lint !e40
        return IVS_PARA_INVALID;
    }

    IVS_DEBUG_TRACE("");
    //解析xml
    CXml xml;
    if (!xml.Parse(pRealPlayParamXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Realplay Param","parse xml error.");	//lint !e40
        return IVS_XML_INVALID;
    }

    if(xml.FindElem("Content"))
    {
        (void)xml.IntoElem();
        if(xml.FindElem("RealplayParam"))
        {				
            (void)xml.IntoElem();
            IVS_INT32 iTemp = 0;
            const IVS_CHAR* szElemValue = NULL;

            GET_ELEM_VALUE_NUM("StreamType", szElemValue, iTemp, xml);
            realPlayParam.uiStreamType = (IVS_UINT32)iTemp;
            GET_ELEM_VALUE_NUM("ProtocolType", szElemValue, iTemp, xml);
            realPlayParam.uiProtocolType = (IVS_UINT32)iTemp;
            GET_ELEM_VALUE_NUM("DirectFirst", szElemValue, iTemp, xml);
            realPlayParam.bDirectFirst = (IVS_BOOL)iTemp;
            GET_ELEM_VALUE_NUM("Multicast", szElemValue, iTemp, xml);
            realPlayParam.bMultiCast = (IVS_BOOL)iTemp;
        }
        else
        {
            BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Realplay Param",
                "Parse xml realplay para fail.");//lint !e40
            return IVS_XML_INVALID;
        }
    }
    else
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Realplay Param","Parse xml Content fail.");//lint !e40
        return IVS_XML_INVALID;
    }

    return IVS_SUCCEED;

}//lint !e1762

LONG COCXRealPlayXMLProcess::EnableGPUParseClientConfig(LPCTSTR pParamXml, BOOL& bEnableGPU)
{
    if (NULL == pParamXml)
    {
        BP_RUN_LOG_ERR(IVS_PARA_INVALID, "Get Realplay Param", "get realplay param error, xml is null");//lint !e40
        return IVS_PARA_INVALID;
    }

    IVS_DEBUG_TRACE("");
    //解析xml
    CXml xml;
    if (!xml.Parse(pParamXml))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "Get Realplay Param","parse xml error.");	//lint !e40
        return IVS_XML_INVALID;
    }

	if(xml.FindElem("NetSetInfo"))
	{
		(void)xml.IntoElem();
		if(xml.FindElem("GpuEnabled"))
		{
			const char* pszValue = xml.GetElemValue();
			if (NULL != pszValue)
			{
				int iEnableGPU = atoi(pszValue);
				bEnableGPU = (1 == iEnableGPU) ? TRUE : FALSE;
			}
			else
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID, "pszValue = NULL","");	//lint !e40
				bEnableGPU = FALSE;
			}
		}
		else
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml find elem GpuEnabled failed","");	//lint !e40
			bEnableGPU = FALSE;
		}
	}
	else
	{
		bEnableGPU = FALSE;
	}

	BP_RUN_LOG_INF("Enable GPU Parse Client Config","bEnableGPU[%d].", bEnableGPU);	//lint !e40
    return IVS_SUCCEED;
}

/*
 * 构造异步实况事件上报结构体
 *
 */

LONG COCXRealPlayXMLProcess::BuildAsyncRealPlayXml(CXml &xml, long winID, long result, unsigned long transid)
{
    CHECK_SUCCESS(xml.AddDeclaration("1.0","UTF-8",""));
	CHECK_SUCCESS(xml.AddElem("Content"));
	CHECK_SUCCESS(xml.AddChildElem("MessageType"));
	xml.IntoElem();

	char buf[64] = {0};
	CHECK_SUCCESS(xml.SetElemValue("IVS_OCX_MSG_START_REALPLAY"));

	CHECK_SUCCESS(xml.AddElem("ResultCode"));
	(void)_itoa_s(result, buf, 10);//lint !e713
	CHECK_SUCCESS(xml.SetElemValue(buf));

	CHECK_SUCCESS(xml.AddElem("StartResult"));
	CHECK_SUCCESS(xml.AddChildElem("WndID"));
	xml.IntoElem();
	(void)_itoa_s(winID, buf, 10);
	CHECK_SUCCESS(xml.SetElemValue(buf));

	CHECK_SUCCESS(xml.AddElem("TransID"));
	(void)_itoa_s(transid, buf, 10);//lint !e713
	CHECK_SUCCESS(xml.SetElemValue(buf));

	return IVS_SUCCEED;
}
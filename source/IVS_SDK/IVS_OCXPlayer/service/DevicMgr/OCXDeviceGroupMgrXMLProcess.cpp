/*****************************************************************
 filename    :    OCXDeviceGroupMgrXMLProcess.cpp
 author      :    guandiqun
 created     :    2012/11/19
 description :    实现设备组XML的拼装和解析;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*****************************************************************/
#include "OCXDeviceGroupMgrXMLProcess.h"
#include "ivs_error.h"
#include "ToolsHelp.h"
#include "IVS_Trace.h"

// 查找info的数量
IVS_UINT32 COCXDeviceGroupMgrXMLProcess::GetGroupInfoNum(CXml &xmlTemp)
{
    IVS_DEBUG_TRACE("");

    IVS_UINT32 uiNum = 0;	

    // 看是否有QueryField
    if (xmlTemp.FindElemEx("Content/GroupList"))
    {
        do 
        {
            if (!xmlTemp.FindElemValue("GroupInfo"))
            {
                break;
            }
            uiNum++;
        } while (xmlTemp.NextElem());           
    }
    xmlTemp.OutOfElem();
    xmlTemp.OutOfElem();

    return uiNum;	
}

// 添加设备组拼接XML
IVS_INT32 COCXDeviceGroupMgrXMLProcess::AddDeviceGroupGetXML(const IVS_UINT32& uiDevGroupID, CXml &xml)
{
	IVS_DEBUG_TRACE("");

	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");
	(void)xml.AddElem("GroupID");  
	xml.SetElemValue(CToolsHelp::Int2Str((IVS_INT32)(uiDevGroupID)).c_str());
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 新增设备组解析XML
IVS_INT32 COCXDeviceGroupMgrXMLProcess::AddDeviceGroupParseXML(IVS_DEV_GROUP* pGroupInfo,CXml &xml)
{
	CHECK_POINTER(pGroupInfo, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	if(!xml.FindElemEx("Content/GroupInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "!xml.FindElemEx(Content/GroupInfo/DomainCode)", "NA");
		return IVS_XML_INVALID;
	}  
	const IVS_CHAR* pElemValue = NULL;
	// 域编码
	GET_ELEM_VALUE_CHAR("DomainCode", pElemValue, pGroupInfo->cDomainCode, IVS_DOMAIN_CODE_LEN, xml);
	// 设备组名
	GET_ELEM_VALUE_CHAR("GroupName", pElemValue, pGroupInfo->cName, IVS_NAME_LEN, xml);
	// 设备组ID
	GET_ELEM_VALUE_NUM_FOR_UINT("GroupID", pElemValue, pGroupInfo->uiID, xml);
	// 父设备组ID
	GET_ELEM_VALUE_NUM_FOR_UINT("ParentGroupID", pElemValue, pGroupInfo->uiParentID, xml);     
	BP_RUN_LOG_INF("Add DeviceGroup ParseXML", "DomainCode=%s", pGroupInfo->cDomainCode);

	return IVS_SUCCEED;
}

// 删除设备组解析xml
IVS_INT32 COCXDeviceGroupMgrXMLProcess::DeleteDeviceGroupParseXML(IVS_DEV_GROUP &stDevGroup, CXml &reqXml)
{
	IVS_DEBUG_TRACE("");

	// 查找是否有Content/DevList元素
	if (!reqXml.FindElemEx("Content/GroupInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "FALSE == reqXml.FindElemEx('Content/GroupInfo'')", "NA");
		return IVS_XML_INVALID;
	}       
	const IVS_CHAR* pTemp = NULL;
	// 域编码
	GET_ELEM_VALUE_CHAR("DomainCode", pTemp, stDevGroup.cDomainCode, IVS_DOMAIN_CODE_LEN, reqXml);
	// 设备组ID
	GET_ELEM_VALUE_NUM_FOR_UINT("GroupID", pTemp, stDevGroup.uiID, reqXml);   
	reqXml.OutOfElem();
	reqXml.OutOfElem();

	return IVS_SUCCEED;
}

// 移动设备组解析XML
IVS_INT32 COCXDeviceGroupMgrXMLProcess::MoveDeviceGroupParseXML(IVS_DEV_GROUP &stDevGroup, CXml& reqXml)
{
	IVS_DEBUG_TRACE("");

	// 查找是否有Content/DevList元素
	if (!reqXml.FindElemEx("Content/GroupInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "FALSE == reqXml.FindElemEx('Content/GroupInfo'')", "NA");
		return IVS_XML_INVALID;
	}       
	const IVS_CHAR* pTemp = NULL;
	// 域编码
	GET_ELEM_VALUE_CHAR("DomainCode", pTemp, stDevGroup.cDomainCode, IVS_DOMAIN_CODE_LEN, reqXml);
	// 设备组ID
	GET_ELEM_VALUE_NUM_FOR_UINT("GroupID", pTemp, stDevGroup.uiID, reqXml);   
	// 设备组ID
	GET_ELEM_VALUE_NUM_FOR_UINT("TargetGroupID", pTemp, stDevGroup.uiParentID, reqXml);  
	reqXml.OutOfElem();
	reqXml.OutOfElem();

	return IVS_SUCCEED;
}
// 查询设备组解析XML
IVS_INT32 COCXDeviceGroupMgrXMLProcess::GetDeviceGroupParseXML(IVS_CHAR* pDomainCode, IVS_CHAR* pDevGroupCode, CXml& xmlReq)
{
	CHECK_POINTER(pDomainCode,IVS_PARA_INVALID);
	CHECK_POINTER(pDevGroupCode,IVS_PARA_INVALID);
    IVS_DEBUG_TRACE("");

    if (!xmlReq.FindElem("Content"))
    {
        return IVS_XML_INVALID;
    }
    (void)xmlReq.IntoElem();

    const char* szElemValue = NULL;
    GET_ELEM_VALUE_CHAR("DomainCode", szElemValue, pDomainCode, IVS_DOMAIN_CODE_LEN, xmlReq);
    GET_ELEM_VALUE_CHAR("GroupID", szElemValue, pDevGroupCode, IVS_DEVICE_GROUP_LEN, xmlReq);
    xmlReq.OutOfElem();

    return IVS_SUCCEED;
}

// 查询设备组拼接XML
IVS_INT32 COCXDeviceGroupMgrXMLProcess::GetDeviceGroupGetXML(IVS_DEVICE_GROUP_LIST* pDeviceGroupList, IVS_UINT32 uiBufferSize, CXml& xml)
{
	CHECK_POINTER(pDeviceGroupList,IVS_OPERATE_MEMORY_ERROR);

    IVS_DEBUG_TRACE("");

    (void)xml.AddDeclaration("1.0","UTF-8","");
    (void)xml.AddElem("Content");
    (void)xml.AddChildElem("ResultCode");
    (void)xml.IntoElem();
    (void)xml.SetElemValue("0");
    (void)xml.AddElem("GroupList");
    (void)xml.IntoElem();

    IVS_UINT32 uiNum = pDeviceGroupList->uiTotal;
    for (IVS_UINT32 i=0; i < uiNum; ++i)
    {        
        if(0 == i)
        {
            (void)xml.AddChildElem("GroupInfo");
        }
        else
        {
            (void)xml.AddElem("GroupInfo");            
        }

        IVS_DEVICE_GROUP &stGroup = pDeviceGroupList->stDeviceGroup[i];

        (void)xml.IntoElem();
        (void)xml.AddChildElem("GroupID");
        (void)xml.IntoElem();
		IVS_CHAR cGroupID[IVS_DEVICE_GROUP_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cGroupID, IVS_DEVICE_GROUP_LEN, stGroup.cGroupCode, IVS_DEVICE_GROUP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cGroupID, IVS_DEVICE_GROUP_LEN, stGroup.cGroupCode, IVS_DEVICE_GROUP_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
        (void)xml.SetElemValue(cGroupID);

		IVS_CHAR cParentGroupID[IVS_DEVICE_GROUP_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cParentGroupID, IVS_DEVICE_GROUP_LEN, stGroup.cParentGroupCode, IVS_DEVICE_GROUP_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cGroupID, IVS_DEVICE_GROUP_LEN, stGroup.cParentGroupCode, IVS_DEVICE_GROUP_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
        (void)xml.AddElem("ParentGroupID");
        (void)xml.SetElemValue(cParentGroupID);

        (void)xml.AddElem("GroupName");
		IVS_CHAR cGroupName[IVS_NAME_LEN + 1] = {0};
		if (!CToolsHelp::Memcpy(cGroupName, IVS_NAME_LEN, stGroup.cGroupName, IVS_NAME_LEN))
		{
			BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "Memcpy(cGroupName, IVS_NAME_LEN, stGroup.cGroupName, IVS_NAME_LEN) fail", "NA");
			return IVS_ALLOC_MEMORY_ERROR;
		}
        (void)xml.SetElemValue(cGroupName);

		(void)xml.AddElem("IsExDomain");
		(void)xml.SetElemValue(CToolsHelp::Int2Str(stGroup.bIsExDomain).c_str());

        xml.OutOfElem();
    }  

    xml.OutOfElem();

    return IVS_SUCCEED;
}//lint !e715

// 修改设备组名称解析XML
IVS_INT32 COCXDeviceGroupMgrXMLProcess::ModifyDeviceGroupNamePraseXML(IVS_DEV_GROUP* pDevGroupInfo, CXml &xml)
{
	CHECK_POINTER(pDevGroupInfo, IVS_OPERATE_MEMORY_ERROR);

	IVS_DEBUG_TRACE("");

	if (!xml.FindElemEx("Content/GroupInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "!xml.FindElemEx(Content/GroupInfo)", "NA");
		return IVS_XML_INVALID;
	} 
	const IVS_CHAR *pTemp = NULL;    
	// 设备组名    
	GET_ELEM_VALUE_CHAR("GroupName", pTemp, pDevGroupInfo->cName, IVS_NAME_LEN, xml);
	// 设备组ID
	GET_ELEM_VALUE_NUM_FOR_UINT("GroupID", pTemp, pDevGroupInfo->uiID, xml);
	// DomainCode   
	GET_ELEM_VALUE_CHAR("DomainCode", pTemp, pDevGroupInfo->cDomainCode, IVS_DOMAIN_CODE_LEN, xml);

	xml.OutOfElem();
	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 新增设备组摄像机解析XML
IVS_INT32 COCXDeviceGroupMgrXMLProcess::AddDeviceToGroupPraseXML(IVS_INT32 iSeesionID, 
	IVS_CHAR* pTargetDomainCode,
	IVS_UINT32 uiDeviceNum,
	IVS_DEV_GROUP_CAMERA* pGroupCameraList,
	IVS_UINT32& uiTargetDevGroupID,
	CXml& xmlReq)
{
	CHECK_POINTER(pTargetDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pGroupCameraList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	if (!xmlReq.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "no 'Content' elemenet", "NA");
		return IVS_XML_INVALID;
	}
	(void)xmlReq.IntoElem();
	const IVS_CHAR *pTemp = NULL;
	// 设备组名    
	GET_ELEM_VALUE_CHAR("DomainCode", pTemp, pTargetDomainCode, IVS_DOMAIN_CODE_LEN, xmlReq);
	// 移动到的设备组ID    
	GET_ELEM_VALUE_NUM_FOR_UINT("TargetGroupID", pTemp, uiTargetDevGroupID, xmlReq);
	IVS_INT32 iRet = IVS_XML_INVALID;
	IVS_UINT32 uiIndex = 0;
	if (xmlReq.FindElemEx("Content/DevList"))
	{
		do 
		{
			if (!xmlReq.FindElemValue("DevInfo"))
			{
				break;
			}
			(IVS_VOID)xmlReq.IntoElem();
			GET_ELEM_VALUE_CHAR("DevCode", pTemp, pGroupCameraList[uiIndex].cCameraCode, IVS_DEV_CODE_LEN, xmlReq);
			GET_ELEM_VALUE_CHAR("DevDomainCode", pTemp, pGroupCameraList[uiIndex].cCameraDomainCode, IVS_DEV_CODE_LEN, xmlReq);
			GET_ELEM_VALUE_NUM_FOR_UINT("FromGroupID", pTemp, pGroupCameraList[uiIndex].uiDevGroupID, xmlReq);
			xmlReq.OutOfElem();
			uiIndex++;
		} while (xmlReq.NextElem() && uiIndex < uiDeviceNum); 
		iRet = IVS_SUCCEED;
	}
	xmlReq.OutOfElem();
	xmlReq.OutOfElem();

	return iRet;
}

// 删除设备组摄像机解析XML
IVS_INT32 COCXDeviceGroupMgrXMLProcess::DeleteDeviceFromGroupPraseXML(IVS_INT32 iSeesionID, 
	IVS_CHAR* pTargetDomainCode,
	IVS_UINT32 uiDeviceNum,
	IVS_DEV_GROUP_CAMERA* pGroupCameraList,
	CXml& xmlReq)
{
    CHECK_POINTER(pTargetDomainCode, IVS_OPERATE_MEMORY_ERROR);
    CHECK_POINTER(pGroupCameraList, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

    if (!xmlReq.FindElem("Content"))
    {
        BP_RUN_LOG_ERR(IVS_XML_INVALID, "no 'Content' elemenet", "NA");
        return IVS_XML_INVALID;
    }
    (void)xmlReq.IntoElem();
	const IVS_CHAR *pTemp = NULL;
    // 设备组名    
    GET_ELEM_VALUE_CHAR("DomainCode", pTemp, pTargetDomainCode, IVS_DOMAIN_CODE_LEN, xmlReq);
    IVS_INT32 iRet = IVS_XML_INVALID;
    IVS_UINT32 uiIndex = 0;
    if (xmlReq.FindElemEx("Content/DevList"))
    {
        do 
        {
            if (!xmlReq.FindElemValue("DevInfo"))
            {
                break;
            }
            (IVS_VOID)xmlReq.IntoElem();
            GET_ELEM_VALUE_CHAR("DevCode", pTemp, pGroupCameraList[uiIndex].cCameraCode, IVS_DEV_CODE_LEN, xmlReq);
            GET_ELEM_VALUE_CHAR("DevDomainCode", pTemp, pGroupCameraList[uiIndex].cCameraDomainCode, IVS_DEV_CODE_LEN, xmlReq);
			GET_ELEM_VALUE_NUM_FOR_UINT("FromGroupID", pTemp, pGroupCameraList[uiIndex].uiDevGroupID, xmlReq);
            xmlReq.OutOfElem();
            uiIndex++;
        } while (xmlReq.NextElem() && uiIndex < uiDeviceNum); 
        iRet = IVS_SUCCEED;
    }
    xmlReq.OutOfElem();
    xmlReq.OutOfElem();

    return iRet;
}//lint !e715

#define IVS_GROUP_MAX_REMARK_LEN 324
// 设置设备组备注信息解析XML
IVS_INT32 COCXDeviceGroupMgrXMLProcess::SetDevGroupRemarkParseXML(IVS_CHAR* pDomainCode, IVS_UINT32 &uiDevGroupID, IVS_CHAR* pDevGroupRemark, CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	CHECK_POINTER(pDevGroupRemark, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	if (!xml.FindElemEx("Content/GroupInfo"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElemEx(Content/GroupInfo) fail", "NA");
		return IVS_XML_INVALID;
	}
	const IVS_CHAR* pTemp = NULL;
	GET_ELEM_VALUE_CHAR("DomainCode", pTemp, pDomainCode, IVS_DOMAIN_CODE_LEN, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("GroupID", pTemp, uiDevGroupID, xml);
	GET_ELEM_VALUE_CHAR("GroupRemark", pTemp, pDevGroupRemark, IVS_GROUP_MAX_REMARK_LEN, xml);

	xml.OutOfElem();

	return IVS_SUCCEED;
}

// 查询设备组备注信息解析XML
IVS_INT32 COCXDeviceGroupMgrXMLProcess::GetDevGroupRemarkParseXML(IVS_CHAR* pDomainCode, IVS_UINT32 &uiDevGroupID, CXml &xml)
{
	CHECK_POINTER(pDomainCode, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	if (!xml.FindElem("Content"))
	{
		BP_RUN_LOG_ERR(IVS_XML_INVALID, "xml.FindElem(Content) fail", "NA");
		return IVS_XML_INVALID;
	}
	(void)xml.IntoElem();
	const IVS_CHAR* pTemp = NULL;
	GET_ELEM_VALUE_CHAR("DomainCode", pTemp, pDomainCode, IVS_DOMAIN_CODE_LEN, xml);
	GET_ELEM_VALUE_NUM_FOR_UINT("GroupID", pTemp, uiDevGroupID, xml);

	xml.OutOfElem();
	return IVS_SUCCEED;
}

// 查询设备组备注信息拼接XML
IVS_INT32 COCXDeviceGroupMgrXMLProcess::GetDevGroupRemarkGetXML(const IVS_CHAR* pDevGroupRemark, CString& strResult)
{
	CHECK_POINTER(pDevGroupRemark, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	CXml xml;
	(void)xml.AddDeclaration("1.0","UTF-8","");
	(void)xml.AddElem("Content");
	(void)xml.AddChildElem("ResultCode");
	(void)xml.IntoElem();
	(void)xml.SetElemValue("0");

	(void)xml.AddElem("GroupRemark");
	(void)xml.SetElemValue(pDevGroupRemark);

	xml.OutOfElem();

	IVS_UINT32 uiLen = 0;
	strResult = xml.GetXMLStream(uiLen);

	return IVS_SUCCEED;
}

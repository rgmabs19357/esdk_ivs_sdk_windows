/*****************************************************************
 filename    :    OCXDeviceParaConfig.cpp
 author      :    guandiqun
 created     :    2012/11/19
 description :    实现设备;
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/11/19 初始版本
*****************************************************************/
#include "OCXDeviceParaConfig.h"
//#include "OCXDeviceParaConfigXMLProcess.h"
#include "ivs_error.h"
#include "IVS_SDK.h"
#include "IVSCommon.h"
#include "IVS_Trace.h"
#include "DevEnumXML.h"

COCXDeviceParaConfig::COCXDeviceParaConfig()
{

}
COCXDeviceParaConfig::~COCXDeviceParaConfig()
{

}
 // 修改设备参数
 IVS_INT32 COCXDeviceParaConfig::SetDeviceConfig(IVS_INT32 iSessionID, LONG lConfigType, const IVS_CHAR *pchDevCode, CXml &xml)
 {
	 CHECK_POINTER(pchDevCode, IVS_OPERATE_MEMORY_ERROR);
     IVS_DEBUG_TRACE("");

     IVS_UINT32 uiBufferSize= 0;
     IVS_VOID* pBuffer= NULL;

     // 获取输入参数
     IVS_INT32 iRet = GetCommDevConfig(uiBufferSize, lConfigType, pBuffer);
     if (IVS_SUCCEED != iRet)
     {
         BP_RUN_LOG_ERR(iRet, "Get CommDevConfig fail", "IVS_SUCCEED != iRet(GetCommDevConfig)");
         return iRet;
     }
     // 解析xml
	 COCXDeviceParaConfigXMLProcess m_pOCXDevParaConfigXML;
	 iRet = m_pOCXDevParaConfigXML.SetDeviceConfigParseXml(pBuffer, (IVS_UINT32)lConfigType, xml);
     if (IVS_SUCCEED != iRet)
     {
         BP_RUN_LOG_ERR(iRet,  "Get DeviceConfig ParseXml fail", "IVS_SUCCEED != iRet");
		 IVS_DELETE(pBuffer, MUILI);
         return iRet;
     }
     // 设置设备参数
     iRet = IVS_SDK_SetDeviceConfig(iSessionID, pchDevCode, (IVS_UINT32)lConfigType, (IVS_VOID*)pBuffer, uiBufferSize);
     IVS_DELETE(pBuffer, MUILI);

     return iRet;
 }

 // 查询设备参数
 IVS_INT32 COCXDeviceParaConfig::GetDeviceConfig(IVS_INT32 iSessionID, LONG lConfigType, const IVS_CHAR *pchDevCode, CXml &xml)
 {
     CHECK_POINTER(pchDevCode, IVS_OPERATE_MEMORY_ERROR);
	 IVS_DEBUG_TRACE("");

     IVS_UINT32 uiBufferSize= 0;
     IVS_VOID* pBuffer= NULL;

     // 获取输入参数
     IVS_INT32 iRet = GetCommDevConfig(uiBufferSize, lConfigType, pBuffer);
     if (IVS_SUCCEED != iRet)
     {
         BP_RUN_LOG_ERR(iRet, "Get CommDevConfig fail", "IVS_SUCCEED != iRet(GetCommDevConfig)");
         return iRet;
     }
     // 查询设备参数
     iRet = IVS_SDK_GetDeviceConfig(iSessionID, pchDevCode, (IVS_UINT32)lConfigType, (IVS_VOID*)pBuffer, uiBufferSize);
     if (IVS_SUCCEED != iRet)
     {
         BP_RUN_LOG_ERR(iRet,  "SDK_GetDeviceConfig fail", "IVS_SUCCEED != iRet(IVS_SDK_GetDeviceConfig)");
		 IVS_DELETE(pBuffer, MUILI);
         return iRet;
     }
	 
     // 组装xml
	 COCXDeviceParaConfigXMLProcess m_pOCXDevParaConfigXML;
	 iRet = m_pOCXDevParaConfigXML.GetDeviceConfigGetXml(pBuffer, pchDevCode, (IVS_UINT32)lConfigType, xml);
     IVS_DELETE(pBuffer, MUILI);

     return iRet;
 }

 // 设备参数统一分配内存
 IVS_INT32 COCXDeviceParaConfig::GetCommDevConfig(IVS_UINT32 &uiBufferSize, LONG lConfigType, IVS_VOID* &pBuffer)
 {
     IVS_DEBUG_TRACE("");

	 uiBufferSize = CDevEnumXML::GetSizeOfConfigType(static_cast<IVS_UINT32>(lConfigType));
     if (0 != uiBufferSize)
     {
         IVS_CHAR *pChBuffer = IVS_NEW(pChBuffer, uiBufferSize);
         pBuffer = reinterpret_cast<IVS_VOID*>(pChBuffer);
         if (NULL == pBuffer)
         {
             BP_RUN_LOG_ERR(IVS_ALLOC_MEMORY_ERROR, "'new pChBuffer' fail", "NULL == pBuffer");
             return IVS_ALLOC_MEMORY_ERROR;
         }
		 memset(pBuffer, 0x0, uiBufferSize);
     }

     return IVS_SUCCEED;
 }

 //// 查询设备参数统一拼接XML
 //IVS_INT32 COCXDeviceParaConfig::GetDeviceConfigGetXml( IVS_VOID* &pBuffer, const IVS_CHAR* pDevCode, LONG lConfigType, CXml &xml )
 //{
 //    CHECK_POINTER(pDevCode, IVS_OPERATE_MEMORY_ERROR);
	// IVS_DEBUG_TRACE("");

	// //IVS_CHAR chDevCode[IVS_SDK_DEV_CODE_LEN+1] = {0};
	// //IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};

	// //IVS_INT32 iRet = COCXDeviceParaConfigXMLProcess::ParseDevCode(pDevCode, chDevCode, chDomaCode);
	// //if (IVS_SUCCEED != iRet)
	// //{
	//	// BP_RUN_LOG_ERR(iRet, "Parse DevCode fail", "(ParseDevCode)iRet=%d", iRet);
	//	// return iRet;
	// //}

	// FnGetDeviceParaGetXML fnGetProcess = GetFnGetDevicePara(static_cast<IVS_UINT32>(lConfigType));
	// if (NULL == fnGetProcess)
	// {
	//	 BP_RUN_LOG_ERR(IVS_FAIL, "no process fn","iRet = %d", IVS_FAIL);
	//	 return IVS_FAIL;
	// }
	// return (this->*fnGetProcess)(pBuffer, pDevCode, xml);
 // //   switch(lConfigType)
 // //   {
 // //   // 主设备基本参数 IVS_DEVICE_CFG
 // //   case CONFIG_DEVICE_CFG:
	//	// {
	//	//	 IVS_DEVICE_CFG *pMainDevCfg = reinterpret_cast<IVS_DEVICE_CFG *>(pBuffer);    
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetMainDevConfigGetXML(pMainDevCfg, xml, chDomaCode);      
	//	// }
	//	// break;
 // //   // 摄像机移动侦测参数设置 
 // //   case CONFIG_CAMERA_MOTION_DETECTION_CFG: 
	//	// {
	//	//	 IVS_MOTION_DETECTION* pMotionDetec = reinterpret_cast<IVS_MOTION_DETECTION*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetMotioinDetecGetXML(pMotionDetec, xml); 
	//	// }
	//	// break; 
 // //   // 摄像机OSD参数设置 
 // //   case CONFIG_CAMERA_OSD_CFG: 
	//	// {
	//	//	 IVS_CAMERA_OSD_CFG *pOSDConfig = reinterpret_cast<IVS_CAMERA_OSD_CFG *>(pBuffer);    
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetOSDConfigGetXML(pOSDConfig, chDomaCode, xml);             
	//	// }	
	//	// break;
 // //   // 主设备网络参数 IVS_DEVICE_NET_CFG
 // //   case CONFIG_DEVICE_NET_CFG: 
	//	// {
	//	//	 IVS_DEVICE_NET_CFG* pDevNetCfg = reinterpret_cast<IVS_DEVICE_NET_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetDevNetCfgGetXML(pDevNetCfg, xml);
	//	// }
	//	// break;
	// ////摄像机基本信息参数
 // //   case CONFIG_CAMERA_CFG: 
	//	// {
	//	//	 IVS_CAMERA_CFG* pCameraCfg = reinterpret_cast<IVS_CAMERA_CFG *>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetCameraCfgGetXML(pCameraCfg,pDevCode,xml);
	//	// }	
	//	// break;
 // //   // 摄像机码流参数设置 IVS_CAMERA_STREAM_CFG
 // //   case CONFIG_CAMERA_STREAM_CFG: 
	//	// {
	//	//	 IVS_CAMERA_STREAM_CFG* pCameraStrCfg = reinterpret_cast<IVS_CAMERA_STREAM_CFG *>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetCameraStreamCfgGetXML(pCameraStrCfg, pDevCode, xml);
	//	// }		 
	//	// break;
	// //// 摄像机图像显示参数 IVS_CAMERA_DISPLAY_CFG
	// //case CONFIG_CAMERA_DISPLAY_CFG:
	//	// {
	//	//	 IVS_CAMERA_DISPLAY_CFG* pCameraDisplayCfg = reinterpret_cast<IVS_CAMERA_DISPLAY_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetCameraDisplayCfgGetXml(pCameraDisplayCfg,pDevCode,xml);
	//	// }	 
	//	// break;
	// //// 告警输入子设备设置
	// //case CONFIG_ALARM_IN_CFG:
	//	// {
	//	//	 IVS_ALARM_IN_CFG* pAlarmInCfg = reinterpret_cast<IVS_ALARM_IN_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetAlarmInCfgGetXML(pAlarmInCfg, xml);
	//	// }
	//	// break;
	// //// 告警输出子设备设置
	// //case CONFIG_ALARM_OUT_CFG:
	//	// {
	//	//	 IVS_ALARM_OUT_CFG* pAlarmOutCfg = reinterpret_cast<IVS_ALARM_OUT_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetALarmOutCfgGetXML(pAlarmOutCfg, xml);
	//	// }	
	//	// break;
	// ////云台参数
 // //   case CONFIG_DEVICE_PTZ_CFG: 
	//	// {
	//	//	 IVS_PTZ_CFG* pCameraPTZCfg = reinterpret_cast<IVS_PTZ_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetPTZCfgGetXML(pCameraPTZCfg, chDevCode,xml);
	//	// }	
	//	// break;
	// //// 图像遮挡
	// //case CONFIG_CAMERA_VIDEO_HIDE_ALARM_CFG:
	//	// {
	//	//	 IVS_VIDEO_HIDE_ALARM* pVideoHideAlarm = reinterpret_cast<IVS_VIDEO_HIDE_ALARM*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetVideoHideAlarmGetXML(pVideoHideAlarm, xml);
	//	// }
	//	// break;
	// ////隐私保护
	// //case CONFIG_CAMERA_VIDEO_MASK_CFG: 
	//	// {
	//	//	 IVS_VIDEO_MASK* pVideoMask = reinterpret_cast<IVS_VIDEO_MASK*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetVideoMaskConfigGetXml(pVideoMask, xml);
	//	// }	
	//	// break;
	// ////语音子设备信息
	// //case CONFIG_CAMERA_AUDIO_CFG:
	//	// {
	//	//	 IVS_AUDIO_CFG* pAudioCfg = reinterpret_cast<IVS_AUDIO_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetAudioCfgGetXML(pAudioCfg,chDevCode,xml);
	//	// }		
	//	// break;
	// //// 设备时间参数
	// //case CONFIG_DEVICE_TIME_CFG:
	//	// {
	//	//	 IVS_DEVICE_TIME_CFG* pDevTimeCfg = reinterpret_cast<IVS_DEVICE_TIME_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetDevTimeCfgGetXML(pDevTimeCfg, xml);
	//	// }	
	//	// break;
	// ////串口子设备
	// //case CONFIG_SERIAL_CHANNEL_CFG:
	//	// {
	//	//	 IVS_SERIAL_CHANNEL_CFG* pSerialChannelCfg = reinterpret_cast<IVS_SERIAL_CHANNEL_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetSerialChannelCfgGetXML(pSerialChannelCfg, xml);
	//	// }	
	//	// break;
	// //// 摄像机图像扩展参数
	// //case CONFIG_CAMERA_IMAGING_CFG:
	//	// {
	//	//	 IVS_CAMERA_IMAGING_CFG* pCameraImgCfg = reinterpret_cast<IVS_CAMERA_IMAGING_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetCameraImgCfgGetXML(pCameraImgCfg, pDevCode, xml);
	//	// }	
	//	// break;
	// //// 摄像机扩展参数
	// //case CONFIG_CAMERA_EXTEND_CFG:
	//	// {
	//	//	 IVS_CAMERA_EXTEND_CFG* pCameraExCfg = reinterpret_cast<IVS_CAMERA_EXTEND_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetCameraExCfgGetXML(pCameraExCfg, xml);
	//	// }
	//	// break;
 // //  // 摄像机抓拍参数设置
	// //case CONFIG_CAMERA_SNAPSHOT_CFG:
	//	// {
	//	//	 IVS_CAMERA_SNAPSHOT_CFG* pCameraSnapshotCfg = reinterpret_cast<IVS_CAMERA_SNAPSHOT_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::GetCameraSnapshotCfgGetXML(pCameraSnapshotCfg, pDevCode, xml);
	//	// }	
	//	// break;
	// //// 其他
 // //   default:
	//	// {
	//	//	 iRet = IVS_OCX_CU_XML_INVALID;
	//	//	 BP_RUN_LOG_ERR(iRet, "Get DeviceConfig GetXml", "lConfigType fail");
	//	// }  
	//	// break;
 // //   }

 // //   return iRet;
 //}

 //// 查询设备参数统一解析XML
 //IVS_INT32 COCXDeviceParaConfig::GetDeviceConfigParseXml(IVS_VOID* &pBuffer, LONG lConfigType, CXml &xml)
 //{
 //    IVS_DEBUG_TRACE("");

	// FnSetDeviceParaParseXML fnSetProcess = GetFnSetDevicePara(static_cast<IVS_UINT32>(lConfigType));
	// if (NULL == fnSetProcess)
	// {
	//	 BP_RUN_LOG_ERR(IVS_FAIL, "no process fn","iRet = %d", IVS_FAIL);
	//	 return IVS_FAIL;
	// }
	// return (this->*fnSetProcess)(pBuffer, xml);
 // //   switch(lConfigType)
 // //   {
 // //   // 主设备基本参数 IVS_DEVICE_CFG
 // //   case CONFIG_DEVICE_CFG:   
	//	// {
	//	//	 IVS_DEVICE_CFG *pMainDevCfg = reinterpret_cast<IVS_DEVICE_CFG*>(pBuffer);   
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetMainDevConfigParseXml(pMainDevCfg, xml);     
	//	// }  
	//	// break;
 // //   // 摄像机移动侦测参数设置 
 // //   case CONFIG_CAMERA_MOTION_DETECTION_CFG:
	//	// {
	//	//	 IVS_MOTION_DETECTION* pMotionDetec = reinterpret_cast<IVS_MOTION_DETECTION*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetMotionDetecParseXML(pMotionDetec, xml);  
	//	// }  
	//	// break;
 // //   // 摄像机OSD参数设置 
 // //   case CONFIG_CAMERA_OSD_CFG:   
	//	// {
	//	//	 IVS_CAMERA_OSD_CFG* pCameraOsgCfg = reinterpret_cast<IVS_CAMERA_OSD_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetCameraOsgCfgParseXML(pCameraOsgCfg, xml);
	//	// }    
	//	// break;
 // //   // 主设备网络参数 IVS_DEVICE_NET_CFG
 // //   case CONFIG_DEVICE_NET_CFG:  
	//	// {
	//	//	 IVS_DEVICE_NET_CFG* pDevNetCfg = reinterpret_cast<IVS_DEVICE_NET_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetDevNetCfgParseXML(pDevNetCfg, xml);
	//	// }	 
	//	// break;
	// //// 摄像机基本参数
 // //   case CONFIG_CAMERA_CFG:
	//	// {
	//	//	 IVS_CAMERA_CFG* pCameraCfg = reinterpret_cast<IVS_CAMERA_CFG *>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetCameraCfgParseXML(xml,pCameraCfg);
	//	// }		
	//	// break;
 // //   // 摄像机码流参数设置 IVS_CAMERA_STREAM_CFG
 // //   case CONFIG_CAMERA_STREAM_CFG:
	//	// {
	//	//	 IVS_CAMERA_STREAM_CFG* pCameraStrCfg = reinterpret_cast<IVS_CAMERA_STREAM_CFG *>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetCameraStreamCfgParseXML(pCameraStrCfg, xml);
	//	// }	
	//	// break;
	// //// 摄像机图像显示参数
	// //case CONFIG_CAMERA_DISPLAY_CFG:
	//	// {
	//	//	 IVS_CAMERA_DISPLAY_CFG* pCameraDisplayCfg = reinterpret_cast<IVS_CAMERA_DISPLAY_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetCameraDisplayCfgParseXml(xml,pCameraDisplayCfg);
	//	// }		
	//	// break;
 // //   // 摄像机云台参数
 // //   case CONFIG_DEVICE_PTZ_CFG: 
	//	// {
	//	//	 IVS_PTZ_CFG* pCameraPTZcfg = reinterpret_cast<IVS_PTZ_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetCameraPTZCfgParseXml(xml,pCameraPTZcfg);
	//	// }	 
	//	// break;
	// //// 摄像机音频参数设置
	// //case CONFIG_CAMERA_AUDIO_CFG:
	//	// {
	//	//	 IVS_AUDIO_CFG* pAudiocfg = reinterpret_cast<IVS_AUDIO_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetAudioCfgParseXml(xml,pAudiocfg);
	//	// }		
	//	// break;
	// //// 告警输入子设备
	// //case CONFIG_ALARM_IN_CFG:
	//	// {
	//	//	 IVS_ALARM_IN_CFG* pAlarmInCfg = reinterpret_cast<IVS_ALARM_IN_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetAlarmInCfgParseXML(pAlarmInCfg, xml);
	//	// }		
	//	// break;
	// //// 告警输出子设备
	// //case CONFIG_ALARM_OUT_CFG:
	//	// {
	//	//	 IVS_ALARM_OUT_CFG* pAlarmOutCfg = reinterpret_cast<IVS_ALARM_OUT_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetAlarmOutCfgParseXML(pAlarmOutCfg, xml);
	//	// }		
	//	// break;
 // //   // 摄像机隐私保护参数设置
	// //case CONFIG_CAMERA_VIDEO_MASK_CFG:
	//	// {
	//	//	 IVS_VIDEO_MASK* pVideoMask = reinterpret_cast<IVS_VIDEO_MASK*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetVideoMaskConfigParseXml(pVideoMask, xml);
	//	// }	
	//	// break;
	// //// 摄像机遮挡告警参数设置
	// //case CONFIG_CAMERA_VIDEO_HIDE_ALARM_CFG:
	//	// {
	//	//	 IVS_VIDEO_HIDE_ALARM* pVideoHideAlarm = reinterpret_cast<IVS_VIDEO_HIDE_ALARM*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetVideHideAlarmParseXML(pVideoHideAlarm, xml);
	//	// }		
	//	// break;
	// //// 主设备时间参数
	// //case CONFIG_DEVICE_TIME_CFG:
	//	// {
	//	//	 IVS_DEVICE_TIME_CFG* pDevTimeCfg = reinterpret_cast<IVS_DEVICE_TIME_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetDevTimeCfgParseXML(pDevTimeCfg, xml);
	//	// }	
	//	// break;
	// //// 摄像机串口参数设置
	// //case CONFIG_SERIAL_CHANNEL_CFG:
	//	// {
	//	//	 IVS_SERIAL_CHANNEL_CFG* pDevTimeCfg = reinterpret_cast<IVS_SERIAL_CHANNEL_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetSerialChannelCfgParseXML(pDevTimeCfg, xml);
	//	// }	
	//	// break;
 // //   // 摄像机扩展图像参数
	// //case CONFIG_CAMERA_IMAGING_CFG:
	//	// {
	//	//	 IVS_CAMERA_IMAGING_CFG* pCameraImgCfg = reinterpret_cast<IVS_CAMERA_IMAGING_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetCameraImgCfgParseXML(pCameraImgCfg, xml);
	//	// }	
	//	// break;
	// //// 摄像机扩展参数设置
	// //case CONFIG_CAMERA_EXTEND_CFG:
	//	// {
	//	//	 IVS_CAMERA_EXTEND_CFG* pCameraExCfg = reinterpret_cast<IVS_CAMERA_EXTEND_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetCameraExCfgParseXML(pCameraExCfg, xml);
	//	// }
	//	// break;
	// //// 摄像机抓拍参数设置
	// //case CONFIG_CAMERA_SNAPSHOT_CFG:
	//	// {
	//	//	 IVS_CAMERA_SNAPSHOT_CFG* pCameraSnapshotCfg = reinterpret_cast<IVS_CAMERA_SNAPSHOT_CFG*>(pBuffer);
	//	//	 iRet = COCXDeviceParaConfigXMLProcess::SetCameraSnapshotCfgParseXML(pCameraSnapshotCfg, xml);
	//	// }
	//	// break;
 // //   // 其他
 // //   default:
	//	// {
	//	//	 iRet = IVS_OCX_CU_XML_INVALID;
	//	//	 BP_RUN_LOG_ERR(iRet, "Get DeviceConfig ParseXml fail", "lConfigType fail");
	//	// }   
	//	// break;
 // //   }
 //}

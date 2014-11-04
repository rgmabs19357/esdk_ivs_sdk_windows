#pragma once
#include <afx.h>
#include <list>
#include "IVS_SDKTVWall.h"
#include "SDKDef.h"
#include "vos.h"

//电视墙窗格结构
typedef struct  
{
	//IVS_CHAR szChannelCode[IVS_TVWALL_DECODER_CODE_LEN];//绑定的通道编码
	unsigned long ulChannelID;							//绑定的通道编码
	IVS_CHAR szChannelName[IVS_TVWALL_DECODER_CODE_LEN];//通道名称
	IVS_UINT32 uiChannelType;
}TVWALL_PANE_CHANNEL;
typedef std::list<TVWALL_PANE_CHANNEL> TVWALL_PANE_CHANNEL_LIST;
typedef struct  
{
	IVS_ULONG ulWndID;									//窗格ID
    // add by wanglei 00165153:2013.7.26 DTS2013071507953
    // 【SDK_多进一出】多进一出布局，开启组轮巡，组轮巡窗格播放窗格错误
    // 原先窗格编号采用 【1】 【2 5 6 7】 【3】 【4】
    // 现在改成【1】 【2 3 4 5】 【6】 【7】
    // 原因是电视墙轮巡采用编号递增访问，方式乱序
    IVS_ULONG ulPaneName;                              // 该标识符记住原先的窗格号，以便建立索引关系
	IVS_CHAR szDecoderID[IVS_TVWALL_DECODER_CODE_LEN];	//绑定的解码器ID
	//IVS_UINT32 uiChannel;								//绑定的通道
	CRect rect;											//窗格位置
	IVS_UINT32 uiChannelNum;							//窗格绑定的通道数
	TVWALL_PANE_CHANNEL_LIST listChanel;				//通道list
}TVWALL_PANE;
typedef std::list<TVWALL_PANE> TVWALL_PANE_LIST;

//电视墙包含通道的虚拟窗格ID
typedef struct
{
	ULONG ulParentWndID;
	ULONG ulChildWndID;
}TVWall_VPANEID;//电视墙虚拟窗格
typedef std::list<TVWall_VPANEID> TVWall_VPANEID_LIST;

class COCXTVWallXMLProcess
{
public:
	~COCXTVWallXMLProcess(void);

	static COCXTVWallXMLProcess& GetInst();

	// 获取电视墙列表
	IVS_VOID GetTVWallList(CString &strResult)const;
	// 获取电视墙信息
	IVS_VOID GetTVWallByID(IVS_UINT32 uiTVWallID, CString& strResult)const;
	// 增加电视墙
	CString AddTVWall(const CString& strTVWall);
	// 删除电视墙
	IVS_INT32 DeleteTVWall(IVS_ULONG uiTVWallID);
	// 更改电视墙信息
	IVS_INT32 ModifyTVWallInfo(const CString& strResult);
	// 更改电视墙布局
	IVS_INT32 ModifyTVWallLayout(const CString& strResult);
	// 根据窗口号获取解码器和通道号
	IVS_INT32 GetDecoderIDAndChannelNo(IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID, IVS_TVWALL_PARAM& stTVWallParam);
	//解码器注册信息XML
	IVS_INT32 GetRemoteDecoderRegXML(const CUMW_NOTIFY_REMOTE_DECODER_STATUS& stDecoderInfo, char* pXmlBuf, UINT bufLen)const;
	//解码器断连信息XML
	IVS_INT32 GetRemoteDecoderEndXML(const CUMW_NOTIFY_REMOTE_DECODER_OFF_LINE_INFO& stInitInfo, char* pXmlBuf, UINT bufLen)const;
    //视频播放异步通知
    IVS_INT32 GetNotifyAsyncRetXML(const CUMW_NOTIFY_ASYNC_RET_EX& stAsyncRet, char* pXmlBuf, UINT bufLen)const;
    //解析实时浏览参数XML
    IVS_INT32 RealPlayParamParseXML(const IVS_CHAR* pRealplayParam, IVS_REALPLAY_PARAM& RealplayParam)const;
    //解析平台回放参数XML
    IVS_INT32 PlatformPlaybackParamParseXML(const IVS_CHAR* pPlaybackParam, IVS_PLAYBACK_PARAM& PlaybackParam)const;
	//获取解码器信息列表（在线列表）
	IVS_INT32 GetDecoderInfoList(CUMW_NOTIFY_REMOTE_DECODER_STATUS* pDecoderInfoList, IVS_UINT32 uiDecoderNum, CString& strResult)const;
	//根据电视墙编号获取布局信息
	IVS_INT32 GetTVWallPaneList(IVS_ULONG ulTVWallID, TVWALL_PANE_LIST& paneList, IVS_UINT32& uiHonCount, IVS_UINT32& uiVerCount)const;
    //获取电视墙回放异常的XML
    IVS_INT32 GetRemotePlayBackXML(int iWinID, unsigned long ulErrorCode, char* pXmlBuf, UINT bufLen)const;
    //获取所有电视墙ID
    IVS_INT32 GetTVWallIdList(std::list<int>& listTVWallId)const;
    //开始上墙的异步通知
    IVS_INT32 GetNotifyStartRealPlayTVWallRetXML(ULONG ulTVWallID, ULONG ulWndID, IVS_INT32 iErrCode, ULONG ulTransID, char* pXmlBuf, UINT bufLen)const;
	//读网络键盘配置文件
	IVS_INT32 GetNetKeyBoardConfig(IVS_INT32& iMin, IVS_INT32& iMax)const;
private:
	COCXTVWallXMLProcess(void);

	/**********************************************************************
	* name			: MakeSureTVWallDirExist
	* description	: 检查电视墙配置的文件夹，默认路径是app data/ivs/tvwall/
	* input			: NA
	* output     	: NA
	* return     	: std::string - 文件夹的目录
	* remark     	: NA   
	*********************************************************************/
	std::string MakeSureTVWallDirExist()const;
	IVS_INT32 AddTVWall(IVS_INT32& iNewID, const CString& strTVWall, CXml& xml)const;
	IVS_INT32 DeleteTVWall(IVS_ULONG uiTVWallID, CXml& xml)const;
	IVS_INT32 ModifyTVWallInfo(const CString& strResult, CXml& xml)const;
	IVS_INT32 ModifyTVWallLayout(const CString& strResult, CXml& xml)const;
	IVS_INT32 GetTVWallByID(IVS_UINT32 uiTVWallID, CString& strResult, CXml& xml)const;

	//根据窗口ID从临时场景获取对应的解码器和通道
	IVS_INT32 GetDecoderIDAndChannelNo(IVS_ULONG ulTVWallID, const TVWall_VPANEID& vPaneID, IVS_TVWALL_PARAM& stTVWallParam, CXml& xmlTVWall)const;
	//获取增加电视墙的返回XML
	CString GetAddTVWallResultXML(IVS_INT32 iTVWallID, IVS_INT32 iRet)const;
	//解析Layout放到list中
	IVS_INT32 ParesTVWallLayoutXML2List(CXml& xml, TVWALL_PANE_LIST& paneList, IVS_UINT32& urHonCount, IVS_UINT32& uiVerCount)const;
	// 增加一个解码器节点
	IVS_VOID AddDecoderNode(CXml& xml, const CUMW_NOTIFY_REMOTE_DECODER_STATUS& stDecoderInfo, bool bChild, bool bOnline)const;

private:
	//读写电视墙XML配置文件的锁
	VOS_Mutex *m_TVWallXmlMutex;    

	//缓存在内存的链表中的电视墙XML配置信息
	TVWALL_PANE_LIST m_paneList;

	//是否读电视墙的XML配置信息放在内存中
	bool m_bSaveinMem;
};


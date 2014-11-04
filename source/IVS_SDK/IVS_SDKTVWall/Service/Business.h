/********************************************************************
 filename    :    Business.h
 author      :    g00209332
 created     :    2012/12/19
 description :    业务对象
 copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
 history     :    2012/12/19 从C30继承
*********************************************************************/

#pragma once

#include "CumwCommon.h"
namespace CUMW
{
	///////////////////////////////////////////////////////////////////////////
	/// @brief 业务类型掩码
	///
	/// Description.
	///////////////////////////////////////////////////////////////////////////
	enum BUSINESS_TYPE_MASK
	{
		BUSINESS_TYPE_MASK_NONE = 0x00000000,   ///< 空业务
		BUSINESS_TYPE_MASK_SRC_ALL = 0x0000FFFF,   ///< 类型掩码: 所有来源
		BUSINESS_TYPE_MASK_DST_ALL = 0xFFFF0000,   ///< 类型掩码: 所有目标
		BUSINESS_TYPE_MASK_ALL = 0xFFFFFFFF,   ///< 类型掩码: 所有类型

		//目标类型掩码
		BUSINESS_TYPE_MASK_DST_LOCAL   = 0x00010000,                    ///< 本地显示
		BUSINESS_TYPE_MASK_DST_REMOTE  = 0x00020000,                    ///< 远程显示
		BUSINESS_TYPE_MASK_DST_PREVIEW = 0x00040000,   ///< 远程显示之本地预览
		BUSINESS_TYPE_MASK_DST_FILE = 0x00080000,   ///< 保存为文件
		BUSINESS_TYPE_MASK_DST_NET_AUDIO = 0x00100000,   ///< 网络语音
		BUSINESS_TYPE_MASK_DST_DATA_CALL_BACK = 0x00200000,   ///< 数据回调
		BUSINESS_TYPE_MASK_DST_REMOTE_DIRECT_CONNECT = 0x00400000,   ///< 远程显示直连视频源，可以是MDU或者PU

		//来源类型掩码
		BUSINESS_TYPE_MASK_SRC_REAL_VIDEO = 0x00000001,   ///< 实时浏览
		BUSINESS_TYPE_MASK_SRC_NET_PLAY_BACK = 0x00000002,   ///< 网络录像回放
		BUSINESS_TYPE_MASK_SRC_RECORD_DOWN = 0x00000004,   ///< 网络录像下载
		BUSINESS_TYPE_MASK_SRC_NET_AUDIO   = 0x00000008,    ///< 网络语音，语音对讲业务的来源类型
		BUSINESS_TYPE_MASK_SRC_LOCAL_AUDIO = 0x00000010   ///< 本地语音，音频广播业务的来源类型
	};

	///////////////////////////////////////////////////////////////////////////
	/// @brief 业务类型
	///
	/// Description.
	///////////////////////////////////////////////////////////////////////////
	enum BUSINESS_TYPE
	{
		BUSINESS_TYPE_NONE = 0x00000000,   ///< 空业务

		//初始化解码器
		BUSINESS_TYPE_INIT_DECORDER,

		//启动解码器
		BUSINESS_TYPE_START_DECORDER
	};

	const unsigned long INVALID_BUSINESS_ID = 0;    //无效的业务ID
}
class CBusiness
{
public:
    /**************************************************************************
    * name       : CBusiness
    * description: Default constructor
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    CBusiness();

    /**************************************************************************
    * name       : ~CBusiness
    * description: Destructor
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual ~CBusiness();

    /**************************************************************************
    * name       : GetBusinessType
    * description: 获取业务类型
    * input      : NA
    * output     : NA
    * return     : 业务类型
    * remark     : NA
    **************************************************************************/
    CUMW::BUSINESS_TYPE      GetBusinessType() const;

    /**************************************************************************
    * name       : GetBusinessSrcType
    * description: 获取业务来源类型
    * input      : NA
    * output     : NA
    * return     : 业务来源类型
    * remark     : NA
    **************************************************************************/
    CUMW::BUSINESS_TYPE_MASK GetBusinessSrcType() const;

    /**************************************************************************
    * name       : GetBusinessDstType
    * description: 获取业务目标类型
    * input      : NA
    * output     : NA
    * return     : 业务目标类型
    * remark     : NA
    **************************************************************************/
    CUMW::BUSINESS_TYPE_MASK GetBusinessDstType() const;

    /**************************************************************************
    * name       : SetLinkMode
    * description: 设置连接模式
    * input      : ulLinkMode  连接模式
    * output     : NA
    * return     : 设置成功与否
    * remark     : NA
    **************************************************************************/
    BOOL                     SetLinkMode
    (   IN unsigned long ulLinkMode);

    /**************************************************************************
    * name       : GetLinkMode
    * description: 获取连接模式
    * input      : NA
    * output     : NA
    * return     : 连接模式
    * remark     : NA
    **************************************************************************/
    unsigned long            GetLinkMode() const;

    /**************************************************************************
    * name       : SetProtocolType
    * description: 设置协议类型
    * input      : ulProtocolType  协议类型
    * output     : NA
    * return     : 设置成功与否
    * remark     : NA
    **************************************************************************/
    BOOL                     SetProtocolType
    (   IN unsigned char ulProtocolType);

    /**************************************************************************
    * name       : GetProtocolType
    * description: 获取协议类型
    * input      : NA
    * output     : NA
    * return     : 协议类型
    * remark     : NA
    **************************************************************************/
    unsigned char            GetProtocolType() const;

    /**************************************************************************
    * name       : GetDecodeProtocolType
    * description: 获取解码器用的协议类型
    * input      : NA
    * output     : NA
    * return     : 协议类型
    * remark     : NA
    **************************************************************************/
    unsigned char            GetDecodeProtocolType() const;

    /**************************************************************************
    * name       : SetPlayType
    * description: 设置播放类型。若没有播放类型不需设置，子类需重写返回FALSE
    * input      : ulPlayType  播放类型，决定业务目标类型
                   hWnd            窗口句柄，有效性取决于播放类型
    * output     : NA
    * return     : 设置成功与否
    * remark     : NA
    **************************************************************************/
    virtual BOOL             SetPlayType
    (   IN unsigned long ulPlayType);

    /**************************************************************************
    * name       : GetBusinessTypeString
    * description: 获取业务类型字符串
    * input      : NA
    * output     : NA
    * return     : 协议类型字符串
    * remark     : NA
    **************************************************************************/
    const char*              GetBusinessTypeString() const;

    /**************************************************************************
    * name       : GetOpenMediaChannelPara
    * description: 获取打开媒体通道参数
    * input      : stPara   打开媒体通道参数
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void             GetOpenMediaChannelPara
    (   IO CUMW::OPEN_MEDIA_CHANNEL_PARA& stPara) const;

    /**************************************************************************
    * name       : SetServerMediaInfo
    * description: 设置服务端的媒体信息
    * input      : stSvrMediaInfo   服务端的媒体信息
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void                     SetServerMediaInfo
    (   IN const CUMW::SERVER_MEDIA_INFO& stSvrMediaInfo);

    /**************************************************************************
    * name       : CopyServerMediaInfo
    * description: 复制服务器端的媒体信息
    * input      : objSharedBusiness    共享的业务对象
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    void                     CopyServerMediaInfo
    (   IN const CBusiness& objSharedBusiness);

    /**************************************************************************
    * name       : SetRecordFrameRate
    * description: 设置业务ID
    * input      : ulFrameRate     视频帧率
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual void             SetBusinessId
    (   IN unsigned long ulBusinessId);

    /**************************************************************************
    * name       : GetBusinessId
    * description: 获取业务ID
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    virtual unsigned long    GetBusinessId() const;

    /**************************************************************************
    * name       : IsRemoteBusiness
    * description: 检查是否为远程上墙业务
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    BOOL                     IsRemoteBusiness() const;

    /**************************************************************************
    * name       : IsRemoteDirectConnect
    * description: 检查是否为远程上墙直连
    * input      : NA
    * output     : NA
    * return     : NA
    * remark     : NA
    **************************************************************************/
    BOOL                     IsRemoteDirectConnect() const;

public: //以下成员变量由外界直接访问
    // 远程解码器ID
    char m_szDecoderId[CUMW_DECODER_ID_LEN + 1];

    //远程解码器的通道编号
    unsigned long m_ulChannelId;

    //媒体会话ID
    char m_szSessionId[CUMW::MAX_LEN_SESSION_ID + 1];

    //媒体服务器IP
    char m_szServerIp[IVS_IP_LEN];

    //媒体本地IP
    char m_szLocalIp[IVS_IP_LEN];

    //解码器IP
    char m_szDecoderIp[IVS_IP_LEN];

    //媒体分发IP
    char m_szMediaDistributeIp[IVS_IP_LEN];

    //解码器端口信息
    CUMW::PORT_INFO m_stDecoderPortInfo;

    //媒体分发端口信息
    CUMW::PORT_INFO m_stMediaDistributePortInfo;

    //媒体服务器端口信息
    CUMW::PORT_INFO m_stServerPortInfo;

    //本地端口信息
    CUMW::PORT_INFO m_stLocalPortInfo;

    //视频PT
    CUMW::PAY_LOAD_TYPE m_enVideoPt;

    //音频PT
    CUMW::PAY_LOAD_TYPE m_enAudioPt;

    // 会话URL
    char m_szSessionUrl[CUMW_PATH_LEN];

    //开始时间,NTP时间，自从1900以来的秒数
    unsigned long m_ulStartTime;

    //录像结束时间,NTP时间，自从1900以来的秒数
    unsigned long m_ulEndTime;

    //异步序号
    unsigned long m_ulAsyncSeq;

    //是否正在复用。InitBusiness和StopBusiness时赋值，如果正在复用，则不需发送媒体通道打开/关闭操作
    BOOL m_bReusing;

    //共享视频流的业务ID。InitBusiness时，若正在复用则赋值。用于备份业务对象信息
    unsigned long m_ulSharedBusinessId;

    //BEGIN R002C01SPC100 ADD 2011-07-26 ligengqiang 00124479 for 支持视频PayloadType动态变化
    //Ingress是否已启动
    BOOL m_bIngressStart;
    //END   R002C01SPC100 ADD 2011-07-26 ligengqiang 00124479 for 支持视频PayloadType动态变化

    unsigned long m_ulInitBusinessId;

    //播放/停止播放音频消息
    unsigned char m_cAudioCmd;

    //音量大小
    unsigned char m_cVolume;

protected:  //以下成员变量只能通过成员函数访问
    //业务来源类型
    CUMW::BUSINESS_TYPE_MASK m_enBusinessSrcType;

    //业务目标类型
    CUMW::BUSINESS_TYPE_MASK m_enBusinessDstType;

    //连接模式
    unsigned long m_ulLinkMode;

    //协议类型
    unsigned char m_ucProtocolType;

    //业务ID
    unsigned long m_ulBusinessId;

    //帧率
    unsigned long m_ulFrameRate;
};


#ifndef UTILITY_H_58C1FA75_4006_44EC_B6F6_12DF299F0ECB_
#define UTILITY_H_58C1FA75_4006_44EC_B6F6_12DF299F0ECB_
#pragma once

#include "stdafx.h"


#ifndef _CVIDEOPANEMGR_DECLARATION_
#define _CVIDEOPANEMGR_DECLARATION_
class CVideoPaneMgr;
#endif


class CUtility
{
public:
    static bool CheckIsWin7(void);

    //从资源获取PNG图片 到 Image
    //static  bool ImageFromIDResource(UINT nID, LPCTSTR sTR,Image * &pImg);

private:
    static CVideoPaneMgr *pVideoPaneMgr;
public:
    static CVideoPaneMgr * GetVideoPaneMgr();
    static void SetVideoPaneMgr(CVideoPaneMgr* val); 

};

#endif // UTILITY_H_58C1FA75_4006_44EC_B6F6_12DF299F0ECB_

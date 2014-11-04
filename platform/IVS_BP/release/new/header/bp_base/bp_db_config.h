/********************************************************************
filename    :    bp_db_config.h
author      :    cwx148380
created     :    2012/11/08
description :    读取数据库配置
copyright   :    Copyright (c) 2012-2016 Huawei Tech.Co.,Ltd
history     :    2012/11/08 初始版本
*********************************************************************/
#ifndef BP_DB_CONFIG_H
#define BP_DB_CONFIG_H

#include <string>
#include "bp_namespace.h"

typedef enum
{
    DB_ORACLE = 0,
    DB_PGSQL = 1
}DB_CONNECT_TYPE; //lint !e129 !e10

NAMESPACE_BP_BEGIN //lint !e129 !e10

class CDatabaseConnectionConfig 
{
public:
    CDatabaseConnectionConfig () : m_ConnType ( DB_ORACLE ), m_iPoolSize ( 10 )  {}
    ~CDatabaseConnectionConfig () {}

    const DB_CONNECT_TYPE GetDbType() const { return this->m_ConnType; }
    void SetDbType ( const DB_CONNECT_TYPE connType ) { this->m_ConnType = connType; }

    const std::string& GetDbService() const { return this->m_strService; }
    void SetDbService ( const std::string& strService ) { this->m_strService = strService; }

    const std::string& GetDbUserName() const { return this->m_strUserName; }
    void SetDbUserName ( const std::string& strUserName ) { this->m_strUserName = strUserName; }

    const std::string& GetDbPassword() const { return this->m_strPassword; }
    void SetDbPassword ( const std::string& strPassword ) { this->m_strPassword = strPassword; }

    int GetDbPoolSize() const { return this->m_iPoolSize; }
    void SetDbPoolSize ( int poolsize ) { this->m_iPoolSize = poolsize; }

private:
    DB_CONNECT_TYPE m_ConnType;  //数据库类型
    std::string m_strService;    //数据库服务名
    std::string m_strUserName;   //数据库用户名
    std::string m_strPassword;   //数据库密码
    int         m_iPoolSize;     //数据库连接池大小
};

NAMESPACE_BP_END   //

#endif //BP_DB_CONFIG_H


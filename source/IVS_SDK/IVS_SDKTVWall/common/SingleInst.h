/********************************************************************
	filename	: 	SingleInst.h
	author		:	s00191067
	created		:	2011/10/28	
	description	:	实现类的单实例
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/28
*********************************************************************/

#ifndef _SIGNLE_INST_H_
#define _SIGNLE_INST_H_

// .h文件中的声明
#define DECLARE_SINGLE_INSTANCE(class_name)         \
public:                                             \
    static class_name* GetInstance();               \
    static void ReleaseInstance();                  \
private:                                            \
    static class_name* m_pInstance;                 \

// .cpp文件中的声明
#define IMPLEMENT_SINGLE_INSTANCE(class_name)       \
class_name* class_name::m_pInstance = NULL;         \
                                                    \
class_name* class_name::GetInstance()               \
{                                                   \
    if (NULL == m_pInstance)                        \
    {                                               \
        try                                         \
        {                                           \
            m_pInstance = new class_name;           \
        }                                           \
        catch (...)                                 \
        {                                           \
            m_pInstance = NULL;                     \
        }                                           \
    }                                               \
    return m_pInstance;                             \
}                                                   \
                                                    \
void class_name::ReleaseInstance()                  \
{                                                   \
    if (NULL != m_pInstance)                        \
    {                                               \
        delete m_pInstance;                         \
        m_pInstance = NULL;                         \
    }                                               \
}

#endif //_SIGNLE_INST_H_

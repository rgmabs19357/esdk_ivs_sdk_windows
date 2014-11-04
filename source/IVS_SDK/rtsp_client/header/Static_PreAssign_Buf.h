//lint -e1733

#ifndef __STATIC_PREASSIGN_BUFFER_H__
#define __STATIC_PREASSIGN_BUFFER_H__

#include <ace_header.h>
#define DEFAULT_BUFFER_SIZE  (128 * 1024)

class Static_PreAssign_Buffer
{
public:
    Static_PreAssign_Buffer()
    {
        m_bufBlock = NULL;
        try
        {
            m_bufBlock = new char[DEFAULT_BUFFER_SIZE]; // lint !e1733
        }
        catch(...)
        {
            m_nBufferLength = 0;
            m_pHead = NULL;
            m_pTail = NULL;
            m_iLength = 0;
            return;
        }
        memset(this->m_bufBlock, 0, DEFAULT_BUFFER_SIZE);
        m_nBufferLength = DEFAULT_BUFFER_SIZE;
        m_pHead = m_bufBlock;
        m_pTail = m_bufBlock;
        m_iLength = 0;
    }



    //Static_PreAssign_Buffer(const Static_PreAssign_Buffer& tmp)
    //{
    //    copyTmpBuff(tmp); 
    //}

    Static_PreAssign_Buffer& operator=(const Static_PreAssign_Buffer& tmp)
    {
        if (this == &tmp)
        {
            return *this;
        }
        else
        {
            if (NULL != m_bufBlock)
            {
                delete []m_bufBlock;
                m_bufBlock = NULL;
            }

            copyTmpBuff(tmp);

        }
        return *this;
    }

    virtual ~Static_PreAssign_Buffer()
    {
        if (NULL != m_bufBlock)
        {
            delete[] m_bufBlock;
        }

        m_bufBlock = NULL;
        m_pHead = NULL;
        m_pTail = NULL;
        m_iLength = 0;
    }


    void copyTmpBuff(const Static_PreAssign_Buffer& tmp)
    {
        m_bufBlock = NULL;
        if (NULL != tmp.m_bufBlock)
        {
            try
            {
                m_bufBlock = new char[tmp.m_nBufferLength];
            }
            catch(...)
            {
                m_nBufferLength = 0;
                m_pHead = NULL;
                m_pTail = NULL;
                m_iLength = 0;
                return;
            }
            memset(this->m_bufBlock, 0, tmp.m_nBufferLength);
            (void)memcpy(m_bufBlock, tmp.m_bufBlock, tmp.m_nBufferLength);

            m_nBufferLength = tmp.m_nBufferLength;
            m_pHead = m_bufBlock + (tmp.m_pHead - tmp.m_bufBlock);
            m_pTail = m_bufBlock + (tmp.m_pTail - tmp.m_bufBlock);
            m_iLength = tmp.m_iLength;
        }
    }



    void wr_ptr(size_t n)
    {
        if (n > m_nBufferLength)
        {
            return;
        }

        if(NULL != m_bufBlock)
        {
            m_pTail = (char *) m_bufBlock + n;
            m_iLength = n;
        }
    }

    char * wr_ptr()
    {
        return m_pTail;
    }

    void rd_ptr(size_t n)
    {
        if (n > m_nBufferLength)
        {
            return;
        }

        if(NULL != m_bufBlock)
        {
            m_pHead = (char *) m_bufBlock + n;
        }
    }

    char * rd_ptr()
    {
        return m_pHead;
    }

    size_t size() const
    {
        return m_nBufferLength;
    }

    size_t length() const
    {
        return m_iLength;
    }

    int copy(const char *buf, size_t n)
    {
        if (n > m_nBufferLength)
        {
            return -1;
        }

        if(NULL != m_bufBlock)
        {
            memmove(m_bufBlock, buf, n);
            m_pHead = m_bufBlock;
            m_pTail = (char *) m_bufBlock + n;
            m_iLength = n;
        }

        return (int) n;
    }

private:
    char    *m_bufBlock;
    size_t  m_nBufferLength;
    char    *m_pHead;
    char    *m_pTail;
    size_t  m_iLength;
};

#endif /* __STATIC_PREASSIGN_BUFFER_H__ */


/*
创建: hotzeng(曾新海)
日期: 2011年12月19日
用途: 协议解析
用法: 
*/
#ifndef _BUFFER_H
#define _BUFFER_H

#include <arpa/inet.h>
#include <string.h>

#define MAX_CVBUFFER_SIZE 4*1024

class CVBuffer
{
public:
    CVBuffer(void);
    CVBuffer(const char* pBuffer,const unsigned int uSize);
    ~CVBuffer(void);

    void WrapperBuffer(char *pbuf, const unsigned int capacity, const unsigned int uSize){
        m_Buffer	= pbuf;
        m_capacity	= capacity;

        m_uOffset	= uSize;
        m_uGetOffset = 0;
    };

    const unsigned int getSize() const;
    const char* getBuffer() const;
    char* getBegin();
    bool setBuffer(const char* pBuffer,const unsigned int uSize);
    bool setSize(unsigned int uSize);

    template <typename type>
    bool Set(const type& tData)
    {
        unsigned int uSize =sizeof(type);

        if(m_uOffset+uSize>m_capacity)return false;

        memcpy(m_Buffer+m_uOffset,&tData,uSize);

        m_uOffset+=uSize;

        return true;
    }

    template <typename type>
    bool Get(type& tData)
    {
        unsigned int uSize =sizeof(type);

        if(m_uGetOffset+uSize>m_uOffset)return false;

        memcpy(&tData,m_Buffer+m_uGetOffset,uSize);

        m_uGetOffset+=uSize;

        return true;
    }

    bool Get(char* pData, const unsigned int uLength);
    bool Copy(char* pData,unsigned int uLength);
    bool Append(const char* pData, const unsigned int uLength);

    void Jump(unsigned int uLength)
    {
        m_uGetOffset+=uLength;

        if( m_uGetOffset > m_uOffset )m_uGetOffset = m_uOffset;
    }

    //获取读取位置内存
    char* JumpBuffer()
    {
        if( m_uOffset > m_uGetOffset )
        {
            return (char*)(m_Buffer+m_uGetOffset);
        }

        return m_Buffer;
    }

    //获取读取位置剩余大小
    int RemainSize()
    {
        if( m_uOffset > m_uGetOffset )
        {
            return m_uOffset-m_uGetOffset;
        }

        return 0;
    }

    void start()
    {
        Set((unsigned short)0x0);
        Set((char)0x02);
    }

	//服务器之间的开始字节
	void ss_start()
	{
		Set((unsigned short)0x0);
		Set((char)0x0a);
	}

    void end()
    {
        Set((char)0x03);
    }

    void setTcpMsgLen(unsigned short usLen)
    {
        unsigned short usTempLen = htons(usLen);
        memcpy(m_Buffer,&usTempLen, sizeof(unsigned short));
    }

    void setTLVLen(unsigned char ucLen)
    {
        memcpy(m_Buffer, &ucLen, sizeof(unsigned char));
    }

    unsigned int getGetOffset() 
    {
        return m_uGetOffset;
    }

    void Clear();

    template <typename type>
    CVBuffer& operator << (const type& v) 
    {                
        type nv = v;
        if(sizeof(type)==sizeof(unsigned short))
            nv = (type)htons((unsigned short)v);
        else if (sizeof(type)==sizeof(unsigned int))
            nv = (type)htonl((unsigned int)v);
        else if (sizeof(type)==sizeof(unsigned long))//for 64
            nv = (type)htonl((unsigned long)v);

        Append((char *)&nv,sizeof(type));
        return *this;  
    }

    template <typename type>
    CVBuffer& operator >> (type& v) 
    {
        if(sizeof(type) == 1 ||sizeof(type) == 2 || sizeof(type) == 4 || sizeof(type) == 8)
        {
            v = 0;
        }

        type nv;
        bool b = Get((char *)&nv,sizeof(type));

        if(b)
        {
            v = nv;
            if(sizeof(type)==sizeof(unsigned short))
                v = (type)ntohs((unsigned short)nv);
            else if ( sizeof(type) == sizeof(unsigned int) || sizeof(type)==sizeof(unsigned long) )
                v = (type)ntohl(nv);
        }

        return *this;  
    }

private:
    char m_Buf[MAX_CVBUFFER_SIZE];
    char *m_Buffer;//以后改为内存池
    unsigned int m_capacity;
    unsigned int m_uOffset;
    unsigned int m_uGetOffset;
};

#endif

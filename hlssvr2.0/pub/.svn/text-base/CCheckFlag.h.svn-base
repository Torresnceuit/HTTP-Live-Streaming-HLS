#ifndef _CCHECKFLAG_H
#define _CCHECKFLAG_H

const unsigned char FLG_CTRL_RELOAD = 0x01;   //通知主控对象重读配置    
const unsigned char FLG_CTRL_QUIT   = 0x02;   //通知主控对象退出

class CCheckFlag
{
public:
    CCheckFlag(): _flag((unsigned char)0){}
    ~CCheckFlag(){}
    void set_flag(unsigned char flag){ _flag |= flag;}
    bool is_flag_set( unsigned char flag )
    { 
        return (( _flag & flag ) == flag);
    };
    void clear_flag()
    {
        _flag = (unsigned char)0;
    }
protected:
    unsigned char _flag;
};

#endif


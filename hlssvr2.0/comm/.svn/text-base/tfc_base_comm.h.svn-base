
#ifndef __TFC_BASE_COMM_H__
#define __TFC_BASE_COMM_H__

#ifndef _XOPEN_SOURCE 
#define _XOPEN_SOURCE
#endif

#include <string>
#include <time.h>
#include <cstdio>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cstdlib>
#include <cctype>

#include <stdexcept>

using namespace std;

namespace tfc { namespace base {

struct comm_error: public logic_error{ comm_error(const string& s);};

void ignore_pipe(void);
void Daemon();

/**
 * @param s 输入字符串
 * @return bool 是否全部为数字(0-9)
 */
inline bool is_digits(const string& s)
{
	if(s.length() == 0) return false;
	for (size_t i = 0; i < s.length(); i++){
		if (!isdigit(s.c_str()[i])) return false;
	}
	return true;
}

/**
 * @param s 输入字符串
 * @return bool 是否全部为16进制数字(0-9/a-f/A-f)
 */
inline bool is_xdigits(const string& s)
{
	if(s.length() == 0) return false;
	for (size_t i = 0; i < s.length(); i++){
        char ch = s.c_str()[i];
		if (!isdigit(ch) && 
            !(('a' <= ch && ch <='f') || ('A' <= ch && ch <= 'F'))) 
        {
            return false;
        }
	}
	return true;
}

/**
 * @param s 输入字符串
 * @param filt filt串
 * @return string trim后的值
 */
inline string trim_left(const string &s,const string& filt=" ")
{
	char *head=const_cast<char *>(s.c_str());
	char *p=head;
	while(*p) {
		bool b = false;
		for(size_t i=0;i<filt.length();i++) {
			if((unsigned char)*p == (unsigned char)filt.c_str()[i]) {b=true;break;}
		}
		if(!b) break;
		p++;
	}
	return string(p,0,s.length()-(p-head));
}

/**
 * @param s 输入字符串
 * @param filt filt串
 * @return string trim后的值
 */
inline string trim_right(const string &s,const string& filt=" ")
{
	if(s.length() == 0) return string();
	char *head=const_cast<char *>(s.c_str());
	char *p=head+s.length()-1;
	while(p>=head) {
		bool b = false;
		for(size_t i=0;i<filt.length();i++) {
			if((unsigned char)*p == (unsigned char)filt.c_str()[i]) {b=true;break;}
		}
		if(!b) {break;}
		p--;
	}
	return string(head,0,p+1-head);
}

/**
 * trim_left+trim_right
 * @param s 输入字符串
 * @param filt filt串
 * @return string trim后的值
 */
inline string trim(const string &s,const string& filt=" ")
{
	return trim_right(trim_left(s, filt),filt);
}

inline string upper(const string &s)
{
	string s2;
	for(size_t i=0;i<s.length();i++) {
		s2 += ((unsigned char)toupper(s.c_str()[i]));
	}
	return s2;
}

inline string lower(const string &s)
{
	string s2;
	for(size_t i=0;i<s.length();i++) {
		s2 += ((unsigned char)tolower(s.c_str()[i]));
	}
	return s2;
}

/**
 * 时间字段
 * @see #time_add
 */
enum TimeField {Year,Month,Day,Hour,Minute,Second};

/**
 * 时间处理函数
 * @param t 源时间
 * @param f 时间字段
 * @param i 加的值,可为负数表示减
 * @return time_t <0时表示超出范围
 */
inline time_t time_add(time_t t,TimeField f,int i)
{
	struct tm curr;
	switch(f) {
	case Second:
		return t+i;
	case Minute:
		return t+i*60;
	case Hour:
		return t+i*60*60;
	case Day:
		return t+i*60*60*24;
	case Month:
		curr = *localtime(&t);
		curr.tm_year += (curr.tm_mon-1+i)/12;
		curr.tm_mon = (curr.tm_mon-1+i) %12+1;
		return mktime(&curr);
	case Year:
		curr = *localtime(&t);
		curr.tm_year+=i;
		return mktime(&curr);
	default:
		return 0;
	}

	return 0;
}

/**
 * unsigned转string
 * @see #s2u
 */
inline string u2s(const unsigned u)
{
	char sTmp[16] ={0};
	snprintf(sTmp, 15, "%u", u);
	return string(sTmp);
}

/**
 * unsigned转string(16进制形式)
 * @see #sx2u
 */
inline string u2sx(const unsigned u)
{
	char sTmp[16] ={0};
	snprintf(sTmp, 15, "%x", u);
	return string(sTmp);
}

/**
 * int转string
 * @see #s2i
 */
inline string i2s(const int i)
{
	char sTmp[16] ={0};
	snprintf(sTmp, 15, "%d", i);
	return string(sTmp);	
}

/**
 * char转string
 * @see #s2c
 */
inline string c2s(const char c)
{
	if(c==0) return string("");
	return string()+c;
}

/**
 * binary->string
 * @param b binary指针
 * @param len binary长度
 * @param block 显示格式<br>
 *                      =0格式为"00 01 02 ..." <br>
 *                      >0格式为(以8为例)"0000h: 00 01 02 03 04 05 06 07 08  09 0a 0b 0c 0d 0e 0f" <br>
 *                                                               "0010h: 10 11 12 13 14 15 16 17 18  19 1a 1b 1c 1d 1e 1f" <br>
 * @return string 得到的格式串
 * @see #s2b
 */
string b2s(const char *b,size_t len,const unsigned block = 8);

/**
 * time_t->string
 * @param t 时间值
 * @param format 显示格式,参见date指令格式
 * @return string 得到的格式串
 * @s2t
 */
inline string t2s(const time_t t,const string& format="%Y-%m-%d %H:%M:%S")
{
	struct tm curr;
	curr = *localtime(&t);
	char sTmp[1024];
	strftime(sTmp,sizeof(sTmp),format.c_str(),&curr);
	return string(sTmp);
}

inline char* t2char(const time_t iTime, char* szDateTime)
{
    struct tm stm ;

    ///////////////////////////////////////////////////////
    int year  = 0;
    int month = 0;

    time_t tm = iTime +3600*8;

    stm.tm_sec = tm%60;
    tm /= 60;

    stm.tm_min = tm%60;
    tm /= 60;

    stm.tm_hour = tm%24;
    tm /= 24;

    stm.tm_wday = (tm+4)%7;

    tm += 365 * 2 - 59;

    year = (tm * 4 + 3) / (365 * 4 + 1);
    tm = tm - year * 365 - year / 4 + 31;

    month = tm * 17 / 520;
    tm = tm - month * 520 / 17;

    stm.tm_mday = tm;

    if(month < 11) 
    {
        month++;
        year -= 2;
    } 
    else 
    {
        month -= 11;
        year--;
    }

    stm.tm_mon = month;
    stm.tm_year = year + 70;
    ///////////////////////////////////////////////////////

    snprintf(szDateTime, 49, "%04d-%02d-%02d %02d:%02d:%02d",
        stm.tm_year+1900, stm.tm_mon+1, stm.tm_mday,
        stm.tm_hour, stm.tm_min, stm.tm_sec );

    return szDateTime;
};

/**
 * now->string
 * @param format 显示格式,参见date指令格式
 * @return string 得到的格式串
 * @see #t2s
 */
inline string now(const string& format="%Y-%m-%d %H:%M:%S")
{
	return t2s(time(0),format);
}


/**
 * string转unsigned
 * @throw comm_error string格式不是unsigned
 * @see #u2s
 */
inline unsigned s2u(const string &s) throw (comm_error)
{
	if(s.length()==0) {
		throw comm_error("s2u: len is 0");
	}
	for (size_t i = 0; i < s.length(); i++){
		if (!isdigit(s.c_str()[i])) {
			throw comm_error(string("s2u: not digits:")+s);
		}
	}

	return strtoul(s.c_str(),NULL,10);
}


/**
 * string转unsigned (格式不对时设为默认值)
 * @see #u2s
 */
inline unsigned s2u(const string &s,unsigned defaultvalue)
{
	if(s.length()==0) {
		return defaultvalue;
	}
	for (size_t i = 0; i < s.length(); i++){
		if (!isdigit(s.c_str()[i])) {
			return defaultvalue;	
		}
	}

	return strtoul(s.c_str(),NULL,10);
}

/**
 * 16进制格式string转unsigned,支持的格式为0x+字符串或者直接为字符串
 * @throw comm_error string格式不是unsigned
 * @see #u2sx
 */
inline unsigned sx2u(const string &s) throw (comm_error)
{
	size_t len = s.length();
	size_t idx=0;
	if(s.c_str()[0] == '0' && s.c_str()[1] == 'x') {
		idx = 2;
		len -= 2;
	}

	if(len==0 || len>8) {
		throw comm_error(string("sx2u: length is invalid:")+u2s(len));
	}
	for (size_t i = idx; i < s.length(); i++){
		if (!isxdigit(s.c_str()[i])) {
			throw comm_error(string("sx2u: not xdigit:")+s);
		}
	}

	return strtoul(s.c_str()+idx,NULL,16);
}

/**
 * 16进制格式string转unsigned (格式不对时设为默认值)
 * @see #u2sx
 */
inline unsigned sx2u(const string &s,unsigned defaultvalue)
{
	size_t len = s.length();
	size_t idx=0;
	if(s.c_str()[0] == '0' && s.c_str()[1] == 'x') {
		idx = 2;
		len -= 2;
	}

	if(len==0 || len>8) {
		return defaultvalue;
	}
	for (size_t i = idx; i < s.length(); i++){
		if (!isxdigit(s.c_str()[i])) {
			return defaultvalue;
		}
	}

	return strtoul(s.c_str()+idx,NULL,16);
}

/**
 * string转int
 * @throw comm_error string格式不是unsigned
 * @see #i2s
 */
inline int s2i(const string &s) throw (comm_error)
{
	if(s.c_str()[0] == '-') {
		return -1 * s2u(s.c_str()+1);
	}
	else {
		return s2u(s);
	}

	return 0;
}

/**
 * string转int (格式不对时设为默认值)
 * @see #i2s
 */
inline int s2i(const string &s,int defaultvalue) 
{
	unsigned d = (defaultvalue < 0?-1*defaultvalue:defaultvalue);
	if(s.c_str()[0] == '-') {
		return -1 * s2u(s.c_str()+1,d);
	}
	else {
		return s2u(s,d);
	}

	return 0;
}

/**
 * string转char (取string第一位)
 * @see #c2s
 */
inline char s2c(const string &s)
{
	if(s.length()==0) return 0;
	return s.c_str()[0];
}

/**
 * string转char (string为""时返回默认值)
 * @see #c2s
 */
inline char s2c(const string &s,char defaultvalue)
{
	if(s.length()==0) return defaultvalue;
	return s.c_str()[0];
}

/**
 * string转binary,支持的格式"00 01 ..."或"0001..."
 * @throw comm_error string格式不对
 * @return size_t binary大小
 * @see #b2s
 */
inline size_t s2b(const string &s,char *b,size_t maxlen) throw (comm_error)
{
	if(maxlen==0) return 0;
	if(s.length()==0) return 0;
	char sTmp[4]={0};
	size_t i=0;
	char *p=const_cast<char *>(s.c_str());

	while(*p != '\0') {
		if(!isxdigit(*p)) throw comm_error(string("s2b: format error:")+s);
		sTmp[0] = *p; p++;
		if(!isxdigit(*p)) throw comm_error(string("s2b: format error:")+s);
		sTmp[1] = *p; p++;
		b[i++] = strtoul(sTmp,NULL,16);
		if(i==maxlen) break;
		if(*p == ' ') p++;
	}
	return i;
}

/**
 * string转time_t
 * @throw comm_error string格式不对
 * @return time_t 返回时间
 * @see #t2s
 */
time_t s2t(const string &s,const string& format="%Y-%m-%d %H:%M:%S") throw (comm_error);

// in: s - src string
// out: s - string after trim
// return : head
/**
 * 根据split去掉string的头,类似GetToken<br>
 * 如string s = "ab cd ef", trim(s)返回ab，s则变为"cd ed"
 * @param s 源字符串
 * @param split 分隔符
 * @return 头
 */
string trim_head(string& s,const string& split=" \t");

}}

#endif //


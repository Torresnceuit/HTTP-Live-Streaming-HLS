#ifndef _INCLUDED_OICQCRYPT_H_
#define _INCLUDED_OICQCRYPT_H_

/*
qqcrypt.h
QQ加密
hyj@tencent.com
1999/12/24
2002/03/27

  实现下列算法:
  Hash算法: MD5,已实现
  对称算法: DES,未实现
  非对称算法: RSA,未实现


2002/06/14
	修改v3解密函数，去掉v3加密函数

*/

#define MD5_DIGEST_LENGTH	16
#define ENCRYPT_PADLEN		18
#define	CRYPT_KEY_SIZE		16
#define MD5_LBLOCK	        16

#ifdef _DEBUG
bool Md5Test();                 /*测试MD5函数是否按照rfc1321工作 */
#endif


/************************************************************************************************
	MD5数据结构
************************************************************************************************/
#if 0
typedef struct MD5state_st
{
    unsigned long A, B, C, D;
    unsigned long Nl, Nh;
    unsigned long data[MD5_LBLOCK];
    int num;
} OI_MD5_CTX;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define TeaEncryptECB(pInBuf,pKey,pOutBuf)			OI_TeaEncryptECB(pInBuf,pKey,pOutBuf)
#define TeaDecryptECB(pInBuf,pKey,pOutBuf)			OI_TeaDecryptECB(pInBuf,pKey,pOutBuf)
#define TeaEncryptECB3(pInBuf,pKey,pOutBuf)			OI_TeaEncryptECB3(pInBuf,pKey,pOutBuf)
#define TeaDecryptECB3(pInBuf,pKey,pOutBuf)			OI_TeaDecryptECB3(pInBuf,pKey,pOutBuf)
#define oi_symmetry_encrypt(pInBuf,nInBufLen,pKey,pOutBuf,pOutBufLen)	OI_oi_symmetry_encrypt(pInBuf,nInBufLen,pKey,pOutBuf,pOutBufLen)
#define oi_symmetry_decrypt(pInBuf,nInBufLen,pKey,pOutBuf,pOutBufLen)	OI_oi_symmetry_decrypt(pInBuf,nInBufLen,pKey,pOutBuf,pOutBufLen)
#define oi_symmetry_encrypt2_len(nInBufLen)			OI_oi_symmetry_encrypt2_len(nInBufLen)
#define oi_symmetry_encrypt2(pInBuf,nInBufLen, pKey,pOutBuf,pOutBufLen)	OI_oi_symmetry_encrypt2(pInBuf,nInBufLen,pKey,pOutBuf,pOutBufLen)
#define oi_symmetry_decrypt2(pInBuf,nInBufLen, pKey,pOutBuf,pOutBufLen)	OI_oi_symmetry_decrypt2(pInBuf,nInBufLen,pKey,pOutBuf,pOutBufLen)
#define oi_symmetry_encrypt3_len(nInBufLen)	OI_oi_symmetry_encrypt3_len(nInBufLen)
//#define qq_symmetry_decrypt3(pInBuf,nInBufLen,chMainVer,chSubVer,dwUin,pKey,pOutBuf,pOutBufLen)	OI_qq_symmetry_decrypt3(pInBuf,nInBufLen,chMainVer,chSubVer,dwUin,pKey,pOutBuf,pOutBufLen)


/************************************************************************************************
	对称加密底层函数
************************************************************************************************/
//pOutBuffer、pInBuffer均为8byte, pKey为16byte
    void OI_TeaEncryptECB(const unsigned char * pInBuf, const unsigned char * pKey, unsigned char * pOutBuf);
    void OI_TeaDecryptECB(const unsigned char * pInBuf, const unsigned char * pKey, unsigned char * pOutBuf);
    void OI_TeaEncryptECB3(const unsigned char * pInBuf, const unsigned char * pKey, unsigned char * pOutBuf);
    void OI_TeaDecryptECB3(const unsigned char * pInBuf, const unsigned char * pKey, unsigned char * pOutBuf);

/************************************************************************************************
	QQ对称加密第一代函数
************************************************************************************************/

/*pKey为16byte*/
/*
	输入:pInBuf为需加密的明文部分(Body),nInBufLen为pInBuf长度;
	输出:pOutBuf为密文格式,pOutBufLen为pOutBuf的长度是8byte的倍数,至少应预留nInBufLen+17;
*/
/*TEA加密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
    void OI_oi_symmetry_encrypt(const unsigned char * pInBuf, int nInBufLen, const unsigned char * pKey, unsigned char * pOutBuf, int *pOutBufLen);

/*pKey为16byte*/
/*
	输入:pInBuf为密文格式,nInBufLen为pInBuf的长度是8byte的倍数; *pOutBufLen为接收缓冲区的长度
		特别注意*pOutBufLen应预置接收缓冲区的长度!
	输出:pOutBuf为明文(Body),pOutBufLen为pOutBuf的长度,至少应预留nInBufLen-10;
	返回值:如果格式正确返回TRUE;
*/
/*TEA解密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
    bool OI_oi_symmetry_decrypt(const unsigned char * pInBuf, int nInBufLen, const unsigned char * pKey, unsigned char * pOutBuf, int *pOutBufLen);







/************************************************************************************************
	QQ对称加密第二代函数
************************************************************************************************/

/*pKey为16byte*/
/*
	输入:nInBufLen为需加密的明文部分(Body)长度;
	输出:返回为加密后的长度(是8byte的倍数);
*/
/*TEA加密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
    int OI_oi_symmetry_encrypt2_len(int nInBufLen);


/*pKey为16byte*/
/*
	输入:pInBuf为需加密的明文部分(Body),nInBufLen为pInBuf长度;
	输出:pOutBuf为密文格式,pOutBufLen为pOutBuf的长度是8byte的倍数,至少应预留nInBufLen+17;
*/
/*TEA加密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
    void OI_oi_symmetry_encrypt2(const unsigned char * pInBuf, int nInBufLen, const unsigned char * pKey, unsigned char * pOutBuf, int *pOutBufLen);



/*pKey为16byte*/
/*
	输入:pInBuf为密文格式,nInBufLen为pInBuf的长度是8byte的倍数; *pOutBufLen为接收缓冲区的长度
		特别注意*pOutBufLen应预置接收缓冲区的长度!
	输出:pOutBuf为明文(Body),pOutBufLen为pOutBuf的长度,至少应预留nInBufLen-10;
	返回值:如果格式正确返回TRUE;
*/
/*TEA解密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
    bool OI_oi_symmetry_decrypt2(const unsigned char * pInBuf, int nInBufLen, const unsigned char * pKey, unsigned char * pOutBuf, int *pOutBufLen);




/************************************************************************************************
	QQ对称解密第三代函数
************************************************************************************************/

/*pKey为16byte*/
/*
	输入:nInBufLen为需加密的明文部分(Body)长度;
	输出:返回为加密后的长度(是8byte的倍数);
*/
/*TEA加密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
    int OI_oi_symmetry_encrypt3_len(int nInBufLen);


/*pKey为16byte*/
/*
	输入:pInBuf为密文格式,nInBufLen为pInBuf的长度是8byte的倍数; *pOutBufLen为接收缓冲区的长度
		特别注意*pOutBufLen应预置接收缓冲区的长度!
	输出:pOutBuf为明文(Body),pOutBufLen为pOutBuf的长度,至少应预留nInBufLen-10;
	返回值:如果格式正确返回TRUE;
*/
/*TEA解密算法,CBC模式*/
/*密文格式:PadLen(1byte)+Padding(var,0-7byte)+Salt(2byte)+Body(var byte)+Zero(7byte)*/
//     bool OI_qq_symmetry_decrypt3(const unsigned char * pInBuf, int nInBufLen, unsigned char chMainVer, unsigned char chSubVer, unsigned int dwUin,
//                                  const unsigned char * pKey, unsigned char * pOutBuf, int *pOutBufLen);

#ifdef __cplusplus
}
#endif

#endif                          // #ifndef _INCLUDED_OICQCRYPT_H_

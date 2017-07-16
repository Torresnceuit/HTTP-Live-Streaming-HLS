#ifndef _VAS_KEY_H
#define _VAS_KEY_H

#ifdef  __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/////////////////加密接口////////////////////
#define ENCRYPT_SUCC 		( 1)//没有任何异常的加密
#define ENCRYPT_FAIL        	( 0)//不用这个返回值,因为一旦加密失败返回值就是以下三个,为了对称才定义
#define ENCRYPT_FAILD_NO_KEY 	(-1)//没有得到vaskey
#define ENCRYPT_FAILD_VAS_ERR 	(-2)//vasid错误
#define ENCRYPT_FAILD_PARA_ERR 	(-3)//参数错误
/*不再对外提供
int IM_OicqEncrypt_DeployCtrl(//老函数接口
		const unsigned short shVersion, 
		const void * pInBuf, const int nInBufLen,
		const char * sDefaultTeaKey,
		const int iVasId,const unsigned long lUin,time_t lCurTime,
		const char *sCldIP, //客户端的IP
		const unsigned short  shCldVer,//客户端的版本
		void* pOutBuf, int *pOutBufLen);
*/
int KeyAuthSys_TeaEncrypt(//请使用这个接口函数
		const void * pInBuf, const int nInBufLen,
		const char * sDefaultTeaKey,
		const int iVasId,const unsigned long lUin,time_t lCurTime,
		const char *sCldIP,//客户端的IP
		const unsigned short  shCldVer,//客户端的版本
		void* pOutBuf, int *pOutBufLen);
/////////////////加密接口////////////////////


/////////////////解密接口////////////////////
#define DECRYPT_SIG_TIMEOUT	( 2)//虽然解密成功，但是签名超期
#define DECRYPT_SUCC 		( 1)//没有任何异常的解密
#define DECRYPT_FAIL        	( 0)//签名解不开
#define DECRYPT_FAILD_NO_KEY 	(-1)//没有得到vaskey
#define DECRYPT_FAILD_VAS_ERR 	(-2)//vasid错误
#define DECRYPT_FAILD_PARA_ERR 	(-3)//参数错误
/*不再对外提供
int IM_OicqDecrypt_DeployCtrl(//老函数接口
		const unsigned short shVersion,
		const void * pInBuf, const int nInBufLen,
		const char * sDefaultTeaKey,
		const int iVasId,const unsigned long lUin,time_t lCurTime,
		const char *sCldIP,//客户端的IP
		const unsigned short  shCldVer,//客户端的版本
		void* pOutBuf, int *pOutBufLen);
*/
int KeyAuthSys_TeaDecrypt(//请使用这个接口函数
		const void * pInBuf, const int nInBufLen,
		const char * sDefaultTeaKey,
		const int iVasId,const unsigned long lUin,time_t lCurTime,
		const char *sCldIP,//客户端的IP
		const unsigned short  shCldVer,//客户端的版本
		const int iSigTimeStampOffset,const int iSigTimeStampIsHostOrder,
		void* pOutBuf, int *pOutBufLen);
/////////////////解密接口////////////////////


/*
	API提供释放API所attach的共享内存的函数, 请谨慎使用，反复shmat 和 shmdt 系统调用会影响性能
*/
int FreeVasKeyAPIAllSource(void);



/*
RETURN:
1-----succ
0-----fail
*/
int KeyAuthSys_VasKeyShmIsValid(int iVasId,int iIsDecrypt);


/*
	解密的程序只需要检查解密的就好
*/
#define KeyAuthSys_DecryptVasKeyShmIsValid(iVasId) KeyAuthSys_VasKeyShmIsValid(iVasId,1)

/*
	加密的程序只需要检查加密的就好
*/
#define KeyAuthSys_EncryptVasKeyShmIsValid(iVasId) KeyAuthSys_VasKeyShmIsValid(iVasId,0)





#ifdef  __cplusplus
}
#endif


#endif


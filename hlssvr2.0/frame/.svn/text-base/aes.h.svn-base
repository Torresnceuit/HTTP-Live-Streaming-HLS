//Tencent aes c++ Algorithm  realize by marmotxu

//用于各种加密算法的密钥生成
// static const unsigned char ucStaticKey[64] = {
//     0x5a, 0xc4, 0x0e, 0x19, 0x88, 0x46, 0x6b, 0x82, 0x8e, 0x33, 0x31, 0x26,
//     0x02, 0xd0, 0xfe, 0x33, 0xb0, 0xee, 0x72, 0xe1, 0xc3, 0xa5, 0x46, 0x5a,
//     0x55, 0x39, 0xc2, 0xc1, 0x30, 0x32, 0x9f, 0x8a, 0x41, 0xe1, 0x3a, 0x4a,
//     0x25, 0x0c, 0x52, 0xc6, 0x5c, 0x49, 0xfb, 0xd4, 0x64, 0xfb, 0x94, 0xd7,
//     0xd7, 0x7f, 0xa8, 0xfa, 0xf0, 0x5d, 0xa1, 0x7d, 0x51, 0x5e, 0x99, 0x89,
//     0xfc, 0x69, 0xd4, 0x90
// };

#define AES_BLOCK_SIZE 16

class AES
{
public:
    typedef enum ENUM_KeySize_
    {
        BIT128 = 0,
        BIT192,
        BIT256
    }ENUM_KEYSIZE;

public:
    AES();
    ~AES(void);

    void initKey(ENUM_KEYSIZE keysize, const unsigned char *key);
    void encrypt(const unsigned char *in, unsigned char *out, const unsigned long length);
    void decrypt(const unsigned char *in, unsigned char *out, const unsigned long length);


    static int hex2str(const char *hex, int hexlen, char *str);
    static int str2hex(const char * src_str, char* hex, unsigned max_len = 0); 
private:
    void Cipher( unsigned char *input, unsigned char *output );
    void InvCipher( unsigned char *input, unsigned char *output );
    unsigned char *RotWord( unsigned char *word );
    unsigned char *SubWord( unsigned char *word );
    void AddRoundKey(int round);
    void SubBytes();
    void InvSubBytes();
    void ShiftRows();
    void InvShiftRows();
    void MixColumns();
    void InvMixColumns();
    static unsigned char gfmultby01(unsigned char b)
    {
        return b;
    }

    static unsigned char gfmultby02(unsigned char b)
    {
        if (b < 0x80)
            return (unsigned char)(int)(b <<1);
        else
            return (unsigned char)( (int)(b << 1) ^ (int)(0x1b) );
    }

    static unsigned char gfmultby03(unsigned char b)
    {
        return (unsigned char) ( (int)gfmultby02(b) ^ (int)b );
    }

    static unsigned char gfmultby09(unsigned char b)
    {
        return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
            (int)b );
    }

    static unsigned char gfmultby0b(unsigned char b)
    {
        return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
            (int)gfmultby02(b) ^
            (int)b );
    }

    static unsigned char gfmultby0d(unsigned char b)
    {
        return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
            (int)gfmultby02(gfmultby02(b)) ^
            (int)(b) );
    }

    static unsigned char gfmultby0e(unsigned char b)
    {
        return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
            (int)gfmultby02(gfmultby02(b)) ^
            (int)gfmultby02(b) );
    }
    int Nb;
    int Nk;
    int Nr;
    unsigned char *key;// the seed key. size will be 4 * keySize from ctor.
    typedef struct char4_
    {
        unsigned char w[4];
    }char4;
    char4 *w;
    unsigned char* State[4];
};

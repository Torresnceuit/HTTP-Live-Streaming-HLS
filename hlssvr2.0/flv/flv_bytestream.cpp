
#include "flv_bytestream.h"

flv_bytestream::flv_bytestream(void)
{
}

flv_bytestream::~flv_bytestream(void)
{
}

unsigned int  flv_bytestream::endian_fix16( unsigned int  x )
{
	return (x<<8)|(x>>8);
}

unsigned int flv_bytestream::endian_fix32( unsigned int x )
{
	return (x<<24) + ((x<<8)&0xff0000) + ((x>>8)&0xff00) + (x>>24);
}

uint64_t flv_bytestream::endian_fix64(uint64_t x )
{
	return endian_fix32(x>>32) + ((uint64_t)endian_fix32(x)<<32);
}

uint64_t flv_bytestream::dbl2int( double value )
{
		//return  (union {double f; uint64_t i;}){value}a.i;
		return *(uint64_t*)(&value);
}

/* Put functions  */

int flv_bytestream::x264_put_byte( flv_buffer *c, unsigned char b )
{
    return flv_append_data( c, &b, 1 );
}

int flv_bytestream::x264_put_be32( flv_buffer *c, unsigned int val )
{
	int ret = 0;

    x264_put_byte( c, val >> 24 );
    x264_put_byte( c, val >> 16 );
    x264_put_byte( c, val >> 8 );
    x264_put_byte( c, val );

	return ret;
}

int flv_bytestream::x264_put_be64( flv_buffer *c, uint64_t val )
{
	int ret = 0;

    x264_put_be32( c, val >> 32 );
    x264_put_be32( c, val );

	return ret;
}

int flv_bytestream::x264_put_be16( flv_buffer *c, unsigned short val )
{
	int ret = 0;

    x264_put_byte( c, val >> 8 );
    x264_put_byte( c, val );

	return ret;
}

int flv_bytestream::x264_put_be24( flv_buffer *c,  unsigned int val )
{
	int ret = 0;

    x264_put_be16( c, val >> 8 );
    x264_put_byte( c, val );

	return ret;
}

int flv_bytestream::x264_put_tag( flv_buffer *c, const char *tag )
{
	int ret = 0;

    while( *tag )
        x264_put_byte( c, *tag++ );

	return ret;
}

int flv_bytestream::x264_put_amf_string( flv_buffer *c, const char *str )
{
	int ret = 0;

    unsigned short len = strlen( str );
    x264_put_be16( c, len );
    ret = flv_append_data( c, ( unsigned char*)str, len );
	
	return ret;
}

int flv_bytestream::x264_put_amf_double( flv_buffer *c, double d )
{
    x264_put_byte( c, AMF_DATA_TYPE_NUMBER );
    x264_put_be64( c, dbl2int( d ) );

	return 0;
}

int flv_bytestream::flv_append_data( flv_buffer *c, unsigned char *ucChunkBuf, unsigned int uiChunkSize )
{

    if(c->d_cur + uiChunkSize > c->d_max)
    {
	return -1;
    }

    memcpy(c->data + c->d_cur, ucChunkBuf, uiChunkSize );

    c->d_cur += uiChunkSize;

    return 0;
}

void flv_bytestream::rewrite_amf_be24( flv_buffer *c, unsigned length, unsigned start )
{
     *(c->data + start + 0) = length >> 16;
     *(c->data + start + 1) = length >> 8;
     *(c->data + start + 2) = length >> 0;
}

int flv_bytestream::flv_flush_data( flv_buffer *c )
{
    if( !c->d_cur )
        return 0;


    c->d_total += c->d_cur;

    c->d_cur = 0;

    return 0;
}

#ifndef _FORMAT_FLV_BYTESTREAM_H
#define _FORMAT_FLV_BYTESTREAM_H

#include "flv.h"

class flv_bytestream
{
public:
	flv_bytestream(void);
	~flv_bytestream(void);

	unsigned int  endian_fix16( unsigned int  x );
	unsigned int endian_fix32( unsigned int x );
	uint64_t endian_fix64(uint64_t x );

	int flv_append_data( flv_buffer *c, unsigned char *data, unsigned int size );
	int flv_write_byte( flv_buffer *c, unsigned char *byte );
	int flv_flush_data( flv_buffer *c );
	void rewrite_amf_be24( flv_buffer *c, unsigned int length, unsigned int start );

	uint64_t dbl2int( double value );
	uint64_t get_amf_double( double value );
	int x264_put_byte( flv_buffer *c, unsigned char b );
	int x264_put_be32( flv_buffer *c, unsigned int val );
	int x264_put_be64( flv_buffer *c, uint64_t val );
	int x264_put_be16( flv_buffer *c, unsigned short val );
	int x264_put_be24( flv_buffer *c, unsigned int val );
	int x264_put_tag( flv_buffer *c, const char *tag );
	int x264_put_amf_string( flv_buffer *c, const char *str );
	int x264_put_amf_double( flv_buffer *c, double d );
};

#endif




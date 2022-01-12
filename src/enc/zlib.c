#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zlib.h>

#define  CAPACITY  (1024 * 1024 * 4)  /* output buffer max (4 mb) */

int
zlibenc(
	void *_src
	, unsigned src_sz
	, void *_dst
	, unsigned *dst_sz
	, void *_ctx
)
{
	unsigned char *src = _src;
	unsigned char *dst = _dst;
	z_stream stream = {0};
	int r;
	unsigned result_sz;
	
	extern int g_hlen; /* header length */
	memset(dst, 0, g_hlen);
	memcpy(dst, "ZLIB", 4);
	dst[4] = (src_sz >> 24);
	dst[5] = (src_sz >> 16);
	dst[6] = (src_sz >>  8);
	dst[7] = (src_sz >>  0);
	
	stream.avail_in = src_sz;
	stream.next_in = src;
	stream.avail_out = CAPACITY;
	stream.next_out = dst + g_hlen;
	
	if ((r = deflateInit(&stream, Z_BEST_COMPRESSION)) != Z_OK)
	{
		fprintf(stderr, "[!] fatal compression error %d\n", r);
		exit(EXIT_FAILURE);
	}
	if ((r = deflate(&stream, Z_FINISH)) == Z_STREAM_ERROR)
	{
		fprintf(stderr, "[!] Z_STREAM_ERROR\n");
		exit(EXIT_FAILURE);
	}
	deflateEnd(&stream);
	
	result_sz = CAPACITY - stream.avail_out;
	*dst_sz = result_sz + g_hlen;
	
	return 0;
	(void)_ctx; /* -Wunused-parameter */
}




#include "netif/ppp/ppp_opts.h"
#if LWIP_INCLUDED_POLARSSL_MD4

#ifndef LWIP_INCLUDED_POLARSSL_MD4_H
#define LWIP_INCLUDED_POLARSSL_MD4_H

/**
 * \brief          MD4 context structure
 */
typedef struct
{
    unsigned long total[2];     /*!< number of bytes processed  */
    unsigned long state[4];     /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
}
md4_context;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          MD4 context setup
 *
 * \param ctx      context to be initialized
 */
void md4_starts( md4_context *ctx );

/**
 * \brief          MD4 process buffer
 *
 * \param ctx      MD4 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void md4_update( md4_context *ctx, const unsigned char *input, int ilen );

/**
 * \brief          MD4 final digest
 *
 * \param ctx      MD4 context
 * \param output   MD4 checksum result
 */
void md4_finish( md4_context *ctx, unsigned char output[16] );

/**
 * \brief          Output = MD4( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   MD4 checksum result
 */
void md4( unsigned char *input, int ilen, unsigned char output[16] );


#ifdef __cplusplus
}
#endif

#endif /* LWIP_INCLUDED_POLARSSL_MD4_H */

#endif /* LWIP_INCLUDED_POLARSSL_MD4 */

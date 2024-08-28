
#include "netif/ppp/ppp_opts.h"
#if PPP_SUPPORT && LWIP_INCLUDED_POLARSSL_ARC4

#include "netif/ppp/polarssl/arc4.h"
/*
 * ARC4 key schedule
 */
void arc4_setup( arc4_context *ctx, unsigned char *key, int keylen )
{
    int i, j, k, a;
    unsigned char *m;

    ctx->x = 0;
    ctx->y = 0;
    m = ctx->m;

    for( i = 0; i < 256; i++ )
        m[i] = (unsigned char) i;

    j = k = 0;

    for( i = 0; i < 256; i++, k++ )
    {
        if( k >= keylen ) k = 0;

        a = m[i];
        j = ( j + a + key[k] ) & 0xFF;
        m[i] = m[j];
        m[j] = (unsigned char) a;
    }
}

/*
 * ARC4 cipher function
 */
void arc4_crypt( arc4_context *ctx, unsigned char *buf, int buflen )
{
    int i, x, y, a, b;
    unsigned char *m;

    x = ctx->x;
    y = ctx->y;
    m = ctx->m;

    for( i = 0; i < buflen; i++ )
    {
        x = ( x + 1 ) & 0xFF; a = m[x];
        y = ( y + a ) & 0xFF; b = m[y];

        m[x] = (unsigned char) b;
        m[y] = (unsigned char) a;

        buf[i] = (unsigned char)
            ( buf[i] ^ m[(unsigned char)( a + b )] );
    }

    ctx->x = x;
    ctx->y = y;
}

#endif /* PPP_SUPPORT && LWIP_INCLUDED_POLARSSL_DES */

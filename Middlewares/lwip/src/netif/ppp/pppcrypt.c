

#include "netif/ppp/ppp_opts.h"
#if PPP_SUPPORT && MSCHAP_SUPPORT /* don't build if not necessary */

#include "netif/ppp/ppp_impl.h"

#include "netif/ppp/pppcrypt.h"


static u_char pppcrypt_get_7bits(u_char *input, int startBit) {
	unsigned int word;

	word  = (unsigned)input[startBit / 8] << 8;
	word |= (unsigned)input[startBit / 8 + 1];

	word >>= 15 - (startBit % 8 + 7);

	return word & 0xFE;
}

/* IN  56 bit DES key missing parity bits
 * OUT 64 bit DES key with parity bits added
 */
void pppcrypt_56_to_64_bit_key(u_char *key, u_char * des_key) {
	des_key[0] = pppcrypt_get_7bits(key,  0);
	des_key[1] = pppcrypt_get_7bits(key,  7);
	des_key[2] = pppcrypt_get_7bits(key, 14);
	des_key[3] = pppcrypt_get_7bits(key, 21);
	des_key[4] = pppcrypt_get_7bits(key, 28);
	des_key[5] = pppcrypt_get_7bits(key, 35);
	des_key[6] = pppcrypt_get_7bits(key, 42);
	des_key[7] = pppcrypt_get_7bits(key, 49);
}

#endif /* PPP_SUPPORT && MSCHAP_SUPPORT */

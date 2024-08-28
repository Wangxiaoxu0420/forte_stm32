

#ifndef EUI64_H
#define EUI64_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @todo:
 *
 * Maybe this should be done by processing struct in6_addr directly...
 */
typedef union
{
    u8_t e8[8];
    u16_t e16[4];
    u32_t e32[2];
} eui64_t;

#define eui64_iszero(e)		(((e).e32[0] | (e).e32[1]) == 0)
#define eui64_equals(e, o)	(((e).e32[0] == (o).e32[0]) && \
				((e).e32[1] == (o).e32[1]))
#define eui64_zero(e)		(e).e32[0] = (e).e32[1] = 0;

#define eui64_copy(s, d)	memcpy(&(d), &(s), sizeof(eui64_t))

#define eui64_magic(e)		do {			\
				(e).e32[0] = magic();	\
				(e).e32[1] = magic();	\
				(e).e8[0] &= ~2;	\
				} while (0)
#define eui64_magic_nz(x)	do {				\
				eui64_magic(x);			\
				} while (eui64_iszero(x))
#define eui64_magic_ne(x, y)	do {				\
				eui64_magic(x);			\
				} while (eui64_equals(x, y))

#define eui64_get(ll, cp)	do {				\
				eui64_copy((*cp), (ll));	\
				(cp) += sizeof(eui64_t);	\
				} while (0)

#define eui64_put(ll, cp)	do {				\
				eui64_copy((ll), (*cp));	\
				(cp) += sizeof(eui64_t);	\
				} while (0)

#define eui64_set32(e, l)	do {			\
				(e).e32[0] = 0;		\
				(e).e32[1] = lwip_htonl(l);	\
				} while (0)
#define eui64_setlo32(e, l)	eui64_set32(e, l)

char *eui64_ntoa(eui64_t);	/* Returns ascii representation of id */

#ifdef __cplusplus
}
#endif

#endif /* EUI64_H */
#endif /* PPP_SUPPORT && PPP_IPV6_SUPPORT */

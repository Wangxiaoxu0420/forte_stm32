

#ifndef MAGIC_H
#define MAGIC_H

#ifdef __cplusplus
extern "C" {
#endif

/***********************
*** PUBLIC FUNCTIONS ***
***********************/

/*
 * Initialize the random number generator.
 */
void magic_init(void);

/*
 * Randomize our random seed value.  To be called for truely random events
 * such as user operations and network traffic.
 */
void magic_randomize(void);

/*
 * Return a new random number.
 */
u32_t magic(void);	/* Returns the next magic number */

/*
 * Fill buffer with random bytes
 *
 * Use the random pool to generate random data.  This degrades to pseudo
 * random when used faster than randomness is supplied using magic_churnrand().
 * Thus it's important to make sure that the results of this are not
 * published directly because one could predict the next result to at
 * least some degree.  Also, it's important to get a good seed before
 * the first use.
 */
void magic_random_bytes(unsigned char *buf, u32_t buf_len);

/*
 * Return a new random number between 0 and (2^pow)-1 included.
 */
u32_t magic_pow(u8_t pow);

#ifdef __cplusplus
}
#endif

#endif /* MAGIC_H */

#endif /* PPP_SUPPORT */



#include "netif/ppp/ppp_impl.h"
#include "netif/ppp/magic.h"

#if PPP_MD5_RANDM /* Using MD5 for better randomness if enabled */

#include "netif/ppp/pppcrypt.h"

#define MD5_HASH_SIZE 16
static char magic_randpool[MD5_HASH_SIZE];   /* Pool of randomness. */
static long magic_randcount;      /* Pseudo-random incrementer */
static u32_t magic_randomseed;    /* Seed used for random number generation. */

/*
 * Churn the randomness pool on a random event.  Call this early and often
 *  on random and semi-random system events to build randomness in time for
 *  usage.  For randomly timed events, pass a null pointer and a zero length
 *  and this will use the system timer and other sources to add randomness.
 *  If new random data is available, pass a pointer to that and it will be
 *  included.
 *
 * Ref: Applied Cryptography 2nd Ed. by Bruce Schneier p. 427
 */
static void magic_churnrand(char *rand_data, u32_t rand_len) {
  lwip_md5_context md5_ctx;

  /* LWIP_DEBUGF(LOG_INFO, ("magic_churnrand: %u@%P\n", rand_len, rand_data)); */
  lwip_md5_init(&md5_ctx);
  lwip_md5_starts(&md5_ctx);
  lwip_md5_update(&md5_ctx, (u_char *)magic_randpool, sizeof(magic_randpool));
  if (rand_data) {
    lwip_md5_update(&md5_ctx, (u_char *)rand_data, rand_len);
  } else {
    struct {
      /* INCLUDE fields for any system sources of randomness */
      u32_t jiffies;
#ifdef LWIP_RAND
      u32_t rand;
#endif /* LWIP_RAND */
    } sys_data;
    magic_randomseed += sys_jiffies();
    sys_data.jiffies = magic_randomseed;
#ifdef LWIP_RAND
    sys_data.rand = LWIP_RAND();
#endif /* LWIP_RAND */
    /* Load sys_data fields here. */
    lwip_md5_update(&md5_ctx, (u_char *)&sys_data, sizeof(sys_data));
  }
  lwip_md5_finish(&md5_ctx, (u_char *)magic_randpool);
  lwip_md5_free(&md5_ctx);
/*  LWIP_DEBUGF(LOG_INFO, ("magic_churnrand: -> 0\n")); */
}

/*
 * Initialize the random number generator.
 */
void magic_init(void) {
  magic_churnrand(NULL, 0);
}

/*
 * Randomize our random seed value.
 */
void magic_randomize(void) {
  magic_churnrand(NULL, 0);
}

/*
 * magic_random_bytes - Fill a buffer with random bytes.
 *
 * Use the random pool to generate random data.  This degrades to pseudo
 *  random when used faster than randomness is supplied using magic_churnrand().
 * Note: It's important that there be sufficient randomness in magic_randpool
 *  before this is called for otherwise the range of the result may be
 *  narrow enough to make a search feasible.
 *
 * Ref: Applied Cryptography 2nd Ed. by Bruce Schneier p. 427
 *
 * XXX Why does he not just call magic_churnrand() for each block?  Probably
 *  so that you don't ever publish the seed which could possibly help
 *  predict future values.
 * XXX Why don't we preserve md5 between blocks and just update it with
 *  magic_randcount each time?  Probably there is a weakness but I wish that
 *  it was documented.
 */
void magic_random_bytes(unsigned char *buf, u32_t buf_len) {
  lwip_md5_context md5_ctx;
  u_char tmp[MD5_HASH_SIZE];
  u32_t n;

  while (buf_len > 0) {
    lwip_md5_init(&md5_ctx);
    lwip_md5_starts(&md5_ctx);
    lwip_md5_update(&md5_ctx, (u_char *)magic_randpool, sizeof(magic_randpool));
    lwip_md5_update(&md5_ctx, (u_char *)&magic_randcount, sizeof(magic_randcount));
    lwip_md5_finish(&md5_ctx, tmp);
    lwip_md5_free(&md5_ctx);
    magic_randcount++;
    n = LWIP_MIN(buf_len, MD5_HASH_SIZE);
    MEMCPY(buf, tmp, n);
    buf += n;
    buf_len -= n;
  }
}

/*
 * Return a new random number.
 */
u32_t magic(void) {
  u32_t new_rand;

  magic_random_bytes((unsigned char *)&new_rand, sizeof(new_rand));

  return new_rand;
}

#else /* PPP_MD5_RANDM */

/*****************************/
/*** LOCAL DATA STRUCTURES ***/
/*****************************/
#ifndef LWIP_RAND
static int  magic_randomized;       /* Set when truely randomized. */
#endif /* LWIP_RAND */
static u32_t magic_randomseed;      /* Seed used for random number generation. */


/***********************************/
/*** PUBLIC FUNCTION DEFINITIONS ***/
/***********************************/

/*
 * Initialize the random number generator.
 *
 * Here we attempt to compute a random number seed but even if
 * it isn't random, we'll randomize it later.
 *
 * The current method uses the fields from the real time clock,
 * the idle process counter, the millisecond counter, and the
 * hardware timer tick counter.  When this is invoked
 * in startup(), then the idle counter and timer values may
 * repeat after each boot and the real time clock may not be
 * operational.  Thus we call it again on the first random
 * event.
 */
void magic_init(void) {
  magic_randomseed += sys_jiffies();
#ifndef LWIP_RAND
  /* Initialize the Borland random number generator. */
  srand((unsigned)magic_randomseed);
#endif /* LWIP_RAND */
}

/*
 * magic_init - Initialize the magic number generator.
 *
 * Randomize our random seed value.  Here we use the fact that
 * this function is called at *truely random* times by the polling
 * and network functions.  Here we only get 16 bits of new random
 * value but we use the previous value to randomize the other 16
 * bits.
 */
void magic_randomize(void) {
#ifndef LWIP_RAND
  if (!magic_randomized) {
    magic_randomized = !0;
    magic_init();
    /* The initialization function also updates the seed. */
  } else {
#endif /* LWIP_RAND */
    magic_randomseed += sys_jiffies();
#ifndef LWIP_RAND
  }
#endif /* LWIP_RAND */
}

/*
 * Return a new random number.
 *
 * Here we use the Borland rand() function to supply a pseudo random
 * number which we make truely random by combining it with our own
 * seed which is randomized by truely random events.
 * Thus the numbers will be truely random unless there have been no
 * operator or network events in which case it will be pseudo random
 * seeded by the real time clock.
 */
u32_t magic(void) {
#ifdef LWIP_RAND
  return LWIP_RAND() + magic_randomseed;
#else /* LWIP_RAND */
  return ((u32_t)rand() << 16) + (u32_t)rand() + magic_randomseed;
#endif /* LWIP_RAND */
}

/*
 * magic_random_bytes - Fill a buffer with random bytes.
 */
void magic_random_bytes(unsigned char *buf, u32_t buf_len) {
  u32_t new_rand, n;

  while (buf_len > 0) {
    new_rand = magic();
    n = LWIP_MIN(buf_len, sizeof(new_rand));
    MEMCPY(buf, &new_rand, n);
    buf += n;
    buf_len -= n;
  }
}
#endif /* PPP_MD5_RANDM */

/*
 * Return a new random number between 0 and (2^pow)-1 included.
 */
u32_t magic_pow(u8_t pow) {
  return magic() & ~(~0UL<<pow);
}

#endif /* PPP_SUPPORT */

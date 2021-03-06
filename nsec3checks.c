/*
 * Part of DNS zone file validator `validns`.
 *
 * Copyright 2011, Anton Berezin <tobez@tobez.org>
 * Modified BSD license.
 * (See LICENSE file in the distribution.)
 *
 */

#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/evp.h>

#include "common.h"
#include "textparse.h"
#include "mempool.h"
#include "carp.h"
#include "rr.h"

static struct binary_data name2hash(char *name, struct rr *param)
{
    struct rr_nsec3param *p = (struct rr_nsec3param *)param;
	EVP_MD_CTX ctx;
	unsigned char md0[EVP_MAX_MD_SIZE];
	unsigned char md1[EVP_MAX_MD_SIZE];
	unsigned char *md[2];
	int mdi = 0;
	struct binary_data r = bad_binary_data();
	struct binary_data wire_name = name2wire_name(name);
	int i;
	int digest_size;

	md[0] = md0;
	md[1] = md1;
	if (wire_name.length < 0)
		return r;

	/* XXX Maybe use Init_ex and Final_ex for speed? */

	EVP_MD_CTX_init(&ctx);
	if (EVP_DigestInit(&ctx, EVP_sha1()) != 1)
		return r;
	digest_size = EVP_MD_CTX_size(&ctx);
	EVP_DigestUpdate(&ctx, wire_name.data, wire_name.length);
	EVP_DigestUpdate(&ctx, p->salt.data, p->salt.length);
	EVP_DigestFinal(&ctx, md[mdi], NULL);

	for (i = 0; i < p->iterations; i++) {
		if (EVP_DigestInit(&ctx, EVP_sha1()) != 1)
			return r;
		EVP_DigestUpdate(&ctx, md[mdi], digest_size);
		mdi = (mdi + 1) % 2;
		EVP_DigestUpdate(&ctx, p->salt.data, p->salt.length);
		EVP_DigestFinal(&ctx, md[mdi], NULL);
	}

	r.length = digest_size;
	r.data = getmem(digest_size);
	memcpy(r.data, md[mdi], digest_size);
	return r;
}

extern void calculate_hashed_names(void)
{
	void *x = name2hash;
	x = name2hash;
}

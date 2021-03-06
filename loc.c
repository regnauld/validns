/*
 * Part of DNS zone file validator `validns`.
 *
 * Copyright 2011, Anton Berezin <tobez@tobez.org>
 * Modified BSD license.
 * (See LICENSE file in the distribution.)
 *
 */
#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"
#include "textparse.h"
#include "mempool.h"
#include "carp.h"
#include "rr.h"

static struct rr *loc_parse(char *name, long ttl, int type, char *s)
{
	struct rr_loc *rr = getmem(sizeof(*rr));
	long long i;
	int deg;
	int min;
	double sec;

	i = extract_integer(&s, "degrees latitude");
	if (i < 0)
		return NULL;
	if (i > 90)
		return bitch("degrees latitude not in the range 0..90");
	deg = i;

	min = 0;
	sec = 0;
	if (isdigit(*s)) {
		i = extract_integer(&s, "minutes latitude");
		if (i < 0)
			return NULL;
		if (i > 59)
			return bitch("minutes latitude not in the range 0..59");
		deg = i;

		if (isdigit(*s)) {
		}
	}

	if (*s) {
		return bitch("garbage after valid LOC data");
	}

	return store_record(type, name, ttl, rr);
}

static char* loc_human(struct rr *rrv)
{
    // struct rr_loc *rr = (struct rr_loc *)rrv;
    // char s[1024];

    // snprintf(s, 1024, "\"%s\" \"%s\"", rr->cpu.data, rr->os.data);
    // return quickstrdup_temp(s);
	return "meow";
}

static struct binary_data loc_wirerdata(struct rr *rrv)
{
    struct rr_loc *rr = (struct rr_loc *)rrv;

    return compose_binary_data("1111444", 1,
		rr->version, rr->size,
		rr->horiz_pre, rr->vert_pre,
		rr->latitude, rr->longitude, rr->altitude);
}

struct rr_methods loc_methods = { loc_parse, loc_human, loc_wirerdata, NULL, NULL };

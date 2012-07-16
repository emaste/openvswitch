#include <sys/types.h>
#include <sys/time.h>

#ifndef DISPATCH_H
#define DISPATCH_H 1

struct pkthdr {
	struct timeval ts;	/* time stamp */
	uint32_t caplen;	/* length of portion present */
	uint32_t len;	/* length this packet (off wire) */
};

typedef void (*pkt_handler)(u_char *user, const struct pkthdr *h, 
        const u_char *pkt);

#endif /* DISPATCH_H */

#include <sys/types.h>
#include "ofpbuf.h"

#ifndef DISPATCH_H
#define DISPATCH_H 1

typedef void (*pkt_handler)(u_char *user, struct ofpbuf* buf);

#endif /* DISPATCH_H */

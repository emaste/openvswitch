/*
 * Copyright (c) 2013 Nicira, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CHECKER__
#error "Use this header only with sparse.  It is not a correct implementation."
#endif

/* Get actual <pthread.h> definitions for us to annotate and build on. */
#include_next <pthread.h>

#include "compiler.h"

int pthread_mutex_lock(pthread_mutex_t *mutex) OVS_ACQUIRES(mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex) OVS_RELEASES(mutex);

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) OVS_ACQUIRES(rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) OVS_ACQUIRES(rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) OVS_RELEASES(rwlock);

int pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *mutex)
    OVS_MUST_HOLD(mutex);

/* Sparse complains about the proper PTHREAD_*_INITIALIZER definitions.
 * Luckily, it's not a real compiler so we can overwrite it with something
 * simple. */
#undef PTHREAD_MUTEX_INITIALIZER
#define PTHREAD_MUTEX_INITIALIZER {}

#undef PTHREAD_RWLOCK_INITIALIZER
#define PTHREAD_RWLOCK_INITIALIZER {}

#undef PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP
#define PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP {}

#undef PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP
#define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP {}

#define pthread_mutex_trylock(MUTEX)                    \
    ({                                                  \
        int retval = pthread_mutex_trylock(mutex);      \
        if (!retval) {                                  \
            OVS_ACQUIRE(MUTEX);                         \
        }                                               \
        retval;                                         \
    })

#define pthread_rwlock_tryrdlock(RWLOCK)                \
    ({                                                  \
        int retval = pthread_rwlock_tryrdlock(rwlock);  \
        if (!retval) {                                  \
            OVS_ACQUIRE(RWLOCK);                        \
        }                                               \
        retval;                                         \
    })
#define pthread_rwlock_trywrlock(RWLOCK)                \
    ({                                                  \
        int retval = pthread_rwlock_trywrlock(rwlock);  \
        if (!retval) {                                  \
            OVS_ACQUIRE(RWLOCK);                        \
        }                                               \
        retval;                                         \
    })

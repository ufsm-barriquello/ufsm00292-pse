#ifndef PT_H
#define PT_H

#include <stdint.h>

typedef uint16_t pt_lc_t;

struct pt {
    pt_lc_t lc;
};

typedef enum {
    PT_WAITING = 0,
    PT_YIELDED,
    PT_EXITED,
    PT_ENDED
} pt_status_t;

#define PT_THREAD(name_args) pt_status_t name_args

#define PT_INIT(pt) ((pt)->lc = 0U)

#define PT_BEGIN(pt) switch ((pt)->lc) { case 0:

#define PT_WAIT_UNTIL(pt, condition)                \
    do {                                            \
        (pt)->lc = (pt_lc_t)__LINE__;               \
        case __LINE__:                              \
        if (!(condition)) {                         \
            return PT_WAITING;                      \
        }                                           \
    } while (0)

#define PT_RESTART(pt)                              \
    do {                                            \
        PT_INIT(pt);                                \
        return PT_WAITING;                          \
    } while (0)

#define PT_EXIT(pt)                                 \
    do {                                            \
        PT_INIT(pt);                                \
        return PT_EXITED;                           \
    } while (0)

#define PT_END(pt)                                  \
    default:                                        \
        break;                                      \
    }                                               \
    PT_INIT(pt);                                    \
    return PT_ENDED

#define PT_SCHEDULE(status) ((status) < PT_EXITED)

#endif

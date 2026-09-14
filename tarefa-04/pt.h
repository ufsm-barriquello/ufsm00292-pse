#ifndef PT_H
#define PT_H

/* 
 * Protothreads minimalistas baseadas na biblioteca pt.h de Adam Dunkels
 * Implementacao via continuacao local usando switch-case (Duff's Device)
 */

typedef unsigned short lc_t;

struct pt {
    lc_t lc;
};

#define PT_WAITING 0
#define PT_YIELDED 1
#define PT_EXITED  2
#define PT_ENDED   3

#define PT_INIT(pt)   do { (pt)->lc = 0; } while(0)

#define PT_BEGIN(pt)  switch((pt)->lc) { case 0:

#define PT_END(pt)    } (pt)->lc = 0; return PT_ENDED;

#define PT_WAIT_UNTIL(pt, condition) \
    do { \
        (pt)->lc = __LINE__; case __LINE__: \
        if(!(condition)) return PT_WAITING; \
    } while(0)

#define PT_WAIT_WHILE(pt, cond)  PT_WAIT_UNTIL((pt), !(cond))

#define PT_YIELD(pt) \
    do { \
        (pt)->lc = __LINE__; return PT_YIELDED; case __LINE__:; \
    } while(0)

#define PT_EXIT(pt) \
    do { \
        (pt)->lc = 0; return PT_EXITED; \
    } while(0)

#define PT_RESTART(pt) \
    do { \
        (pt)->lc = 0; return PT_WAITING; \
    } while(0)

#endif /* PT_H */

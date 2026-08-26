#ifndef PT_H
#define PT_H

typedef unsigned short lc_t;
struct pt { lc_t lc; };

#define PT_WAITING 0
#define PT_YIELDED 1
#define PT_EXITED  2
#define PT_ENDED   3

#define PT_INIT(pt)          ((pt)->lc = 0)
#define PT_BEGIN(pt)         switch((pt)->lc) { case 0:
#define PT_WAIT_UNTIL(pt, c) do { \
                               (pt)->lc = __LINE__; case __LINE__: \
                               if (!(c)) return PT_WAITING; \
                             } while(0)
#define PT_WAIT_WHILE(pt, c) PT_WAIT_UNTIL((pt), !(c))
#define PT_RESTART(pt)       do { (pt)->lc = 0; return PT_WAITING; } while(0)
#define PT_EXIT(pt)          do { (pt)->lc = 0; return PT_EXITED; } while(0)
#define PT_END(pt)           } (pt)->lc = 0; return PT_ENDED;
#define PT_SCHEDULE(f)       ((f) == PT_WAITING)

#endif // PT_H
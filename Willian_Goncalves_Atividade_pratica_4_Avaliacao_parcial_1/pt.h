#ifndef PT_H_
#define PT_H_

#define PT_WAITING 0
#define PT_YIELDED 1
#define PT_ENDED   2
#define PT_ENDED_OK 3

typedef struct pt {
    unsigned short lc;
} pt;

#define PT_INIT(pt) (pt)->lc = 0

#define PT_BEGIN(pt) switch((pt)->lc) { case 0:

/* Atributo fallthrough adicionado para evitar erros de compilação estrita no GCC */
#if defined(__GNUC__) && __GNUC__ >= 7
  #define PT_FALLTHROUGH __attribute__((fallthrough))
#else
  #define PT_FALLTHROUGH
#endif

#define PT_WAIT_UNTIL(pt, condition) \
    do { \
        (pt)->lc = __LINE__; PT_FALLTHROUGH; case __LINE__: \
        if(!(condition)) return PT_WAITING; \
    } while(0)

#define PT_END(pt) } (pt)->lc = 0; return PT_ENDED;

#define PT_RESTART(pt) do { (pt)->lc = 0; return PT_WAITING; } while(0)

#endif /* PT_H_ */
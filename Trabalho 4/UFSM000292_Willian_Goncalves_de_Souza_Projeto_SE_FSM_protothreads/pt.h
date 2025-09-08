// ============================================================
// Arquivo: pt.h
// Protothreads minimalistas
// ============================================================
#ifndef PT_H
#define PT_H

typedef unsigned short pt_state;

struct pt {
    pt_state state;
};

#define PT_INIT(pt) ((pt)->state = 0)
#define PT_BEGIN(pt) switch ((pt)->state) { case 0:
#define PT_YIELD(pt) do { (pt)->state = __LINE__; return 0; case __LINE__:; } while (0)
#define PT_END(pt) } (pt)->state = 0; return 1
#define PT_WAIT_UNTIL(pt, cond) do { (pt)->state = __LINE__; case __LINE__: if (!(cond)) return 0; } while (0)

#endif

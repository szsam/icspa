#ifndef __MONITOR_H__
#define __MONITOR_H__
#include "common.h"

enum { STOP, RUNNING, END };
extern int nemu_state;

extern bool do_not_add_instr_len_to_eip;

#endif

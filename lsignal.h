/*
 * lsignal.h -- Signal Handler Header for Lua
 *
 *
*/


#include <signal.h>

#include "lua.h"
#include "lauxlib.h"

#ifndef lsig

#define lsig

struct lua_signal
{
  char *name; /* name of the signal */
  int sig; /* the signal */
};

#endif

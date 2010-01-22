/*
 * lsignal.c -- Signal Handler Library for Lua
 *
 *
*/

#include <signal.h>

#include "lua.h"
#include "lauxlib.h"
#include "lsignal.h"
#define LUA_SIGNAL "lua_signal"

static const struct lua_signal lua_signals[] = {
  {"SIGABRT", SIGABRT},
  {"SIGFPE", SIGFPE},
  {"SIGILL", SIGILL},
  {"SIGINT", SIGINT},
  {"SIGSEGV", SIGSEGV},
  {"SIGTERM", SIGTERM},
  {NULL, 0}
};

static int Nsig = 0;
static lua_State *Lsig = NULL;
static lua_Hook Hsig = NULL;
static int Hmask = 0;
static int Hcount = 0;

static void sighook(lua_State *L, lua_Debug *ar)
{
  lua_pushstring(L, LUA_SIGNAL);
  lua_gettable(L, LUA_REGISTRYINDEX);
  lua_pushnumber(L, Nsig);
  lua_gettable(L, -2);

  lua_call(L, 0, 0);

  /* set the old hook */
  lua_sethook(L, Hsig, Hmask, Hcount);
}

static void handle(int sig)
{
  Hsig = lua_gethook(Lsig);
  Hmask = lua_gethookmask(Lsig);
  Hcount = lua_gethookcount(Lsig);
  Nsig = sig;

  lua_sethook(Lsig, sighook, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
  /*
  switch (sig)
  {
    case SIGABRT: ;
    case SIGFPE: ;
    case SIGILL: ;
    case SIGINT: ;
    case SIGSEGV: ;
    case SIGTERM: ;
  } */
}

/*
 * l_signal == signal(signal [, func [, chook]])
 *
 * signal = signal number or string
 * func = Lua function to call
 * chook = catch within C functions
 *         if caught, Lua function _must_
 *         exit, as the stack is most likely
 *         in an unstable state.
*/  

static int l_signal(lua_State *L)
{
  int args = lua_gettop(L);
  int t, sig; /* type, signal */

  /* get type of signal */
  luaL_checkany(L, 1);
  t = lua_type(L, 1);
  if (t == LUA_TNUMBER)
    sig = (int) lua_tonumber(L, 1);
  else if (t == LUA_TSTRING)
  {
    lua_pushstring(L, LUA_SIGNAL);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 1);
    lua_gettable(L, -2);
    if (!lua_isnumber(L, -1))
      luaL_error(L, "invalid signal string");
    sig = (int) lua_tonumber(L, -1);
    lua_pop(L, 1); /* get rid of number we pushed */
  } else
    luaL_checknumber(L, 1); /* will always error, with good error msg */

  /* set handler */
  if (args == 1 || lua_isnil(L, 2)) /* clear handler */
  {
    lua_pushstring(L, LUA_SIGNAL);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_pushnumber(L, sig);
    lua_gettable(L, -2); /* return old handler */
    lua_pushnumber(L, sig);
    lua_pushnil(L);
    lua_settable(L, -4);
    lua_remove(L, -2); /* remove LUA_SIGNAL table */
  } else
  {
    luaL_checktype(L, 2, LUA_TFUNCTION);

    lua_pushstring(L, LUA_SIGNAL);
    lua_gettable(L, LUA_REGISTRYINDEX);

    lua_pushnumber(L, sig);
    lua_pushvalue(L, 2);
    lua_settable(L, -3);

    /* Set the state for the handler */
    Lsig = L;

    if (lua_toboolean(L, 3)) /* c hook? */
    {
      if (signal(sig, handle) == SIG_ERR)
        lua_pushboolean(L, 0);
      else
        lua_pushboolean(L, 1);
    } else /* lua_hook */
    {
      if (signal(sig, handle) == SIG_ERR)
        lua_pushboolean(L, 0);
      else
        lua_pushboolean(L, 1);
    }
  }
  return 1;
}

/*
 * l_raise == raise(signal)
 *
 * signal = signal number or string
*/  

static int l_raise(lua_State *L)
{
  int args = lua_gettop(L);
  int t = 0; /* type */
  lua_Number ret;

  luaL_checkany(L, 1);

  t = lua_type(L, 1);
  if (t == LUA_TNUMBER)
  {
    ret = (lua_Number) raise((int) lua_tonumber(L, 1));
    lua_pushnumber(L, ret);
  } else if (t == LUA_TSTRING)
  {
    lua_pushstring(L, LUA_SIGNAL);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, 1);
    lua_gettable(L, -2);
    if (!lua_isnumber(L, -1))
      luaL_error(L, "invalid signal string");
    ret = (lua_Number) raise((int) lua_tonumber(L, -1));
    lua_pop(L, 1); /* get rid of number we pushed */
    lua_pushnumber(L, ret);
  } else
    luaL_checknumber(L, 1); /* will always error, with good error msg */

  return 1;
}

static const struct luaL_Reg sig[] = {
  {"signal", l_signal},
  {"raise", l_raise},
  {NULL, NULL}
};

int luaopen_signal(lua_State *L)
{
  int i = 0;

  /* add the library */
  luaL_register(L, "signal", sig);

  /* push lua_signals table into the registry */
  /* put the signals inside the library table too,
   * they are only a reference */
  lua_pushstring(L, LUA_SIGNAL);
  lua_createtable(L, 0, 0);

  while (lua_signals[i].name != NULL)
  {
    /* registry table */
    lua_pushstring(L, lua_signals[i].name);
    lua_pushnumber(L, lua_signals[i].sig);
    lua_settable(L, -3);
    /* signal table */
    lua_pushstring(L, lua_signals[i].name);
    lua_pushnumber(L, lua_signals[i].sig);
    lua_settable(L, -5);
    i++;
  }

  /* add newtable to the registry */
  lua_settable(L, LUA_REGISTRYINDEX);

  return 1;
}

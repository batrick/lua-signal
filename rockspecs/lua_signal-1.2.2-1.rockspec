package = "Lua_Signal"
version = "1.2.2-1"
source = {
   url = "git://github.com/JorjBauer/lua-signal",
   tag = "1.2.2"
}
description = {
   summary = "Library for handling signals",
   detailed = [[
      This is a signal library for Lua 5.1+. It depends on ANSI C signals and has
      some extensions that are available in POSIX, such as kill().
   ]],
   homepage = "http://github.com/batrick/lua-signal",
   license = "MIT",
}
dependencies = {
   "lua >= 5.1"
}
build = {
   type = "builtin",
   modules = {
      signal = "lsignal.c"
   }
}
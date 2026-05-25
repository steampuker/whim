#define LUA_IMPL
#include "minilua.h"

#include <stdio.h>

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "Invalid number of arguments! REPL is not supported.\n");
        return -1;
    }
    
    lua_State *L = luaL_newstate();
    if(L == NULL) {
        fprintf(stderr, "Could not initialize Lua context\n");
        return -1;
    }

    luaL_openlibs(L);
    luaL_loadfile(L, argv[1]);
    lua_call(L, 0, 0);
    lua_close(L);
    return 0;
}

#include "../../inc.h"

hook::fn::setCursorPos_t oSetCursorPos;

BOOL __stdcall hook::setCursorPos(int X, int Y)
{
    return g_vars.menu.opened || oSetCursorPos(X, Y);
}
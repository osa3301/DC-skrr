#include "../../inc.h"

void __stdcall hook::LockCursor( ) {

	if (g_vars.menu.opened)
	{
		g_csgo.m_input_system->enable_input(false);
		g_csgo.m_surface->unlock_cursor();
	}
	else
	{
		// call the original (lock cursor again).
		g_csgo.m_input_system->enable_input(true);
		g_hooks.m_surface.get_old_method< fn::LockCursor_t >(hook::idx::LOCK_CURSOR)(g_csgo.m_surface);
	}
}
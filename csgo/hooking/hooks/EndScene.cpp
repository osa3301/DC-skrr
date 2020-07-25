#include "../../inc.h"
#include "../../features/visuals/visuals.h"
#include "../../features/nade_pred/nade_pred.h"

/*
void Gui_Init(IDirect3DDevice9* device)
{
	// some shit
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = 1.f / 60.f;
	D3DDEVICE_CREATION_PARAMETERS d3dcp{ 0 };
	device->GetCreationParameters(&d3dcp);

	ImGui_ImplDX9_Init(device);
}

void Menu_render(IDirect3DDevice9* device)
{
	static bool toogle = false;
	{
		if (GetAsyncKeyState(VK_INSERT) && !toogle)
			g_vars.menu.MenuOpen = !g_vars.menu.MenuOpen, toogle = true;
		else if (!GetAsyncKeyState(VK_INSERT) && toogle)
			toogle = false;

		if (g_vars.menu.MenuOpen)
			g_csgo.m_input->m_mouse_initialized = false, g_csgo.m_input->m_mouse_active = false;
		else
			g_csgo.m_input->m_mouse_initialized = true;
	}

	if (g_vars.menu.MenuOpen)
	{
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();

		Menu::get().Render();

		ImGui::Render();

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
}
*/


long __stdcall hook::EndScene( IDirect3DDevice9Ex *device ) {
	static auto wanted_ret_address = _ReturnAddress();

	if ( _ReturnAddress( ) == wanted_ret_address )
	{
		static bool once = { false };
		if ( !once )
		{
			once = true;
		}
	}

	return g_hooks.m_directx.get_old_method< fn::EndScene_t >(hook::idx::END_SCENE)(device);
}
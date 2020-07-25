#include "../../inc.h"
#include "../../features/visuals/visuals.h"
#include "../../features/nade_pred/nade_pred.h"
#include "../../features/anti-aim/antiaim.h"

hook::fn::Present_t oPresent;

void handle( IDirect3DDevice9* device )
{
	if (!render::is_ready())
		return;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::GetIO().MouseDrawCursor = g_vars.menu.opened;

	render_cheat_visuals();

	ImGui::EndFrame();

	ImGui::Render();

	im_renderer::renderEnd();
}

HRESULT __stdcall hook::Present( IDirect3DDevice9Ex *device, const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion ) {

	/*
	static uintptr_t gameoverlay_return_address = 0;
	if (!gameoverlay_return_address)
	{
		MEMORY_BASIC_INFORMATION info;
		VirtualQuery(_ReturnAddress(), &info, sizeof(MEMORY_BASIC_INFORMATION));

		char mod[MAX_PATH];
		GetModuleFileNameA((HMODULE)info.AllocationBase, mod, MAX_PATH);

		if (strstr(mod, "gameoverlay"))
			gameoverlay_return_address = (uintptr_t)(_ReturnAddress());
	}

	if (gameoverlay_return_address != (uintptr_t)(_ReturnAddress()) && g_vars.misc.hide_from_obs)
		return g_hooks.m_directx.get_old_method< fn::Present_t >(hook::idx::PRESENT)(device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);

	static const auto _ = ImGui_ImplDX9_Init(device);

	IDirect3DVertexDeclaration9* vertexDeclaration;
	device->GetVertexDeclaration(&vertexDeclaration);

	ImGui_ImplDX9_InvalidateDeviceObjects();
	ImGui_ImplDX9_CreateDeviceObjects();

	handle(device);

	device->SetVertexDeclaration(vertexDeclaration);
	vertexDeclaration->Release();

	//return oPresent(device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	*/

	return g_hooks.m_directx.get_old_method< fn::Present_t >(hook::idx::PRESENT)(device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

#include "../../inc.h"

hook::fn::Reset_t oReset;

HRESULT __stdcall hook::Reset( IDirect3DDevice9Ex *device, D3DPRESENT_PARAMETERS *pPresentationParameters ) {

	const long ret = g_hooks.m_directx.get_old_method< fn::Reset_t >( hook::idx::RESET )( device, pPresentationParameters );

	if (render::is_ready())
		render::device_lost();

	if (ret >= 0 && render::is_ready())
		render::device_reset();

	//return oReset(device, pPresentationParameters);
	return ret;
}

LRESULT CALLBACK hook::Hooked_WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		g_Menu.keys[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		g_Menu.keys[VK_LBUTTON] = false;
		break;
	case WM_RBUTTONDOWN:
		g_Menu.keys[VK_RBUTTON] = true;
		break;
	case WM_RBUTTONUP:
		g_Menu.keys[VK_RBUTTON] = false;
		break;
	case WM_KEYDOWN:
		g_Menu.keys[wParam] = true;
		break;
	case WM_KEYUP:
		g_Menu.keys[wParam] = false;
		break;
	default: break;
	}

	static const auto once = [](HWND window) noexcept {

		return true;
	}(hwnd);

	LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);

	return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
}
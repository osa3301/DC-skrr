#include "../../inc.h"

HRESULT __stdcall hook::ResetEx( IDirect3DDevice9Ex *device, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode ) {

	const long ret = g_hooks.m_directx.get_old_method< fn::ResetEx_t >( hook::idx::RESETEX )( device, pPresentationParameters, pFullscreenDisplayMode);

	return ret;
}

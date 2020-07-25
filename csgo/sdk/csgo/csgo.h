#pragma once

#define sig_directx "A1 ? ? ? ? 50 8B 08 FF 51 0C"

#define discord_hook_reset "8B 1F 68"
#define discord_hook_present "8B 46 10 68"
#define discord_hook_setcursorpos "74 1B 8B 4D 08"

#define steamoverlay_reset "53 57 C7 45"
#define steamoverlay_setcursorpos "C2 08 ? 5D"

enum class Overlay {
	Steam,
	Discord
};

constexpr auto overlay = Overlay::Steam;

static_assert(overlay == Overlay::Steam || overlay == Overlay::Discord, "Invalid overlay selected!");

class c_csgo : protected c_interface_mgr {
private:
	using command_line_t = i_command_line *( __stdcall * )( );

public:
	bool m_interfaces_successful = false;

	std::uintptr_t reset;
	std::uintptr_t present;
	std::uintptr_t setcursorpos;

	// modules.
	pe::Module m_client_dll;
	pe::Module m_engine_dll;
	pe::Module d3dx9;
	pe::Module discordhook_dll;
	pe::Module steam_overlay_dll;

	// misc ptrs;
	IDirect3DDevice9 *m_d3d9_vmt					= nullptr;

	// interfaces.
	chl_client *m_client							= nullptr;
	i_engine_client *m_engine						= nullptr;
	i_client_mode_shared *m_client_mode				= nullptr;
	i_client_entity_list *m_entity_list				= nullptr;
	c_input *m_input								= nullptr;
	c_global_vars *m_global_vars					= nullptr;
	i_cvar *m_convar								= nullptr;
	iv_model_info *m_model_info						= nullptr;
	i_material_system *m_material_system			= nullptr;
	iv_render_view *m_render_view					= nullptr;
	iv_model_render *m_model_render					= nullptr;
	i_surface *m_surface							= nullptr;
	iv_debug_overlay *m_debug_overlay				= nullptr;
	i_panel *m_panel								= nullptr;
	i_studio_render *m_studio_render				= nullptr;
	i_engine_trace *m_engine_trace					= nullptr;
	i_game_movement *m_game_movement				= nullptr;
	i_inputsytem* m_input_system					= nullptr;
	i_prediction *m_prediction						= nullptr;
	i_move_helper *m_movehelper						= nullptr;
	i_game_event_manager *m_game_event				= nullptr;
	i_view_render_beams *m_render_beams				= nullptr;
	i_weapon_system *m_weapon_system				= nullptr;
	iv_efx *m_effects								= nullptr;
	client_state *m_clientstate						= nullptr;
	c_physics_surface *m_physics_surface			= nullptr;
	i_localize *m_localize							= nullptr;
	i_mem_alloc *m_memalloc							= nullptr;
	i_mdl_cache *m_modelcache						= nullptr;
	i_view_render *m_viewrender						= nullptr;
	c_glow_object_mngr *m_glow_obj_manager			= nullptr;
	i_command_line *m_command_line					= nullptr;

	c_csgo() {  }

	bool init( ) {

		// modules
		m_client_dll		= pe::get_module( CT_HASH32( "client_panorama.dll" ) );
		m_engine_dll		= pe::get_module( CT_HASH32( "engine.dll" ) );
		d3dx9				= pe::get_module( CT_HASH32( "shaderapidx9.dll" ) );
		discordhook_dll		= pe::get_module( CT_HASH32( "discordhook.dll") );
		steam_overlay_dll	= pe::get_module( CT_HASH32( "gameoverlayrenderer.dll" ) );

		if( !m_client_dll || !m_engine_dll || !d3dx9 || !(steam_overlay_dll))
			return false;

		if (GetModuleHandleW(L"discordhook") && overlay == Overlay::Discord)
		{
			reset = *reinterpret_cast<std::uintptr_t*>(pattern::find(discordhook_dll, discord_hook_reset, 3));
			present = *reinterpret_cast<std::uintptr_t*>(pattern::find(discordhook_dll, discord_hook_present, 4));
			setcursorpos = *reinterpret_cast<std::uintptr_t*>(pattern::find(discordhook_dll, discord_hook_setcursorpos, 32));
		}
		else
		{
			reset = *reinterpret_cast<std::uintptr_t*>(pattern::find(steam_overlay_dll, steamoverlay_reset, 11));
			present = reset + 4;
			setcursorpos = *reinterpret_cast<std::uintptr_t*>(pattern::find(steam_overlay_dll, steamoverlay_setcursorpos, 6));
		}

		// misc patterns

		m_d3d9_vmt = **pattern::find< IDirect3DDevice9*** >( d3dx9, sig_directx, 1 );
		if( !m_d3d9_vmt )
			return false;

		m_memalloc = *pe::get_export< i_mem_alloc ** >( pe::get_module( "tier0.dll" ), "g_pMemAlloc" );
		if( !m_memalloc )
			return false;

		m_command_line = pe::get_export< command_line_t >( pe::get_module( "tier0.dll" ), "CommandLine" )();
		if( !m_command_line )
			return false;

		// interfaces.
		m_client = get_interface< chl_client >( CT_HASH32( "VClient" ) );
		if( !m_client )
			return false;

		m_engine = get_interface< i_engine_client >( CT_HASH32( "VEngineClient" ) );;
		if ( !m_engine )
			return false;

		m_client_mode = **c_vmt::get_method( m_client, 10 ).add< i_client_mode_shared *** >( 5 );
		//m_client_mode = **reinterpret_cast<i_client_mode_shared***>((*reinterpret_cast<uintptr_t**>(m_client))[10] + 5);
		if( !m_client_mode )
			return false;

		m_input = c_vmt::get_method( m_client, 20 ).at< c_input * >( 4 );
		if( !m_input )
			return false;

		m_entity_list = get_interface< i_client_entity_list >( CT_HASH32( "VClientEntityList" ) );
		if( !m_entity_list )
			return false;

		m_global_vars = **c_vmt::get_method( m_client, 0 ).add< c_global_vars *** >( 27 );
		if( !m_global_vars )
			return false;

		m_convar = get_interface< i_cvar >( CT_HASH32( "VEngineCvar" ) );
		if( !m_convar )
			return false;

		m_model_info = get_interface< iv_model_info >( CT_HASH32( "VModelInfoClient" ) );
		if( !m_model_info )
			return false;

		m_material_system = get_interface< i_material_system >( CT_HASH32( "VMaterialSystem" ) );
		if( !m_material_system )
			return false;

		m_render_view = get_interface< iv_render_view >( CT_HASH32( "VEngineRenderView" ) );
		if( !m_render_view )
			return false;
		
		m_model_render = get_interface< iv_model_render >( CT_HASH32( "VEngineModel" ) );
		if( !m_model_render )
			return false;

		m_debug_overlay = get_interface< iv_debug_overlay >( CT_HASH32( "VDebugOverlay" ) );
		if( !m_debug_overlay )
			return false;

		m_panel = get_interface< i_panel >( CT_HASH32( "VGUI_Panel" ) );
		if( !m_panel )
			return false;

		m_surface = get_interface< i_surface >( CT_HASH32( "VGUI_Surface" ) );
		if( !m_surface )
			return false;
		
		m_input_system = get_interface< i_inputsytem >( CT_HASH32( "InputSystemVersion" ) );
		if( !m_input_system )
			return false;

		m_studio_render = get_interface< i_studio_render >( CT_HASH32( "VStudioRender" ) );
		if( !m_studio_render )
			return false;

		m_engine_trace = get_interface< i_engine_trace >( CT_HASH32( "EngineTraceClient" ) );
		if( !m_engine_trace )
			return false;

		m_game_movement = get_interface< i_game_movement >( CT_HASH32( "GameMovement" ) );
		if( !m_game_movement )
			return false;

		m_prediction = get_interface< i_prediction >( CT_HASH32( "VClientPrediction" ) );
		if( !m_prediction )
			return false;

		m_viewrender = *c_vmt::get_method( m_client, 28 ).at< i_view_render** >( 8 );
		if ( !m_viewrender )
			return false;

		m_movehelper = **reinterpret_cast< i_move_helper *** >( pattern::find( m_client_dll, "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01" ) + 2 );
		if( !m_movehelper )
			return false;

		m_game_event = get_interface< i_game_event_manager >( CT_HASH32( "GAMEEVENTSMANAGER002" ), 0, false );
		if( !m_game_event )
			return false;

		m_render_beams = *pattern::find< i_view_render_beams ** >( m_client_dll, "B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9", 1 );
		if( !m_render_beams )
			return false;

		m_weapon_system = *pattern::find< i_weapon_system ** >( m_client_dll, "8B 35 ? ? ? ? FF 10 0F B7 C0", 2 );
		if( !m_weapon_system )
			return false;

		m_effects = get_interface< iv_efx >( CT_HASH32( "VEngineEffects" ) );
		if( !m_effects )
			return false;

		m_clientstate = **c_vmt::get_method( m_engine, 12 ).add< client_state*** >( 16 );
		if( !m_clientstate )
			return false;

		m_physics_surface = get_interface< c_physics_surface >( CT_HASH32( "VPhysicsSurfaceProps" ) );
		if( !m_physics_surface )
			return false;

		m_localize = get_interface< i_localize >( CT_HASH32( "Localize_001" ), 0, false );
		if( !m_localize )
			return false;

		m_modelcache = get_interface< i_mdl_cache >( CT_HASH32( "MDLCache" ) );
		if( !m_modelcache )
			return false;

		m_glow_obj_manager = *pattern::find< c_glow_object_mngr ** >( m_client_dll, "0F 11 05 ? ? ? ? 83 C8 01", 3 );
		if( !m_glow_obj_manager )
			return false;

		m_interfaces_successful = true;

		return true;
	}
};

extern c_csgo g_csgo;
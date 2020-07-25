#pragma once
#include "../../inc.h"

class c_visuals {
private:
	struct bbox_t {
		int x, y, w, h;
	};

	struct context_t {
		bool m_enabled;
		int offset{ 0 };
		int flag_count{ 0 };
	} m_ctx;

	void player( c_csplayer *entity );
	void draw_healthbar( c_csplayer * entity, float x, float y, float w, float h );
	void dynamic_box( c_csplayer* entity, color col, bool object, int index);
	void draw_box( vec3_t* pointList, color col );
	bool calculate_bbox( c_base_entity *entity, bbox_t &box ) const;
	void activation_type();
	void run_dormancy_fade( c_csplayer *entity, int idx );
	void draw_box( color colour, int index, float x, float y, float w, float h );
	void rounded_box_static( int x, int y, int w, int h, int radius, color col );
	void ammo_bar( c_base_combat_weapon *weapon, c_csplayer *player, color colour, float x, float y, float w, float h, int clip );
	void flash_bar( c_csplayer * player, color colour, float x, float y, float w, float h );
	void weapon_name( c_base_combat_weapon *weapon, c_csplayer *player, color colour, float x, float y, float w, float h );
	void draw_flags( c_csplayer *player, color colour, float x, float y, float w, float h );
	void draw_spectators( ) const;

	void world( c_base_entity * entity );
	void draw_scope( ) const;
	void draw_crosshair( ) const;
	static void draw_hitmarker( );
	void draw_bomb_timer( float time_left ) const;

	std::array< float, 64 > m_alpha;
public:
	static bool world_to_screen( const vec3_t &origin, vec3_t &screen );
	void run( );
	color rainbow( float frequency );
	void watermark( ) const;
	void handle_glow( );
};

extern c_visuals g_visuals;
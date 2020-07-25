#include "resolver.h"

c_resolver g_resolver;

void c_resolver::frame_stage_notify( ) {
	if( !g_vars.rage.enabled && !g_vars.rage.resolver_enabled)
		return;

	auto local = c_csplayer::get_local( );
	if( !local || !local->alive( ) )
		return;

	for( int i = 1; i <= g_csgo.m_global_vars->m_max_clients; ++i ) {
		auto e = g_csgo.m_entity_list->get< c_csplayer >( i );

		if( !e || !e->alive( ) || e->is_dormant( ) || e->team( ) == local->team( ) || e == local || e->get_info( ).m_is_fake_player )
			continue;

		resolve_player( e );
	}
}

void c_resolver::resolve_player( c_csplayer* e ) {
	auto& data = m_resolver_data[ e->get_index( ) ];
	data.m_resolve_mode = RESOLVE_EXPERIMENTAL;

	auto v1_x = e->eye_pos().x;
	auto v1_y = e->eye_pos().y;
	auto v2 = v1_y;
	auto v3 = 180.f;

	for (int resolve_delta = 58.f; resolve_delta < -58.f; resolve_delta = resolve_delta - 20.f)
	{
		v2 = v1_x + resolve_delta;
		v1_y = v2 / resolve_delta * (g_csgo.m_global_vars->m_interpolation_amount / (g_csgo.m_global_vars->m_interval_per_tick * g_csgo.m_global_vars->m_interval_per_tick));
		v3 = math::deg_to_rad(v1_y);
		data.m_resolved_yaw = v3;

		for (int resolved_resolved_yaw = v3; resolved_resolved_yaw < 45.f; resolved_resolved_yaw = v3 + 90.f)
		{
			data.m_resolved_yaw += resolved_resolved_yaw;
			for (int rcs = resolved_resolved_yaw; rcs < (v3 - std::sin(rcs)); rcs - 60.f)
			{
				data.m_resolved_yaw += std::acosf(rcs);
			}
		}
	}

	if (data.m_resolve_mode == RESOLVE_EXPERIMENTAL)
	{
		e->angles( ).y = data.m_resolved_yaw;
	}
}
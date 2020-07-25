#include "../../inc.h"
#include "../visuals/visuals.h"

c_autobot g_autobot;

//std::unique_ptr<nav_mesh::nav_file> nav;
static std::unique_ptr<nav_mesh::nav_file> map_nav;
//static std::vector<nav_mesh::nav_area*> crumbs;
static std::vector<vec3_t> breadcrumbs;
static u32 last_area;

static std::unordered_map<int, float> area_score;
static std::vector<nav_mesh::nav_area*> findClosestNavSquare_localAreas(6);
static constexpr int crumb_limit = 64;
static vec3_t startPoint, endPoint;

bool ReadyForCommands = true;
static bool can_repath = false;
static bool initialized;

static Timer inactivity{};
static Timer reset_pather_timer{};
static Timer last_jump{};

//vars
static int stuck_time = 4000;
static int unreachable_time = 1000;

std::pair<float, float> c_autobot::ComputeMove(c_user_cmd *cmd, const vec3_t& a, const vec3_t& b)
{
	vec3_t diff = (b - a);

	if (diff.length() == 0.0f)
		return { 0, 0 };

	const float x = diff.x;
	const float y = diff.y;

	vec3_t vsilent(x, y, 0);

	float speed = sqrt(vsilent.x * vsilent.x + vsilent.y * vsilent.y);

	vec3_t ang;

	math::vector_angle(vsilent, ang);

	float yaw = math::deg_to_rad(ang.y - cmd->m_viewangles.y);

	return { cos(yaw) * 450.0f, -sin(yaw) * 450.0f };
}

void c_autobot::WalkTo(const vec3_t& vector)
{
	auto local = c_csplayer::get_local();

	if (!local || !local->alive())
		return;

	// Calculate how to get to a vector
	auto result = ComputeMove(g_cl.m_cmd, local->origin(), vector);

	// Push our move to usercmd
	g_cl.m_cmd->m_forwardmove = result.first;
	g_cl.m_cmd->m_sidemove = result.second;
}

void c_autobot::init()
{
	auto pmapname = std::string(reinterpret_cast<const char*>(g_csgo.m_clientstate) + 0x28C); // >> mapname.bsp
	std::string mapname = pmapname.c_str(); std::string dir; std::string subdir = "/maps/"; std::string gamedir = g_csgo.m_engine->get_game_directory(); std::string finalstr = gamedir + subdir + mapname;
	std::replace(finalstr.begin(), finalstr.end(), '\\', '/');
	int dotpos = finalstr.find('.');
	finalstr = finalstr.substr(0, dotpos);
	finalstr.append(".nav");
	dir.append(finalstr);

	map_nav.get()->load(dir.c_str());

	console::print("map %s \n", dir.c_str());
}

bool c_autobot::navTo(const vec3_t& destination, bool should_repath)
{
	can_repath	= should_repath;
	endPoint	= destination;

	auto local = c_csplayer::get_local();

	if (!local || !local->alive())
		return false;

	vec3_t o = local->origin();

	nav_mesh::vec3_t start	= { o.x, o.y, o.z };
	nav_mesh::vec3_t dest	= { destination.x, destination.y, destination.z };

	try
	{
		static std::vector<nav_mesh::vec3_t> path;

		if (initialized)
			path = map_nav.get()->find_path(start, dest);
		else
			return false;

		if (!path.empty())
		{
			console::print("Path found \n");

			if (!should_repath)
				findClosestNavSquare_localAreas.clear();

			for (size_t i = 0; i < path.size(); i++)
			{
				breadcrumbs.push_back( vec3_t{ path.at(i).x, path.at(i).y, path.at(i).z } );
			}

			if (path.size() == breadcrumbs.size())
				return true; console::print(" p == bc");

			ReadyForCommands = false;
		}
		else
			return false;
	}
	catch (const std::exception & e)
	{
		console::print(e.what());
		return false;
	}
}

void c_autobot::repath()
{
	if (!can_repath)
		return;

	vec3_t last;

	if (!breadcrumbs.empty())
	{
		auto local = c_csplayer::get_local();
		auto pos = local->origin();

		auto area = map_nav.get()->get_area_by_position(nav_mesh::vec3_t{ pos.x, pos.y, pos.z });
		auto center = area.get_center();

		last = vec3_t{ center.x, center.y, center.z };
	}
	else if (endPoint.is_valid())
		last = endPoint;
	else
		return;

	clearInstructions();
	ResetPather();

	navTo(last, true);
}

void c_autobot::clearInstructions()
{
	breadcrumbs.clear();
	endPoint.invalidate();
}

void c_autobot::ResetPather()
{
	map_nav.get()->m_pather.reset();
}

bool IsVectorVisible(vec3_t origin, vec3_t target, bool enviroment_only, c_csplayer* self, unsigned int mask)
{
	if (!enviroment_only)
	{
		trace_t trace_visible;
		c_trace_filter_players_only_skip_one f(self);

		g_csgo.m_engine_trace->trace_ray(ray_t{ origin, target }, mask, &f, &trace_visible);
		return (trace_visible.m_fraction == 1.0f);
	}
	else
	{
		trace_t trace_visible;

		c_trace_filter_players_only_skip_one f(self);

		g_csgo.m_engine_trace->trace_ray(ray_t{ origin, target }, mask, &f, &trace_visible);
		return (trace_visible.m_fraction == 1.0f);
	}
}

bool IsVectorVisibleNavigation(vec3_t origin, vec3_t target, unsigned int mask)
{
	trace_t trace_visible;

	c_trace_filter_skip_entity filter(c_csplayer::get_local());

	g_csgo.m_engine_trace->trace_ray(ray_t{ origin, target }, mask, &filter, &trace_visible);
	return (trace_visible.m_fraction == 1.0f);
}

// Function for getting closest Area to player, aka "LocalNav"
nav_mesh::nav_area* c_autobot::findClosestNavSquare(const vec3_t& vec)
{
	auto local = c_csplayer::get_local();
	bool isLocal = vec == local->origin();
	if (isLocal && findClosestNavSquare_localAreas.size() > 5)
		findClosestNavSquare_localAreas.erase(findClosestNavSquare_localAreas.begin());

	float ovBestDist = FLT_MAX, bestDist = FLT_MAX;
	// If multiple candidates for LocalNav have been found, pick the closest
	nav_mesh::nav_area* ovBestSquare = nullptr, * bestSquare = nullptr;
	for (auto& i : map_nav.get()->m_areas)
	{
		// Make sure we're not stuck on the same area for too long
		if (isLocal && std::count(findClosestNavSquare_localAreas.begin(), findClosestNavSquare_localAreas.end(), &i) >= 3)
		{
			continue;
		}
		vec3_t center = { i.get_center().x, i.get_center().y, i.get_center().z };
		float dist = center.distance(vec);
		if (dist < bestDist)
		{
			bestDist = dist;
			bestSquare = &i;
		}
		// Check if we are within x and y bounds of an area
		if (ovBestDist >= dist || !IsVectorVisibleNavigation(vec, center, MASK_PLAYERSOLID)) // !i.IsOverlapping(vec) || 
		{
			continue;
		}
		ovBestDist = dist;
		ovBestSquare = &i;
	}
	if (!ovBestSquare)
		ovBestSquare = bestSquare;

	if (isLocal)
		findClosestNavSquare_localAreas.push_back(ovBestSquare);

	return ovBestSquare;
}

// Update area score to prefer paths used by actual players a bit more
void c_autobot::updateAreaScore()
{
	for (int i = 1; i <= g_csgo.m_entity_list->get_highest_entity_index(); i++)
	{
		auto entity = g_csgo.m_entity_list->get<c_base_entity>(i);
		auto local = c_csplayer::get_local();

		if (!(entity && entity->is_player()))
			continue;

		if (i == local->get_index())
			continue;

		// Get area
		nav_mesh::nav_area* closest_area = nullptr;

		if (entity->is_dormant())
			findClosestNavSquare(entity->origin());

		// Add usage to area if valid
		if (closest_area)
			area_score[closest_area->get_id()] += g_csgo.m_global_vars->m_interval_per_tick;
	}
	if (reset_pather_timer.test_and_set(10000))
		ResetPather();
}

void c_autobot::Move()
{
	static float dist_to_dest = { 0 };

	if (breadcrumbs.size() > crumb_limit)
		return;

	if (breadcrumbs.empty())
		return;

	auto local = c_csplayer::get_local();

	if (!local || !local->alive())
		return;

	if (breadcrumbs.size() != 0)
	{
		vec3_t origin = local->origin();
		dist_to_dest = (origin - endPoint).length();

		for (size_t i = 0; i < breadcrumbs.size() - 1; i++)
		{
			if (origin.distance(breadcrumbs.at(i)) < 60.f)
			{
				for (int j = 0; j <= i; j++)
					breadcrumbs.erase(breadcrumbs.begin());
			}
		}

		if (dist_to_dest <= 0.f)
		{
			breadcrumbs.clear();
			return;
		}

		WalkTo(breadcrumbs[0]);
	}
}

void c_autobot::Begin()
{
	static vec3_t v_Destination;
	static nav_mesh::vec3_t nv_dest;

	if (!initialized)
	{
		initialized = true;
		init();
	}

	updateAreaScore();

	auto local = c_csplayer::get_local();
	auto pos = local->origin();

	if (!local)
		return;

	if (!local->alive())
	{
		// Clear path
		clearInstructions();
		return;
	}

	auto crumb = breadcrumbs.begin();
	const vec3_t* crumb_vec;

	// Breadcrumbs empty, prepare for next instruction
	if (crumb == breadcrumbs.end())
	{
		if (endPoint.is_valid())
			crumb_vec = &endPoint;
		else
		{
			ReadyForCommands = true;
			can_repath = false;
			return;
		}
	}
	else
	{
		auto area = map_nav.get()->get_area_by_position(nav_mesh::vec3_t{ pos.x, pos.y, pos.z });
		auto center = area.get_center();

		crumb_vec = new vec3_t{ center.x, center.y, center.z };
	}

	// Remove old breadcrumbs
	if (pos.distance(*crumb_vec) < 50.0f)
	{
		inactivity.update();
		if (crumb_vec == &endPoint)
		{
			endPoint.invalidate();
			return;
		}
		auto area = map_nav.get()->get_area_by_position(nav_mesh::vec3_t{ crumb_vec->x, crumb_vec->y, crumb_vec->z });
		last_area = area.get_id();

		breadcrumbs.erase(crumb);
		crumb = breadcrumbs.begin();
		if (crumb == breadcrumbs.end())
		{
			if (!endPoint.is_valid())
			{
				console::print("AutoBot.cpp:Begin() -> endPoint.is_valid() == false");
				return;
			}
			crumb_vec = &endPoint;
		}
	}

	// Detect when jumping is necessary
	if ((!(crumb_vec->z - pos.z > 18 && last_jump.check(200)) || (last_jump.check(200) && inactivity.check(stuck_time / 2))))
	{
		auto local = findClosestNavSquare(pos);
		// Check if current area allows jumping
		if (!local ) // !(local & (NAV_MESH_NO_JUMP | NAV_MESH_STAIRS))
		{
			static bool flip_action = false;
			// Make it crouch the second tick
			g_cl.m_cmd->m_buttons |= flip_action ? IN_DUCK : IN_JUMP;

			// Update jump timer now
			if (flip_action)
				last_jump.update();
			flip_action = !flip_action;
		}
	}

	Move();

	if (inactivity.check(stuck_time) || (inactivity.check(unreachable_time) && !IsVectorVisible(pos, *crumb_vec + vec3_t(.0f, .0f, 41.5f), false, local, MASK_PLAYERSOLID)))
	{
		/* crumb is invalid if endPoint is used 
		if (crumb_vec != &endPoint)
			addTime(last_area, *crumb, inactivity);
			*/

		repath();
		return;
	}

	try
	{
		SHORT k2 = GetKeyState(VK_DOWN);
		SHORT k1 = GetKeyState(VK_NUMPAD0);
		bool isDown2 = k2 & 0x8000;
		bool isDown1 = k1 & 0x8000;
		
		if (isDown2)
		{
			//breadcrumbs.clear();

			v_Destination = local->origin(); // set destination
			nv_dest = { v_Destination.x, v_Destination.y, v_Destination.z }; // set destination

			console::print("destination -> [ x: %f, y: %f, z: %f ] \n", nv_dest.x, nv_dest.y, nv_dest.z);
		}

		if (isDown1)
		{
			bool success = navTo(vec3_t{ nv_dest.x, nv_dest.y, nv_dest.z }, false);

			if (!success)
			{
				console::print("No valid spots found! \n");
			}
		}
	}
	catch (const std::exception & e)
	{
		console::print(e.what());
	}
}

void c_autobot::DrawPath()
{
	if (breadcrumbs.size() < 2)
		return;

	vec3_t vecBottom, vecTop;

	for (size_t i = 0; i < breadcrumbs.size() - 1; i++)
	{
		if (g_visuals.world_to_screen(breadcrumbs[i], vecTop) && g_visuals.world_to_screen(breadcrumbs[i + 1], vecBottom))
		{
			render::draw_line(vecTop[0], vecTop[1], vecBottom[0], vecBottom[1], color(255, 255, 255, 255));
		}
	}

	vec3_t vecBottom1, vecTop1;

	if (g_visuals.world_to_screen(breadcrumbs[0], vecTop1) && g_visuals.world_to_screen(breadcrumbs[0], vecBottom1))
	{
		render::draw_rect(vecTop1[0] - 4, vecTop1[1] - 4, 8, 8, color(255, 0, 100, 255));
		render::draw_outline(vecTop1[0] - 4, vecTop1[1] - 4, 7, 7, color(255, 0, 100, 255));
	}
}

void c_autobot::run()
{
	if ( g_csgo.m_engine->is_in_game() && g_csgo.m_engine->is_connected() )
	{
		Begin();
	}
}
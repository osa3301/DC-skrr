#include "../inc.h"

#include "../../images.h"

c_menu g_Menu;

#define MAX_TABS 6
#define TAB_WEIGHT 72
#define TAB_HEIGHT 60
#define BORDER_SIZE 4

typedef std::basic_string<TCHAR> tstring;

tstring VirtualKeyCodeToString(UCHAR virtualKey)
{
	UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

	TCHAR szName[128];
	int result = 0;
	switch (virtualKey)
	{
	case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
	case VK_PRIOR: case VK_NEXT:
	case VK_END: case VK_HOME:
	case VK_INSERT: case VK_DELETE:
	case VK_DIVIDE:
	case VK_NUMLOCK:
		scanCode |= KF_EXTENDED;
	default:
		result = GetKeyNameText(scanCode << 16, szName, 128);
	}
	if (result == 0)
		throw std::system_error(std::error_code(GetLastError(), std::system_category()),
			"WinAPI Error occured.");
	return szName;
}

std::string getKeyPressedString(UCHAR vkey)
{
	TCHAR lpszName[256];
	if (!GetKeyNameText(vkey, lpszName, sizeof(lpszName)))
	{
		return std::string("");
	}
	return std::string(lpszName);
}

void c_menu::Init()
{
	if (bInitialised)
		return;

	if (!background.index) background.index = g_csgo.m_surface->create_new_texture_id(true);

	if (!icon_headshot.index) icon_headshot.index = g_csgo.m_surface->create_new_texture_id(true);

	if (!icon_aimbot.index) icon_aimbot.index = g_csgo.m_surface->create_new_texture_id(true);

	if (!icon_systems.index) icon_systems.index = g_csgo.m_surface->create_new_texture_id(true);

	if (!icon_options.index) icon_options.index = g_csgo.m_surface->create_new_texture_id(true);

	if (!icon_visuals.index) icon_visuals.index = g_csgo.m_surface->create_new_texture_id(true);

	if (!icon_members.index) icon_members.index = g_csgo.m_surface->create_new_texture_id(true);

	if (!icon_headshot_not_selected.index) icon_headshot_not_selected.index = g_csgo.m_surface->create_new_texture_id(true);

	if (!icon_aimbot_not_selected.index) icon_aimbot_not_selected.index = g_csgo.m_surface->create_new_texture_id(true);

	if (!icon_systems_not_selected.index) icon_systems_not_selected.index = g_csgo.m_surface->create_new_texture_id(true);

	if (!icon_options_not_selected.index) icon_options_not_selected.index = g_csgo.m_surface->create_new_texture_id(true);

	if (!icon_visuals_not_selected.index) icon_visuals_not_selected.index = g_csgo.m_surface->create_new_texture_id(true);

	if (!icon_members_not_selected.index) icon_members_not_selected.index = g_csgo.m_surface->create_new_texture_id(true);

	if (background.index &&
		icon_headshot.index && icon_aimbot.index && icon_systems.index && icon_options.index && icon_visuals.index && icon_members.index &&
		icon_headshot_not_selected.index && icon_aimbot_not_selected.index && icon_systems_not_selected.index && icon_options_not_selected.index && icon_visuals_not_selected.index && icon_members_not_selected.index)
	{
		background.width = 650;
		background.height = 490;

		icon_headshot.width = icon_headshot.width = 45;
		icon_aimbot.width = icon_aimbot.height = 45;
		icon_systems.width = icon_systems.height = 45;
		icon_options.width = icon_options.height = 45;
		icon_visuals.width = icon_visuals.height = 45;
		icon_members.width = icon_members.height = 45;

		icon_headshot_not_selected.width = icon_headshot_not_selected.width = 45;
		icon_aimbot_not_selected.width = icon_aimbot_not_selected.height = 45;
		icon_systems_not_selected.width = icon_systems_not_selected.height = 45;
		icon_options_not_selected.width = icon_options_not_selected.height = 45;
		icon_visuals_not_selected.width = icon_visuals_not_selected.height = 45;
		icon_members_not_selected.width = icon_members_not_selected.height = 45;

		g_csgo.m_surface->draw_set_texture_RGBA(background.index, gui_background, background.width, background.height);

		g_csgo.m_surface->draw_set_texture_RGBA(icon_headshot.index, gui_headshot, icon_headshot.width, icon_headshot.width);
		g_csgo.m_surface->draw_set_texture_RGBA(icon_aimbot.index, gui_aim, icon_aimbot.width, icon_aimbot.height);
		g_csgo.m_surface->draw_set_texture_RGBA(icon_options.index, gui_options, icon_options.width, icon_options.height);
		g_csgo.m_surface->draw_set_texture_RGBA(icon_members.index, gui_members, icon_members.width, icon_members.height);
		g_csgo.m_surface->draw_set_texture_RGBA(icon_visuals.index, gui_visuals, icon_visuals.width, icon_visuals.height);
		g_csgo.m_surface->draw_set_texture_RGBA(icon_systems.index, gui_system, icon_systems.width, icon_systems.height);

		g_csgo.m_surface->draw_set_texture_RGBA(icon_headshot_not_selected.index, gui_headshot_off, icon_headshot.width, icon_headshot.width);
		g_csgo.m_surface->draw_set_texture_RGBA(icon_aimbot_not_selected.index, gui_aim_off, icon_aimbot.width, icon_aimbot.height);
		g_csgo.m_surface->draw_set_texture_RGBA(icon_options_not_selected.index, gui_options_off, icon_options.width, icon_options.height);
		g_csgo.m_surface->draw_set_texture_RGBA(icon_members_not_selected.index, gui_members_off, icon_members.width, icon_members.height);
		g_csgo.m_surface->draw_set_texture_RGBA(icon_visuals_not_selected.index, gui_visuals_off, icon_visuals.width, icon_visuals.height);
		g_csgo.m_surface->draw_set_texture_RGBA(icon_systems_not_selected.index, gui_system_off, icon_systems.width, icon_systems.height);

		bInitialised = true;
		g_vars.menu.opened = false;
		dwCheckboxBlockedTime = 0;
		dwSliderBlockedTime = 0;
		dwPaletteBlockedTime = 0;
		dwListBlockedTime = 0;

		// default menu position
		MenuX = 60;
		MenuY = 20;

		iCurrentTab = 1; // default tab
	}
}

void c_menu::Run()
{
	Init();

	static bool m_bMouseCapture = false;

	static DWORD dwTemporaryBlockTimer = 0;

	// ============================ TOGGLE MENU ============================ 
	int key = VK_INSERT;

	static bool toogle = false;
	{
		if (GetAsyncKeyState(key) && !toogle)
			g_vars.menu.opened = !g_vars.menu.opened, toogle = true;
		else if (!GetAsyncKeyState(key) && toogle)
			toogle = false;
	}
	// ===================================================================== 

	// IF MENU IS CLOSED

	if (!g_vars.menu.opened && g_csgo.m_engine->is_in_game()) // && !g_pGameUI->IsGameUIActive()
	{
		m_bMouseCapture = false;

		int s_w, s_h;

		g_csgo.m_surface->get_screen_size(s_w, s_h);

		// center of screen ( where crosshair is )
		unsigned int midw = s_w / 2;
		unsigned int midh = s_h / 2;

		g_csgo.m_surface->surface_set_cursor_pos(midw, midh);
	}

	if (!g_vars.menu.opened ||
		!g_csgo.m_surface->is_texture_id_valid(background.index) ||
		!g_csgo.m_surface->is_texture_id_valid(icon_headshot.index) || !g_csgo.m_surface->is_texture_id_valid(icon_aimbot.index) || !g_csgo.m_surface->is_texture_id_valid(icon_options.index) || !g_csgo.m_surface->is_texture_id_valid(icon_visuals.index) || !g_csgo.m_surface->is_texture_id_valid(icon_systems.index) || !g_csgo.m_surface->is_texture_id_valid(icon_members.index) ||
		!g_csgo.m_surface->is_texture_id_valid(icon_headshot_not_selected.index) || !g_csgo.m_surface->is_texture_id_valid(icon_aimbot_not_selected.index) || !g_csgo.m_surface->is_texture_id_valid(icon_options_not_selected.index) || !g_csgo.m_surface->is_texture_id_valid(icon_visuals_not_selected.index) || !g_csgo.m_surface->is_texture_id_valid(icon_systems_not_selected.index) || !g_csgo.m_surface->is_texture_id_valid(icon_members_not_selected.index))
		return;

	// IF MENU IS OPENED

	if (g_csgo.m_engine->is_in_game()) // && !g_pGameUI->IsGameUIActive()
	{
		m_bMouseCapture = true;

		g_csgo.m_surface->surface_set_cursor_pos(CursorX, CursorY);
	}

	//GetCursorPos(&cursor); CursorX = cursor.x; CursorY = cursor.y;

	g_csgo.m_surface->surface_get_cursor_pos(CursorX, CursorY);

	g_csgo.m_surface->draw_texture(background.index, MenuX, MenuY, background.width + MenuX, background.height + MenuY, 100, 100, 100, 255); // Draw background

	// Reset all
	bCursorInPalette = false;
	bCursorInList = false;
	iPaletteIndex = 0;
	iListIndex = 0;

	SelectTab();
	DrawMenuTabs();
	Tabs();
	Drag();
}

void c_menu::Tabs()
{
	static int m_iOldTab = iCurrentTab;

	if (m_iOldTab != iCurrentTab)
	{
		for (unsigned int i = 0; i < LIMIT_LISTBOX; i++)
			bListBoxOpened[i] = false;

		for (unsigned int i = 0; i < LIMIT_PALETTE; i++)
			bPaletteOpened[i] = false;

		m_iOldTab = iCurrentTab;
	}

	static unsigned int indent_x = 10;
	static unsigned int indent_y = 14;

	if (iCurrentTab == 1) // Rage aimbot
	{
		unsigned int x = MenuX + 100;
		unsigned int y = MenuY + 30;

		unsigned int line_y = indent_y;

		unsigned int save[LIMIT_SAVE];

		{ // BOX1
			DrawBox(x, y, 250, 230); // Aimbot

			render::draw_filled_rect_old(x + 14, y - 1, x + 60, y + 2, color(30, 30, 30, 255));
			render::draw_string(x + 20, y, render::fonts::main, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3], g_vars.menu.cheat_color[0]), FONT_LEFT, "Aimbot");

			Checkbox(x + indent_x, y + line_y, g_vars.rage.enabled, "Enabled");

			KeyBind(x + indent_x + 180, y + line_y - 10, g_vars.rage.key);
			line_y += 30;

			static char* szTargetSelection[] = { "Highest damage", "Closest distance", "Cycle" };
			save[7] = y + line_y;
			line_y += 40;

			static char* szTargetHitbox[] = { "Head", "Neck", "Pelvis", "Stomach", "Thorax" };
			save[8] = y + line_y;
			line_y += 40;

			static char* szMultipoint[] = { "Low", "Medium", "High" };
			save[9] = y + line_y;
			line_y += 40;

			Checkbox(x + indent_x, y + line_y, g_vars.rage.teammate, "Teammates");
			line_y += 18;

			Checkbox(x + indent_x, y + line_y, g_vars.rage.autowall, "Automatic penetration");
			line_y += 18;

			Checkbox(x + indent_x, y + line_y, g_vars.rage.silent, "Silent aim");
			line_y += 30;

			ListBox(iListIndex++, x + indent_x, save[7], "Target selection", g_vars.rage.target_selection, szTargetSelection, 3, true);
			ListBox(iListIndex++, x + indent_x, save[8], "Target hitbox", g_vars.rage.primary_hitbox, szTargetHitbox, 5, true);
		}

		{//BOX1.2
			y = y + 250;
			line_y = 15;

			DrawBox(x, y, 250, 180);//Fakelag

			render::draw_filled_rect_old(x + 14, y - 1, x + 68, y + 2, color(0, 0, 0, 255));
			render::draw_string(x + 20, y, render::fonts::main, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3]), FONT_LEFT, "Fake lag");

			Checkbox(x + indent_x, y + line_y, g_vars.misc.fakelag.enabled, "Enabled");
			line_y += 20;

			char* tmp4[] = { "Default", "On land", "In air" };
			save[4] = y + line_y;
			line_y += 30;

			char* tmp3[] = { "Maximum", "Adaptive" };
			save[3] = y + line_y;
			line_y += 40;

			Slider(x + indent_x, y + line_y, 0, 100, g_vars.misc.fakelag.amount, "Choke");
			line_y += 30;

			ListBox(iListIndex++, x + indent_x, save[4], "", g_vars.misc.fakelag.lagjump, tmp4, 3, true);
			ListBox(iListIndex++, x + indent_x, save[3], "", g_vars.misc.fakelag.type, tmp3, 2, true);
		}

		{//BOX2
			y = MenuY + 30;
			x = x + 270;
			line_y = 15;

			DrawBox(x, y, 250, 200); // Other

			render::draw_filled_rect_old(x + 14, y - 1, x + 54, y + 2, color(0, 0, 0, 255));
			render::draw_string(x + 20, y, render::fonts::main, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3]), FONT_LEFT, "Other");

			//Checkbox(x + indent_x, y + line_y, g_vars.t.btest, "Remove recoil");
			//line_y += 15;

			//Checkbox(x + indent_x, y + line_y, g_vars.t.btest, "Remove spread");
			//line_y += 15;

			Checkbox(x + indent_x, y + line_y, g_vars.rage.autostop, "Quick stop");
			line_y += 15;

			//Checkbox(x + indent_x, y + line_y, g_vars.t.btest, "Quick stop + duck");
			//line_y += 15;

			//Checkbox(x + indent_x, y + line_y, g_vars.t.btest, "Automatic scope");
			//line_y += 15;

			Checkbox(x + indent_x, y + line_y, g_vars.rage.resolver_enabled, "Anti-aim resolver");
			line_y += 30;

			//char* tmp[] = { "Pitch / Yaw", "Roll" };
			//ListBox(iListIndex++, x + indent_x, y + line_y, "Remove spread method", cvar.nospread_method, tmp, 2, false);
			//line_y += 40;

			Slider(x + indent_x, y + line_y, 0, 180, g_vars.rage.hitchance, "Hit chance");
			line_y += 30;

			Slider(x + indent_x, y + line_y, 0, 180, g_vars.rage.pointscale, "Pointscale");
			line_y += 15;
		}

		{//BOX3
			y = y + 220;
			line_y = 30;

			DrawBox(x, y, 250, 210); // Anti-Aimbot

			render::draw_filled_rect_old(x + 14, y - 1, x + 86, y + 2, color(0, 0, 0, 255));
			render::draw_string(x + 20, y, render::fonts::main, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3]), FONT_LEFT, "Anti-Aimbot");

			char* tmp1[] = { "Fakedown", "Down", "Up", "Jitter", "Random" };
			save[0] = y + line_y;
			line_y += 40;

			char* tmp2[] = { "180", "180 jitter", "Spin", "Jitter", "Sideways", "Random", "Static" };
			save[1] = y + line_y;
			line_y += 40;

			char* tmp3[] = { "180", "180 jitter", "Spin", "Jitter", "Sideways", "Random", "Static" };
			save[2] = y + line_y;
			line_y += 30;

			char* tmp4[] = { "Standing", "In move", "All", "All + Fake" };
			save[5] = y + line_y;
			line_y += 30;

			char* tmp0[] = { "90", "180" };

			Slider(x + indent_x, y + line_y, -180, 180, g_vars.antiaim.yaw, "Static yaw");
			line_y += 30;

			Slider(x + indent_x, y + line_y, -180, 180, g_vars.antiaim.fake_yaw, "Fake yaw");
			line_y += 20;

			//ListBox(iListIndex++, x + indent_x, save[5], "Edge", g_vars.t.itest, tmp4, 4, true);

			//ListBox(iListIndex++, x + indent_x, save[10], "Legit AA", g_vars.t.itest, tmp0, 2, true);

			//ListBox(iListIndex++, x + indent_x, save[2], "Yaw while running", g_vars.t.itest, tmp3, 7, true);

			ListBox(iListIndex++, x + indent_x, save[1], "Yaw", g_vars.antiaim.yaw, tmp2, 7, true);

			ListBox(iListIndex++, x + indent_x, save[0], "Pitch", g_vars.antiaim.pitch, tmp1, 5, true);
		}
	}
	else if (iCurrentTab == 2) // Legit
	{
		int x = MenuX + 100;
		int y = MenuY + 30;
		int box_indent_x = 10;
		int box_indent_y = 15;

		int save[256];

		int line_y = 15;

		int weapon_id = 0;

		if (g_vars.legitbot.menu_legit_global_section == 1)
		{
			if (g_vars.legitbot.menu_legit_sub_section == 1)
				weapon_id = WEAPON_GLOCK;
			else if (g_vars.legitbot.menu_legit_sub_section == 2)
				weapon_id = WEAPON_P250;
			else if (g_vars.legitbot.menu_legit_sub_section == 3)
				weapon_id = WEAPON_DEAGLE;
			else if (g_vars.legitbot.menu_legit_sub_section == 4)
				weapon_id = WEAPON_ELITE;
			else if (g_vars.legitbot.menu_legit_sub_section == 5)
				weapon_id = WEAPON_FIVESEVEN;
			else if (g_vars.legitbot.menu_legit_sub_section == 6)
				weapon_id = WEAPON_USP_SILENCER;
		}
		else if (g_vars.legitbot.menu_legit_global_section == 2)
		{
			if (g_vars.legitbot.menu_legit_sub_section == 1)
				weapon_id = WEAPON_AWP;
			else if (g_vars.legitbot.menu_legit_sub_section == 2)
				weapon_id = WEAPON_SSG08;
			else if (g_vars.legitbot.menu_legit_sub_section == 3)
				weapon_id = WEAPON_G3SG1;
			else if (g_vars.legitbot.menu_legit_sub_section == 4)
				weapon_id = WEAPON_SG558;
		}
		else if (g_vars.legitbot.menu_legit_global_section == 3)
		{
			if (g_vars.legitbot.menu_legit_sub_section == 1)
				weapon_id = WEAPON_M4A1;
			else if (g_vars.legitbot.menu_legit_sub_section == 2)
				weapon_id = WEAPON_GALILAR;
			else if (g_vars.legitbot.menu_legit_sub_section == 3)
				weapon_id = WEAPON_FAMAS;
			else if (g_vars.legitbot.menu_legit_sub_section == 4)
				weapon_id = WEAPON_AUG;
			else if (g_vars.legitbot.menu_legit_sub_section == 5)
				weapon_id = WEAPON_AK47;
			else if (g_vars.legitbot.menu_legit_sub_section == 6)
				weapon_id = WEAPON_SG558;
		}
		else if (g_vars.legitbot.menu_legit_global_section == 4)
		{
			if (g_vars.legitbot.menu_legit_sub_section == 1)
				weapon_id = WEAPON_XM1014;
			else if (g_vars.legitbot.menu_legit_sub_section == 2)
				weapon_id = WEAPON_NOVA;
		}
		else if (g_vars.legitbot.menu_legit_global_section == 5)
		{
			if (g_vars.legitbot.menu_legit_sub_section == 1)
				weapon_id = WEAPON_M249;
		}
		else if (g_vars.legitbot.menu_legit_global_section == 6)
		{
			if (g_vars.legitbot.menu_legit_sub_section == 1)
				weapon_id = WEAPON_UMP45;
			else if (g_vars.legitbot.menu_legit_sub_section == 2)
				weapon_id = WEAPON_P90;
			else if (g_vars.legitbot.menu_legit_sub_section == 3)
				weapon_id = WEAPON_MP5SD;
			else if (g_vars.legitbot.menu_legit_sub_section == 4)
				weapon_id = WEAPON_MAC10;
			else if (g_vars.legitbot.menu_legit_sub_section == 5)
				weapon_id = WEAPON_UMP45;
		}

		if (weapon_id > 0)
		{
			y = y + 45 + 20;

			{//BOX2
				DrawBox(x, y, 250, 365); // Aimbot

				render::draw_filled_rect_old(x + 14, y - 1, x + 60, y + 2, color(0, 0, 0, 255));
				render::draw_string(x + 20, y, render::fonts::main, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3]), FONT_LEFT, "Aimbot");

				Checkbox(x + box_indent_x, y + line_y, g_vars.legit[weapon_id].aim, "Enabled");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 100, g_vars.legit[weapon_id].aim_speed, "Speed (auto)");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 100, g_vars.legit[weapon_id].aim_speed_in_attack, "Speed (in attack)");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 100, g_vars.legit[weapon_id].aim_speed_scale_fov, "Speed scale - FOV", true, "%");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 1000, g_vars.legit[weapon_id].aim_reaction_time, "Reaction time", true, "ms");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 180, g_vars.legit[weapon_id].aim_fov, "Maximum FOV", false, "°", true);
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 100, g_vars.legit[weapon_id].aim_recoil_compensation_pitch, "Recoil compensation (P/Y)", true, "%");
				line_y += 20;

				Slider(x + box_indent_x, y + line_y, 0, 100, g_vars.legit[weapon_id].aim_recoil_compensation_yaw, "", true, "%");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, g_vars.legit[weapon_id].aim_humanize, "Humanize");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, g_vars.legit[weapon_id].aim_quick_stop, "Quick stop");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, g_vars.legit[weapon_id].aim_head, "Head");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, g_vars.legit[weapon_id].aim_chest, "Chest");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, g_vars.legit[weapon_id].aim_stomach, "Stomach");
				line_y += 30;
			}

			x = x + 270;
			line_y = 15;

			{//BOX3

				DrawBox(x, y, 250, 175); // Triggerbot

				render::draw_filled_rect_old(x + 14, y - 1, x + 80, y + 2, color(1, 8, 8, 255));
				render::draw_string(x + 20, y, render::fonts::main, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3]), FONT_LEFT, "Triggerbot");

				Checkbox(x + box_indent_x, y + line_y, g_vars.legit[weapon_id].trigger, "Enabled");
				KeyBind(x + box_indent_x + 180, y + line_y - 10, g_vars.legitbot.trigger_key);
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, g_vars.legit[weapon_id].trigger_penetration, "Automatic penetration");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, g_vars.legit[weapon_id].trigger_head, "Head");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, g_vars.legit[weapon_id].trigger_chest, "Chest");
				line_y += 20;

				Checkbox(x + box_indent_x, y + line_y, g_vars.legit[weapon_id].trigger_stomach, "Stomach");
				line_y += 30;

				char* tmp[] = { "Recoil", "Recoil / Spread" };
				save[0] = y + line_y;

				line_y += 30;

				//Checkbox(x + box_indent_x, y + line_y, g_vars.t.btest, "Only zoomed");
				//line_y += 20;

				ListBox(iListIndex++, x + box_indent_x, save[0], "Accuracy boost", g_vars.legit[weapon_id].trigger_accuracy, tmp, 2, true);
			}

			y = y + 175 + 20;
			line_y = 15;

			{//BOX4

				DrawBox(x, y, 250, 170); // Other

				render::draw_filled_rect_old(x + 14, y - 1, x + 54, y + 2, color(1, 8, 8, 255));
				render::draw_string(x + 20, y, render::fonts::main, color(220, 220, 220), FONT_LEFT, "Other");

				Checkbox(x + box_indent_x, y + line_y, g_vars.legitbot.legit_teammates, "Teammates");
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 1000, g_vars.legitbot.block_attack_after_kill, "Block attack after kill", true, "ms");
				line_y += 30;

				char* tmp[] = { "Aiming", "Recoil", "Recoil / Spread" };
				save[0] = y + line_y;
				line_y += 40;

				Slider(x + box_indent_x, y + line_y, 0, 1, g_vars.legit[weapon_id].aim_psilent, "Perfect silent", false, "°", true);
				line_y += 30;

				Slider(x + box_indent_x, y + line_y, 0, 10, g_vars.legit[weapon_id].aim_recoil_compensation_after_shots_fired, "Recoil compensation after shots fired", true);
				line_y += 20;

				ListBox(iListIndex++, x + box_indent_x, save[0], "Accuracy boost", g_vars.legit[weapon_id].aim_accuracy, tmp, 3, true);
			}
		}

		x = MenuX + 100;
		y = MenuY + 30;
		line_y = 15;

		{//BOX1
			DrawBox(x, y, 520, 45); // Section

			render::draw_filled_rect_old(x + 14, y - 1, x + 60, y + 2, color(1, 8, 8, 255));
			render::draw_string(x + 20, y, render::fonts::main, color(220, 220, 220), FONT_LEFT, "Section");

			char* tmp[] = { "Pistol", "Sniper", "Rifle", "Shotgun", "Machine gun", "Submachine gun" };
			ListBox(iListIndex++, x + box_indent_x, y + line_y, "", g_vars.legitbot.menu_legit_global_section, tmp, 6, false);

			if (g_vars.legitbot.menu_legit_global_section == 1)
			{
				char* tmp2[] = { "GLOCK18", "P228", "DEAGLE", "ELITE", "FIVESEVEN", "USP" };
				ListBox(iListIndex++, x + box_indent_x + 270, y + line_y, "", g_vars.legitbot.menu_legit_sub_section, tmp2, 6, false);

				if (g_vars.legitbot.menu_legit_sub_section > 6)
					g_vars.legitbot.menu_legit_sub_section = 6;
			}
			else if (g_vars.legitbot.menu_legit_global_section == 2)
			{
				char* tmp2[] = { "AWP", "SCOUT", "G3SG1", "SG550", };
				ListBox(iListIndex++, x + box_indent_x + 270, y + line_y, "", g_vars.legitbot.menu_legit_sub_section, tmp2, 4, false);

				if (g_vars.legitbot.menu_legit_sub_section > 4)
					g_vars.legitbot.menu_legit_sub_section = 4;
			}
			else if (g_vars.legitbot.menu_legit_global_section == 3)
			{
				char* tmp2[] = { "M4A1", "GALIL", "FAMAS", "AUG", "AK47", "SG552" };
				ListBox(iListIndex++, x + box_indent_x + 270, y + line_y, "", g_vars.legitbot.menu_legit_sub_section, tmp2, 6, false);

				if (g_vars.legitbot.menu_legit_sub_section > 6)
					g_vars.legitbot.menu_legit_sub_section = 6;
			}
			else if (g_vars.legitbot.menu_legit_global_section == 4)
			{
				char* tmp2[] = { "XM1014", "M3" };
				ListBox(iListIndex++, x + box_indent_x + 270, y + line_y, "", g_vars.legitbot.menu_legit_sub_section, tmp2, 2, false);

				if (g_vars.legitbot.menu_legit_sub_section > 2)
					g_vars.legitbot.menu_legit_sub_section = 2;
			}
			else if (g_vars.legitbot.menu_legit_global_section == 5)
			{
				char* tmp2[] = { "M249" };
				ListBox(iListIndex++, x + box_indent_x + 270, y + line_y, "", g_vars.legitbot.menu_legit_sub_section, tmp2, 1, false);

				if (g_vars.legitbot.menu_legit_sub_section > 1)
					g_vars.legitbot.menu_legit_sub_section = 1;
			}
			else if (g_vars.legitbot.menu_legit_global_section == 6)
			{
				char* tmp2[] = { "TMP", "P90", "MP5N", "MAC10", "UMP45" };
				ListBox(iListIndex++, x + box_indent_x + 270, y + line_y, "", g_vars.legitbot.menu_legit_sub_section, tmp2, 5, false);

				if (g_vars.legitbot.menu_legit_sub_section > 5)
					g_vars.legitbot.menu_legit_sub_section = 5;
			}
		}
	}
	else if (iCurrentTab == 3) // Systems
	{
		int x = MenuX + 100;
		int y = MenuY + 30;
		int box_indent_x = 10;
		int box_indent_y = 15;

		int save[256];

		int line_y = 15;

		{//BOX1
			DrawBox(x, y, 250, 430); // Movement

			render::draw_filled_rect_old(x + 14, y - 1, x + 60, y + 2, color(0, 0, 0, 255));
			render::draw_string(x + 20, y, render::fonts::main, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3]), FONT_LEFT, "Movement");

			Checkbox(x + box_indent_x, y + line_y, g_vars.misc.bhop, "Auto jump");
			line_y += 15;

			//Checkbox(x + box_indent_x, y + line_y, g_vars.t.btest, "Jump duck");
			//line_y += 15;

			//Checkbox(x + box_indent_x, y + line_y, g_vars.t.btest, "Jump bug");
			//line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.misc.air_strafe, "Air strafe");
			line_y += 15;
		}

		{//BOX2
			x = x + 270;
			line_y = 15;

			DrawBox(x, y, 250, 430); // Other

			render::draw_filled_rect_old(x + 14, y - 1, x + 54, y + 2, color(1, 8, 8, 255));
			render::draw_string(x + 20, y, render::fonts::main, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3]), FONT_LEFT, "Other");

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.extra.speclist, "Spectator list");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.misc.vm_aimbot, "VM Aimbot");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.misc.autozeus, "Auto zeus");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.misc.log_damage, "Log damage");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.misc.log_purchases, "Log purchases");
			line_y += 15;

			//Checkbox(x + box_indent_x, y + line_y, g_vars.t.btest, "Knifebot");
			//line_y += 20;
		}

		{//BOX3
			y = y + 220;
			line_y = 15;

			DrawBox(x, y, 250, 210);

			render::draw_filled_rect_old(x + 14, y - 1, x + 58, y + 2, color(0, 0, 0, 255));
			render::draw_string(x + 20, y, render::fonts::main, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3]), FONT_LEFT, "Followbot");

			/*

			Checkbox(x + box_indent_x, y + line_y, g_vars.t.btest, "enabled");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.t.btest, "friends");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.t.btest, "AFK");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.t.btest, "corners");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.t.btest, "visualize");
			line_y += 30;

			Slider(x + box_indent_x, y + line_y, 80, 1000, g_vars.t.itest, "activation distance");
			line_y += 30;

			Slider(x + box_indent_x, y + line_y, 200, 15000, g_vars.t.itest, "AFK time (ms)");
			line_y += 30;

			Slider(x + box_indent_x, y + line_y, 0, 400, g_vars.t.itest, "distance");
			line_y += 30;
			*/
		}
	}
	else if (iCurrentTab == 4) // Visuals
	{
		int x = MenuX + 100;
		int y = MenuY + 30;
		int box_indent_x = 10;
		int box_indent_y = 15;

		int save[256];

		int line_y = 15;

		{//BOX1
			DrawBox(x, y, 250, 430); // Player ESP

			render::draw_filled_rect_old(x + 14, y - 1, x + 80, y + 2, color(0, 0, 0, 255));
			render::draw_string(x + 20, y, render::fonts::main, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3]), FONT_LEFT, "Player ESP");

			KeyBind(x + indent_x + 180, y + line_y - 10, g_vars.rage.key);

			line_y += 15;

			char* tmp10[] = { "Always", "On-key", "Toggle" };
			save[10] = y + line_y;
			line_y += 30;

			//Checkbox(x + box_indent_x, y + line_y, g_vars.t.btest, "Teammates");
			//line_y += 18;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.bbox, "Bounding box");
			save[0] = y + line_y;
			line_y += 30;

			char* tmp2[] = { "2D", "3D" };
			ListBox(iListIndex++, x + indent_x, y + line_y, "box type", g_vars.visuals.box_type, tmp2, 2, false);
			line_y += 30;

			//Checkbox(x + box_indent_x, y + line_y, cvar.esp_box_outline, "Bounding box outline");
			//line_y += 18;

			//Checkbox(x + box_indent_x, y + line_y, cvar.esp_behind, "Behind wall");
			//save[5] = y + line_y;
			//line_y += 18;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.healthbar, "Health bar");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.name, "Name");
			line_y += 15;

			//Checkbox(x + box_indent_x, y + line_y, cvar.esp_trace_angles, "Trace angles");
			//line_y += 30;

			//char *tmp3[] = { "Text", "Icon" };
			//ListBox(iListIndex++, x + box_indent_x, y + line_y, "Weapon", cvar.chams_type, tmp3, 2, true);
			//save[6] = y + line_y;
			//line_y += 30;

			//Checkbox(x + box_indent_x, y + line_y, cvar.esp_distance, "Distance");
			//line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.flags, "Flags");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.chicken, "Chicken nuggets");
			line_y += 15;

			//Checkbox(x + box_indent_x, y + line_y, cvar.esp_line_of_sight, "Line of sight");
			//save[1] = y + line_y;
			//line_y += 15;

			//Checkbox(x + box_indent_x, y + line_y, cvar.esp_sound, "Visualize sounds");
			//line_y += 15;

			//Checkbox(x + box_indent_x, y + line_y, cvar.esp_shots_fired, "Shots fired");
			//line_y += 15;

			//Checkbox(x + box_indent_x, y + line_y, g_vars.misc.client_hitboxes, "Hitboxes");
			//line_y += 15;

			//Checkbox(x + box_indent_x, y + line_y, cvar.esp_screen, "Screen");
			//line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.glow, "Glow");
			save[2] = y + line_y;
			line_y += 30;

			//char* tmp4[] = { "STUDIO_HAS_NORMALS", "STUDIO_HAS_VERTICES", "STUDIO_HAS_BBOX", "STUDIO_HAS_CHROME", "STUDIO_NF_FLATSHADE", "STUDIO_NF_CHROME" };
			//ListBox(iListIndex++, x + indent_x, y + line_y, "Glow type", cvar.glow_type, tmp4, 6, false);
			//line_y += 30;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.chams.enabled, "Chams");
			save[3] = y + line_y;
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.chams.teammates, "Chams teammates");
			save[4] = y + line_y;
			line_y += 15;

			char* tmp[] = { "Vertex lit", "Unlit", "Modulate" };
			ListBox(iListIndex++, x + box_indent_x, y + line_y, "", g_vars.visuals.chams.type, tmp, 3);

			ListBox(iListIndex++, x + indent_x, save[10], "Activation", g_vars.visuals.activation_type, tmp10, 3, true);

			line_y += 30;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.dropped_ammo, "Dropped ammo");
			save[14] = y + line_y;
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.dropped_weapons, "Dropped weapons");
			line_y += 15;

			//char* tmp3[] = { "Text", "Icon" };
			//ListBox(iListIndex++, x + box_indent_x, save[6], "Weapon", cvar.esp_weapon, tmp3, 2, true);
		}

		{//BOX2
			x = x + 270;
			line_y = 15;

			DrawBox(x, y, 250, 200); // Other ESP

			render::draw_filled_rect_old(x + 14, y - 1, x + 76, y + 2, color(0, 0, 0, 255));
			render::draw_string(x + 20, y, render::fonts::main, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3]), FONT_LEFT, "Other ESP");

			line_y += 15;

			//Checkbox(x + box_indent_x, y + line_y, cvar.crosshair, "Crosshair");
			//Palette(x + 227, y + line_y, cvar.crosshair_r, cvar.crosshair_g, cvar.crosshair_b);
			//line_y += 15;

			//Checkbox(x + box_indent_x, y + line_y, cvar.recoil_overlay, "Recoil overlay");
			//Palette(x + 227, y + line_y, cvar.recoil_overlay_r, cvar.recoil_overlay_g, cvar.recoil_overlay_b);
			//line_y += 15;

			char* tmp11[] = { "None", "Circle", "Dots" };
			save[11] = y + line_y;
			line_y += 30;

			ListBox(iListIndex++, x + indent_x, save[11], "Spread overlay", g_vars.visuals.visualize_spread, tmp11, 3, true);
			Palette(x + 227, y + line_y, g_vars.visuals.visualize_spread_color[1], g_vars.visuals.visualize_spread_color[2], g_vars.visuals.visualize_spread_color[3]);
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.grenade_pred, "Grenade trajectory");
			line_y += 15;

			//Checkbox(x + box_indent_x, y + line_y, cvar.penetration_info, "Penetration info");
			//line_y += 30;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.misc.no_flash, "Remove flashbang effects");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.misc.remove_scope_zoom, "Remove scope zoom");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.misc.fog, "Remove fog");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.visuals.hitmarker, "Hitmarker");
			line_y += 15;
		}

		{//BOX3
			y = y + 220;
			line_y = 15;

			DrawBox(x, y, 250, 210); // Effects

			render::draw_filled_rect_old(x + 14, y - 1, x + 58, y + 2, color(0, 0, 0, 255));
			render::draw_string(x + 20, y, render::fonts::main, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3]), FONT_LEFT, "Effects");
			
			line_y += 15;

			char* tmp12[] = { "None", "Remove", "Wireframe" };
			save[12] = y + line_y;
			line_y += 40;

			char* tmp13[] = { "None", "Static", "Dynamic" };
			save[13] = y + line_y;
			line_y += 40;

			ListBox(iListIndex++, x + indent_x, save[12], "Remove smoke", g_vars.visuals.misc.remove_smoke, tmp12, 3, true);
			ListBox(iListIndex++, x + indent_x, save[13], "Remove scope", g_vars.visuals.misc.remove_scope, tmp13, 3, true);

			line_y += 10;

			Slider(x + box_indent_x, y + line_y, 0, 255, g_vars.misc.prop_transparency, "Prop transparency", true);
			line_y += 20;

			//Checkbox(x + box_indent_x, y + line_y, g_vars.misc.disable_render_teammates, "Disable rendering of teammates");
			//line_y += 18;

			//Checkbox(x + box_indent_x, y + line_y, g_vars.misc.bullets_trace, "Bullets trace");
			//line_y += 18;
		}

		{//draw no bug
			x = MenuX + 100;
			y = MenuY + 30;

			Palette(x + 227, save[4], g_vars.visuals.chams.vis_color[1], g_vars.visuals.chams.vis_color[2], g_vars.visuals.chams.vis_color[3]);
			Palette(x + 207, save[4], g_vars.visuals.chams.hid_color[1], g_vars.visuals.chams.hid_color[2], g_vars.visuals.chams.hid_color[3]);

			Palette(x + 227, save[0], g_vars.visuals.box_color[1], g_vars.visuals.box_color[2], g_vars.visuals.box_color[3]);

			Palette(x + 227, save[14], g_vars.visuals.dropped_ammo_color[1], g_vars.visuals.dropped_ammo_color[2], g_vars.visuals.dropped_ammo_color[3]);

			Palette(x + 227, save[6] - 15, g_vars.visuals.weapon_color[1], g_vars.visuals.weapon_color[2], g_vars.visuals.weapon_color[3]);
		}
	}
	else if (iCurrentTab == 5)
	{
		int x = MenuX + 100;
		int y = MenuY + 30;
		int box_indent_x = 10;
		int box_indent_y = 15;

		int save[256];

		int line_y = 15;

		{//BOX1
			DrawBox(x, y, 250, 430); // Miscellaneous

			render::draw_filled_rect_old(x + 14, y - 1, x + 94, y + 2, color(0, 0, 0, 255));
			render::draw_string(x + 11, y, render::fonts::main, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3]), FONT_LEFT, "Miscellaneous");

			//Checkbox(x + box_indent_x, y + line_y, g_vars.misc.hud_clear, "HUD clear");
			//line_y += 18;

			//Checkbox(x + box_indent_x, y + line_y, g_vars.misc.autopistol, "Automatic weapons");
			//line_y += 18;

			//Checkbox(x + box_indent_x, y + line_y, g_vars.misc.autoreload, "Automatic reload");
			//line_y += 18;

			//Checkbox(x + box_indent_x, y + line_y, g_vars.misc.fastzoom, "Fast zoom");
			//line_y += 18;

			Checkbox(x + box_indent_x, y + line_y, g_vars.misc.fast_duck, "Fast duck");
			line_y += 15;

			//Slider(x + box_indent_x, y + line_y, 0, 10000, g_vars.misc.name_stealer, "Name stealer", true, "ms");
			//line_y += 30;
		}

		{//BOX2
			x = x + 270;
			line_y = 15;

			DrawBox(x, y, 250, 160); // Presets

			render::draw_filled_rect_old(x + 14, y - 1, x + 64, y + 2, color(1, 8, 8, 255));
			render::draw_string(x + 20, y, render::fonts::main, color(220, 220, 220), FONT_LEFT, "Presets");

			if (DrawButton(x + box_indent_x, y + box_indent_y, "Load"))
			{
				//func.LoadCvars();
			}

			line_y += 24;

			if (DrawButton(x + box_indent_x, y + line_y, "Save"))
			{
				//func.SaveCvars();
			}

			line_y += 28;

			render::draw_string(x + box_indent_x, y + line_y, render::fonts::main, color(180, 180, 180), "Menu color");
			save[0] = y + line_y;
			line_y += 14;

			render::draw_string(x + box_indent_x, y + line_y, render::fonts::main, color(180, 180, 180), "UI color");
			save[158] = y + line_y;
			line_y += 30;

			render::draw_string(x + box_indent_x, y + line_y, render::fonts::main, color(180, 180, 180), "Menu key");
			KeyBind(x + box_indent_x + 180, y + line_y - 10, g_vars.menu.menu_key);

			Palette(x + 227, save[0], g_vars.menu.menu_color[1], g_vars.menu.menu_color[2], g_vars.menu.menu_color[3]);
			Palette(x + 227, save[158], g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3]);
		}

		{//BOX3
			y = y + 180;
			line_y = 15;

			DrawBox(x, y, 250, 250); // Other

			render::draw_filled_rect_old(x + 14, y - 1, x + 54, y + 2, color(1, 8, 8, 255));
			render::draw_string(x + 20, y, render::fonts::main, color(220, 220, 220), FONT_LEFT, "Other");

			Checkbox(x + box_indent_x, y + line_y, g_vars.misc.debug, "Debug");
			line_y += 15;

			Checkbox(x + box_indent_x, y + line_y, g_vars.misc.hide_from_obs, "Hide from OBS");
			line_y += 20;

			if (DrawButton(x + box_indent_x, y + line_y, "Patch interp cvars"))
			{
				//g_Offsets.PatchInterpolation();
			}

			line_y += 22;

			if (DrawButton(x + box_indent_x, y + line_y, "Disable lag compensation (server)"))
			{
				//g_Engine.PlayerInfo_SetValueForKey("cl_lc", "0");
			}

			line_y += 22;
		}
	}
}

bool c_menu::DrawButton(int x, int y, char* text)
{
	unsigned int w = 220;
	unsigned int h = 16;

	render::draw_outline_old(x - 2, y - 2, x + w + 2, y + h + 2, color(20, 20, 20, 255));

	bool clicked = false;

	static DWORD dwTemporaryBlockTimer = 0;

	if (GetTickCount() - dwPaletteBlockedTime > 200 && GetTickCount() - dwListBlockedTime > 200 && !bCursorInPalette && !bCursorInList && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		if (GetTickCount() - dwTemporaryBlockTimer > 200)
		{
			clicked = true;
			dwTemporaryBlockTimer = GetTickCount();
		}
	}

	if (clicked || CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		render::draw_filled_rect_old(x, y, x + w, y + h, color(30, 30, 30, 255));
	}
	else
	{
		render::draw_filled_rect_old(x, y, x + w, y + h, color(10, 10, 10, 255));
	}

	if (text)
		render::draw_string(x + w / 2, y + (h / 2), render::fonts::main, color(200, 200, 200, 255), FONT_CENTER, text);

	return clicked;
}

void c_menu::Palette(int x, int y, float& r, float& g, float& b)
{
	unsigned int w = 16;
	unsigned int h = 8;

	static DWORD dwTemporaryBlockTimer = 0;
	// Close others and open this
	if (GetTickCount() - dwListBlockedTime > 200 && GetTickCount() - dwPaletteBlockedTime > 200 && !bCursorInList && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		if (GetTickCount() - dwTemporaryBlockTimer > 200)
		{
			bPaletteOpened[iPaletteIndex] = !bPaletteOpened[iPaletteIndex];

			dwTemporaryBlockTimer = GetTickCount();

			for (unsigned int i = 0; i < LIMIT_PALETTE; i++)
			{
				if (i == iPaletteIndex) continue;

				if (bPaletteOpened[i])
					bPaletteOpened[i] = false;
			}
		}
	}

	render::draw_filled_rect_old(x, y, x + w, y + h, color(r, g, b, 230));
	render::draw_filled_rect_old(x - 1, y - 1, x + w + 1, y + h + 1, color(30, 30, 30, 230));

	// shadow
	render::draw_outline_old(x - 2, y - 2, x + w + 2, y + h + 2, color(0, 0, 0, 230));

	static unsigned int panel_w = 230;
	static unsigned int panel_h = 54;
	static unsigned int indent = 8;
	static unsigned int border = 4;

	if (bPaletteOpened[iPaletteIndex])
	{
		render::draw_filled_rect_old(x + w + indent - border, y - indent, x + w + panel_w + indent + border, y + panel_h + indent, color(1, 1, 1, 255));

		render::draw_outline_old(x + w + indent - border - 1, y - indent - 1, x + w + panel_w + indent + border + 1, y + panel_h + indent + 1, color(60, 60, 60, 255));

		render::draw_outline_old(x + w + indent - border - 2, y - indent - 2, x + w + panel_w + indent + border + 2, y + panel_h + indent + 2, color(0, 0, 0, 255)); // shadow

		SliderInPallete(x + w + indent, y, 0, 255, r, "", true);
		SliderInPallete(x + w + indent, y + 20, 0, 255, g, "", true);
		SliderInPallete(x + w + indent, y + 40, 0, 255, b, "", true);
	}

	if (bPaletteOpened[iPaletteIndex] && !IsDragging && CursorX >= x + w + indent - border && CursorX <= x + w + panel_w + indent + border && CursorY >= y - indent && CursorY <= y + panel_h + indent)
	{
		bCursorInPalette = true;
		dwCheckboxBlockedTime = GetTickCount();
		dwPaletteBlockedTime = GetTickCount();
		dwSliderBlockedTime = GetTickCount();
	}

	iPaletteIndex++;
}

void c_menu::ListBox(int index, int x, int y, char* name, float& value, char** text, int size, bool default)
{
	unsigned int w = 230;
	unsigned int h = 18;

	if (name)
		render::draw_string( x + 1, y - 10, render::fonts::main, color(180, 180, 180, 255), name);

	static DWORD dwTemporaryBlockTimer = 0;
	// Close others and open this
	if (!bListBoxOpened[index] && GetTickCount() - dwPaletteBlockedTime > 200 && GetTickCount() - dwListBlockedTime > 200 && !bCursorInPalette && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		if (GetTickCount() - dwTemporaryBlockTimer > 200)
		{
			bListBoxOpened[index] = !bListBoxOpened[index];

			dwTemporaryBlockTimer = GetTickCount();

			for (unsigned int i = 0; i < LIMIT_LISTBOX; i++)
			{
				if (i == index) continue;

				if (bListBoxOpened[i])
				{
					bListBoxOpened[i] = false;
				}
			}
		}
	}

	bool effect_cursor = false;
	int effect_cursor_x0, effect_cursor_y0, effect_cursor_x1, effect_cursor_y1;
	int tmp = h;

	if (!bListBoxOpened[index])
	{
		render::draw_filled_rect_old(x, y, x + w, y + tmp, color(1, 1, 1, 255));
		render::draw_outline_old(x - 1, y - 1, x + w + 1, y + tmp + 1, color(30, 30, 30, 255));
		render::draw_outline_old(x - 2, y - 2, x + w + 2, y + tmp + 2, color(0, 0, 0, 255)); // shadow

		bool m_bDrawDefault = true;

		for (unsigned int i = 0; i < size; i++)
		{
			if (text[i] && value == i + 1)
			{
				m_bDrawDefault = false;
				render::draw_string( x + 4, y + (h / 2), render::fonts::main, color(220, 220, 220, 255), text[i]);
			}
		}

		if (m_bDrawDefault && default)
			render::draw_string( x + 4, y + (h / 2), render::fonts::main, color(220, 220, 220, 255), "None");
	}
	else
	{
		for (unsigned int i = 0; i < size; i++)
		{
			if (text[i])
			{
				if (CursorX >= x && CursorX <= x + w && CursorY >= y + tmp - h && CursorY <= y + tmp)
				{
					effect_cursor_x0 = x;
					effect_cursor_x1 = x + w;
					effect_cursor_y0 = y + tmp - h;
					effect_cursor_y1 = y + tmp;
					effect_cursor = true;
				}

				if (GetTickCount() - dwTemporaryBlockTimer > 200 && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y + tmp - h && CursorY <= y + tmp)
				{
					value = i + 1;
					bListBoxOpened[index] = false;
					dwTemporaryBlockTimer = GetTickCount();
				}

				tmp += h;
			}
		}

		if (default)
		{
			if (GetTickCount() - dwTemporaryBlockTimer > 200 && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + tmp)
			{
				value = 0;
				bListBoxOpened[index] = false;
				dwTemporaryBlockTimer = GetTickCount();
			}
		}
		else
			tmp -= h;

		render::draw_filled_rect_old(x, y, x + w, y + tmp, color(1, 1, 1, 255));
		render::draw_outline_old(x - 1, y - 1, x + w + 1, y + tmp + 1, color(30, 30, 30, 255));
		render::draw_outline_old(x - 2, y - 2, x + w + 2, y + tmp + 2, color(0, 0, 0, 255)); // shadow
	}

	if (effect_cursor)
	{
		render::draw_filled_rect_old(effect_cursor_x0, effect_cursor_y0, effect_cursor_x1, effect_cursor_y1, color(30, 30, 30, 255));
	}

	if (bListBoxOpened[index])
	{
		tmp = h;

		for (unsigned int i = 0; i < size; i++)
		{
			if (text[i])
			{
				render::draw_string( x + 4, y + tmp - (h / 2), render::fonts::main, color(220, 220, 220, 255), text[i]);
				tmp += h;
			}
		}

		if (default)
			render::draw_string( x + 4, y + tmp - (h / 2), render::fonts::main, color(220, 220, 220, 255), "None");
		else
			tmp -= h;
	}

	if (bListBoxOpened[index] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + tmp)
	{
		bCursorInList = true;
		dwCheckboxBlockedTime = GetTickCount();
		dwListBlockedTime = GetTickCount();
		dwSliderBlockedTime = GetTickCount();
	}
}

void c_menu::SliderInPallete(int x, int y, float min, float max, float& value, char* text, bool ThisINT, char* amout, bool extra)
{
	unsigned int w = 230;
	unsigned int h = 12;

	render::draw_filled_rect_old(x, y, x + w, y + h, color(1, 1, 1, 100));
	render::draw_outline_old(x - 1, y - 1, x + w + 1, y + h + 1, color(30, 30, 30, 100));
	render::draw_outline_old(x - 2, y - 2, x + w + 2, y + h + 2, color(0, 0, 0, 200)); // shadow

	if (text)
		render::draw_string( x + 1, y - 10, render::fonts::main, color(180, 180, 180, 255), text);

	if (ThisINT)
		value = (int)value;

	if (value < min)
		value = min;
	else if (value > max)
		value = max;

	float one = (w / max);

	int fill = one * value;

	if (value < 0)
		fill *= -1;

	render::draw_filled_rect_old(x, y, x + fill, y + h, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3], g_vars.menu.cheat_color[0]));

	if (!amout)
		amout = "";

	if (!ThisINT)
	{
		if (!extra)
			render::draw_string( x + (w / 2), y + (h / 2), render::fonts::main, color(255, 255, 255, 255), FONT_CENTER, "%.2f%s", value, amout);
		else
			render::draw_string( x + (w / 2), y + (h / 2), render::fonts::main, color(255, 255, 255, 255), FONT_CENTER, "%.2f%s", value, amout);
	}
	else
	{
		if (!extra)
			render::draw_string( x + (w / 2), y + (h / 2), render::fonts::main, color(255, 255, 255, 255), FONT_CENTER, "%.f%s", value, amout);
		else
			render::draw_string( x + (w / 2), y + (h / 2), render::fonts::main, color(255, 255, 255, 255), FONT_CENTER, "%.f%s", value, amout);
	}

	if (GetTickCount() - dwListBlockedTime > 200 && !bCursorInPalette && !bCursorInList && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		if (keys[VK_LBUTTON])
		{
			value = (CursorX - x) / one;

			if (value < min)
				value = min;
			else if (value > max)
				value = max;
		}
		else if (keys[VK_RBUTTON] && min < 0)
		{
			value = (CursorX - x) / one;
			value *= -1;

			if (value < min)
				value = min;
			else if (value > max)
				value = max;
		}
	}
}

void c_menu::KeyBind(int x, int y, int &key)
{
	unsigned int w = 60;
	unsigned int h = 18;

	bool clicked = false;

	static DWORD dwTemporaryBlockTimer = 0;

	if (GetTickCount() - dwPaletteBlockedTime > 200 && GetTickCount() - dwListBlockedTime > 200 && !bCursorInPalette && !bCursorInList && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		if (GetTickCount() - dwTemporaryBlockTimer > 200)
		{
			clicked = true;
			dwTemporaryBlockTimer = GetTickCount();
		}
	}

	if (key == -2 && GetTickCount() - dwTemporaryBlockTimer > 200)
	{
		for (unsigned int i = 0; i < 255; i++)
		{
			if (g_Menu.keys[i])
			{
				if (i == VK_ESCAPE || i == VK_LBUTTON || i == VK_RBUTTON || i == g_vars.menu.menu_key)
				{
					key = -1;
					break;
				}
				key = i;
			}
		}
	}

	if (clicked)
	{
		if (key == -1)
		{
			key = -2;
		}
		else
		{
			key = -1;
		}
	}

	if (key == -1)
	{
		render::draw_string( x + w / 2, y + (h / 2), render::fonts::main, color(200, 200, 200, 255), FONT_CENTER, "[not set]");
	}
	else if (key == -2)
	{
		render::draw_string( x + w / 2, y + (h / 2), render::fonts::main, color(220, 220, 220, 255), FONT_CENTER, "[Press key]");
	}
	else
	{
		//render::draw_string( x + w / 2, y + (h / 2), render::fonts::main, color(255, 255, 255, 255), FONT_CENTER, "[%s]", VirtualKeyCodeToString(key).c_str());
	}
}

void c_menu::Slider(int x, int y, float min, float max, float& value, char *text, bool ThisINT, char *amout, bool extra)
{
	unsigned int w = 230;
	unsigned int h = 12;

	render::draw_filled_rect_old(x, y, x + w, y + h, color(30, 30, 30, 255));

	render::draw_outline_old(x - 1, y - 1, x + w + 1, y + h + 1, color(0, 0, 0, 255));

	//shadow
	render::draw_outline_old(x - 2, y - 2, x + w + 2, y + h + 2, color(0, 0, 0, 255));

	if (text)
		render::draw_string( x + 1, y - 10, render::fonts::main, color(60, 60, 60, 255), FONT_LEFT, text);

	if (ThisINT)
		value = (int)value;

	if (value < min)
		value = min;
	else if (value > max)
		value = max;

	float one = (w / max);

	int fill = one * value;

	if (value < 0)
		fill *= -1;

	render::draw_filled_rect_old(x, y, x + fill, y + h, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3], g_vars.menu.cheat_color[0]));

	if (!amout)
		amout = "";

	if (!ThisINT)
	{
		if (!extra)
			render::draw_string( x + (w / 2), y + (h / 2), render::fonts::main, color(255, 255, 255, 255), FONT_CENTER, "%.2f%s", value, amout);
		else
			render::draw_string_ACP( x + (w / 2), y + (h / 2), render::fonts::main, color(255, 255, 255, 255), FONT_CENTER, "%.2f%s", value, amout);
	}
	else
	{
		if (!extra)
			render::draw_string( x + (w / 2), y + (h / 2), render::fonts::main, color(255, 255, 255, 255), FONT_CENTER, "%.f%s", value, amout);
		else
			render::draw_string_ACP( x + (w / 2), y + (h / 2), render::fonts::main, color(255, 255, 255, 255), FONT_CENTER, "%.f%s", value, amout);
	}

	if (GetTickCount() - dwSliderBlockedTime > 200 && GetTickCount() - dwListBlockedTime > 200 && !bCursorInPalette && !bCursorInList && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		if (keys[VK_LBUTTON])
		{
			value = (CursorX - x) / one;

			if (value < min)
				value = min;
			else if (value > max)
				value = max;
		}
		else if (keys[VK_RBUTTON] && min < 0)
		{
			value = (CursorX - x) / one;
			value *= -1;

			if (value < min)
				value = min;
			else if (value > max)
				value = max;
		}
	}
}

void c_menu::DrawMenuTabs()
{
	int tab_x = BORDER_SIZE + 1;
	int tab_y = BORDER_SIZE + 1 + 10;
	int tab_h = ((background.height - (BORDER_SIZE * 2)) / MAX_TABS) - 20;
	int tab_w = tab_x + TAB_WEIGHT;

	int y = BORDER_SIZE + 1 + 15;
	int x = BORDER_SIZE + 1 + 12;

	for (unsigned int i = 1; i <= MAX_TABS; i++)
	{
		if (i != iCurrentTab)
		{
			if (i == 1)
				g_csgo.m_surface->draw_texture(icon_headshot_not_selected.index, MenuX + x, MenuY + y, MenuX + x + icon_headshot.width, MenuY + y + icon_headshot.width, 100, 100, 100, 100);
			else if (i == 2)
				g_csgo.m_surface->draw_texture(icon_aimbot_not_selected.index, MenuX + x, MenuY + y, MenuX + x + icon_aimbot.width, MenuY + y + icon_aimbot.height, 100, 100, 100, 100);
			else if (i == 3)
				g_csgo.m_surface->draw_texture(icon_systems_not_selected.index, MenuX + x, MenuY + y, MenuX + x + icon_systems.width, MenuY + y + icon_systems.height, 100, 100, 100, 100);
			else if (i == 4)
				g_csgo.m_surface->draw_texture(icon_visuals_not_selected.index, MenuX + x, MenuY + y, MenuX + x + icon_visuals.width, MenuY + y + icon_visuals.height, 100, 100, 100, 100);
			else if (i == 5)
				g_csgo.m_surface->draw_texture(icon_options_not_selected.index, MenuX + x, MenuY + y, MenuX + x + icon_options.width, MenuY + y + icon_options.height, 100, 100, 100, 100);
			else if (i == 6)
				g_csgo.m_surface->draw_texture(icon_members_not_selected.index, MenuX + x, MenuY + y, MenuX + x + icon_members.width, MenuY + y + icon_members.height, 100, 100, 100, 100);
		}
		else
		{
			
			render::draw_filled_rect_old(MenuX + tab_x, MenuY + tab_y, MenuX + tab_w, MenuY + tab_y + tab_h, color(30, 30, 30, 200));
			render::draw_filled_rect_old(MenuX + tab_x, MenuY + tab_y - 1, MenuX + tab_w, MenuY + tab_y, color(100, 100, 100, 200));
			render::draw_filled_rect_old(MenuX + tab_x, MenuY + tab_y - 1, MenuX + tab_w - 1, MenuY + tab_y - 2, color(0, 0, 0, 255));
			render::draw_filled_rect_old(MenuX + tab_x, MenuY + tab_y + tab_h - 1, MenuX + tab_w, MenuY + tab_y + tab_h, color(100, 100, 100, 200));
			render::draw_filled_rect_old(MenuX + tab_x, MenuY + tab_y + tab_h + 1, MenuX + tab_w - 1, MenuY + tab_y + tab_h + 2, color(0, 0, 0, 255));

			if (iCurrentTab == 1)
				g_csgo.m_surface->draw_texture(icon_headshot.index, MenuX + x, MenuY + y, MenuX + x + icon_headshot.width, MenuY + y + icon_headshot.width, 100, 100, 100, 100);
			else if (iCurrentTab == 2)
				g_csgo.m_surface->draw_texture(icon_aimbot.index, MenuX + x, MenuY + y, MenuX + x + icon_aimbot.width, MenuY + y + icon_aimbot.height, 100, 100, 100, 100);
			else if (iCurrentTab == 3)
				g_csgo.m_surface->draw_texture(icon_systems.index, MenuX + x, MenuY + y, MenuX + x + icon_systems.width, MenuY + y + icon_systems.height, 100, 100, 100, 100);
			else if (iCurrentTab == 4)
				g_csgo.m_surface->draw_texture(icon_visuals.index, MenuX + x, MenuY + y, MenuX + x + icon_visuals.width, MenuY + y + icon_visuals.height, 100, 100, 100, 100);
			else if (iCurrentTab == 5)
				g_csgo.m_surface->draw_texture(icon_options.index, MenuX + x, MenuY + y, MenuX + x + icon_options.width, MenuY + y + icon_options.height, 100, 100, 100, 100);
			else if (iCurrentTab == 6)
				g_csgo.m_surface->draw_texture(icon_members.index, MenuX + x, MenuY + y, MenuX + x + icon_members.width, MenuY + y + icon_members.height, 100, 100, 100, 100);
		}

		y = y + tab_h + 20;
		tab_y = tab_y + tab_h + 20;
	}
}

void c_menu::Drag()
{
	if (bCursorInList || bCursorInPalette)
	{
		IsDragging = false;
		return;
	}

	static int drag_x = 0;
	static int drag_y = 0;

	if (IsDragging && !keys[VK_LBUTTON])
	{
		IsDragging = false;
	}
	else if (IsDragging && keys[VK_LBUTTON])
	{
		MenuX = CursorX - drag_x;
		MenuY = CursorY - drag_y;
	}

	//head move
	if (keys[VK_LBUTTON] && (
		(CursorX >= MenuX && CursorX <= background.width + MenuX && CursorY >= MenuY && CursorY <= MenuY + BORDER_SIZE) ||
		(CursorX >= MenuX && CursorX <= MenuX + BORDER_SIZE && CursorY >= MenuY && CursorY <= background.height + MenuY) ||
		(CursorX >= MenuX + background.width - BORDER_SIZE && CursorX <= MenuX + background.width && CursorY >= MenuY && CursorY <= background.height + MenuY) ||
		(CursorX >= MenuX && CursorX <= background.width + MenuX && CursorY >= MenuY + background.height - BORDER_SIZE && CursorY <= MenuY + background.height)))
	{
		drag_x = CursorX - MenuX;
		drag_y = CursorY - MenuY;
		IsDragging = true;
	}
}

void c_menu::SelectTab()
{
	int x = BORDER_SIZE + 1;
	int y = BORDER_SIZE + 1;
	int h = (background.height - (BORDER_SIZE * 2)) / MAX_TABS;
	int w = x + TAB_WEIGHT - 1;

	if (keys[VK_LBUTTON] && !IsDragging)
	{
		for (unsigned int i = 1; i <= MAX_TABS; i++)
		{
			if (CursorX >= MenuX + x && CursorX <= MenuX + w && CursorY >= MenuY + y && CursorY <= MenuY + y + h)
			{
				iCurrentTab = i;
				return;
			}
			y = y + h;
		}
	}
}

void c_menu::Checkbox(int x, int y, bool& value, char* text)
{
	static unsigned int w = 10;
	static unsigned int h = 10;

	render::draw_outline_old(x, y, x + w, y + h, color(30, 30, 30, 255));

	render::draw_outline_old(x - 1, y - 1, x + w + 1, y + h + 1, color(0, 0, 0, 200));

	if (value)
	{
		render::draw_filled_rect_old(x + 1, y + 1, x + w - 1, y + h - 1, color(g_vars.menu.cheat_color[1], g_vars.menu.cheat_color[2], g_vars.menu.cheat_color[3], g_vars.menu.cheat_color[0]));
	}

	if (text)
		render::draw_string(x + 16, y + 4, render::fonts::main, color(200, 200, 200, 255), text);

	if (GetTickCount() - dwCheckboxBlockedTime > 200 && !bCursorInPalette && !bCursorInList && keys[VK_LBUTTON] && !IsDragging && CursorX >= x && CursorX <= x + w && CursorY >= y && CursorY <= y + h)
	{
		value = !value;
		dwCheckboxBlockedTime = GetTickCount();
	}
}

void c_menu::DrawBox(int x, int y, int w, int h)
{
	//box
	render::draw_filled_rect_old(x, y, x + w, y + h, color(1, 1, 1, 255));

	//outline
	render::draw_outline_old(x - 1, y - 1, x + w + 1, y + h + 1, color(0, 0, 0, 255));

	//shadow
	render::draw_outline_old(x - 2, y - 2, x + w + 2, y + h + 2, color(6, 6, 6, 255));
}
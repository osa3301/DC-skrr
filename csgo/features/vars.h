#pragma once

class c_vars {
public:
	struct rage {
		int key;
		bool enabled;
		float target_selection;
		float pointscale = 60.f;
		bool prefer_bodyaim;
		bool save_fps;
		bool selectively_multipoint_records;
		bool bodyaim_lethal;
		float hitchance;
		float min_dmg;
		bool auto_fire;
		bool autowall;
		bool silent;
		bool autostop;
		float autoscope;
		bool head;
		bool pelvis;
		bool legs;
		bool arms;
		bool teammate;
		bool hitbox_head;
		bool hitbox_pelvis;
		bool hitbox_legs;
		bool hitbox_arms;
		float primary_hitbox;
		bool dynamic_hitbox;
		bool resolver_enabled;
	} rage;

	struct legitbot {
		float menu_legit_global_section;
		float menu_legit_sub_section;
		bool legit_teammates;
		bool trigger_only_zoomed;
		float block_attack_after_kill;
		int trigger_key;
	} legitbot;

	struct legit_weapons
	{
		bool aim;
		bool aim_head;
		bool aim_chest;
		bool aim_stomach;
		bool aim_quick_stop;
		bool aim_humanize;
		bool aim_spread_compenstation;
		float aim_accuracy;
		float aim_psilent;
		float aim_fov;
		float aim_speed;
		float aim_speed_in_attack;
		float aim_speed_scale_fov;
		float aim_recoil_compensation_pitch;
		float aim_recoil_compensation_yaw;
		float aim_recoil_compensation_after_shots_fired;
		float aim_reaction_time;

		bool trigger;
		bool trigger_head;
		bool trigger_chest;
		bool trigger_stomach;
		bool trigger_penetration;
		float trigger_accuracy;
	} legit[31];

	struct visuals {
		float activation_type;
		int activation_key;
		bool bbox;
		float box_type;
		float box_color[ 4 ] = { 220, 255, 0, 0 };
		bool name;
		float name_color[ 4 ] = { 220, 255, 255, 255 };
		bool weapon;
		float weapon_color[ 4 ] = { 220, 255, 255, 255 };
		bool healthbar;
		bool ammo_bar;
		float ammo_bar_color[ 4 ] = { 220, 79, 130, 255 };
		bool money;
		float money_color[ 4 ] = { 200, 149, 184, 6 };
		bool flags;
		float flag_color[ 4 ] = { 200, 255, 255, 255 };
		bool flash_bar;
		float flash_bar_color[ 4 ] = { 220, 255, 255, 255 };
		bool glow;
		float glow_type;
		float glow_color[4] = { 155, 206, 115, 136 };
		float filter;
		bool dropped_weapons;
		bool dropped_ammo;
		float dropped_ammo_color[ 4 ] = { 220, 79, 130, 255 };
		bool chicken;
		bool projectiles;
		bool objectives;
		bool dormancy_fade;
		bool grenade_pred;
		float visualize_spread;
		bool hitmarker;
		bool impact;
		float impact_color[ 4 ] = { 255, 255, 0, 0 };
		bool radar;
		float visualize_spread_color[ 4 ] = { 70, 113, 59, 198 };
		struct chams{
			bool teammates;
			bool enabled;
			bool twopass;
			float type;
			float alpha;
			float reflectivity;
			float luminance;
			bool blend_scope;
			float vis_color[ 4 ] = { 220, 206, 115, 136 };
			float hid_color[ 4 ] = { 220, 255, 255, 255 };
			bool local;
			float local_col[ 4 ] = { 220, 206, 115, 136 };
		} chams;
		struct misc {
			bool no_flash;
			bool fog;
			float remove_scope;
			bool remove_scope_zoom;
			bool remove_blur;
			float scope_color[ 4 ] = { 220, 0, 0, 0 };
			float remove_smoke;
		} misc;
		struct extra {
			bool speclist;
			bool antiaim_direction;
			float antiaim_direction_color[ 4 ] = { 178, 113, 59, 198 };
			bool misc_flags;
			bool points;
		} extra;
		struct effects {
			float camera_fov = 90.f;
			float weapon_fov = 64.f;
		} effects;
	} visuals;
	struct misc {
		struct fakelag {
			bool enabled;
			float type;
			float amount = 6;
			float lagjump;
		} fakelag;
		bool tick_base_manipulation_rapidfire_enabled;
		int tick_base_manipulation_rapidfire_key;
		bool vm_aimbot;
		bool bhop;
		bool air_strafe;
		bool thirdperson;
		bool thirdperson_dead;
		bool thirdperson_grenade;
		int thirdperson_key;
		int flashlight_key;
		bool autozeus;
		bool log_damage;
		bool log_purchases;
		float nightmode;
		float prop_transparency;
		bool fast_duck;
		bool dangerzone_menu;
		int hitmarker_sound;
		bool client_hitboxes;
		float client_hitboxes_duration = 2.f;
		bool bullet_impacts;
		float bullet_impacts_duration = 4.f;
		bool hud_clear;
		bool autopistol;
		bool autoreload;
		bool fastzoom;
		float name_stealer;
		bool disable_render_teammates;
		bool bullets_trace;

		bool hide_from_obs;
		bool debug;
	} misc;
	struct antiaim{
		bool enabled;
		float yaw;
		float jitter;
		float fake_yaw;
		float pitch;
		int left_override_key = 37;
		int right_override_key = 39;
		int backwards_override_key = 40;
		int reset_override_key = 38;
		int side_switch_key;
	} antiaim;
	struct dangerzone{
		float max_item_distance;
		bool healthshot;
		bool drone;
		bool turret;
		bool cash;
		bool safe;
		bool weapon_case;
		bool tool_case;
		bool tablet_upgrade;
		bool airdrop;
		bool armor;
		bool contract;
		bool parachute;
		bool ammobox;
		bool jammer;
		bool melee_weapon;
		bool breach_charge;
	} dz;
	struct menu {
		bool opened = false;
		float cheat_color[4] = { 255, 30, 115, 136 };
		float menu_color[4] = { 255, 255, 140, 0 };
		int menu_key = 45;
		int x = 140, y = 140;
		int w = 400, h = 300;
	} menu;
	struct t {
		bool btest;
		int itest;
		float ftest;
	}t;
	struct global {
		WNDPROC WndProcBackup;
		HWND hWindow;
	}global;
};

extern c_vars g_vars;
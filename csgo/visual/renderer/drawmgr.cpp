#include "../../inc.h"

#include "../../features/visuals/visuals.h"
#include "../../features/nade_pred/nade_pred.h"
#include "../../features/anti-aim/antiaim.h"

void paint()
{
	int w, h;

	g_csgo.m_engine->get_screen_size(w, h);

	g_visuals.run();

	g_nadepred.draw();

	g_autobot.DrawPath();

	//g_notify.draw();

	if (g_vars.visuals.extra.misc_flags && g_csgo.m_engine->is_connected())
	{

		int flag_count = 0;

		if (g_cl.m_under_tickrate)
		{
			auto c = color(255, 255, 255, 255);

			vec3_t v1 = { 15.f, ((float)h / 2.f) - flag_count++ * 21.f, 0.f };
			vec3_t v2 = { 15.f, ((float)h / 2.f) - 1.f * 11.f, 0.f };

			render::draw_string(v1.x, v1.y, render::fonts::main, c, "FPS: %i", g_cl.m_client_framerate);
		}
	}

	g_visuals.watermark();
}

void render_cheat_visuals()
{
	if (render::current_target() == render::RenderTarget::overlay)
	{
		{
			BeginCheatVisuals();
		}
		{
			DrawCheatVisuals();
		}
		{
			EndCheatVisuals();
		}
	}
	else
		DrawCheatVisuals();
}

void BeginCheatVisuals()
{
    im_renderer::bufferBegin();
}

int currentBuffer = 0;

std::mutex drawing_mutex;

void DrawCheatVisuals()
{
	{
		g_Menu.Run();
	}
	{
		paint();
	}
}

void EndCheatVisuals()
{
    currentBuffer = !currentBuffer;
}
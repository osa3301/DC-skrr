#pragma once

#include "ImRenderer.hpp"

enum font_flags {
	fontflag_none,
	fontflag_italic = 0x001,
	fontflag_underline = 0x002,
	fontflag_strikeout = 0x004,
	fontflag_symbol = 0x008,
	fontflag_antialias = 0x010,
	fontflag_gaussianblur = 0x020,
	fontflag_rotary = 0x040,
	fontflag_dropshadow = 0x080,
	fontflag_additive = 0x100,
	fontflag_outline = 0x200,
	fontflag_custom = 0x400,
	fontflag_bitmap = 0x800,
};

enum FontRenderFlag_t
{
	FONT_LEFT = 0,
	FONT_RIGHT = 1,
	FONT_CENTER = 2
};

namespace render {

	enum class RenderTarget {
		surface,
		overlay
	};

	RenderTarget current_target();

	bool surface();
	bool overlay();

	bool is_ready();

	void device_lost();
	void device_reset();

	void destroy();

	void initialize(RenderTarget t);

	void draw_line(int x1, int y1, int x2, int y2, color colour);
	void draw_text_wchar(int x, int y, unsigned long font, const wchar_t* string, color colour);
	void draw_text_string(int x, int y, unsigned long font, std::string string, bool text_centered, color colour);
	void draw_string(int x, int y, unsigned long font, color colour, const char* msg, ...);
	void draw_string(int x, int y, unsigned long font, color colour, DWORD alignment, const char* msg, ...);
	void draw_string_ACP(int x, int y, unsigned long font, color colour, DWORD alignment, const char* msg, ...);
	void draw_rect(int x, int y, int w, int h, color color);
	void draw_filled_rect(int x, int y, int w, int h, color colour);
	void draw_outline(int x, int y, int w, int h, color colour);
	void draw_filled_rect_old(int x, int y, int w, int h, color colour);
	void draw_outline_old(int x, int y, int w, int h, color colour);
	void draw_textured_polygon(int n, vertex_t* vertice, color col);
	void draw_circle(int x, int y, int r, int s, color col);
	vec2_t get_text_size(unsigned long font, std::string text);

	namespace fonts {
		extern unsigned long main;
		extern ImFont* im_main;
		extern std::unique_ptr<im_renderer::font> font;
	}

}
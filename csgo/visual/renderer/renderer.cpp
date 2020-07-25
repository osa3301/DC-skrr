#include "../../inc.h"

render::RenderTarget current_render_target;

render::RenderTarget render::current_target()
{
	return current_render_target;
}

bool render::surface() { return current_target() == RenderTarget::surface; }
bool render::overlay() { return current_target() == RenderTarget::overlay; }

unsigned long render::fonts::main;
ImFont* render::fonts::im_main;

std::unique_ptr<im_renderer::font> render::fonts::font{ nullptr };

auto s_ready = false;

bool render::is_ready()
{
	return s_ready;
}

void render::device_lost()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void render::device_reset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void render::destroy()
{
	s_ready = false;

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void render::initialize(RenderTarget t) {
	if (t == RenderTarget::overlay)
	{
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(g_csgo.m_input_system->get_window());

		auto io = &ImGui::GetIO();
		current_render_target = RenderTarget::overlay;

		fonts::im_main = io->Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 14.0f);

		fonts::font.reset(new im_renderer::font("C:/Windows/Fonts/Tahoma.ttf", 13, false));

		s_ready = true;
	}

	if (t == RenderTarget::surface)
	{
		current_render_target = RenderTarget::surface;

		render::fonts::main = g_csgo.m_surface->font_create();

		g_csgo.m_surface->set_font_glyph(render::fonts::main, "Tahoma", 13, 500, 0, 0, font_flags::fontflag_outline);

		s_ready = true;
	}
}

void render::draw_line(int x1, int y1, int x2, int y2, color colour) {
	if (surface())
	{
		g_csgo.m_surface->set_drawing_color(colour.r, colour.g, colour.b, colour.a);
		g_csgo.m_surface->draw_line(x1, y1, x2, y2);
	}
	else if (overlay())
	{
		im_renderer::draw::line(x1, y1, x2, y2, colour, 0.3f);
	}
}

void render::draw_text_wchar(int x, int y, unsigned long font, const wchar_t* string, color colour) {
	if (surface())
	{
		g_csgo.m_surface->set_text_color(colour.r, colour.g, colour.b, colour.a);
		g_csgo.m_surface->draw_text_font(font);
		g_csgo.m_surface->draw_text_pos(x, y);
		g_csgo.m_surface->draw_render_text(string, wcslen(string));
	}
	else if (overlay())
	{

	}
}

void render::draw_text_string(int x, int y, unsigned long font, std::string string, bool text_centered, color colour) {
	if (surface())
	{
		const auto converted_text = std::wstring(string.begin(), string.end());

		int width, height;
		g_csgo.m_surface->get_text_size(font, converted_text.c_str(), width, height);

		g_csgo.m_surface->set_text_color(colour.r, colour.g, colour.b, colour.a);
		g_csgo.m_surface->draw_text_font(font);
		if (text_centered)
			g_csgo.m_surface->draw_text_pos(x - (width / 2), y);
		else
			g_csgo.m_surface->draw_text_pos(x, y);
		g_csgo.m_surface->draw_render_text(converted_text.c_str(), wcslen(converted_text.c_str()));
	}
	else if (overlay())
	{

	}
}

void render::draw_string(int x, int y, unsigned long font, color colour, const char* msg, ...)
{
	if (surface())
	{
		va_list va_alist;
		char buf[1024];
		va_start(va_alist, msg);
		_vsnprintf(buf, sizeof(buf), msg, va_alist);
		va_end(va_alist);
		wchar_t wbuf[1024];
		MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

		int width, height;
		g_csgo.m_surface->get_text_size(font, wbuf, width, height);

		g_csgo.m_surface->draw_text_font(font);
		g_csgo.m_surface->set_text_color(colour.r, colour.g, colour.b, colour.a);
		g_csgo.m_surface->draw_text_pos(x, y - height / 2);
		g_csgo.m_surface->draw_render_text(wbuf, wcslen(wbuf));
	}
	else if (overlay())
	{
		char output[1024] = {};
		va_list args;
		va_start(args, msg);
		vsprintf_s(output, msg, args);
		va_end(args);
		auto coord = ImVec2(x, y);
		auto size = ImGui::CalcTextSize(output);
		auto coord_out = ImVec2{ coord.x + 1.f, coord.y + 1.f };

		im_renderer::draw::string(coord.x, coord.y, colour, msg, *fonts::font);
	}
}
void render::draw_string(int x, int y, unsigned long font, color colour, DWORD alignment, const char* msg, ...)
{
	if (surface())
	{
		va_list va_alist;
		char buf[1024];
		va_start(va_alist, msg);
		_vsnprintf(buf, sizeof(buf), msg, va_alist);
		va_end(va_alist);
		wchar_t wbuf[1024];
		MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

		int width, height;
		g_csgo.m_surface->get_text_size(font, wbuf, width, height);

		if (alignment & FONT_RIGHT)
			x -= width;
		if (alignment & FONT_CENTER)
			x -= width / 2;

		g_csgo.m_surface->draw_text_font(font);
		g_csgo.m_surface->set_text_color(colour.r, colour.g, colour.b, colour.a);
		g_csgo.m_surface->draw_text_pos(x, y - height / 2);
		g_csgo.m_surface->draw_render_text(wbuf, wcslen(wbuf));
	}
	else if (overlay())
	{
		char output[1024] = {};
		va_list args;
		va_start(args, msg);
		vsprintf_s(output, msg, args);
		va_end(args);
		auto coord = ImVec2(x, y);
		auto size = ImGui::CalcTextSize(output);
		auto alig = ImVec2{ coord.x, coord.y };
		auto coord_out = ImVec2{ coord.x + 1.f, coord.y + 1.f };

		x = coord_out.x;
		y = coord_out.y;

		if (alignment & FONT_RIGHT)
			x -= alig.x;
		if (alignment & FONT_CENTER)
			x -= alig.x / 2;

		im_renderer::draw::string(x, y - alig.y / 2, colour, msg, *fonts::font);
	}
}
void render::draw_string_ACP(int x, int y, unsigned long font, color colour, DWORD alignment, const char* msg, ...)
{
	if (surface())
	{
		va_list va_alist;
		char buf[1024];
		va_start(va_alist, msg);
		_vsnprintf(buf, sizeof(buf), msg, va_alist);
		va_end(va_alist);
		wchar_t wbuf[1024];
		MultiByteToWideChar(CP_ACP, 0, buf, 256, wbuf, 256);

		int width, height;
		g_csgo.m_surface->get_text_size(font, wbuf, width, height);

		if (alignment & FONT_RIGHT)
			x -= width;
		if (alignment & FONT_CENTER)
			x -= width / 2;

		g_csgo.m_surface->draw_text_font(font);
		g_csgo.m_surface->set_text_color(colour.r, colour.g, colour.b, colour.a);
		g_csgo.m_surface->draw_text_pos(x, y - height / 2);
		g_csgo.m_surface->draw_render_text(wbuf, wcslen(wbuf));
	}
	else if (overlay())
	{

	}
}

void render::draw_rect(int x, int y, int w, int h, color color) {
	if (surface())
	{
		g_csgo.m_surface->set_drawing_color(color.r, color.g, color.b, color.a);
		g_csgo.m_surface->draw_outlined_rect(x, y, x + w, y + h);
	}
	else if (overlay())
	{
		im_renderer::draw::rectangleOutlined(x, y, w, h, color, 1.0f);
	}
}

void render::draw_filled_rect(int x, int y, int w, int h, color colour) {
	if (surface())
	{
		g_csgo.m_surface->set_drawing_color(colour.r, colour.g, colour.b, colour.a);
		g_csgo.m_surface->draw_filled_rectangle(x, y, x + w, y + h);
	}
	else if (overlay())
	{
		im_renderer::draw::rectangle(x, y, w, h, colour);
	}
}

void render::draw_outline(int x, int y, int w, int h, color colour) {
	if (surface())
	{
		g_csgo.m_surface->set_drawing_color(colour.r, colour.g, colour.b, colour.a);
		g_csgo.m_surface->draw_outlined_rect(x, y, x + w, y + h);
	}
	else if (overlay())
	{
		im_renderer::draw::rectangleOutlined(x, y, w, h, colour, 1.0f);
	}
}

void render::draw_filled_rect_old(int x, int y, int w, int h, color colour)
{
	if (surface())
	{
		g_csgo.m_surface->set_drawing_color(colour.r, colour.g, colour.b, colour.a);
		g_csgo.m_surface->draw_filled_rectangle(x, y, w, h);
	}
	else if (overlay())
	{
		im_renderer::draw::rectangle(x, y, w, h, colour);
	}
}

void render::draw_outline_old(int x, int y, int w, int h, color colour)
{
	if (surface())
	{
		g_csgo.m_surface->set_drawing_color(colour.r, colour.g, colour.b, colour.a);
		g_csgo.m_surface->draw_outlined_rect(x, y, w, h);
	}
	else if (overlay())
	{
		im_renderer::draw::rectangleOutlined(x, y, w, h, colour, 1.0f);
	}
}

void render::draw_textured_polygon(int n, vertex_t* vertice, color col) {
	if (surface())
	{
		static unsigned char buf[4] = { 255, 255, 255, 255 };
		unsigned int texture_id{};
		if (!texture_id)
		{
			texture_id = g_csgo.m_surface->create_new_texture_id(true);
			g_csgo.m_surface->draw_set_texture_RGBA(texture_id, buf, 1, 1);
		}
		g_csgo.m_surface->set_drawing_color(col.r, col.g, col.b, col.a);
		g_csgo.m_surface->set_texture(texture_id);
		g_csgo.m_surface->draw_polygon(n, vertice);
	}
	else if (overlay())
	{

	}
}

void render::draw_circle(int x, int y, int r, int s, color col) {
	float Step = math::pi * 2.0 / s;
	for (float a = 0; a < (math::pi * 2.0); a += Step)
	{
		float x1 = r * cos(a) + x;
		float y1 = r * sin(a) + y;
		float x2 = r * cos(a + Step) + x;
		float y2 = r * sin(a + Step) + y;
		if (surface())
		{
			g_csgo.m_surface->set_drawing_color(col.r, col.g, col.b, col.a);
			g_csgo.m_surface->draw_line(x1, y1, x2, y2);
		}
		else if (overlay())
		{
			im_renderer::draw::line(x1, y1, x2, y2, col, 1.0f);
		}
	}
}

vec2_t render::get_text_size(unsigned long font, std::string text) {
	if (surface())
	{
		std::wstring a(text.begin(), text.end());
		const wchar_t* wstr = a.c_str();
		static int w, h;

		g_csgo.m_surface->get_text_size(font, wstr, w, h);
		return { static_cast<float>(w), static_cast<float>(h) };
	}
	else if (overlay())
	{

	}
}
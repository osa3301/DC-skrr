#pragma once

class SDL_Window;
class ImFontAtlas;
class ImFont;

namespace im_renderer
{
    struct font
    {
        font(std::string path, int fontsize, bool outline = false);
        std::string path;
        int size;
        int new_size;
        bool outline = false;
        bool needs_rebuild = true;
        operator ImFont* ();
        void stringSize(std::string string, float* x, float* y);
        void changeSize(int new_font_size);
        ImFontAtlas* font_atlas{ nullptr };
        std::map<int, ImFont*> size_map;
    };

    class Texture
    {
    public:
        explicit Texture(std::string path);
        Texture(unsigned int id, int h, int w);
        ~Texture();
        void load();
        unsigned int get();
        int getWidth()
        {
            return width;
        }
        int getHeight()
        {
            return height;
        }

    private:
        unsigned char* data{ nullptr };
        unsigned int texture_id{ 0 };
        std::string path{ "" };
        int height{ 0 };
        int width{ 0 };
    };

    void bufferBegin();
    void renderEnd();

	namespace draw
	{
		void line(float x1, float y1, float x2, float y2, color colour, float thickness);
		void string(int x, int y, color colour, const char* text, font& font);
		void rectangle(float x, float y, float w, float h, color colour);
		void triangle(float x, float y, float x2, float y2, float x3, float y3, color colour);
		void rectangleOutlined(float x, float y, float w, float h, color colour, float thickness);
		void rectangleTextured(float x, float y, float w, float h, color colour, Texture& texture, float tx, float ty, float tw, float th, float angle);
		void circle(float x, float y, float radius, color colour, float thickness, int steps);
	}
}
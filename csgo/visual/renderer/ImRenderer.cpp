#include "../../inc.h"

#include <fstream>
#include <stack>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib")

extern int currentBuffer;
ImDrawListSharedData shared{};
ImDrawList bufferA{ &shared };
ImDrawList bufferB{ &shared };
ImDrawList* buffers[] = { &bufferA, &bufferB };

namespace im_renderer
{
    std::vector<font*>& fonts()
    {
        static std::vector<font*> stack{};
        return stack;
    }
    std::stack<Texture*>& textures()
    {
        static std::stack<Texture*> stack{};
        return stack;
    }

    font::font(std::string path, int fontsize, bool outline) : size{ fontsize }, new_size{ fontsize }, path{ path }, outline{ outline }
    {
        font_atlas = new ImFontAtlas();
        fonts().push_back(this);
    }
    font::operator ImFont* ()
    {
        if (size_map[size])
            return size_map[size];
        if (!needs_rebuild)
            needs_rebuild = true;
        return nullptr;
    }
    void font::stringSize(std::string string, float* x, float* y)
    {
        if (!size_map[size])
        {
            needs_rebuild = true;
            return;
        }
        ImVec2 result = size_map[size]->CalcTextSizeA(size, FLT_MAX, 0.f, string.c_str());
        if (x)
            *x = result.x;
        if (y)
            *y = result.y;
    }
    void font::changeSize(int new_font_size)
    {
        if (needs_rebuild)
            return;
        needs_rebuild = true;
        new_size = new_font_size;
    }

    Texture::Texture(std::string path) : path{ path }
    {
        textures().push(this);
    }

    Texture::Texture(unsigned int id, int h, int w) : texture_id(id), height(h), width(w)
    {
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &texture_id);
    }

    void Texture::load()
    {
        std::ifstream file(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

        std::streamsize size = 0;
        if (file.seekg(0, std::ios::end).good())
            size = file.tellg();
        if (file.seekg(0, std::ios::beg).good())
            size -= file.tellg();

        if (size < 1)
        {
            throw std::runtime_error("Error loading texture, size is under 1!");
        }

        unsigned char* buffer = new unsigned char[(size_t)size + 1];
        file.read((char*)buffer, size);
        file.close();
        int error = decodePNG(data, width, height, buffer, size);

        // if there's an error, display it and return false to indicate failure
        if (error != 0)
        {
            throw std::runtime_error("Couldn't init texture!");
        }
        {
            // Upload texture to graphics system
            GLint last_texture;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            // Restore state
            glBindTexture(GL_TEXTURE_2D, last_texture);
        }
        return;
    }

    unsigned int Texture::get()
    {
        return texture_id;
    }

    void bufferBegin()
    {
        buffers[currentBuffer]->Clear();
        buffers[currentBuffer]->AddDrawCmd();
        buffers[currentBuffer]->PushClipRectFullScreen();
    }

    void renderEnd()
    {
        auto drawdata = ImGui::GetDrawData();

        if (drawdata && drawdata->Valid && buffers[!currentBuffer]) // anti crash check
        {
            auto c = ImGui::GetCurrentContext();

            ImGui::AddDrawListToDrawData(&c->DrawDataBuilder.Layers[0], buffers[!currentBuffer]); // this is a hack

            drawdata->CmdLists = c->DrawDataBuilder.Layers[0].Data;
            drawdata->CmdListsCount = c->DrawDataBuilder.Layers[0].Size;
        }
        ImGui_ImplDX9_RenderDrawData(drawdata);
    }

	namespace draw
	{
        void line(float x1, float y1, float x2, float y2, color colour, float thickness)
        {
            buffers[currentBuffer]->AddLine(ImVec2(x1, y1), ImVec2(x1 + x2, y1 + y2 /* why */), ImGui::GetColorU32(ImVec4(colour.r / 255.0f, colour.g / 255.0f, colour.b / 255.0f, colour.a / 255.0f)), thickness);
        }
        void string(int x, int y, color colour, const char* text, font& font)
        {
            ImFont* internal_font = font;
            if (!internal_font || !internal_font->ContainerAtlas) // stupid crash
                return;

            buffers[currentBuffer]->PushTextureID(internal_font->ContainerAtlas->TexID);

            auto pos = ImVec2(x, y);
            if (font.outline)
                for (int i = -1; i < 2; i += 2) // ty ben xd
                {
                    buffers[currentBuffer]->AddText(internal_font, 0, ImVec2(pos.x + i, pos.y), ImGui::GetColorU32(ImVec4(0, 0, 0, colour.a / 255.0f)), text);
                    buffers[currentBuffer]->AddText(internal_font, 0, ImVec2(pos.x, pos.y + i), ImGui::GetColorU32(ImVec4(0, 0, 0, colour.a / 255.0f)), text);
                }

            buffers[currentBuffer]->AddText(internal_font, 0, pos, ImGui::GetColorU32(ImVec4(colour.r / 255.0f, colour.g / 255.0f, colour.b / 255.0f, colour.a / 255.0f)), text);

            buffers[currentBuffer]->PopTextureID();
        }
        void rectangle(float x, float y, float w, float h, color colour)
        {
            buffers[currentBuffer]->AddRectFilled(ImVec2(x, y), ImVec2(w, h), ImGui::GetColorU32(ImVec4(colour.r / 255.0f, colour.g / 255.0f, colour.b / 255.0f, colour.a / 255.0f)), 0.0f, -1);
        }
        void triangle(float x, float y, float x2, float y2, float x3, float y3, color colour)
        {
            buffers[currentBuffer]->AddTriangleFilled(ImVec2(x, y), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::GetColorU32(ImVec4(colour.r / 255.0f, colour.g / 255.0f, colour.b / 255.0f, colour.a / 255.0f)));
        }
        void rectangleOutlined(float x, float y, float w, float h, color colour, float thickness)
        {
            buffers[currentBuffer]->AddRect(ImVec2(x, y), ImVec2(w, h), ImGui::GetColorU32(ImVec4(colour.r / 255.0f, colour.g / 255.0f, colour.b / 255.0f, colour.a / 255.0f)), 0.0f, -1, thickness);
        }
        void rectangleTextured(float x, float y, float w, float h, color colour, Texture& texture, float tx, float ty, float tw, float th, float angle)
        {
            if (!texture.get())
                return;
            x += w / 2 - 1;
            y += h / 2;
            float cos_a;
            float sin_a;
            math::sin_cos(angle, &sin_a, &cos_a);
            float tex_width = texture.getWidth();
            float tex_height = texture.getHeight();
            ImVec2 center = ImVec2(x, y);
            ImVec2 pos[4] = { center + ImRotate(ImVec2(-w * 0.5f, -h * 0.5f), cos_a, sin_a), center + ImRotate(ImVec2(+w * 0.5f, -h * 0.5f), cos_a, sin_a), center + ImRotate(ImVec2(+w * 0.5f, +h * 0.5f), cos_a, sin_a), center + ImRotate(ImVec2(-w * 0.5f, +h * 0.5f), cos_a, sin_a) };
            ImVec2 uvs[4] = { ImVec2(tx / tex_width, ty / tex_height), ImVec2((tx + tw) / tex_width, ty / tex_height), ImVec2((tx + tw) / tex_width, (ty + th) / tex_height), ImVec2(tx / tex_width, (ty + th) / tex_height) };

            buffers[currentBuffer]->AddImageQuad((void*)texture.get(), pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], ImGui::GetColorU32(ImVec4(colour.r, colour.g, colour.b, colour.a)));
        }
        void circle(float x, float y, float radius, color color, float thickness, int steps)
        {
            buffers[currentBuffer]->AddCircle(ImVec2(x, y), radius, ImGui::GetColorU32(ImVec4(color.r, color.g, color.b, color.a)), steps, thickness);
        }
	}
}
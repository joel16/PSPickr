#pragma once

#include <glib2d.h>
#include <intraFont.h>
#include <string>

extern intraFont *g_font;
extern bool g_running;

namespace GUI {
    void DrawRect(float x, float y, float width, float height, g2dColor colour);
    void DrawImage(g2dTexture *tex, float x, float y, float w, float h);
    void DrawImageTint(g2dTexture *tex, float x, float y, float w, float h, g2dColor colour);
    void FontSetStyle(float size, unsigned int colour, unsigned int options);
    float GetTextHeight(void);
    float GetTextWidth(const std::string &text);
    float DrawText(float x, float y, const std::string &text);
}

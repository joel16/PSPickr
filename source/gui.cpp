#include "gui.hpp"

intraFont *g_font;

namespace GUI {
    void DrawRect(float x, float y, float width, float height, g2dColor colour) {
        g2dBeginRects(nullptr); {
            g2dSetColor(colour);
            g2dSetScaleWH(width, height);
            g2dSetCoordXY(x, y);
            g2dAdd();
        }
        g2dEnd();
    }

    void DrawImage(g2dTexture *tex, float x, float y, float w, float h) {
        g2dBeginRects(tex); {
            g2dSetScaleWH(w, h);
            g2dSetCoordXY(x, y);
            g2dAdd();
        }
        g2dEnd();
    }

    void DrawImageTint(g2dTexture *tex, float x, float y, float w, float h, g2dColor colour) {
        g2dBeginRects(tex); {
            g2dSetColor(colour);
            g2dSetScaleWH(w, h);
            g2dSetCoordXY(x, y);
            g2dAdd();
        }
        g2dEnd();
    }

    void FontSetStyle(float size, unsigned int colour, unsigned int options) {
        intraFontSetStyle(g_font, size, colour, G2D_RGBA(0, 0, 0, 0), 0.f, options);
    }

    float GetTextHeight(void) {
        return g_font->advancey * g_font->size / 4.f + 2.f;
    }

    float GetTextWidth(const std::string &text) {
        return intraFontMeasureText(g_font, text.c_str());
    }

    float DrawText(float x, float y, const std::string &text) {
        return intraFontPrintf(g_font, x, y, text.c_str());
    }
}

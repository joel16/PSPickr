#include <time.h>

#include "audio.hpp"
#include "fs.hpp"
#include "gui.hpp"
#include "textures.hpp"
#include "timer.hpp"
#include "utils.hpp"

namespace Game {
    static const float tex_size = 12.f;
    static int time_left = 0, diff = 0, stages = 0, max_stages = 0, lives = 0, matches = 0, row = 0, selection = 0;
    static bool pattern[16];
    static unsigned int color[3];

    static const g2dColor CLEAR_COLOUR           = G2D_RGBA(245, 245, 245, 255);
    static const g2dColor TITLE_COLOUR           = G2D_RGBA(175, 175, 175, 255);
    static const g2dColor TITLE_COLOUR2          = G2D_RGBA(236, 143, 140, 255);
    static const g2dColor FONT_COLOUR            = G2D_RGBA(145, 145, 145, 255);
    static const g2dColor INSTR_BOX_COLOUR1      = G2D_RGBA(88, 163, 229, 255);
    static const g2dColor INSTR_BOX_COLOUR2      = G2D_RGBA(151, 147, 231, 255);
    static const g2dColor TINT_COLOUR            = G2D_RGBA(50, 50, 50, 200);
    static const g2dColor HIGHLIGHT_COLOUR       = G2D_RGBA(0, 0, 0, 255);
    static const g2dColor TIMER_COLOUR_HEALTHY   = G2D_RGBA(0, 255, 0, 255);
    static const g2dColor TIMER_COLOUR_UNHEALTHY = G2D_RGBA(255, 0, 0, 255);
    static const g2dColor TRANSPARENT_BG_COLOUR  = G2D_RGBA(0, 0, 0, 100);

    void MainMenu(void) {
        const std::string subtitle_0 = "One of these colours is not like the other one!";
        const std::string subtitle_1 = "Which one?";
        const std::string instruc_0 = "Press X to start new game (with timer)";
        const std::string instruc_1 = "Press [ ] to start new game (without timer)";
        const std::string instruc_2 = "Press Start to exit";
        const std::string text = "Based on Sean M. Tracey's Pickr";
        
        g2dClear(CLEAR_COLOUR);
        GUI::FontSetStyle(1.2f, TITLE_COLOUR, INTRAFONT_ALIGN_LEFT);
        GUI::DrawText((480 - GUI::GetTextWidth("PSPickr")) / 2, 70, "PSPickr");
        
        GUI::FontSetStyle(1.0f, FONT_COLOUR, INTRAFONT_ALIGN_LEFT);
        GUI::DrawText((480 - GUI::GetTextWidth(subtitle_0)) / 2, 95, subtitle_0);
        GUI::DrawText((480 - GUI::GetTextWidth(subtitle_1)) / 2, 110, subtitle_1);
        
        GUI::FontSetStyle(1.0f, CLEAR_COLOUR, INTRAFONT_ALIGN_LEFT);
        
        GUI::DrawRect(((480 - GUI::GetTextWidth(instruc_0)) / 2) - 10, 120, GUI::GetTextWidth(instruc_0) + 20, 32, INSTR_BOX_COLOUR1);
        GUI::DrawText((480 - GUI::GetTextWidth(instruc_0)) / 2, 140, instruc_0);
        
        GUI::DrawRect(((480 - GUI::GetTextWidth(instruc_1)) / 2) - 10, 160, GUI::GetTextWidth(instruc_1) + 20, 32, INSTR_BOX_COLOUR2);
        GUI::DrawText((480 - GUI::GetTextWidth(instruc_1)) / 2, 180, instruc_1);

        GUI::FontSetStyle(1.0f, FONT_COLOUR, INTRAFONT_ALIGN_LEFT);
        GUI::DrawText((480 - GUI::GetTextWidth(instruc_2)) / 2, 210, instruc_2);
        
        GUI::FontSetStyle(0.8f, TITLE_COLOUR, INTRAFONT_ALIGN_LEFT);
        intraFontPrintf(g_font, 10, 262, "Version %d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
        GUI::DrawText((470 - GUI::GetTextWidth(text)), 262, text);
        g2dFlip(G2D_VSYNC);
    }

    static void Init(void) {
        lives = 5;
        time_left = 480;
        diff = 10;
        stages = 0;
        row = 0;
    }

    static void Fill(void) {
        for (unsigned int i = 0; i < 16; i++) {
            pattern[i] = false;
        }
        
        pattern[rand() % 16] = true;
        color[0] = rand() % (255 - 2 * diff) + diff;
        color[1] = rand() % (255 - 2 * diff) + diff;
        color[2] = rand() % (255 - 2 * diff) + diff;
    }

    static void Update(void) {
        diff = 10;
    }
    
    static int GetHighscore(void) {
        char buf[10] = { 0 };
        int score = 0;
        
        if (FS::FileExists("highscore.txt")) {
            FS::ReadFile("highscore.txt", buf, 10);
            std::sscanf(buf, "%d", &score);
        }
        
        return score;
    }

    static void SaveHighscore(void) {
        int score = Game::GetHighscore();

        if (max_stages > score) {
            char buf[10] = { 0 };
            std::snprintf(buf, 10, "%d\n", max_stages);
            FS::WriteFile("highscore.txt", buf, 10);
        }
    }

    static void Begin(bool timer) {
        int x, y;
        bool win = false, paused = false, hide = false;
        
        char stage_str[10], row_str[13];
        std::snprintf(stage_str, 10, "Score %d", stages);
        std::snprintf(row_str, 13, "%d in a row", row);
        
        time_left = 480;
        Game::Fill();
        diff = (rand() % 2) ? diff : -diff;
        Timer::Init();
        
        while (true && time_left > 0 && lives > 0) {
            if ((!hide) && (Timer::Get() >= 5.f)) {
                hide = true;
                Timer::Reset();
            }
            else if (hide && Timer::Get() >= 0.5f) {
                hide = false;
                Timer::Reset();
            }

            int ctrl = Utils::ReadControls();
            Utils::SetBounds(selection, 0, 15);

            if (ctrl & PSP_CTRL_LEFT) {
                selection--;
            }
            else if (ctrl & PSP_CTRL_RIGHT) {
                selection++;
            }

            if (ctrl & PSP_CTRL_UP) {
                selection -= 4;
            }
            else if (ctrl & PSP_CTRL_DOWN) {
                selection += 4;
            }
            
            if (Utils::IsButtonPressed(PSP_CTRL_ENTER)) {
                if (!paused) {
                    if (pattern[selection]) {
                        win = true;
                    }
                    else {
                        Audio::PlayNopeWav();
                        lives--;
                        row = 0;
                    }
                }
            }

            if (Utils::IsButtonPressed(PSP_CTRL_START)) {
                paused = !paused;
            }

            y = 34;
            g2dClear(CLEAR_COLOUR);

            for (int i = 0, max = (lives < 5) ? 5 : lives; i < max; i++) {
                if (i < lives) {
                    GUI::DrawImage(Textures::GetLives(), 5 + i * 13, 5, tex_size, tex_size);
                }
                else {
                    GUI::DrawImageTint(Textures::GetLives(), 5 + i * 13, 5, tex_size, tex_size, TINT_COLOUR);
                }
            }

            GUI::FontSetStyle(1.0f, FONT_COLOUR, INTRAFONT_ALIGN_LEFT);
            GUI::DrawText((472 - GUI::GetTextWidth(stage_str)), 17, stage_str);
            GUI::DrawText((472 - GUI::GetTextWidth(row_str)), 35, row_str);
            
            for (int i = 0; i < 4; i++) {
                x = 138;
                
                for (int j = 0; j < 4; j++) {
                    if (selection == (i * 4 + j)) {
                        GUI::DrawRect(x - 1, y - 1, 52, 52, HIGHLIGHT_COLOUR);
                    }

                    if (!hide || paused) {
                        GUI::DrawRect(x, y, 50, 50, (pattern[i * 4 + j]) ? G2D_RGBA(color[0] - diff, color[1] - diff, color[2] - diff, 255) : G2D_RGBA(color[0], color[1], color[2], 255));
                    }
                    
                    x += 52;
                }
                
                y += 52;
            }
            
            if (timer) {
                GUI::DrawRect(0, 257, time_left, 15, (time_left > 100) ? TIMER_COLOUR_HEALTHY : TIMER_COLOUR_UNHEALTHY);
            }
            
            if (paused) {
                GUI::DrawRect(0, 0, 480, 282, TRANSPARENT_BG_COLOUR);

                GUI::FontSetStyle(1.0f, TITLE_COLOUR2, INTRAFONT_ALIGN_LEFT);
                GUI::DrawText((472 - GUI::GetTextWidth("PAUSED")) / 2, 100, "PAUSED");
                
                GUI::FontSetStyle(1.0f, CLEAR_COLOUR, INTRAFONT_ALIGN_LEFT);
                GUI::DrawText((472 - GUI::GetTextWidth("Press start to continue")) / 2, 130, "Press start to continue");
                GUI::DrawText((472 - GUI::GetTextWidth(PSP_CTRL_CANCEL == PSP_CTRL_CIRCLE? "Press O button to exit" : "Press X button to exit")) / 2,
                    150, PSP_CTRL_CANCEL == PSP_CTRL_CIRCLE? "Press O button to exit" : "Press X button to exit");

                if (Utils::IsButtonPressed(PSP_CTRL_CANCEL)) {
                    lives = 0;
                }
            }

            g2dFlip(G2D_VSYNC);
            
            if (timer) {
                if (!paused) {
                    if (--time_left == 0 && lives > 0) {
                        Audio::PlayNopeWav();
                        time_left = 480;
                        lives--;
                        row = 0;
                    }
                }
            }
            
            if (win) {
                if (++stages % 10 == 0 && lives < 10) {
                    lives++;
                }
                
                Game::Update();
                max_stages = (stages >= max_stages) ? stages : max_stages;
                row++;
                
                if (timer == true) {
                    Game::Begin(true);
                }
                else {
                    Game::Begin(false);
                }
            }
        }
    }
    
    static void End(void) {
        char stages_str[26], score_str[20], matches_str[20];
        
        Game::SaveHighscore();
        std::snprintf(stages_str, 24, "You scored %d!", stages);
        std::snprintf(score_str, 20, "Best score: %d", Game::GetHighscore());
        std::snprintf(matches_str, 20, "Games played: %d", matches);
        Audio::PlayGameOverWav();
        
        while(true) {
            Utils::ReadControls();

            if (Utils::IsButtonPressed(PSP_CTRL_ENTER)) {
                break;
            }

            g2dClear(CLEAR_COLOUR);

            GUI::FontSetStyle(1.2f, TITLE_COLOUR2, INTRAFONT_ALIGN_LEFT);
            GUI::DrawText((480 - GUI::GetTextWidth("GAME OVER")) / 2, 40, "GAME OVER");

            GUI::FontSetStyle(1.0f, CLEAR_COLOUR, INTRAFONT_ALIGN_LEFT);
            GUI::DrawRect(((480 - GUI::GetTextWidth("Press X to try again")) / 2) - 10, 60, GUI::GetTextWidth("Press X to try again") + 20, 32, INSTR_BOX_COLOUR1);
            GUI::DrawText((480 - GUI::GetTextWidth("Press X to try again")) / 2, 80, "Press X to try again");

            GUI::FontSetStyle(1.0f, FONT_COLOUR, INTRAFONT_ALIGN_LEFT);
            GUI::DrawText((480 - GUI::GetTextWidth(stages_str)) / 2, 110, stages_str);
            GUI::DrawText((480 - GUI::GetTextWidth(score_str)) / 2, 130, score_str);
            GUI::DrawText((480 - GUI::GetTextWidth(matches_str)) / 2, 150, matches_str);

            g2dFlip(G2D_VSYNC);
        }
    }

    void Loop(void) {
        srand(time(nullptr));
        Game::Init();
        matches = 0;
        max_stages = 0;
        
        while(g_running) {
            int ctrl = Utils::ReadControls();

            if (Utils::IsButtonPressed(PSP_CTRL_ENTER)) {
                Game::Begin(true);
                matches++;
                Game::End();
                Game::Init();
            }
            
            if (Utils::IsButtonPressed(PSP_CTRL_SQUARE)) {
                Game::Begin(false);
                matches++;
                Game::End();
                Game::Init();
            }

            Game::MainMenu();

            if (Utils::IsButtonPressed(PSP_CTRL_START)) {
                g_running = false;
            }
        }
    }
}

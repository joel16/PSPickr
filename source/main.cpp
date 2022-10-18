#include <pspkernel.h>

#include "audio.hpp"
#include "game.hpp"
#include "gui.hpp"
#include "textures.hpp"
#include "utils.hpp"

PSP_MODULE_INFO("PSPickr", 0, VERSION_MAJOR, VERSION_MINOR);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

bool g_running = true;

namespace Services {
    int Init(void) {
        int ret = 0;

        sceCtrlSetSamplingCycle(0);
        sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
        
        if (R_FAILED(ret = intraFontInit())) {
            pspDebugScreenPrintf("intraFontInit failed: 0x%08x\n", ret);
            return ret;
        }

        // Load audio, font and textures.
        Audio::Init();
        Audio::InitGameOverWav();
        Audio::InitNopeWav();
        g_font = intraFontLoad("flash0:/font/ltn8.pgf", INTRAFONT_CACHE_ALL);
        Textures::Init();
        
        PSP_CTRL_ENTER = Utils::GetEnterButton();
        PSP_CTRL_CANCEL = Utils::GetCancelButton();
        return 0;
    }
    
    void Exit(void) {
        Textures::Exit();
        intraFontUnload(g_font);
        Audio::Exit();
        sceKernelExitGame();
    }
    
    static int ExitCallback(int arg1, int arg2, void *common) {
        g_running = false;
        Services::Exit();
        return 0;
    }
    
    static int CallbackThread(SceSize args, void *argp) {
        int callback = 0;
        callback = sceKernelCreateCallback("ExitCallback", Services::ExitCallback, nullptr);
        sceKernelRegisterExitCallback(callback);
        sceKernelSleepThreadCB();
        return 0;
    }
    
    int SetupCallbacks(void) {
        int thread = 0;
        
        if (R_SUCCEEDED(thread = sceKernelCreateThread("CallbackThread", Services::CallbackThread, 0x11, 0xFA0, 0, nullptr)))
            sceKernelStartThread(thread, 0, 0);
        
        return thread;
    }
}

int main(int argc, char *argv[]) {
    Services::SetupCallbacks();
    Services::Init();
    Game::Loop();
    Services::Exit();
}

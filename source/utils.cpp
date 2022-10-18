#include <cstring>
#include <pspkernel.h>
#include <pspreg.h>
#include <psputility_sysparam.h>

#include "utils.hpp"

enum PspCtrlButtons PSP_CTRL_ENTER, PSP_CTRL_CANCEL;

namespace Utils {
    constexpr unsigned int CTRL_DEADZONE_DELAY = 500000;
    constexpr unsigned int CTRL_DELAY = 100000;

    static SceCtrlData pad, prev_pad;
    static unsigned int last_button = 0;
    static int last_button_tick = 0, deadzone_tick = 0;
    
    void SetBounds(int &set, int min, int max) {
        if (set > max)
            set = min;
        else if (set < min)
            set = max;
    }

    static int GetRegistryValue(const char *dir, const char *name, unsigned int *value) {
        int ret = 0;
        struct RegParam reg_param;
        REGHANDLE reg_handle = 0, reg_handle_cat = 0, reg_handle_key = 0;
        unsigned int type = 0, size = 0;
        
        std::memset(&reg_param, 0, sizeof(RegParam));
        reg_param.regtype = 1;
        reg_param.namelen = std::strlen("/system");
        reg_param.unk2 = 1;
        reg_param.unk3 = 1;
        std::strcpy(reg_param.name, "/system");

        if (R_FAILED(ret = sceRegOpenRegistry(&reg_param, 2, &reg_handle))) {
            pspDebugScreenPrintf("sceRegOpenRegistry() failed: 0x%08x\n", ret);
            return ret;
        }

        if (R_FAILED(ret = sceRegOpenCategory(reg_handle, dir, 2, &reg_handle_cat))) {
            sceRegCloseRegistry(reg_handle);
            pspDebugScreenPrintf("sceRegOpenCategory() failed: 0x%08x\n", ret);
            return ret;
        }

        if (R_FAILED(ret = sceRegGetKeyInfo(reg_handle_cat, name, &reg_handle_key, &type, &size))) {
            sceRegCloseCategory(reg_handle_cat);
            sceRegCloseRegistry(reg_handle);
            pspDebugScreenPrintf("sceRegGetKeyInfo() failed: 0x%08x\n", ret);
            return ret;
        }

        if (R_FAILED(ret = sceRegGetKeyValue(reg_handle_cat, reg_handle_key, value, 4))) {
            sceRegCloseCategory(reg_handle_cat);
            sceRegCloseRegistry(reg_handle);
            pspDebugScreenPrintf("sceRegGetKeyValue() failed: 0x%08x\n", ret);
            return ret;
        }

        if (R_FAILED(ret = sceRegFlushCategory(reg_handle_cat))) {
            sceRegCloseCategory(reg_handle_cat);
            sceRegCloseRegistry(reg_handle);
            pspDebugScreenPrintf("sceRegFlushCategory() failed: 0x%08x\n", ret);
            return ret;
        }

        if (R_FAILED(ret = sceRegCloseCategory(reg_handle_cat))) {
            sceRegCloseRegistry(reg_handle);
            pspDebugScreenPrintf("sceRegCloseCategory() failed: 0x%08x\n", ret);
            return ret;
        }

        if (R_FAILED(ret = sceRegFlushRegistry(reg_handle))) {
            sceRegCloseRegistry(reg_handle);
            pspDebugScreenPrintf("sceRegFlushRegistry() failed: 0x%08x\n", ret);
            return ret;
        }

        if (R_FAILED(ret = sceRegCloseRegistry(reg_handle))) {
            pspDebugScreenPrintf("sceRegFlushRegistry() failed: 0x%08x\n", ret);
            return ret;
        }
            
        return 0;
    }
    
    int ReadControls(void) {
        prev_pad = pad;
        sceCtrlReadBufferPositive(&pad, 1);
        
        if (pad.Buttons == last_button) {
            if (pad.TimeStamp - deadzone_tick < CTRL_DEADZONE_DELAY)
                return 0;
                
            if (pad.TimeStamp - last_button_tick < CTRL_DELAY)
                return 0;
                
            last_button_tick = pad.TimeStamp;
            return last_button;
        }
        
        last_button = pad.Buttons;
        deadzone_tick = last_button_tick = pad.TimeStamp;
        return last_button;
    }
    
    int IsButtonPressed(enum PspCtrlButtons buttons) {
        return ((pad.Buttons & buttons) == buttons) && ((prev_pad.Buttons & buttons) != buttons);
    }
    
    int IsButtonHeld(enum PspCtrlButtons buttons) {
        return pad.Buttons & buttons;
    }
    
    enum PspCtrlButtons GetEnterButton(void) {
        int ret = 0, button = -1;

        if (R_FAILED(ret = sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &button))) {
            pspDebugScreenPrintf("sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN) failed: 0x%08x\n", ret);

            unsigned int reg_button = -1;
            if (R_SUCCEEDED(Utils::GetRegistryValue("/CONFIG/SYSTEM/XMB", "button_assign", &reg_button))) {
                if (reg_button == 0)
                    return PSP_CTRL_CIRCLE;
                
                return PSP_CTRL_CROSS;
            }
        }

        if (button == 0)
            return PSP_CTRL_CIRCLE;
            
        return PSP_CTRL_CROSS; // By default return PSP_CTRL_CROSS
    }
    
    // Basically the opposite of GetEnterButton()
    enum PspCtrlButtons GetCancelButton(void) {
        int ret = 0, button = -1;

        if (R_FAILED(ret = sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &button))) {
            pspDebugScreenPrintf("sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN) failed: 0x%08x\n", ret);

            unsigned int reg_button = -1;
            if (R_SUCCEEDED(Utils::GetRegistryValue("/CONFIG/SYSTEM/XMB", "button_assign", &reg_button))) {
                if (reg_button == 0)
                    return PSP_CTRL_CROSS;
                
                return PSP_CTRL_CIRCLE;
            }
        }

        if (button == 0)
            return PSP_CTRL_CROSS;
            
        return PSP_CTRL_CIRCLE; // By default return PSP_CTRL_CIRCLE
    }
    
    float GetAnalogX(void) {
        return ((static_cast<float>(pad.Lx - 122.5f)) / 122.5f);
    }
    
    float GetAnalogY(void) {
        return ((static_cast<float>(pad.Ly - 122.5f)) / 122.5f);
    }
}

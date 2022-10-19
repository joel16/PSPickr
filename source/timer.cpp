#include <pspkernel.h>
#include <time.h>
#include <psprtc.h>

namespace Timer {
    static u64 tick = 0;
    static u32 res = 0;

    void Init(void) {
        res = sceRtcGetTickResolution();
        sceRtcGetCurrentTick(&tick);
    }

    void Reset(void) {
        sceRtcGetCurrentTick(&tick);
    }

    float Get(void) {
        u64 curr_tick = 0;
        sceRtcGetCurrentTick(&curr_tick);
        return (curr_tick - tick) * 1.0f / res;
    }
}

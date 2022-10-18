#pragma once

#include <pspctrl.h>
#include <psptypes.h>

/// Checks whether a result code indicates success.
#define R_SUCCEEDED(res) ((res) >= 0)
/// Checks whether a result code indicates failure.
#define R_FAILED(res)    ((res) < 0)

extern enum PspCtrlButtons PSP_CTRL_ENTER, PSP_CTRL_CANCEL;

namespace Utils {
    void SetBounds(int &set, int min, int max);
    int ReadControls(void);
    int IsButtonPressed(enum PspCtrlButtons buttons);
    int IsButtonHeld(enum PspCtrlButtons buttons);
    enum PspCtrlButtons GetEnterButton(void);
    enum PspCtrlButtons GetCancelButton(void);
    float GetAnalogX(void);
    float GetAnalogY(void);
}

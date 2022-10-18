#pragma once

#include <cstddef>

namespace Audio {
    void Init(void);
    void Exit(void);
    int InitGameOverWav(void);
    int InitNopeWav(void);
    void PlayGameOverWav(void);
    void PlayNopeWav(void);
}

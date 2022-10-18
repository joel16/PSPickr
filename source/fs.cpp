#include <pspiofilemgr.h>

#include "fs.hpp"
#include "utils.hpp"

namespace FS {
    bool FileExists(const std::string &path) {
        SceIoStat stat = { 0 };
        return sceIoGetstat(path.c_str(), &stat) >= 0;
    }

    int ReadFile(const std::string &path, void *buf, int size) {
        SceUID file = 0;
        
        if (R_SUCCEEDED(file = sceIoOpen(path.c_str(), PSP_O_RDONLY, 0))) {
            int read = sceIoRead(file, buf, size);
            sceIoClose(file);
            return read;
        }
        
        return file;
    }
    
    int WriteFile(const std::string &path, void *buf, int size) {	
        SceUID file = 0;
        
        if (R_SUCCEEDED(file = sceIoOpen(path.c_str(), PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777))) {
            int written = sceIoWrite(file, buf, size);
            sceIoClose(file);
            return written;
        }
        
        return file;
    }
}

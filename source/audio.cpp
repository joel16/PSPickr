#include <pspaudio.h>
#include <pspaudiolib.h>
#include <pspthreadman.h>

#include "audio.hpp"
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

extern unsigned char game_over_wav_start[], nope_wav_start[];
extern unsigned int game_over_wav_size, nope_wav_size;

namespace Audio {
    static drwav game_over_wav, nope_wav;
    static drwav_uint64 frames_read = 0;
    static bool playing = false;
    
    static void Decode(void *buf, unsigned int length, void *userdata) {
        drwav *wav = static_cast<drwav *>(userdata);
        frames_read += drwav_read_pcm_frames_s16(wav, static_cast<drwav_uint64>(length), static_cast<drwav_int16 *>(buf));
        
        if (frames_read >= wav->totalPCMFrameCount) {
            playing = false;
        }
    }

    void Init(void) {
        pspAudioInit();
    }
    
    void Exit(void) {
        playing = false;
        frames_read = 0;
        pspAudioEndPre();
        pspAudioEnd();
        drwav_uninit(&game_over_wav);
        drwav_uninit(&nope_wav);
    }
    
    int InitGameOverWav(void) {
        if (!drwav_init_memory(&game_over_wav, game_over_wav_start, game_over_wav_size, nullptr))
            return -1;
        
        return 0;
    }

    int InitNopeWav(void) {
        if (!drwav_init_memory(&nope_wav, nope_wav_start, nope_wav_size, nullptr))
            return -1;
        
        return 0;
    }

    static void Clear(void) {
        playing = true;
        pspAudioSetChannelCallback(0, nullptr, nullptr); // Clear channel callback
        frames_read = 0;
    }

    void PlayGameOverWav(void) {
        Audio::Clear();
        drwav_seek_to_pcm_frame(&game_over_wav, 0);
        pspAudioSetChannelCallback(0, Audio::Decode, &game_over_wav);
    }

    void PlayNopeWav(void) {
        Audio::Clear();
        drwav_seek_to_pcm_frame(&nope_wav, 0);
        pspAudioSetChannelCallback(0, Audio::Decode, &nope_wav);
    }
}

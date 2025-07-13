#ifndef ANDROID_SOUND_SYSTEM_H
#define ANDROID_SOUND_SYSTEM_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/asset_manager.h>
#include "../DArray.h"


struct WAVHeader{
    char                RIFF[4];
    unsigned long       ChunkSize;
    char                WAVE[4];
    char                fmt[4];
    unsigned long       Subchunk1Size;
    unsigned short      AudioFormat;
    unsigned short      NumOfChan;
    unsigned long       SamplesPerSec;
    unsigned long       bytesPerSec;
    unsigned short      blockAlign;
    unsigned short      bitsPerSample;
    char                Subchunk2ID[4];
    unsigned long       Subchunk2Size;
};
struct SoundBuffer{
    WAVHeader* header;
    char* buffer;
    int length;
};


struct SoundData{
    char name[255];
};


struct ResourseDescriptor{
    int32_t decriptor;
    off_t start;
    off_t length;
};




//----------

class SoundSystem{

    // engine
    SLObjectItf engineObj;
    SLEngineItf engine;
    // output
    SLObjectItf outPutMixObj;


    // sound
    SLObjectItf  playerObj;
    SLObjectItf  musicPlayerObj;
    SLPlayItf soundPlayer;
    SLObjectItf playerVolume;
   // SLObjectItf bufferQueueObj;
    SLAndroidSimpleBufferQueueItf bufferQueueObj;

    DArray<SoundData> audioInfo;
    DArray<SoundBuffer*> buffers;


public:
    SoundSystem(){ outPutMixObj = 0; engineObj = 0; outPutMixObj = 0; playerObj = 0; musicPlayerObj = 0; }
    bool init();
    void loadFiles(const char* BasePath, const char* list, AAssetManager* man);
    void setupListener(float * pos, float * orientation);
    void setSoundPos(unsigned int index, float * pos);
    void playSound(unsigned int index, bool loop=false);
    void freeData();
    bool isPlaying(unsigned int index);
    void stopAll();
    void playMusic(const char* path, AAssetManager* man);
    void stopMusic();
    void exit();


};




#endif //ANDROID_SOUND_SYSTEM_H

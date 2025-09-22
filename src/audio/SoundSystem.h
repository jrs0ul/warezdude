#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H

#ifdef _WIN32
    #ifdef _MSC_VER
        #include <alc.h>
        #include <al.h>
    #else
        #include <AL/alc.h>
        #include <AL/al.h>
    #endif
    #elif __APPLE__
        #include <OpenAL/alc.h>
        #include <OpenAL/al.h>
    #else
        #include <AL/alc.h>
        #include <AL/al.h>
#endif


#include "../DArray.h"

struct SoundData{
    char name[255];
};

//----------

class SoundSystem{

    ALCdevice* alcdev;
    ALCcontext* alccont;

    ALuint* buffers;
    ALuint* sources;
    DArray<SoundData> audioInfo;

    char* LoadOGG(char *fileName,  ALsizei & size, ALenum &format, ALsizei &freq);


public:

    static SoundSystem* getInstance()
    {
        static SoundSystem instance;
        return &instance;
    }

    bool init(ALCchar* dev);
    void loadFiles(const char* BasePath, const char* list);
    void setupListener(float * pos, float * orientation);
    void setSoundPos(unsigned int index, float * pos);
    void setVolume(unsigned int index, int volume);
    void playsound(unsigned int index, bool loop=false);
    void freeData();
    bool isPlaying(unsigned int index);
    ALuint getBuffer(unsigned int index){return buffers[index];}
    void stopAll();
    void exit();

private:

    SoundSystem(){}
    ~SoundSystem(){}
};




#endif //SOUND_SYSTEM_H

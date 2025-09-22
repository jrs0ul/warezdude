#ifndef __ogg_h__
#define __ogg_h__


#ifdef _WIN32
    #ifdef _MSC_VER
        #include <al.h>
    #else
        #include <AL/al.h>
    #endif
#elif __APPLE__
    #include <OpenAL/al.h>
    #include <TargetConditionals.h>
#else
    #include <AL/al.h>
#endif

#include <cstring>
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
	#include "vorbisfile.h"
#else
	#include <vorbis/vorbisfile.h>
#endif



#define BUFFER_SIZE (4096 * 8)
#define BUFFERCOUNT 8




class OggStream{
    
    FILE*           oggFile;
    OggVorbis_File  oggStream;
    vorbis_info*    vorbisInfo;
    
    ALuint buffers[BUFFERCOUNT];
    ALuint source;
    ALenum format;
    
    float volume;
public:

        //opens stream from ogg file
        OggStream()
        {
            oggFile = nullptr;
            vorbisInfo = nullptr;
            source = 0;
            memset(&oggStream, 0, sizeof(OggVorbis_File));
            memset(buffers, 0 , sizeof(ALuint)*BUFFERCOUNT);
            volume = 0.2f;
        }
        bool open(const char* path);
        void release();
        bool playback();
        bool playing();
        bool update();
        void stop();
        void setVolume(float vol);

    protected:

       long  stream(char* decbuff);
        void empty();
        void check(const char* place);

           
};


#endif // __ogg_h__

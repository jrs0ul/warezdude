#ifndef _SYSTEMCONFIG_H
#define _SYSTEMCONFIG_H

#include <cstring>

    class SystemConfig{


    public:
        float musicVolume;

        int ScreenWidth;
        int ScreenHeight;
        int useWindowed;
        int soundFXVolume;


        SystemConfig(){
            musicVolume = 0.2f;

            useWindowed = true;
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
            ScreenWidth = 640;
            ScreenHeight = 480;
#else
            ScreenWidth = 640;
            ScreenHeight = 480;

            useWindowed = true;
#endif

        }

#ifndef __ANDROID__
        bool load(const char * config);
        bool write(const char * config);
#endif
    };



#endif //SYSTEMCONFIG_H

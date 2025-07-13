#pragma once

#ifndef __ANDROID__
#include "audio/SoundSystem.h"
#include "audio/OggStream.h"
#else
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#include "audio/AndroidSoundSystem.h"
#endif
#include "Vectors.h"
#include "TouchData.h"
#include "SysConfig.h"
#include "ShaderProgram.h"
#include "CClient.h"
#include "CServer.h"
#include "map.h"



class Game
{
    CServer serveris;
    CClient clientas;


    CMap mapas;

    float DT;
public:

    void init();
    void render();
    void logic();
    void destroy();

private:
    void MoveDude();
    void SendMapData(int clientIndex);
    void SendWarpMessage();
    void ItemPickup();
    void SendItemCRemove(int itemIndex);
    void GoToLevel(int level, int otherplayer);
    void SendItemSRemove(int ItemIndex, int clientIndex, bool playerTaked);
    void SendMapInfo(int clientIndex);
    void PutExit();
    long Threadproc(void *param);
    void InitServer();
    void StopServer();
    bool JoinServer(const char* ip, unsigned port);
    void QuitServer();
    void SendServerDoorState(unsigned int clientIndex, int doorx,int doory, unsigned char doorframe);
};

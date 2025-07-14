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
#include "Threads.h"



class Game
{
    static CServer serveris;
    static CClient clientas;


    static SystemConfig sys;

    static CMap mapas;
    ThreadManager threadman;

    float DT;
    bool Works;
    char DocumentPath[255];

public:

    void init();
    void render();
    void logic();
    void destroy();

private:
    void MoveDude();
    static void SendMapData(int clientIndex);
    void SendWarpMessage();
    void ItemPickup();
    void SendItemCRemove(int itemIndex);
    void GoToLevel(int level, int otherplayer);
    void SendItemSRemove(int ItemIndex, int clientIndex, bool playerTaked);
    static void SendMapInfo(int clientIndex);
    void PutExit();
    static long Threadproc(void *param);
    void InitServer();
    void StopServer();
    bool JoinServer(const char* ip, unsigned port);
    void QuitServer();
    void SendServerDoorState(unsigned int clientIndex, int doorx,int doory, unsigned char doorframe);
    void DoorsInteraction();
    void SlimeReaction(int index);
    void SendClientAtackImpulse(int victimID, int hp);
    void SendClientCoords();
    void SendClientDoorState(int doorx,int doory, unsigned char doorframe);
    void AddaptMapView();
    void DrawMissionObjectives();
    void MonsterAI(int index);
    void BeatEnemy(int aID, int damage);
    void SendAtackImpulse(unsigned int clientIndex, int victim, int hp);
    void SendBulletImpulse(int monsterindex, int ammo, int clientIndex, bool isMine);
    void findpskxy();
    void LoadFirstMap();
    void TitleMeniuHandle();
    void HandleBullets();
    void AnimateSlime();
    void ResetVolume();
    bool InitAudio();
    void GetData();
    void GetClientAtackImpulse(const char* buf, int * index, int ClientIndex);
    void QuitApp();
    void SendData();
    bool OnHit(Bullet& bul);
    void DrawStats();
    void SendPlayerInfoToClient(int clientindex);
    void GetMapInfo(const char* bufer, int bufersize, int* index);
    void KillPlayer(int index);
    void GetAtackImpulse(const char* buf,int* index);
    void SendItemCreation(float x, float y, int value, unsigned int clientIndex);
    void GetNewItemInfo(char* bufer, int* index);
    void GetMapData(const char* bufer, int bufersize, int* index);
};

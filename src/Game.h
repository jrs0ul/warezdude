#pragma once

#ifndef __ANDROID__
#include "audio/SoundSystem.h"
#include "audio/OggStream.h"
#else
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#include "audio/AndroidSoundSystem.h"
#endif
#include "ActionKeys.h"
#include "Vectors.h"
#include "TouchData.h"
#include "SysConfig.h"
#include "ShaderProgram.h"
#include "gui/SelectMenu.h"
#include "gui/Slider.h"
#include "gui/EditBox.h"
#include "Intro.h"
#include "network/Server.h"
#include "network/Client.h"
#include "BulletContainer.h"
#include "Inventory.h"
#include "Collection.h"
#include "GameData.h"
#include "map.h"
#include "maplist.h"






enum GameState
{
    GAMESTATE_TITLE,
    GAMESTATE_INTRO,
    GAMESTATE_HELP,
    GAMESTATE_GAME,
    GAMESTATE_ENDING
};

enum MultiplayerModes
{
    MPMODE_COOP,
    MPMODE_DEATHMATCH
};

enum NetworkingModes
{
    NETMODE_NONE,
    NETMODE_SERVER,
    NETMODE_CLIENT
};

enum NetworkCommands
{
    NET_NOP = 0,

    NET_CLIENT_MSG_CONNECT = 1,
    NET_CLIENT_MSG_CHARACTER_DATA,
    NET_CLIENT_MSG_WEAPON_SHOT,
    NET_CLIENT_MSG_ITEM,
    NET_CLIENT_MSG_DOOR,
    NET_CLIENT_MSG_NEXT_LEVEL,
    NET_CLIENT_MSG_MELEE_ATTACK,
    NET_CLIENT_MSG_QUIT,
    NET_CLIENT_MSG_COOP_RESURRECT,
    NET_CLIENT_MSG_PONG = 20,

    NET_SERVER_MSG_CHARACTER_DATA,
    NET_SERVER_MSG_REMOVE_CHARACTER,
    NET_SERVER_MSG_SERVER_INFO,
    NET_SERVER_MSG_MAP_DATA,
    NET_SERVER_MSG_KILL_CHARACTER,
    NET_SERVER_MSG_FRAG,
    NET_SERVER_MSG_WEAPON_SHOT,
    NET_SERVER_MSG_ITEM,
    NET_SERVER_MSG_DOOR,
    NET_SERVER_MSG_NEW_ITEM,
    NET_SERVER_MSG_MELEE_ATTACK,
    NET_SERVER_MSG_PING,
    NET_SERVER_MSG_SHUTTING_DOWN,
    NET_SERVER_MSG_SYNC_TIMER,
    NET_SERVER_MSG_COOP_GAME_OVER,
    NET_SERVER_MSG_COOP_RESURRECT
};


class Game
{

    float   OrthoMatrix[16];
    float   ViewMatrix[16];

    ShaderProgram        defaultShader;
    ShaderProgram        colorShader;

    MapList mapai;
    CMap mapas;

    ScroollControl SfxVolumeC;
    ScroollControl MusicVolumeC;
    EditBox ipedit;

    Intro intro;

    DArray<int> loot;
    DArray<int> stash;
    Inventory inventory;

    Collection cartridgeCollection;

    GameData gameData;

    Client client;
    Server serveris;

    SystemConfig sys;
    OggStream music;
    CBulletContainer bulbox;
    PicsContainer pics;


    SelectMenu mainmenu;
    SelectMenu netmenu;
    SelectMenu netgame;
    SelectMenu options;
    SelectMenu mapmenu;

    unsigned int imgCount;
    unsigned int maxwavs;


    int frags; //frags of current player
    DArray<int> fragTable;
    int equipedGame;
    int timeleft;
    bool showdebugtext;
    bool FirstTime;
    bool gameOver;
    bool noAmmo;
    int ms;


    int otherClientCount; //  This number is received from server, for server itself it is 0

    int itmtim;
    int fadeTimer;
    int slimeTimer;

    NetworkingModes netMode;

    int clientInfoSendCounter;

    int  clientMyIndex;
    bool Client_GotMapData;
    bool showMiniMap;


public:

    static const int GameKeyCount = 20;

    GameState state;
    MultiplayerModes netGameState;

    char EditText[255];
    DArray<int> clientIds; // how it is in server

    TouchData   touches;

    int DebugMode;
    Vector3D gamepadLAxis;
    Vector3D OldGamepadLAxis;

    Vector3D gamepadRAxis;
    Vector3D OldGamepadRAxis;

    bool doRumble;

    float MouseX;
    float MouseY;

    float OldMouseX;
    float OldMouseY;

    float RelativeMouseX;
    float RelativeMouseY;
    unsigned char globalKEY;
    unsigned char globalKeyUp;
    unsigned char OldKeys[GameKeyCount]; //zaidimo mygtukai
    unsigned char Keys[GameKeyCount]; //zaidimo mygtukai
    char DocumentPath[255];
    bool Works;
    bool windowed;
    unsigned ScreenWidth;
    unsigned ScreenHeight;
    float TimeTicks;
    float DeltaTime;
    float DT;
    float Accumulator;

public:

    Game();

    void init();
    void render();
    void network();
    void logic();
    void destroy();
    void loadConfig();

private:
    void DrawTitleScreen();
    void DrawEndScreen();
    void DrawGameplay();
    void MoveDude();
    void SendWarpMessage();
    void ItemPickup();
    void SendItemCRemove(int itemIndex);
    void goToEnding();
    void GoToLevel(int currentHp, int currentAmmo, int level, int otherplayer);
    void SendItemSRemove(int ItemIndex, int clientIndex, bool playerTaked);
    //mapo pavadinimas, klientu skaicius
    void SendMapInfo(int clientIndex, CMap& map);
    //monster races amd item positions
    void SendMapData(int clientIndex, CMap& map);
    void InitServer();
    void StopServer();
    bool JoinServer(const char* ip, unsigned port);
    void QuitServer();
    void SendServerDoorState(unsigned int clientIndex, int doorx,int doory, unsigned char doorframe);
    void DoorsInteraction();
    void HandleInteractionsWithDeadPlayers();
    void CheckForExit();
    int  slimeReaction(int index);
    void SendClientMeleeImpulseToServer(int victimID, int hp);
    void SendClientShootImpulseToServer();
    void SendClientCoords();
    void SendClientDoorState(int doorx,int doory, unsigned char doorframe);
    void SendResurrectMessageToClient(unsigned clientIdx, unsigned playerIdx);
    void SendResurrectMessageToServer();
    void SendGameOverMessageToClient(unsigned clientIndex);
    void AdaptMapView();
    void DrawMissionObjectives();
    void MonsterAI(int index);
    void BeatEnemy(int aID, int damage);
    void SendServerMeleeImpulseToClient(unsigned int clientIndex, int victim, int hp, int attacker);
    void SendBulletImpulse(int monsterindex, int ammo, int clientIndex, bool isMine);
    void GenerateTheMap(int currentHp, int currentAmmo);
    void LoadTheMap(const char* name, bool createItems, int otherPlayers, int currentHp);
    void LoadFirstMap();
    void TitleMenuLogic();
    void HandleBullets();
    void AnimateSlime();
    void ResetVolume();
    bool InitAudio();
    void ParseMessagesServerGot();
    void ParseMessagesClientGot();
    void GetClientAtackImpulse(const unsigned char* buf, unsigned * index, int ClientIndex);
    bool OnHit(Bullet& bul);
    void DrawStats();
    void SendPlayerInfoToClient(int clientindex);
    void GetMapInfo(const unsigned char* bufer, int* index);
    void GetServerResurrectMsg(const unsigned char* buffer, int* index);
    void KillPlayer(int index);
    void GetAttackImpulseFromServer(const unsigned char* buf, int* index);
    void SendItemCreation(float x, float y, int value, unsigned int clientIndex);
    void GetNewItemInfo(unsigned char* bufer, int* index);
    void GetMapData(const unsigned char* bufer, int* index);
    void populateClientDudes(int oldClientCount);
    void DrawMap(float r,float g, float b);
    void GetDoorInfo(const unsigned char* bufer, unsigned * index, int* dx, int* dy, unsigned char* frame);
    void KillEnemy(unsigned ID);
    int FPS();
    void DrawSomeText();
    void ServerParseCharacterData(const unsigned char* bufer, unsigned * buferindex, int clientIndex);
    void ServerParseWeaponShot(const unsigned char* buffer, unsigned * bufferindex, int clientIndex);
    void ServerParseClientResurrect(unsigned* bufferindex, int clientIndex);
    void ServerSendTimerSync(unsigned clientIdx);
    void GetServerTimeMsg(const unsigned char* buffer, int * bufferindex);
    void GetServerCoopGameOverMsg(int* bufferindex);
    void GetCharData(const unsigned char* bufer, int bufersize, int* index);
    void DrawMiniMap(int x, int y);
    void LoadMap(const char* mapname, int otherplayers);
    void HelpScreenLogic();
    void EndingLogic();
    int PlayerCount();
    void LoadShader(ShaderProgram* shader, const char* name);
    void IntroScreenLogic();
    void PlayNewSong(const char* songName);

    void SendKillCommandToClient(unsigned clientIdx, int victimID);
    void SendFragsToClient(int clientIdx, int frags);

    void DeleteAudio();

    void CoreGameLogic();
    void DrawNum(int x, int y, int num);
    void DrawHelp();
};



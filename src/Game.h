#pragma once

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#endif
#include <disarray/audio/OggStream.h>
#include <disarray/ActionKeys.h>
#include <disarray/Vectors.h>
#include <disarray/TouchData.h>
#include <disarray/SysConfig.h>
#include <disarray/ShaderProgram.h>
#include <disarray/gui/SelectMenu.h>
#include <disarray/gui/Slider.h>
#include <disarray/gui/EditBox.h>
#include <disarray/GameProto.h>
#include "Intro.h"
#include <disarray/network/Server.h>
#include <disarray/network/Client.h>
#include "BulletContainer.h"
#include "NetworkCommands.h"
#include <disarray/RenderTexture.h>
#include "Inventory.h"
#include "Collection.h"
#include "GameData.h"
#include "GameMap.h"


class MapList;
class VulkanVideo;

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


struct ClientLoot
{
    DArray<int> cartridges;
};

class Game : public GameProto
{
    MapList* mapai;
    GameMap mapas;

    ScroollControl SfxVolumeC;
    ScroollControl MusicVolumeC;
    EditBox ipedit;

    Intro intro;

    std::vector<int> loot;

    std::vector<ClientLoot> clientLoot; // for server

    std::vector<int> stash;
    Inventory inventory;

    Collection cartridgeCollection;

    GameData gameData;

    Client client;
    Server serveris;

    CBulletContainer bulbox;


    SelectMenu mainmenu;
    SelectMenu netmenu;
    SelectMenu netgame;
    SelectMenu options;
    SelectMenu mapmenu;

    unsigned int imgCount;
    unsigned int maxwavs;

    int frags; //frags of current player
    DArray<int> fragTable;
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

    int clientMyIndex;
    int door_tim;
    int objectivetim;

    RenderTexture screenTexture;
    int fboTextureIndex;

    bool Client_GotMapData;
    bool showMiniMap;
    bool doFadein;
    bool godmode;
    bool slimeswap;


public:
    GameState state;
    MultiplayerModes netGameState;

    DArray<int> clientIds; // how it is in server, info for the new client

    Vector3D OldGamepadLAxis;
    Vector3D OldGamepadRAxis;

    float OldMouseX;
    float OldMouseY;

    int renderer; //renderer index from the config file

    bool showTextInput;

public:

    Game();

    void init(bool useVulkan) override;
    void render() override;
    void renderToFBO(bool useVulkan);
    void renderFBO(bool useVulkan);
    void network();
    void logic() override;
    void destroy() override;
    void loadConfig(const char* path, uint32_t initialWidth, uint32_t initialHeight, int initialRenderIdx) override;
    void onBack(){};
    SystemConfig* getSysConfig(){return sys;}
    EditBox* getIpEdit(){return &ipedit;}

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
    //map name, client count
    void SendMapInfo(int clientIndex, GameMap& map);
    //monster races and item positions
    void SendMapData(int clientIndex, GameMap& map);
    void InitServer();
    void StopServer();
    bool JoinServer(const char* ip, unsigned port);
    void QuitServer();
    void SendServerDoorState(unsigned int clientIndex, int doorx,int doory, unsigned char doorframe);
    void DoorsInteraction();
    void HandleInteractionsWithDeadPlayers();
    void CheckForExit();
    int  slimeReaction(int index);
    void SendServerEquipedCartridgeToAllClients(unsigned dudeIdx, int equipedGame);
    void SendClientEquipedCartridgeIdxToServer(unsigned index);
    void SendClientMeleeImpulseToServer(int victimID, int hp);
    void SendClientShootImpulseToServer(WeaponTypes weaponType);
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
    void SendBulletImpulse(int monsterindex, int ammo, int clientIndex, unsigned char weaponType);
    void GenerateTheMap(int level, int currentHp, int currentAmmo);
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
    void DrawSomeText();
    void ServerParseClientGameEquip(const unsigned char* buffer, unsigned* bufferindex, int clientIndex);
    void ServerParseCharacterData(const unsigned char* bufer, unsigned * buferindex, int clientIndex);
    void ServerParseWeaponShot(const unsigned char* buffer, unsigned * bufferindex, int clientIndex);
    void ServerParseClientResurrect(unsigned* bufferindex, int clientIndex);
    void ServerSendTimerSync(unsigned clientIdx);
    void GetServerTimeMsg(const unsigned char* buffer, int * bufferindex);
    void GetServerCoopGameOverMsg(int* bufferindex);
    void GetServerEquipedGame(const unsigned char* buffer, int * bufferindex);

    void GetCharData(const unsigned char* bufer, int bufersize, int* index);

    void equipCartridge(Dude* dude, int game);

    void DrawMiniMap(int x, int y);
    void LoadMap(const char* mapname, int otherplayers);
    void HelpScreenLogic();
    void EndingLogic();
    int PlayerCount();
    void LoadShader(ShaderProgram* shader, const char* name, bool useVulkan, bool useUVS, SpriteBatcher* pics, bool needAlphaBlend);
    void IntroScreenLogic();
    void PlayNewSong(const char* songName);

    void SendKillCommandToClient(unsigned clientIdx, int victimID);
    void SendFragsToClient(int clientIdx, int frags);

    void DeleteAudio();
    void HandlePlayerAttacks(Dude* player, int clientIndex);
    void CoreGameLogic();
    void DrawNumber(int x, int y, int num);
    void DrawHelp();
};



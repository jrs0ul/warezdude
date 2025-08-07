#include "Game.h"


#include <cstdio>     //sprinf
#include <ctime>
#include <thread>
#include <cassert>

#include "Dude.h"
#include "BulletContainer.h"
#include "TextureLoader.h"
#include "EditBox.h"
#include "bullet.h"
#include "ScroollControl.h"
#include "gui/Text.h"
#include "Usefull.h"
#include "TextureLoader.h"
#include "Matrix.h"
#include "audio/OggStream.h"
#include "Item.h"
#include "Consts.h"
#ifndef _WIN32
#include <arpa/inet.h>
#endif


//===================================GLOBALS

const int slime=35;

const unsigned NetPort=6666;

int slimtim=0;
bool slimeswap=false;

bool godmode=false;

bool ShowMiniMap=false;
bool mapkey=false;
bool mainkey=false;
bool triger=false;

char IntroText[100][100]={0};
int lin=0; //kiek intro eiluciu
int intro_tim=0;
int intro_cline=0;
int intro_gline=0; 
int intro_cchar=0;

int door_tim=0;

bool fadein=true;
int fadetim=0;
int objectivetim=200;

ScroollControl SfxVolumeC, MusicVolumeC;
EditBox ipedit;

int leterKey;

bool Client_GotMapData=false;

//long mousepowx,mousepowy;

bool noAmmo=false;
bool nextWepPressed=false;



//==================================================

Game::Game()
: imgCount(0)
{
    Accumulator = 0;
    DT = 1000.0f/60.0f/1000.0f;
    Works = true;
    TimeTicks = 0;

    DebugMode = 0;

    state = GAMESTATE_TITLE;
    netGameState = MPMODE_COOP;


    otherClientCount = 0;
    maxwavs=0;


    frags = 0;
    mustCollectItems = 0;
    timeleft=0;
    exitSpawned = false;
    showdebugtext=false;
    FirstTime=true;
    ms=0;

    mapas.itmframe = 0;
    itmtim=0;
    netMode = NETMODE_NONE;
}


void AdaptSoundPos(int soundIndex, float soundx,float soundy){

    SoundSystem* ss = SoundSystem::getInstance();
    Vector3D v = Vector3D(soundx, 0, soundy);
    ss->setSoundPos(soundIndex, v.v);
}
//-----------------------------------------------------------
int Game::FPS()
{
    static int ctime = 0, FPS = 0, frames = 0, frames0 = 0;
    if ((int)TimeTicks >= ctime) {
        FPS = frames - frames0;
        ctime = (int)TimeTicks + 1000;
        frames0 = frames;
    }
    frames = frames+1;
    return FPS;
}


//-------------------------------------------

void Game::DeleteAudio(){


    music.stop();
    music.release();


}


//----------------------------------
bool Game::InitAudio()
{

    SoundSystem::getInstance()->init(0);
    SoundSystem::getInstance()->loadFiles("sfx/", "list.txt");

    return true;
}

//----------------------------------------
void Game::PlayNewSong(const char* songName)
{
    if (music.playing())
    {
        music.stop();
        music.release();
    }

    char buf[255];
    sprintf(buf,"music/%s",songName);
    music.open(buf);
    //music.setVolume(sys.musicVolume);
    music.playback();
}


//-------------------

void Game::DrawSomeText()
{

    char buf[255];

    sprintf(buf,"FPS : %d",FPS());
    WriteText(20,20, pics, 10, buf, 0.8f,0.6f);

    sprintf(buf, "Map width %d height %d", mapas.width(), mapas.height());
    WriteText(20, 40, pics, 10, buf, 0.8f,0.8f);

    if (netMode == NETMODE_SERVER)
    {
        sprintf(buf,"Client count : %d",serveris.clientCount());
        WriteText(20, 60, pics, 10,buf, 0.8f, 1);
    }


}


//------------------
void Game::DrawMap(float r=1.0f,float g=1.0f, float b=1.0f)
{

    mapas.draw(pics, r, g, b, sys.ScreenWidth, sys.ScreenHeight);

    bulbox.draw(pics, mapas.getPos().x, mapas.getPos().y, sys.ScreenWidth, sys.ScreenHeight);

    for (int i = 0; i < PlayerCount(); ++i)
    {
        if (mapas.mons[mapas.enemyCount + i].isAlive())
        {
            mapas.mons[mapas.enemyCount + i].draw(pics, 5, mapas.getPos().x, mapas.getPos().y, sys.ScreenWidth, sys.ScreenHeight);
        }
    }

    mapas.drawEntities(pics, sys.ScreenWidth, sys.ScreenHeight);


}

//-------------------------------------------
void Game::DrawMiniMap(int x, int y)
{

    pics.draw(12, x, y, 0, false ,mapas.width(), mapas.height(), 0, COLOR(1,1,1, 0.6f), COLOR(1,1,1, 0.6f));

    for (unsigned i = 0; i < mapas.height(); i++)
    {
        for (unsigned a = 0; a < mapas.width(); a++)
        {
            int frame=0;

            if ((mapas.tiles[i][a]!=65) && (mapas.tiles[i][a]!=67) && (mapas.tiles[i][a]!=69)
                && (mapas.tiles[i][a]!=71))
            {
                frame = mapas.colide(a, i);
            }

            if (frame)
            {
                pics.draw(12, a*4+x,i*4+y,frame, false, 1.f, 1.f, 0.f, COLOR(1,1,1,0.6f), COLOR(1,1,1,0.6f));
            }
        }
    }

        pics.draw(12, x+(round(mapas.mons[mapas.enemyCount].x/32.0f)*4),
                                            y+(round(mapas.mons[mapas.enemyCount].y/32.0f)*4),
                                            3, false);

        for (unsigned i = 0; i<mapas.items.count(); i++)
        {
            pics.draw(12, x+(round(mapas.items[i].x/32.0f)*4),
                          y+(round(mapas.items[i].y/32.0f)*4),
                                            4, false);
        }


}

//---------------------
//sudelioja 3zenkli skaiciu is bmp
void Game::DrawNum(int x, int y,int num)
{
    int arr[3]={0};

    int i=2;
    while (num!=0)
    {
        arr[i]=num%10;
        num=num/10;
        i--;
    }

    for (int a = 0; a < 3; a++)
    {
        pics.draw(8, x + a*16, y, arr[a], false, 0.98f, 1.f, 0.f, COLOR(1.f, 1.f, 1.f, 0.6f), COLOR(1.f, 1.f, 1.f, 0.6f));
    }
}
//-------------------------------
void Game::SendItemCreation(float x, float y, int value, unsigned int clientIndex)
{

    char buferis[MAX_MESSAGE_DATA_SIZE];
    int index = 0;

    memcpy(&buferis[index], "WD3", 3);
    index += 3;
    buferis[index] = NET_SERVER_MSG_NEW_ITEM;
    ++index;
    memcpy(&buferis[index], &x, sizeof(float));
    index += sizeof(float);
    memcpy(&buferis[index], &y, sizeof(float));
    index += sizeof(float);
    memcpy(&buferis[index], &value, sizeof(int));
    index += sizeof(int);
    serveris.sendData(clientIndex, buferis, index);

}

//---------------------------
void Game::KillPlayer(int index)
{
    mapas.mons[mapas.enemyCount+index].shot=true;
    mapas.mons[mapas.enemyCount+index].frame=mapas.mons[mapas.enemyCount+index].weaponCount*4;
    AdaptSoundPos(2,mapas.mons[mapas.enemyCount+index].x,mapas.mons[mapas.enemyCount+index].y);
    SoundSystem::getInstance()->playsound(2);

    mapas.mons[mapas.enemyCount+index].stim=0;
    timeleft=mapas.timeToComplete;
    mapas.mons[mapas.enemyCount+index].setHP(100);

    Decal decalas;
    decalas.x = round(mapas.mons[mapas.enemyCount+index].x);
    decalas.y = round(mapas.mons[mapas.enemyCount+index].y);
    decalas.color = COLOR(1.f, 0.f, 0.f);
    decalas.frame = rand() % 2;
    mapas.decals.add(decalas);
}

//------------------
void Game::KillEnemy(unsigned ID)
{
    printf("KILLED BY %d\n", mapas.mons[ID].lastDamagedBy);

    if (ID < (unsigned)mapas.enemyCount)
    {
        mapas.mons[ID].shot=true;

        AdaptSoundPos(3,mapas.mons[ID].x,mapas.mons[ID].y);
        SoundSystem::getInstance()->playsound(3);

        Decal decalas;
        decalas.x = round(mapas.mons[ID].x);
        decalas.y = round(mapas.mons[ID].y);
        decalas.color = COLOR(1.f, 0.f, 0.f);
        decalas.frame = rand() % 2;
        mapas.decals.add(decalas);

        if (mapas.mons[ID].item) // if monster swallowed an item
        {
            Item swallowedItem;
            swallowedItem.value = mapas.mons[ID].item;
            swallowedItem.x     = mapas.mons[ID].x;
            swallowedItem.y     = mapas.mons[ID].y;
            mapas.addItem(swallowedItem.x, swallowedItem.y, swallowedItem.value);

            mapas.mons[ID].item = 0;

            if (netMode == NETMODE_SERVER)
            {
                for (unsigned int i = 0; i < serveris.clientCount(); i++)
                {
                    SendItemCreation(swallowedItem.x, swallowedItem.y, swallowedItem.value, i);
                }
            }

        }
    }
    else
    {
        KillPlayer(ID - 254);
    }



}



//--------------------
//jei kulka pataike i kazka
bool Game::OnHit(Bullet& bul)
{

    int dmg = PROJECTILE_BULLET_DAMAGE;

    if (bul.isMine)
    {
        dmg = PROJECTILE_MINE_DAMAGE;
    }

    bool hit = false;
    int tmpID=0;

    int players = PlayerCount();

    for (int i=0; i < mapas.enemyCount + players; i++)
    {
        if (CirclesColide(mapas.mons[i].x,mapas.mons[i].y,8,bul.x,bul.y,8))
        {

            tmpID = mapas.mons[i].id;

            if ((bul.parentID!=tmpID) && //bullet should not hit the shooter
                (!mapas.mons[i].shot) &&
                (!mapas.mons[i].spawn))
            {
                if (!hit)
                {
                    hit = true;
                }

                mapas.mons[i].hit = true;
                mapas.mons[i].damage(dmg);
                mapas.mons[i].lastDamagedBy = bul.parentID;

            }
        }
    }

    return hit;
}


//---------------------------
//nupaiso kiek turi gyvybiu soviniu etc
void Game::DrawStats()
{
    const unsigned STAT_ICON = 9;

    pics.draw(STAT_ICON,
              30,
              sys.ScreenHeight - 40,
              2, false, 1.f, 1.f, 0.f, COLOR(1.f, 1.f, 1.f, 0.6f), COLOR(1.f, 1.f, 1.f, 0.6f));
    DrawNum(58,
            sys.ScreenHeight - 40,
            mapas.mons[mapas.enemyCount].getHP());


    pics.draw(STAT_ICON,
              120,
              sys.ScreenHeight - 40,
              0, false, 1.f, 1.f, 0.f, COLOR(1.f, 1.f, 1.f, 0.6f), COLOR(1.f, 1.f, 1.f, 0.6f)); 
    DrawNum(155,
            sys.ScreenHeight - 40,
            mapas.mons[mapas.enemyCount].ammo);

    if (mapas.misionItems && netGameState == MPMODE_COOP)
    {
        pics.draw(STAT_ICON,
                  220,
                  sys.ScreenHeight - 40,
                  1, false, 1.f, 1.f, 0.f, COLOR(1.f, 1.f, 1.f, 0.6f), COLOR(1.f, 1.f, 1.f, 0.6f));
        DrawNum(255,
                sys.ScreenHeight - 40,
                mustCollectItems);
    }

    if (netGameState == MPMODE_DEATHMATCH)
    {
        pics.draw(STAT_ICON, 220, 440, 3, false, 1.f, 1.f, 0.f, COLOR(1.f, 1.f, 1.f, 0.6f), COLOR(1.f, 1.f, 1.f, 0.6f));
        DrawNum(255,440, frags);
    }


    if (mapas.timeToComplete && netGameState == MPMODE_COOP)
    {
        int min = timeleft / 60;
        int sec = timeleft - min * 60;
        char buf[50];
        sprintf(buf,"%d:%d",min,sec);
        WriteText(450, 20, pics, 10, buf, 0.7f,1.5f, COLOR(1.0f,0.5f,0.5f, 1.f), COLOR(1.0f,0.5f,0.5f, 1.f));
    }

}

//------------------------
//the hero and camera movement
void Game::MoveDude()
{
    Dude* player = mapas.getPlayer();

    if ((Keys[0]) || (Keys[1]) || (Keys[2]) || (Keys[3]))
    {
        float walkSpeed = 0.f;
        float strifeSpeed = 0.f;

        if (Keys[1])
        {
            walkSpeed = -1.0f;
        }

        if (Keys[0])
        {
            walkSpeed = 1.0f;
        }

        if (Keys[2])
        {
            strifeSpeed = -1.0f;
        }

        if (Keys[3])
        {
            strifeSpeed = 1.0f;
        }


        int characterCount = (netMode == NETMODE_SERVER) ? mapas.enemyCount + serveris.clientCount() + 1 :
                                                           mapas.enemyCount + otherClientCount + 1;

        player->move(walkSpeed, strifeSpeed, PLAYER_RADIUS, (const bool**)mapas._colide,
                     mapas.width(), mapas.height(), mapas.mons,
                     characterCount);

        AdaptMapView();

    }

    

}



//--------------
void Game::AdaptMapView()
{

    Dude* player = mapas.getPlayer();
    const float HALF_SCREEN_W = sys.ScreenWidth / 2.f;
    const float HALF_SCREEN_H = sys.ScreenHeight / 2.f;

    float posX = HALF_SCREEN_W - player->x;

    if (posX > HALF_TILE_WIDTH)
    {
        posX = HALF_TILE_WIDTH;
    }
    else if (posX < -1.f * (mapas.width() * TILE_WIDTH - sys.ScreenWidth - HALF_TILE_WIDTH))
    {
        posX = -1.f * (mapas.width() * TILE_WIDTH - sys.ScreenWidth - HALF_TILE_WIDTH);
    }


    float posY = HALF_SCREEN_H - player->y;

    if (posY > HALF_TILE_WIDTH)
    {
        posY = HALF_TILE_WIDTH;
    }
    else if (posY < -1.f * (mapas.height() * TILE_WIDTH - sys.ScreenHeight - HALF_TILE_WIDTH))
    {
        posY = -1.f * (mapas.height() * TILE_WIDTH - sys.ScreenHeight - HALF_TILE_WIDTH);
    }



    if ((int)(mapas.width() * TILE_WIDTH) <= sys.ScreenWidth )
    {
        posX = HALF_SCREEN_W - (mapas.width() * HALF_TILE_WIDTH - HALF_TILE_WIDTH);
    }

    if ((int)(mapas.height() * TILE_WIDTH) <= sys.ScreenHeight)
    {
        posY = HALF_SCREEN_H- (mapas.height() * HALF_TILE_WIDTH - HALF_TILE_WIDTH);
    }


    mapas.setPosX(posX);
    mapas.setPosY(posY);

}

//---------------------------
void Game::PutExit(){

    if (netGameState == MPMODE_DEATHMATCH)
    {
        return;
    }

    int ex = mapas.exit.x;
    int ey = mapas.exit.y;

    mapas.addItem(ex * TILE_WIDTH, ey * TILE_WIDTH, ITEM_EXIT);
    exitSpawned = true;
}

//----------------------------
void Game::GoToLevel(int level, int otherplayer)
{
    exitSpawned = false;

    if (netGameState == MPMODE_DEATHMATCH)
    {
        char mapname[255];
        mapai.getMapName(level,mapname);

        LoadTheMap(mapname, true, otherplayer);
    }
    else
    {
        GenerateTheMap();
    }

    fadein = true;
    objectivetim = 200;

    if (ShowMiniMap)
    {
        ShowMiniMap = false;
    }
}
//----------------------------------
void Game::SendKillCommandToClient(unsigned clientIdx, int victimId)
{
    char buffer[MAX_MESSAGE_DATA_SIZE];
    int pos = 0;

    memcpy(&buffer[pos], "WD3", 3);
    pos += 3;
    buffer[pos] = NET_SERVER_MSG_KILL_CHARACTER;
    ++pos;

    memcpy(&buffer[pos], &victimId, sizeof(int));
    pos += sizeof(int);
    serveris.sendData(clientIdx, buffer, pos);
}
//----------------------------------
void Game::SendFragsToClient(int clientIdx, int frags)
{
    char buffer[MAX_MESSAGE_DATA_SIZE];
    int len = 0;
    memcpy(&buffer[len], "WD3", 3);
    len += 3;
    buffer[len] = NET_SERVER_MSG_FRAG;
    ++len;
    memcpy(&buffer[len], &frags, sizeof(int));
    len += sizeof(int);

    serveris.sendData(clientIdx, buffer, len);
}
//-----------------------------------
//klientas servui isiuncia infa apie paimta daikta
void Game::SendItemCRemove(int itemIndex)
{
    char bufer[MAX_MESSAGE_DATA_SIZE];
    int index = 0;
    bufer[index] = NET_CLIENT_MSG_ITEM;
    ++index;
    memcpy(&bufer[index], &itemIndex, sizeof(int));
    index += sizeof(int);
    client.sendData(bufer, index);
}
//-----------------------------------
void Game::SendItemSRemove(int ItemIndex, int clientIndex, bool playerTaked)
{
    char bufer[MAX_MESSAGE_DATA_SIZE];
    int index = 0;
    memcpy(&bufer[index], "WD3", 3);
    index += 3;
    bufer[index] = NET_SERVER_MSG_ITEM;
    ++index;
    memcpy(&bufer[index],&ItemIndex,sizeof(int));
    index += sizeof(int);
    unsigned char plt=0;

    if (playerTaked)
    {
        plt=1;
    }

    memcpy(&bufer[index],&plt,sizeof(unsigned char));
    index++;
    serveris.sendData(clientIndex,bufer,index);
}

//-------------------------------------
void Game::SendMapInfo(int clientIndex, CMap& map)
{
    char bufer[MAX_MESSAGE_DATA_SIZE];
    int index = 0;
    memcpy(&bufer[index], "WD3", 3);
    index += 3;
    bufer[index] = NET_SERVER_MSG_SERVER_INFO;

    ++index;
    int len = (int)strlen(map.name); 
    int totalpacketlen = len + sizeof(int) + 1 + 1;

    memcpy(&bufer[index], &totalpacketlen,sizeof(int)); //paketo dydis
    index += sizeof(int);

    memcpy(&bufer[index], &len, sizeof(int)); //mapo pavadinimo ilgis
    index += sizeof(int);

    memcpy(&bufer[index], map.name, strlen(map.name)); //mapo pavadinimas
    index += (int)strlen(map.name);

    unsigned int kiekis = serveris.clientCount(); 
    memcpy(&bufer[index], &kiekis, sizeof(unsigned int)); //client count
    index += sizeof(unsigned int);

    char gameType = (char)netGameState;
    bufer[index] = gameType;
    ++index;

    serveris.sendData(clientIndex,bufer,index);

}
//------------------------------------------------
void Game::SendMapData(int clientIndex, CMap& map)
{

    char bufer[MAX_MESSAGE_DATA_SIZE];

    int index = 0;


    memcpy(&bufer[index], "WD3", 3);
    index += 3;
    bufer[index] = NET_SERVER_MSG_MAP_DATA;
    ++index;

    int height = (netGameState == MPMODE_DEATHMATCH) ? 0 : map.height();
    int width =  (netGameState == MPMODE_DEATHMATCH) ? 0 : map.width();

    memcpy(&bufer[index], &height, sizeof(int));
    index += sizeof(int);
    memcpy(&bufer[index], &width, sizeof(int));
    index += sizeof(int);

    if (width && height)
    {
        unsigned char tile = 0;

        for (unsigned i = 0; i < map.height(); ++i)
        {
            for (unsigned a = 0; a < map.width(); ++a)
            {
                tile = map.tiles[i][a];
                memcpy(&bufer[index], &tile, sizeof(unsigned char));
                index += sizeof(unsigned char);
            }
        }
    }

    assert(index < MAX_MESSAGE_DATA_SIZE);

    memcpy(&bufer[index], &mustCollectItems, sizeof(int));
    index += sizeof(int);

    memcpy(&bufer[index], &map.enemyCount, sizeof(int));
    index += sizeof(int);

    for (int i = 0; i < map.enemyCount; i++)
    {
        memcpy(&bufer[index], &map.mons[i].race, sizeof(int));
        index += sizeof(int);
    }

    int itmcount = (int)map.items.count();
    memcpy(&bufer[index], &itmcount, sizeof(int));
    index+=sizeof(int);

    for (unsigned long i = 0; i < map.items.count(); i++)
    {
        memcpy(&bufer[index], &map.items[i].x,sizeof(float));
        index += sizeof(float);
        memcpy(&bufer[index], &map.items[i].y,sizeof(float));
        index += sizeof(float);
        memcpy(&bufer[index], &map.items[i].value,sizeof(int));
        index += sizeof(int);

    }

    serveris.sendData(clientIndex,bufer,index);
}

//---------------------------------
//siuncia servui msg kad keistu mapa
void Game::SendWarpMessage()
{
    char buferis[MAX_MESSAGE_DATA_SIZE];
    int index = 0;
    buferis[index] = NET_CLIENT_MSG_NEXT_LEVEL;
    ++index;
    client.sendData(buferis, index);
}


//-------------------------
void Game::ItemPickup()
{

    for (unsigned long i=0; i<mapas.items.count(); i++)
    {
        if (CirclesColide(mapas.mons[mapas.enemyCount].x,
                    mapas.mons[mapas.enemyCount].y,8,
                    mapas.items[i].x,mapas.items[i].y,4))
        {

            int item=mapas.items[i].value;
            if ((item != 0) && ((item != 6) || (mapas.mons[mapas.enemyCount].getHP() < 100)))
            { //daiktas bus paimtas

                SoundSystem* ss = SoundSystem::getInstance();

                if ((item != 6) && (item != 3) && (item != 4))
                {
                    AdaptSoundPos(1, mapas.mons[mapas.enemyCount].x,mapas.mons[mapas.enemyCount].y);
                    ss->playsound(1);
                }
                else
                {
                    if (item==6)
                    {//hp up
                        AdaptSoundPos(6, mapas.mons[mapas.enemyCount].x,mapas.mons[mapas.enemyCount].y);
                        ss->playsound(6);  
                    }
                    else
                    {
                        if (item==3)
                        {
                            AdaptSoundPos(7,mapas.mons[mapas.enemyCount].x,mapas.mons[mapas.enemyCount].y);
                            ss->playsound(7);
                        }
                    }
                }


                //siunciam infa----------
                switch (netMode)
                {
                    case NETMODE_NONE : break;
                    case NETMODE_CLIENT : SendItemCRemove(i); break;
                    case NETMODE_SERVER :
                    {
                        for (unsigned int a=0; a<serveris.clientCount(); a++)
                        {
                            SendItemSRemove(i, a, true);
                        }
                    }
                }

                //------------------------
                mapas.removeItem(i);



                if ((item < ITEM_AMMO_PACK) && (item > 0))
                {
                    mustCollectItems--;
                }

                else
                    if (item==5)
                        mapas.mons[mapas.enemyCount].ammo+=20;
                    else
                        if (item==6)
                            mapas.mons[mapas.enemyCount].heal();

            }

            //exitas
            if (item==4){
                
            }
        }

    }

}
//---------------------------------
void Game::InitServer()
{
    netMode = NETMODE_SERVER;

    printf("Launching server on port: %d\n", NetPort);

    if (serveris.launch(NetPort))
    {
        printf("Server launched!\n");

        std::thread t([&]
        {
            while(serveris.isRunning())
            {
                std::lock_guard<std::mutex> lock(messageMutex);
                serveris.getData();
            }
        });

        t.detach();
    }
    else
    {
        printf("SERVER LAUNCH FAILED!\n");
    }
}
//--------------------------------
void Game::StopServer()
{

    int cnt = 0;
    char buffer[MAX_MESSAGE_DATA_SIZE]{};

    memcpy(&buffer[cnt], "WD3", 3);
    cnt += 3;
    buffer[cnt] = NET_SERVER_MSG_SHUTTING_DOWN;
    ++cnt;

    for (unsigned i = 0; i < serveris.clientCount(); ++i)
    {
        serveris.sendData(i, buffer, cnt);
    }

    netMode = NETMODE_NONE;
    serveris.shutDown();
}
//---------------------------------
bool Game::JoinServer(const char* ip, unsigned port)
{
    if (client.open())
    {
        std::thread c([&]
            {
                while(client.isOpen())
                {
                    std::lock_guard<std::mutex> lock(messageMutex);
                    client.getData();
                }
            }
        );

        c.detach();

        char buf[4];
        buf[0] = NET_CLIENT_MSG_CONNECT;

        sockaddr_in serverAddress;
        memset(&serverAddress, 0, sizeof(sockaddr_in));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr(ip);
        serverAddress.sin_port = htons(port);

        client.setServerAddress(serverAddress);
        client.sendData(buf, 1);

        netMode = NETMODE_CLIENT;
        return true;
    }

    return false;
}
//--------------------------
void Game::QuitServer()
{
    netMode = NETMODE_NONE;
    char buf[4];
    buf[0] = NET_CLIENT_MSG_QUIT;
    client.sendData(buf, 1);
    client.shutdown();
}

//--------------------------------
void Game::SendClientDoorState(int doorx,int doory, unsigned char doorframe)
{
    int index = 0;
    char bufer[MAX_MESSAGE_DATA_SIZE];
    bufer[0] = NET_CLIENT_MSG_DOOR;
    ++index;
    memcpy(&bufer[index], &doorx, sizeof(int));
    index += sizeof(int);
    memcpy(&bufer[index], &doory, sizeof(int));
    index += sizeof(int);
    memcpy(&bufer[index], &doorframe, sizeof(unsigned char));
    index += sizeof(unsigned char);
    client.sendData(bufer, index);
}
//--------------------------------
void Game::SendServerDoorState(unsigned int clientIndex, int doorx,int doory, unsigned char doorframe)
{
    int index = 0;
    char bufer[MAX_MESSAGE_DATA_SIZE];

    memcpy(&bufer[index], "WD3", 3);
    index += 3;
    bufer[index] = NET_SERVER_MSG_DOOR;
    ++index;
    memcpy(&bufer[index],&doorx,sizeof(int));
    index += sizeof(int);
    memcpy(&bufer[index],&doory,sizeof(int));
    index += sizeof(int);
    memcpy(&bufer[index],&doorframe,sizeof(unsigned char));
    index += sizeof(unsigned char);

    serveris.sendData(clientIndex,bufer,index);
}

//--------------------------------
//viskas kas susije su durimis
void Game::DoorsInteraction()
{

    if (door_tim == 0)
    {
        Vector3D vec = MakeVector(20.0f, 0, mapas.mons[mapas.enemyCount].angle);

        int drx=round(((mapas.mons[mapas.enemyCount].x+vec.x)/32.0f));
        int dry=round(((mapas.mons[mapas.enemyCount].y-vec.y)/32.0f));

        SoundSystem* ss = SoundSystem::getInstance();


        if (Keys[ACTION_OPEN])
        {
            if ((mapas.tiles[dry][drx]==67)
                ||(mapas.tiles[dry][drx]==65)
                ||(mapas.tiles[dry][drx]==69)
                ||(mapas.tiles[dry][drx]==71))
            {// atidarom
                mapas.tiles[dry][drx]++;
                mapas._colide[dry][drx] = false;
                door_tim = 1;
                AdaptSoundPos(8, mapas.mons[mapas.enemyCount].x, mapas.mons[mapas.enemyCount].y);
                ss->playsound(8);
            }
            else                        //uzdarom
                if ((mapas.tiles[dry][drx]==68)
                    ||(mapas.tiles[dry][drx]==66)
                    ||(mapas.tiles[dry][drx]==70)
                    ||(mapas.tiles[dry][drx]==72)
                    )
            {

                if (!CirclesColide(mapas.mons[mapas.enemyCount].x,mapas.mons[mapas.enemyCount].y,16.0f,drx*32.0f,dry*32.0f,8.0f))
                {
                    mapas.tiles[dry][drx]--;
                    mapas._colide[dry][drx] = true;
                    door_tim=1;
                    AdaptSoundPos(9, mapas.mons[mapas.enemyCount].x, mapas.mons[mapas.enemyCount].y);
                    ss->playsound(9);
                }
            }


            switch(netMode)
            {
                case NETMODE_NONE: break;
                case NETMODE_SERVER:
                {
                    for (unsigned int i = 0; i < serveris.clientCount(); i++)
                    {
                        SendServerDoorState(i, drx, dry, mapas.tiles[dry][drx]);
                    }
                 } break;
                case NETMODE_CLIENT:
                {
                    SendClientDoorState(drx,dry,mapas.tiles[dry][drx]);
                }
            }

        }
    } 
    else
    {
        door_tim++;
        if (door_tim == 30)
        {
            door_tim=0;
        }
    }

}
//-----------------------------
void Game::CheckForExit()
{
    int playerX = round(mapas.getPlayer()->x / 32);
    int playerY = round(mapas.getPlayer()->y / 32);

    //printf("%d %d %d\n", playerY, playerX, mapas.tiles[playerY][playerX]);

    if (mapas.tiles[playerY][playerX] == 81)
    {
        mapai.current++;
        if (mapai.current == MAP_COUNT)
        {
            mapai.current=0;
            state = GAMESTATE_ENDING;
            PlayNewSong("crazy.ogg");

        }

        int otherPlayers = (netMode == NETMODE_SERVER) ? serveris.clientCount() : 0;

        if (netMode == NETMODE_CLIENT)
        {
            SendWarpMessage();
        }
        else
        {
            GoToLevel(mapai.current, otherPlayers); //server, offline
        }


        if (netMode == NETMODE_SERVER)
        {
            for (int a = 0; a < (int)serveris.clientCount(); ++a)
            {
                SendMapInfo(a, mapas);
                SendMapData(a, mapas);
            }

        }

    }


}

//-------
void Game::SlimeReaction(int index)
{

    const int montileY = round(mapas.mons[index].y/32);
    const int montileX = round(mapas.mons[index].x/32);

    if ((!mapas.mons[index].shot)&&(!mapas.mons[index].spawn)&&(!godmode)
        &&((mapas.tiles[montileY][montileX] == slime)
        ||(mapas.tiles[montileY][montileX] == slime + 1))&&
        (slimtim == 5))
    {
        mapas.mons[index].damage(SLIME_DAMAGE);
        mapas.mons[index].lastDamagedBy = DAMAGER_NOBODY;
        mapas.mons[index].hit = true;
    }
}
//-----------------------------
void Game::SendBulletImpulse(int monsterindex, int ammo, int clientIndex, bool isMine)
{
    int index = 0;
    char buf[MAX_MESSAGE_DATA_SIZE];

    memcpy(&buf[index], "WD3", 3);
    index += 3;
    buf[index] = NET_SERVER_MSG_WEAPON_SHOT;
    ++index;
    memcpy(&buf[index], &monsterindex, sizeof(int));
    index += sizeof(int);
    memcpy(&buf[index], &ammo, sizeof(int));
    index += sizeof(int);
    memcpy(&buf[index], &isMine, sizeof(unsigned char));
    index += sizeof(unsigned char);

    serveris.sendData(clientIndex, buf, index);
}
//------------------------------------------
void Game::SendClientAtackImpulse(int victimID, int hp)
{
    char buferis[MAX_MESSAGE_DATA_SIZE];
    int index = 0;
    buferis[index] = NET_CLIENT_MSG_MELEE_ATTACK;
    ++index;
    memcpy(&buferis[index], &victimID, sizeof(int));
    index += sizeof(int);
    memcpy(&buferis[index], &hp, sizeof(int));
    index += sizeof(int);
    client.sendData(buferis, index);
}
//----------------------------------------
void Game::SendAtackImpulse(unsigned int clientIndex, int victim, int hp)
{
    char buferis[MAX_MESSAGE_DATA_SIZE];
    int index = 0;

    memcpy(&buferis[index], "WD3", 3);
    index += 3;
    buferis[index] = NET_SERVER_MSG_MELEE_ATTACK;
    ++index;
    memcpy(&buferis[index],&victim,sizeof(int));
    index += sizeof(int);
    memcpy(&buferis[index],&hp,sizeof(int));
    index += sizeof(int);
    serveris.sendData(clientIndex,buferis,index);
}

//----------------------------------
void Game::BeatEnemy(int aID, int damage)
{
    if (mapas.mons[aID].canAtack)
    {
        mapas.mons[aID].shoot(false, false, &bulbox);
        Vector3D vec = MakeVector(16.0f, 0, mapas.mons[aID].angle);

        for (unsigned long i = 0; i < mapas.mons.count(); ++i)
        {

            if (mapas.mons[aID].hitIt(mapas.mons[i], vec.x, vec.y, damage) > -1)
            {

                switch(netMode)
                {
                    case NETMODE_NONE: break;
                    case NETMODE_SERVER:
                                       {
                                           for (unsigned a = 0; a < serveris.clientCount(); a++)
                                           {

                                               int z = i;

                                               if (i >= (unsigned)mapas.enemyCount)
                                               {
                                                   if ((i - mapas.enemyCount - 1) < a)
                                                   {
                                                       z = i + 1;
                                                   }

                                                   if ((i - mapas.enemyCount - 1) == a)
                                                   {
                                                       z = mapas.enemyCount;
                                                   }
                                               }

                                               SendAtackImpulse(a, z, mapas.mons[i].getHP());
                                           }
                                       } break;
                    case NETMODE_CLIENT:
                                       {
                                           SendClientAtackImpulse(mapas.mons[i].id, mapas.mons[i].getHP());
                                       }
                }

            }

        }
    }
}



//-------------------------
void Game::MonsterAI(int index)
{
    SoundSystem * ss = SoundSystem::getInstance();

    int linijosIlgis=0;

    int tmpcnt = bulbox.count();

    if (mapas.mons[index].spawn)
    {
        mapas.mons[index].respawn();


        //kill entity if you spawn on it
        if (!mapas.mons[index].spawn)
        {
            for (unsigned i=0; i< mapas.mons.count(); i++)
            {
                if (i != (unsigned)index)
                {
                    if (CirclesColide(mapas.mons[index].x,mapas.mons[index].y,10.0f,mapas.mons[i].x,mapas.mons[i].y,8.0f))
                    {
                        if (i < (unsigned)mapas.enemyCount)
                        {
                            KillEnemy(i);
                        }
                        else
                        {
                            KillPlayer(i-mapas.enemyCount);
                        }
                    }
                }
            }

        }
    }

    if (!mapas.mons[index].shot && !mapas.mons[index].spawn)
    {
        bool movedFreely = mapas.mons[index].move(1.0f, 0.0f, 8.0f, 
                               (const bool**)mapas._colide, mapas.width(), mapas.height(),
                               mapas.mons, mapas.enemyCount + 1 + serveris.clientCount());

        if (!movedFreely)
        {
            mapas.mons[index].rotate(0.1f);
        }


        //1 kas kolidina super duper figuroje
        int kiekplayeriu = 1;

        if (netMode == NETMODE_SERVER)
        {
            kiekplayeriu += serveris.clientCount();
        }

        int victimindex=0;

        for (int i = mapas.enemyCount; i < mapas.enemyCount + kiekplayeriu; ++i)
        {
            if ((CirclesColide(mapas.mons[i].x,mapas.mons[i].y,16,mapas.mons[index].x,mapas.mons[index].y,128))
                &&(!mapas.mons[i].shot)&&(!mapas.mons[i].spawn))
            {
                victimindex=i;
                break;
            }
        }

        if (victimindex)
        {
            //2 rask spindulio kelia iki pirmojo kolidinancio
            Vector3D * line = Line(round(mapas.mons[index].x),
                    round(mapas.mons[index].y),
                    round(mapas.mons[victimindex].x),
                    round(mapas.mons[victimindex].y),
                    linijosIlgis, 32);

            if (linijosIlgis)
            {
                bool colide=false;
                for(int i=0; i < linijosIlgis; i++)
                {

                    unsigned ly = (unsigned)line[i].y;
                    unsigned lx = (unsigned)line[i].x;

                    if ((ly >= mapas.height()) || (lx >= mapas.width()))
                    {
                        colide = true;
                        break;
                    }

                    if (mapas.colide(lx, ly))
                    {
                        colide = true;
                        break;
                    }

                }
                if (!colide)
                {
                    mapas.mons[index].enemyseen = true;
                    mapas.mons[index].angle = atan2(mapas.mons[victimindex].y - mapas.mons[index].y,
                            mapas.mons[victimindex].x - mapas.mons[index].x);
                }
                delete []line;
            }
        }

        if ((mapas.mons[index].enemyseen))
        {
            if (mapas.mons[index].race == 3)
            {//jei mentas, tai pasaudom
                mapas.mons[index].enemyseen = false;

                if (mapas.mons[index].canAtack)
                {
                    mapas.mons[index].shoot(true,false,&bulbox);
                    mapas.mons[index].ammo++;
                    mapas.mons[index].reloadtime=0;

                    if (netMode == NETMODE_SERVER)
                    {
                        for (unsigned int i=0;i < serveris.clientCount(); i++)
                        {
                            SendBulletImpulse(index,mapas.mons[index].ammo,i, false);
                        }
                    }
                }
                else
                {
                    mapas.mons[index].reload(50);
                }
            }
            else
            {//baigiam saudima ;)
                if ((linijosIlgis<2) && (linijosIlgis))
                {
                    BeatEnemy(index,10);
                    mapas.mons[index].enemyseen=false;
                    if (!mapas.mons[index].canAtack)
                    {
                        mapas.mons[index].reload(25);
                    }
                }
            }
        }
       

    }

    else if (mapas.mons[index].shot)
    { //jei mus kazkas pasove shot=true
        mapas.mons[index].splatter();

        if (!mapas.mons[index].shot)
        {
            mapas.mons[index].initMonsterHP();
            mapas.mons[index].appearInRandomPlace(mapas._colide, mapas.width(), mapas.height());
        }
    }

//visa sita slamasta reiks sukrauti i virsu kur tikrinama ar ne shot

    if ((!mapas.mons[index].shot)&&(!mapas.mons[index].spawn))
    {

        if (mapas.mons[index].hit)
        {
            mapas.mons[index].damageAnim();
            if (!mapas.mons[index].hit)
            {
                AdaptSoundPos(11,mapas.mons[index].x,mapas.mons[index].y);
                ss->playsound(11);
            }
        }

        //eats items

        if (mapas.mons[index].race == 2)
        {
            for (unsigned i=0; i < mapas.items.count(); i++)
            {
                if ((!mapas.mons[index].item) && (mapas.items[i].value != ITEM_EXIT))
                {
                    if (CirclesColide(mapas.items[i].x, mapas.items[i].y, 8.0f, mapas.mons[index].x,mapas.mons[index].y,16.0f))
                    {
                        mapas.mons[index].item = mapas.items[i].value;
                        AdaptSoundPos(5,mapas.items[i].x,mapas.items[i].y);
                        ss->playsound(5);

                        if (netMode == NETMODE_SERVER)
                        {
                            for (unsigned a = 0; a < serveris.clientCount(); ++a)
                            {
                                SendItemSRemove(i, a, false);
                            }
                        }

                        mapas.removeItem(i);
                    }
                }

            }

        }

    }

    if (bulbox.count()>tmpcnt)
    {
        AdaptSoundPos(0,mapas.mons[index].x,mapas.mons[index].y);
        ss->playsound(0);
    }

}
//--------------------------------
//kulku valdymas
void Game::HandleBullets()
{

    for (int i=0;i<bulbox.count();i++){
        bulbox.buls[i].ai(mapas._colide, mapas.width(), mapas.height());

        if (OnHit(bulbox.buls[i]))
            if (!bulbox.buls[i].explode)
            {
                bulbox.buls[i].explode = true;
                bulbox.buls[i].frame=2;
            }

        bulbox.removeDead(); //pasaliname neaktyvias kulkas
    }   
}
//-------------------------------
void Game::AnimateSlime()
{
    if (mapas.tiles){
        slimtim++;
        if (slimtim>25){
            slimtim=0;
            if (!slimeswap){
                mapas.ReplaceTiles(slime,slime+1);
                slimeswap=true;
            }
            else{
                mapas.ReplaceTiles(slime+1,slime);
                slimeswap=false;
            }

        }
    }
}
//-------------------------------------------------------
void Game::GenerateTheMap()
{
    bulbox.destroy();
    mapas.destroy();

    mapas.generate();


    mapas.buildCollisionmap();

    mapas.exit.x = rand() % mapas.width();
    mapas.exit.y = rand() % mapas.height();

    while(mapas._colide[(int)mapas.exit.y][(int)mapas.exit.x])
    {
        mapas.exit.x = rand() % mapas.width();
        mapas.exit.y = rand() % mapas.height();
    }

    mapas.start.x = rand() % mapas.width();
    mapas.start.y = rand() % mapas.height();

    while(mapas._colide[(int)mapas.start.y][(int)mapas.start.x])
    {
        mapas.start.x = rand() % mapas.width();
        mapas.start.y = rand() % mapas.height();
    }

    mapas.start.x *= 32;
    mapas.start.y *= 32;


    mapas.tiles[(int)mapas.exit.y][(int)mapas.exit.x] = TILE_EXIT;

    mapas.enemyCount = 2 + rand() % 10;

    mapas.misionItems = 4;
    mustCollectItems = mapas.misionItems;
    mapas.goods = 10;

    for (int i = 0; i < mapas.enemyCount; ++i)
    {
        Dude m;
        m.id = i;
        m.race = rand() % MONSTER_MAX_RACE + 1;
        m.initMonsterHP();
        m.appearInRandomPlace(mapas._colide, mapas.width(), mapas.height());
        mapas.mons.add(m);
    }

    Dude d;
    mapas.addMonster(d);

    Dude* player = mapas.getPlayer();
    player->appearInRandomPlace(mapas._colide, mapas.width(), mapas.height());
    player->id = 254;
    player->weaponCount = 3;
    player->currentWeapon = 1;
    player->frame = (player->currentWeapon + 1) * 4 - 2;


    for (int i = 0; i < PlayerCount() - 1; ++i)
    {
        Dude p;
        p.appearInRandomPlace(mapas._colide, mapas.width(), mapas.height());
        p.id = mapas.mons[mapas.mons.count() - 1].id + 1;
        mapas.mons.add(p);

    }

    mapas.arangeItems();

    AdaptMapView();
}


//-------------------------------------------------------
void Game::LoadTheMap(const char* name, bool createItems, int otherPlayers)
{
    bulbox.destroy();
    mapas.destroy();

    if (!mapas.load(name, createItems, otherPlayers))
    {
        mapas.destroy();
        mapai.Destroy();
        printf("Can't find first map!\n");
        Works = false;
    }

    if (netGameState != MPMODE_DEATHMATCH)
    {
        mapas.tiles[(int)mapas.exit.y][(int)mapas.exit.x] = TILE_EXIT;
    }

    mustCollectItems = mapas.misionItems;
    timeleft = mapas.timeToComplete;

    Dude* player = mapas.getPlayer();

    if (netMode == NETMODE_NONE)
    {
        player->x = (float)mapas.start.x;
        player->y = (float)mapas.start.y;
    }
    else
    {
        player->appearInRandomPlace(mapas._colide, mapas.width(), mapas.height());
    }

    player->id = 254;
    AdaptMapView();
}
//-------------------------------------------------------
void Game::LoadFirstMap()
{
    if (netMode != NETMODE_CLIENT) //offline & server
    {
        GenerateTheMap();
    }
}

//-------------------------------------
void Game::ResetVolume()
{

    SoundSystem* ss = SoundSystem::getInstance();

    for (unsigned i = 0; i < maxwavs; i++)
    {
        ss->setVolume(i, sys.soundFXVolume);
    }

    music.setVolume(sys.musicVolume);
}


//-------------------------------------------------------------
void Game::TitleMenuLogic()
{

    if (mainmenu.active())
    {
        switch(netMode)
        {
            case NETMODE_NONE: break;
            case NETMODE_SERVER: StopServer(); break;
            case NETMODE_CLIENT: QuitServer(); break;
        }

        if (!mainmenu.selected)
        {
            mainmenu.getInput(Keys, OldKeys);
        }
        else
        {
            switch(mainmenu.state)
            {
                case 0:
                    {
                        mainmenu.reset();
                        state = GAMESTATE_INTRO;
                        PlayNewSong("crazy.ogg");   
                    } break;
                case 1:
                    {
                        netmenu.activate();
                        mainmenu.reset();
                        mainmenu.deactivate();
                    }break;
                case 2:
                    {
                        options.activate();
                        mainmenu.reset();
                        mainmenu.deactivate();
                    }break;
                case 3:
                    {
                        Works = false;
                        mainmenu.reset();
                    }break;
            }
        }
    }

    if (netmenu.active())
    {
        if (!netmenu.selected)
            netmenu.getInput(Keys, OldKeys);
        else{
            switch(netmenu.state)
            {
                case 0:{
                    netgame.activate();
                    netmenu.deactivate();
                    netmenu.reset();
                } break;

                case 1:
                {
                    if (netmenu.active())
                        netmenu.deactivate();
                    netmenu.reset();
                    ipedit.activate();
                    EditText[0] = 0;
                }
            }
        }


        if (netmenu.canceled){
            mainmenu.activate();
            netmenu.reset();
            netmenu.deactivate();
        }
    }

        //-------IP EDIT    
        if (ipedit.active())
        {
            if (!ipedit.entered)
            {
                ipedit.getInput(EditText, globalKEY);
                leterKey=0;
            }
            else
            {
                netmenu.reset();
                PlayNewSong("music.ogg");
                JoinServer(ipedit.text,NetPort);
                ipedit.deactivate();
                ipedit.reset();
            }

            if (ipedit.canceled)
            {
                ipedit.deactivate();
                ipedit.reset();
                netmenu.activate();
                
            }

        }
    //----------------
        if (netgame.active())
        {
            if (!netgame.selected)
            {
                netgame.getInput(Keys, OldKeys);

                if (netgame.canceled)
                {
                    netmenu.activate();
                    netgame.reset();
                    netgame.deactivate();
                }

            }
            else
            {
                if (netgame.state == 1)
                {
                    netGameState = MPMODE_DEATHMATCH;
                    mapmenu.activate();
                    netgame.reset();
                    netgame.deactivate();
                }
                else if (netgame.state == 0)
                {
                    netGameState = MPMODE_COOP;
                    state = GAMESTATE_GAME;
                    netgame.reset();
                    netgame.deactivate();
                    PlayNewSong("music.ogg");
                    InitServer();
                    LoadFirstMap();
                }
            }
        }

        if (mapmenu.active())
        {
            if (!mapmenu.selected)
            {
                mapmenu.getInput(Keys, OldKeys);

                if (mapmenu.canceled)
                {
                    netgame.activate();
                    mapmenu.reset();
                    mapmenu.deactivate();
                }

            }
            else
            {
                mapai.current = mapmenu.state;
                state = GAMESTATE_GAME;
                PlayNewSong("music.ogg");
                mapmenu.reset();
                mapmenu.deactivate();
                InitServer();
                GoToLevel(mapai.current, serveris.clientCount());
            }
        }

        if (options.active())
        {
            if (!options.selected)
                options.getInput(Keys, OldKeys);
            else{
                
                options.deactivate();
                //mainmenu.activate();
                switch (options.state){
                    case 0:{
                        options.reset();
                        MusicVolumeC.activate();
                           } break;
                    case 1:{
                            options.reset();
                            SfxVolumeC.activate();
                           } break;
                }
    
             
            }
            if (options.canceled){
                mainmenu.activate();
                options.reset();
                options.deactivate();
            }
        }


        if (MusicVolumeC.active()){
            if (!MusicVolumeC.selected)
            {
                MusicVolumeC.getInput(Keys, OldKeys);
            }
            else{
                MusicVolumeC.deactivate();
                options.activate();
                sys.musicVolume=MusicVolumeC.state*-1;
                ResetVolume();
                MusicVolumeC.reset();
            }

            if (MusicVolumeC.canceled){
                MusicVolumeC.deactivate();
                options.activate();
                MusicVolumeC.reset();
            }

        }

        if (SfxVolumeC.active()){
            if (!SfxVolumeC.selected)
                SfxVolumeC.getInput(Keys, OldKeys);
            else{
                SfxVolumeC.deactivate();
                options.activate();
                sys.soundFXVolume=SfxVolumeC.state*-1;
                ResetVolume();
                SfxVolumeC.reset();

            }

            if (SfxVolumeC.canceled){
                SfxVolumeC.deactivate();
                options.activate();
                SfxVolumeC.reset();

            }

        }


}
//---------------------------------------------------------
void Game::IntroScreenLogic()
{
    if ((Keys[ACTION_OPEN] && !OldKeys[ACTION_OPEN]) || (!FirstTime))
    {
        state = GAMESTATE_HELP;
        intro_cline = 0;
        intro_cchar = 0;
        intro_gline = 0;
    }

    if (intro_gline <= lin)
    {
        intro_tim++;
        if (intro_tim==5)
        {
            intro_tim=0;
            intro_cchar++;

            if ((IntroText[intro_cline][intro_cchar]=='\0'))
            {
                intro_cline++;
                intro_gline++;

                if (intro_cline==18)
                    intro_cline=0;

                intro_cchar=0;

            }
        }
    }
}
//---------------------------------------------------------------
void Game::HelpScreenLogic()
{
    itmtim++;

    if (itmtim>10)
    {
        mapas.itmframe++; //animuoti daiktai
        if (mapas.itmframe > 3)
        {
            mapas.itmframe = 0;
        }

        itmtim=0;
    }

    if ((Keys[ACTION_OPEN] && !OldKeys[ACTION_OPEN])||(!FirstTime))
    {

        if (FirstTime)
        {
            FirstTime=false;
        }

        state = GAMESTATE_GAME;

        PlayNewSong("music.ogg");
        LoadFirstMap();

    }

}

//------------------------------
int Game::PlayerCount()
{
    int players = 1;

    switch(netMode)
    {
        case NETMODE_NONE: break;
        case NETMODE_SERVER: players += serveris.clientCount(); break;
        case NETMODE_CLIENT: players += otherClientCount; break;
    }

    return players;
}

//===============================================================

void Game::logic(){

    if ((mapas.timeToComplete) && (state == GAMESTATE_GAME) && (netGameState == MPMODE_COOP))
    {
        ms += 10;

        if (ms >= 1000)
        {
            if (timeleft)
                timeleft--;
            else 
                KillPlayer(0);
            ms = 0;
        }
    }

    switch (netMode)
    {
        case NETMODE_NONE: break;
        case NETMODE_SERVER:
        {
            if (serveris.isRunning())
            {
                for (int a = 0; a < (int)serveris.clientCount(); ++a)
                {
                    char buf[MAX_MESSAGE_DATA_SIZE];
                    memcpy(&buf, "WD3", 3);
                    buf[4] = NET_SERVER_MSG_PING;
                    serveris.sendData(a, buf, 4);

                    SendPlayerInfoToClient(a);
                }

                ParseMessagesServerGot();
            }
        } break;
        case NETMODE_CLIENT:
        {
            if (client.isOpen())
            {
                if (Client_GotMapData)
                {
                    SendClientCoords();
                }

                ParseMessagesClientGot();
            }
        }
    }

    if (music.playing()) //updeitinam muzona
    {
        music.update();
    }


    switch(state)
    {
        case GAMESTATE_TITLE  : TitleMenuLogic();  break;
        case GAMESTATE_INTRO  : IntroScreenLogic(); break;
        case GAMESTATE_HELP   : HelpScreenLogic();  break;
        case GAMESTATE_ENDING :
                                {
                                    if (Keys[ACTION_OPEN] && !OldKeys[ACTION_OPEN])
                                    {
                                        state = GAMESTATE_TITLE;

                                        mainmenu.activate();
                                        FirstTime = true;
                                        PlayNewSong("evil.ogg");

                                    }

                                } break;
        case GAMESTATE_GAME : CoreGameLogic(); break;
    }

    if (fadein)
    {
        fadetim++;

        if (fadetim == 160)
        {
            fadein = false;
            fadetim = 0;
        }

    }

    touches.up.destroy();
    touches.down.destroy();
    touches.move.destroy();

    OldMouseX = MouseX;
    OldMouseY = MouseY;

    OldGamepadLAxis = gamepadLAxis;
    OldGamepadRAxis = gamepadRAxis;

}

//------------------------------------
void Game::CoreGameLogic()
{

    if (Keys[ACTION_MAP] && !OldKeys[ACTION_MAP])
    {
        ShowMiniMap = !ShowMiniMap;

    }

    mapas.fadeDecals();

    if (objectivetim)  //  decrement objective timer
    {
        objectivetim--;
    }

    itmtim++;

    if (itmtim > 10)
    {
        mapas.itmframe++;  //  item animation

        if (mapas.itmframe > 3)
        {
            mapas.itmframe = 0;
        }

        itmtim=0;
    }


    if ((mustCollectItems < 1) && (!exitSpawned))  //  no more items ? Let's spawn exit
    {
        PutExit();
    }


    AnimateSlime();

    //hero movement

    if (mapas.mons[mapas.enemyCount].shot)
    { //hero dies here
        mapas.mons[mapas.enemyCount].splatter();

        if (!mapas.mons[mapas.enemyCount].shot)
        {

            if (netMode == NETMODE_NONE)
            {
                mapas.mons[mapas.enemyCount].x = (float)mapas.start.x;
                mapas.mons[mapas.enemyCount].y = (float)mapas.start.y;
            }
            else
            {
                mapas.mons[mapas.enemyCount].appearInRandomPlace(mapas._colide, mapas.width(), mapas.height());
            }

            AdaptMapView();
        }

    }


    Dude* player = mapas.getPlayer();

    if (player->spawn)
    { //respawn      
        player->respawn();

        if ((player->ammo < 1) && (!player->spawn))
        {
            player->ammo = 20;
        }
    }



    if (((OldGamepadLAxis.x != gamepadLAxis.x ) || (OldGamepadLAxis.y != gamepadLAxis.y )) &&
        !((int)gamepadLAxis.x == 0 && (int)gamepadLAxis.y == 0)&& (!player->shot) && (!player->spawn))
    {
        Vector3D dir = gamepadLAxis;

        player->angle = M_PI / 2 - atan2(dir.x, dir.y);
    }


    if ((((int)MouseX != (int)OldMouseX) || ((int)MouseY != (int)OldMouseY)) &&
            (!player->shot) && (!player->spawn))
    {

        Vector3D dudePosOnScreen(player->x + mapas.getPos().x,
                                 player->y + mapas.getPos().y, 0);

        Vector3D mouse(MouseX, MouseY, 0);

        Vector3D fv = mouse - dudePosOnScreen;
        fv.normalize();
        float newAngle = M_PI / 2 - atan2(fv.x, fv.y);
        player->angle = newAngle;
    }

    if (!Keys[ACTION_NEXT_WEAPON])
    {
        nextWepPressed=false;
    }

    if ((Keys[ACTION_NEXT_WEAPON]) && (!Keys[ACTION_FIRE]) && (!nextWepPressed))
    {
        nextWepPressed = true;
        player->chageNextWeapon();
    }

    if (!player->shot && !player->spawn && player->canAtack)
    {
        int characterCount = (netMode == NETMODE_SERVER) ? mapas.enemyCount + serveris.clientCount() + 1 :
                                                           mapas.enemyCount + otherClientCount + 1;
        Vector3D mov = Vector3D(gamepadRAxis.x, -gamepadRAxis.y, 0);
        mov.normalize();

        bool movedWithGamepad = false;

        if (fabs(mov.x) > 0.5f || fabs(mov.y) > 0.5f)
        {
            movedWithGamepad = true;
        }

        player->moveGamePad(mov, PLAYER_RADIUS, (const bool**)mapas._colide, mapas.width(), mapas.height(), mapas.mons, characterCount);
        AdaptMapView();

        if ((Keys[0] || Keys[1] || Keys[2] || Keys[3]) && !movedWithGamepad)
        {
            MoveDude();
        }
    }

    SoundSystem::getInstance()->setupListener(Vector3D(player->x, 0, player->y).v,
                                              Vector3D(player->x, 0, player->y).v);

    // jei uzeina ant daikto, ji pasiima
    ItemPickup();


    if (touches.allfingersup)
    {
        if (touches.up.count())
        {
            Keys[ACTION_FIRE] = 1;
        }
    }


    //saunam
    if ((Keys[ACTION_FIRE]) && (!mapas.mons[mapas.enemyCount].shot) && 
            (mapas.mons[mapas.enemyCount].isAlive()) && (!mapas.mons[mapas.enemyCount].spawn))
    {

        if (mapas.mons[mapas.enemyCount].canAtack)
        {
            bool res = false;

            switch(mapas.mons[mapas.enemyCount].currentWeapon)
            {
                case 1: res = mapas.mons[mapas.enemyCount].shoot(true, false, &bulbox); break;
                case 2: res = mapas.mons[mapas.enemyCount].shoot(true, true, &bulbox); break;
                case 0: BeatEnemy(mapas.enemyCount, PLAYER_MELEE_DAMAGE); break;
            }

            if ((mapas.mons[mapas.enemyCount].currentWeapon>0)&&(res))
            {
                if (mapas.mons[mapas.enemyCount].currentWeapon==1){

                    AdaptSoundPos(0,mapas.mons[mapas.enemyCount].x,mapas.mons[mapas.enemyCount].y); 
                    SoundSystem::getInstance()->playsound(0); 

                }
                if (mapas.mons[mapas.enemyCount].currentWeapon==2)
                {
                    AdaptSoundPos(12,mapas.mons[mapas.enemyCount].x,mapas.mons[mapas.enemyCount].y);
                    SoundSystem::getInstance()->playsound(12);
                }

                if (netMode == NETMODE_CLIENT)
                {
                    char buf[10];
                    int pos = 0;
                    buf[pos] = NET_CLIENT_MSG_WEAPON_SHOT;
                    ++pos;
                    memcpy(&buf[1], &mapas.getPlayer()->ammo, sizeof(int));
                    pos += sizeof(int);
                    unsigned char isMine = 0;

                    if (mapas.mons[mapas.enemyCount].currentWeapon == 2)
                    {
                        isMine = 1;
                    }

                    memcpy(&buf[5], &isMine, sizeof(unsigned char));
                    pos += sizeof(unsigned char);
                    client.sendData(buf, pos);
                }

                const bool isMine = (mapas.mons[mapas.enemyCount].currentWeapon == 2) ? true : false;

                if (netMode == NETMODE_SERVER)
                {
                    for (int i=0;i<(int)serveris.clientCount();i++)
                    {
                        SendBulletImpulse(255, mapas.mons[mapas.enemyCount].ammo, i, isMine);
                    }
                }

            }
            else if (mapas.mons[mapas.enemyCount].currentWeapon>0)
            {
                if (noAmmo)
                {
                    mapas.mons[mapas.enemyCount].chageNextWeapon();
                    noAmmo=false;
                }
                else
                {
                    AdaptSoundPos(4,mapas.mons[mapas.enemyCount].x,mapas.mons[mapas.enemyCount].y);
                    SoundSystem::getInstance()->playsound(4);

                    if (!noAmmo)
                    {
                        noAmmo=true;
                    }
                }
            }
        }
    }
    //--
    if (!mapas.mons[mapas.enemyCount].canAtack)
    {
        mapas.mons[mapas.enemyCount].reload(25);
    }


    for (int i = mapas.enemyCount;i<mapas.enemyCount+PlayerCount();i++)
    {
        if (mapas.mons[i].hit)
        {
            mapas.mons[i].damageAnim();

            if (!mapas.mons[i].hit)
            {
                AdaptSoundPos(10,mapas.mons[i].x,mapas.mons[i].y);
                SoundSystem::getInstance()->playsound(10);
            }
        }
    }


    SlimeReaction(mapas.enemyCount); // herojaus reakcija i slime

    DoorsInteraction();//open doors

    CheckForExit();

    HandleBullets();//kulku ai-------------

    for (int i=0; i<mapas.enemyCount; ++i)
    {
        if (netMode != NETMODE_CLIENT) //  offline & server
        {
            MonsterAI(i);
        }
    }

    //zudom playerius jei pas juos nebeliko gifkiu

    if (netMode != NETMODE_CLIENT)
    {
        for (unsigned i = 0; i < mapas.mons.count();i++)
        {
            if ((mapas.mons[i].getHP() <= 0) && (!mapas.mons[i].shot))
            {
                KillEnemy(mapas.mons[i].id);

                if (netMode == NETMODE_SERVER)
                {
                    for (unsigned clientIdx = 0; clientIdx < serveris.clientCount(); ++clientIdx)
                    {
                        int victimId = mapas.mons[i].id;

                        if (victimId >= 254) //players
                        {
                            int lastDamager = mapas.mons[mapas.enemyCount + (victimId - 254)].lastDamagedBy;

                            if (lastDamager == 254)
                            {
                                ++frags;
                            }
                            else
                            {
                                int clientWithFrag = lastDamager - 255;
                                ++fragTable[clientWithFrag];
                                SendFragsToClient(clientWithFrag, fragTable[clientWithFrag]);
                            }

                            if (victimId - 255 < (int)clientIdx)
                            {
                                ++victimId;
                            }
                            else if (victimId - 255 == (int)clientIdx)
                            {
                                victimId = 254;
                            }
                        }

                        SendKillCommandToClient(clientIdx, victimId);
                    }
                }
            }
        }
    }

}


//------------------------------------
void Game::DrawHelp()
{
    pics.draw(13, 320, 240, 0, true);
    WriteShadedText(130, 70, pics, 10, "Colect these:");
    pics.draw(11, 150, 90, mapas.itmframe, false);
    pics.draw(11, 200, 90, mapas.itmframe + 4, false);
    pics.draw(11, 250, 90, mapas.itmframe + 8, false);

    pics.draw(7, 100,210, 0, false);
    WriteText(140,220, pics, 10,"Ammo");
    pics.draw(7, 100, 230, 1, false);
    WriteText(140,240, pics, 10,"Health Up");

    WriteShadedText(140,320, pics, 10, "Controls:");
    WriteShadedText(140,340, pics, 10, "Aim with the mouse, and move with arrows");
    WriteShadedText(140,355, pics, 10, "Tab: minimap");
    WriteShadedText(140,370, pics, 10, "CTRL: fire");
    WriteShadedText(140,385, pics, 10, "SPACE: opens door");
    int monframe = mapas.itmframe;

    if (monframe==3)
    {
        monframe=0;
    }

    pics.draw(3, 100,270, monframe, false);
    WriteShadedText(140,270, pics, 10, "These monsters can eat items");
    WriteShadedText(140,290, pics, 10, "kill them to retrieve items back.");

    WriteShadedText(300,460, pics, 10, "hit RETURN to play...");


}
//-------------------------------------
void Game::DrawIntro()
{
    pics.draw(13, 320, 240, 0, true);

    char buf[2];

    for (int i=0;i<intro_cline;i++)
    {
        WriteShadedText(30, 25 * i + 20, pics, 10, IntroText[i+18*(intro_gline/18)]);
    }

    for (int a=0; a<intro_cchar; a++)
    {
        sprintf(buf, "%c", IntroText[intro_gline][a]);
        WriteShadedText(
                  30 + a * 11, 
                  25 * intro_cline + 20,
                  pics,
                  10,
                  buf
                  );
    }

    WriteShadedText(30,450, pics, 10, "hit RETURN to skip ...");
}



//---------------------------
void Game::DrawMissionObjectives()
{
    char buf[50];
    if (mapas.misionItems)
    {
        sprintf(buf, "%d collectibles here",mapas.misionItems);
        WriteText(sys.ScreenWidth / 2 - 100,
                  sys.ScreenHeight / 2 + 10,
                  pics,
                  10,
                  buf,
                  1.0f,1.0f,
                  COLOR(1.0f,0.0,0.0, 1.f),
                  COLOR(1.0f,0.0,0.0, 1.f)
                  );
    }

    sprintf(buf,"Time remaining:%d:%d",mapas.timeToComplete/60,mapas.timeToComplete-60*(mapas.timeToComplete/60));
    WriteText(sys.ScreenWidth/2-100, 
              sys.ScreenHeight/2+50,
              pics,
              10,
              buf,
              1.0f,1.0,
              COLOR(1.0,0.0,0.0, 1.f),
              COLOR(1.0,0.0,0.0, 1.f)
              );


}

//---------------------------

void Game::render()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    FlatMatrix identity;
    MatrixIdentity(identity.m);

    FlatMatrix finalM = identity * OrthoMatrix;
    defaultShader.use();
    int MatrixID = defaultShader.getUniformID("ModelViewProjection");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, finalM.m);
    colorShader.use();
    int MatrixIDColor = colorShader.getUniformID("ModelViewProjection");
    glUniformMatrix4fv(MatrixIDColor, 1, GL_FALSE, finalM.m);

    switch(state)
    {
        case GAMESTATE_TITLE  : DrawTitleScreen(); break;
        case GAMESTATE_INTRO  : DrawIntro();       break;
        case GAMESTATE_HELP   : DrawHelp();        break;
        case GAMESTATE_ENDING : DrawEndScreen();   break;
        case GAMESTATE_GAME   : DrawGameplay();    break;
    }

    pics.drawBatch(&colorShader, &defaultShader, 666);

}
//-------------------------------------
void Game::DrawTitleScreen()
{
    pics.draw(0, 320, 240, 0, true,1.25f,1.9f);
    pics.draw(16, 0,0,0);



    char buf[80];   

    sprintf(buf,"Jrs%dul",0);
    WriteText(540,10, pics, 10, buf);
    sprintf(buf,"%d",2007);
    WriteText(550,30, pics, 10, buf);

    if (mainmenu.active())
    {
        mainmenu.draw(pics,
                pics.findByName("pics/pointer.tga"),
                pics.findByName("pics/charai.tga"));
    }

    if (netmenu.active())
    {
        netmenu.draw(pics,
                pics.findByName("pics/pointer.tga"),
                pics.findByName("pics/charai.tga")
                );
    }

    if (netgame.active())
    {
        netgame.draw(pics,
                pics.findByName("pics/pointer.tga"),
                pics.findByName("pics/charai.tga")
                );
    }

    if (mapmenu.active())
    {
        mapmenu.draw(pics,
                     pics.findByName("pics/pointer.tga"),
                     pics.findByName("pics/charai.tga")
                    );
    }

    if (options.active())
    {
        options.draw(pics,
                pics.findByName("pics/pointer.tga"),
                pics.findByName("pics/charai.tga")
                );
    }

    if (ipedit.active())
    {
        ipedit.draw(pics, pics.findByName("pics/charai.tga"));
    }

    if (SfxVolumeC.active())
    {
        SfxVolumeC.draw(pics, 
                pics.findByName("pics/pointer.tga"),
                pics.findByName("pics/pointer.tga"),
                pics.findByName("pics/charai.tga")
                );
    }

    if (MusicVolumeC.active())
    {
        MusicVolumeC.draw(pics,
                pics.findByName("pics/pointer.tga"),
                pics.findByName("pics/pointer.tga"),
                pics.findByName("pics/charai.tga")
                );
    }
}
//------------------------------------
void Game::DrawEndScreen()
{
    pics.draw(14, 320, 240, 0, true,  1.25f,1.9f);
    WriteText(260,430, pics, 10, "The End...to be continued ?");
}
//------------------------------------
void Game::DrawGameplay()
{
    if ((mapas.width() > 0) && (mapas.height() > 0))
    {

        if (fadein)
            DrawMap(fadetim/160.0f,fadetim/160.0f,fadetim/160.0f);
        else
            DrawMap();


    }

    if (fadein)
    {
        WriteText(sys.ScreenWidth / 2-150, 
                sys.ScreenHeight/2-64,
                pics, 10, "Get Ready!", 2,2);
    }

    if (objectivetim && netGameState == MPMODE_COOP)
    {
        DrawMissionObjectives();
    }

    if (mapas.mons.count())
    {
        DrawStats();
    }

    pics.draw(18, MouseX, MouseY, 0, true);

    if (ShowMiniMap)
    {
        DrawMiniMap(sys.ScreenWidth - mapas.width() * 4, sys.ScreenHeight - mapas.height() * 4);
    }



    if (DebugMode == 1)
    {
        DrawSomeText();
    }
}

//------------------------------------

/*void fullscreenswitch(){

    render.destroy();
    pics.deleteContainer();


    if (sys.windowed){   
        ShowCursor(false);
        sys.windowed=false;
        InitD3D(); 
    }//is windowed
    else {
        ShowCursor(true);
        sys.windowed=true;   
        InitD3D(); 
        SetWindowPos(hwndMain,0,100,100,sys.width+4,sys.height+25,0);
    }
}*/


//-----------------------------------

/*LRESULT CALLBACK MainWinProc( HWND hwnd,
                             UINT msg,
                             WPARAM wparam,
                             LPARAM lparam)
{
    switch(msg)
    {
    

    case WM_ACTIVATE:{

        if (wparam==WA_INACTIVE){
            ClipCursor(0);
        }
        else{
             RECT rcClip;
            GetWindowRect(hwndMain, &rcClip);  
            rcClip.top+=30;
            ClipCursor(&rcClip); 
        }

        if( WA_INACTIVE != wparam && input.pKeyboard ){
            input.pKeyboard->Acquire();
            input.pMouse->Acquire();
        }
                     }
        break;


    case WM_CHAR:{

        leterKey = (int)wparam;
                 } break;

    case WM_KEYUP:{
        if (wparam == VK_F4) fullscreenswitch();
        if (wparam ==VK_F1) showdebugtext=!showdebugtext;
        if (wparam ==VK_F7){
            int index=0;
            char name[50];
            sprintf(name,"screenshot%d.bmp",index); 
            FILE* f;

            f=fopen(name,"rb");
            if (!f)
                render.writeScreenshot(name);

            while (f){
                fclose(f);
                index++; 
                sprintf(name,"screenshot%d.bmp",index); 
                f=fopen(name,"rb");
            }
            render.writeScreenshot(name);

        }
        if (wparam ==VK_F5) godmode=!godmode;
        if (wparam ==VK_F6) mapas.mons[mapas.enemyCount].ammo+=5;
        if (wparam==VK_ESCAPE){
            if ((IntroScreen)||(HelpScreen)||
                ((!TitleScreen)&&(!IntroScreen)&&(!HelpScreen))){
                TitleScreen=true;
                mainmenu.activate();    
                PlayNewSong("Evil.ogg");
            }
        }


        break;
                  }

    }

    return DefWindowProc(hwnd, msg,wparam, lparam);
}*/


//-------------------------------

void LoadIntro(){
    FILE* f;
    puts("Loading intro text...");
    f = fopen("intro.itf","rt");

    if (!f)
    {
        puts("bumer,could not open intro.itf");
        return;
    }


    int pos=0;
    int c=';';
    while (c!=EOF){
        c=fgetc(f);
        printf("%c", c);
        if ((c!=EOF)||(lin<100)){
            if ((c!='\n')&&(pos<100)){
                IntroText[lin][pos]=c;
                pos++;
            }
            else{
                IntroText[lin][pos]='\0';
                puts(IntroText[lin]);
                lin++;
                pos=0;
            }


        }
    }
    fclose(f);
    

}

//----------------------------------------
//siuncia kliento info i serva
void Game::SendClientCoords()
{
    char coords[MAX_MESSAGE_DATA_SIZE];
    int cnt = 0;
    coords[0] = NET_CLIENT_MSG_CHARACTER_DATA;
    ++cnt;
    memcpy(&coords[cnt], &mapas.mons[mapas.enemyCount].x, sizeof(float));
    cnt += sizeof(float);
    memcpy(&coords[cnt], &mapas.mons[mapas.enemyCount].y, sizeof(float));
    cnt += sizeof(float);
    memcpy(&coords[cnt], &mapas.mons[mapas.enemyCount].angle, sizeof(float));
    cnt += sizeof(float);
    memcpy(&coords[cnt], &mapas.mons[mapas.enemyCount].frame, sizeof(unsigned char));
    cnt += sizeof(unsigned char);
    unsigned char stats=0x0;

    if (mapas.mons[mapas.enemyCount].shot)
        stats|=0x80;
    if (mapas.mons[mapas.enemyCount].spawn)
        stats|=0x40;
    coords[cnt]=stats;
    cnt++;

    client.sendData(coords, cnt);
}
//-----------------------------------------
void Game::SendPlayerInfoToClient(int clientindex)
{
    unsigned char z = 0;
    char coords[1024];
    int cnt = 0;

    for (int i = 0; i < mapas.enemyCount+(int)serveris.clientCount()+1; i++)
    {
        if (i-mapas.enemyCount-1 != clientindex)
        {

            if ((i >= mapas.enemyCount) && ((i - mapas.enemyCount - 1) < clientindex))
            {
                z = (unsigned char)(i+1);
            }
            else
            {
                z = (unsigned char)i;
            }

            memcpy(&coords[cnt], "WD3", 3);
            cnt += 3;

            coords[cnt] = NET_SERVER_MSG_CHARACTER_DATA;
            ++cnt;
            memcpy(&coords[cnt],&z,sizeof(unsigned char));
            cnt += sizeof(unsigned char);
            memcpy(&coords[cnt],&mapas.mons[i].x, sizeof(float));
            cnt += sizeof(float);
            memcpy(&coords[cnt],&mapas.mons[i].y, sizeof(float));
            cnt += sizeof(float);
            memcpy(&coords[cnt],&mapas.mons[i].angle, sizeof(float));
            cnt += sizeof(float);
            memcpy(&coords[cnt],&mapas.mons[i].frame, sizeof(unsigned char));
            cnt += sizeof(unsigned char);
            unsigned char stats = 0x0;

            if (mapas.mons[i].shot)
            {
                stats|=0x80;
            }

            if (mapas.mons[i].spawn)
            {
                stats|=0x40;
            }

            coords[cnt] = stats;
            cnt++;

            if (mapas.mons[i].spawn)
            {
                int hp = mapas.mons[i].getHP();
                memcpy(&coords[cnt], &hp, sizeof(int));
                cnt+=sizeof(int);
            }

        }
    }

    serveris.sendData(clientindex, coords, cnt);
}
//-----------------------------------
void Game::GetCharData(const unsigned char* bufer, int bufersize, int* index )
{
    if ((bufersize-(*index))>=14)
    {

        unsigned char monum=0;
        memcpy(&monum,&bufer[*index],sizeof(unsigned char));
        *index+=sizeof(unsigned char);
        memcpy(&mapas.mons[monum].x,&bufer[*index],sizeof(float));
        *index+=sizeof(float);
        memcpy(&mapas.mons[monum].y,&bufer[*index],sizeof(float));
        *index+=sizeof(float);
        memcpy(&mapas.mons[monum].angle,&bufer[*index],sizeof(float));
        *index+=sizeof(float);
        memcpy(&mapas.mons[monum].frame,&bufer[*index],sizeof(unsigned char));
        *index+=sizeof(unsigned char);
        unsigned char stats=bufer[*index];
        if (stats & 0x80)
            mapas.mons[monum].shot=true;
        else
            mapas.mons[monum].shot=false;

        if (stats & 0x40)
        {
            mapas.mons[monum].spawn=true;
        }
        else
        {
            mapas.mons[monum].spawn=false;
        }

        (*index)++;

        if (mapas.mons[monum].spawn)
        {
            int hp = 0;
            memcpy(&hp, &bufer[*index], sizeof(int));
            mapas.mons[monum].setHP(hp);
            *index+=sizeof(int);
        }


    }
}
//---------------------------------------------
void Game::GetMapInfo(const unsigned char* bufer, int bufersize, int* index)
{

    int mapnamelen = 0;
    char mapname[255];

    if (bufersize-(*index) >= (int)sizeof(int))
    {
        int totallen = 0;
        memcpy(&totallen, &bufer[*index], sizeof(int));
        *index += sizeof(int);

        if ((bufersize-(*index) >= totallen) && (totallen>0))
        {
            memcpy(&mapnamelen, &bufer[*index], sizeof(int));
            *index += sizeof(int);

            if (mapnamelen)
            {

                if (bufersize-(*index) >= mapnamelen)
                {
                    memcpy(mapname,&bufer[*index],mapnamelen);
                }

                *index+=mapnamelen;
                mapname[mapnamelen] = 0;
            }

            int klientaiold = otherClientCount;
            //gaunam klientu skaiciu
            if (bufersize-(*index) >= (int)sizeof(int))
            {
                memcpy(&otherClientCount, &bufer[*index],sizeof(int));
            }

            *index += sizeof(unsigned int);

            netGameState = (MultiplayerModes)bufer[*index];

            ++(*index);

            if (mapnamelen)
            {

                if (strcmp(mapname,mapas.name)!=0)
                {//jei mapas ne tas pats tai uzloadinam
                    LoadTheMap(mapname, false, otherClientCount);
                    Client_GotMapData = true;
                    state = GAMESTATE_GAME;
                }
                else
                {

                    if (otherClientCount - klientaiold)
                    {
                        for (int i=0; i < (otherClientCount - klientaiold); ++i)
                        {
                            Dude n;
                            n.id = mapas.mons[mapas.mons.count() - 1].id + 1;
                            mapas.mons.add(n);
                        }
                    }

                }
            }
            else
            {
                state = GAMESTATE_GAME;

                if ((otherClientCount - klientaiold) && Client_GotMapData)
                {
                    Dude naujas;
                    for (int i=0; i < (otherClientCount - klientaiold); i++)
                    {
                        Dude n;
                        n.id = mapas.mons[mapas.mons.count() - 1].id + 1;
                        mapas.mons.add(n);
                    }
                }


            }
        }
    }
}
//------------------------------------
void Game::GetMapData(const unsigned char* bufer, int* index)
{

    int mapHeight = 0;
    int mapWidth = 0;

    memcpy(&mapHeight, &bufer[*index], sizeof(int));
    *index += sizeof(int);
    memcpy(&mapWidth, &bufer[*index], sizeof(int));
    *index += sizeof(int);

    if (mapHeight && mapWidth)
    {
        mapas.destroy();
        mapas.tiles = new unsigned char * [mapHeight];

        for (int i = 0; i < mapHeight; ++i)
        {
            mapas.tiles[i] = new unsigned char[mapWidth];

            for (int a = 0; a < mapWidth; ++a)
            {

                memcpy(&mapas.tiles[i][a], &bufer[*index], sizeof(unsigned char));
                *index += sizeof(unsigned char);
            }
        }

        mapas._height = mapHeight;
        mapas._width = mapWidth;

        mapas.buildCollisionmap();

    }

    int missionItems = 0;
    memcpy(&missionItems, &bufer[*index], sizeof(int));
    *index += sizeof(int);
    mapas.misionItems = missionItems;
    mustCollectItems = missionItems;


    int moncount = 0;
    memcpy(&moncount, &bufer[*index], sizeof(int));
    *index += sizeof(int);

    if (!mapas.mons.count()) //  No monsters in the map ? Must be coop map
    {
        mapas.enemyCount = moncount;

        for (int i = 0; i < moncount; i++)
        {
            Dude monster;
            monster.id = i;
            monster.initMonsterHP();
            mapas.addMonster(monster);
        }

        Dude d;
        mapas.addMonster(d);

        Dude* player = mapas.getPlayer();
        player->appearInRandomPlace(mapas._colide, mapas.width(), mapas.height());
        player->id = 254;
        player->weaponCount = 3;
        player->currentWeapon = 1;
        player->frame = (player->currentWeapon + 1) * 4 - 2;

        Dude naujas;
        for (int i = 0; i < otherClientCount; i++)
        {
            mapas.mons.add(naujas);
            mapas.mons[mapas.mons.count()-1].appearInRandomPlace(mapas._colide, mapas.width(), mapas.height());

            mapas.mons[mapas.mons.count()-1].id = mapas.mons[mapas.mons.count()-2].id + 1;
        }

        AdaptMapView();

        Client_GotMapData = true;

    }

    for (int i = 0; i < moncount; i++)
    {
        memcpy(&mapas.mons[i].race, &bufer[*index], sizeof(int));
        *index+=sizeof(int);
    }

    int itmcount = 0;
    memcpy(&itmcount, &bufer[*index], sizeof(int));
    *index+=sizeof(int);

    for (int i = 0; i < itmcount; i++)
    {
        Item itm;

        memcpy(&itm.x, &bufer[*index], sizeof(float));
        *index += sizeof(float);
        memcpy(&itm.y, &bufer[*index], sizeof(float));
        *index += sizeof(float);
        memcpy(&itm.value, &bufer[*index], sizeof(int));
        *index += sizeof(int);
        mapas.addItem(itm.x, itm.y, itm.value);
    }

}
//----------------------------------
void Game::UpdateClientPosition(const unsigned char* bufer, unsigned * buferindex, unsigned int clientIndex)
{
    memcpy(&mapas.mons[mapas.enemyCount+1+clientIndex].x,&bufer[*buferindex],sizeof(float));
    *buferindex+=sizeof(float);
    memcpy(&mapas.mons[mapas.enemyCount+1+clientIndex].y,&bufer[*buferindex],sizeof(float));    
    *buferindex+=sizeof(float);
    memcpy(&mapas.mons[mapas.enemyCount+1+clientIndex].angle,&bufer[*buferindex],sizeof(float));    
    *buferindex+=sizeof(float);
    memcpy(&mapas.mons[mapas.enemyCount+1+clientIndex].frame,&bufer[*buferindex],sizeof(unsigned char));
    (*buferindex)++;

    unsigned char stats=bufer[*buferindex];
    (stats & 0x80) ?  mapas.mons[mapas.enemyCount+1+clientIndex].shot=true : mapas.mons[mapas.enemyCount+1+clientIndex].shot=false;
    (stats & 0x40) ? mapas.mons[mapas.enemyCount+1+clientIndex].spawn=true : mapas.mons[mapas.enemyCount+1+clientIndex].spawn=false;
}

//--------------------------------------------------------------
void Game::GetDoorInfo(const unsigned char* bufer, unsigned * index, int* dx, int* dy, unsigned char* frame)
{

    int doorx,doory;
    unsigned char doorframe=0;
    memcpy(&doorx,&bufer[*index],sizeof(int));
    *index+=sizeof(int);
    memcpy(&doory,&bufer[*index],sizeof(int));
    *index+=sizeof(int);
    memcpy(&doorframe,&bufer[*index],sizeof(unsigned char));
    *index+=sizeof(unsigned char);

    mapas.tiles[doory][doorx]=doorframe;
    if ((doorframe==65)||(doorframe==67)||(doorframe==69)||(doorframe==71))
    {
        mapas._colide[doory][doorx] = true;
    }
    else
    {
        mapas._colide[doory][doorx]=false;
    }

    if (dx)
        *dx=doorx;
    if (dy)
        *dy=doory;
    if (frame)
        *frame=doorframe;
}
//---------------------------------------
void Game::GetNewItemInfo(unsigned char* bufer, int* index)
{
    float x,y;
    int value;
    memcpy(&x,&bufer[*index],sizeof(float));
    *index+=sizeof(float);
    memcpy(&y,&bufer[*index],sizeof(float));
    *index+=sizeof(float);
    memcpy(&value,&bufer[*index],sizeof(int));
    *index+=sizeof(int);

    mapas.addItem(x,y,value);
}
//---------------------------------------
void Game::GetClientAtackImpulse(const unsigned char* buf, unsigned * index, int ClientIndex)
{
    int victim;
    int hp;
    memcpy(&victim,&buf[*index],sizeof(int));
    *index+=sizeof(int);
    memcpy(&hp,&buf[*index],sizeof(int));
    *index+=sizeof(int);



    int playerID=0;

    if (victim>254)
    {
        int tmpID=victim-254;
        if (ClientIndex>=tmpID)
        {
            playerID=tmpID-1;
        }

        playerID=mapas.enemyCount+playerID;
    }
    else
    {
        playerID=victim;
    }

    mapas.mons[playerID].hit=true;
    mapas.mons[playerID].setHP(hp);



    for (unsigned a = 0; a < serveris.clientCount(); a++)
    {

        int z=playerID;
        if ((playerID >= mapas.enemyCount) && ((playerID-mapas.enemyCount-1) < (int)a))
            z= playerID+1;

        if (ClientIndex != (int)a)
        {
            SendAtackImpulse(a, z, hp);
        }
    }

}
//----------------------------------
//klientas paima infa apie atka
void Game::GetAtackImpulse(const unsigned char* buf,int* index)
{
    int victim;
    int hp;
    memcpy(&victim,&buf[*index],sizeof(int));
    *index+=sizeof(int);
    memcpy(&hp,&buf[*index],sizeof(int));
    *index+=sizeof(int);

    mapas.mons[victim].hit=true;
    mapas.mons[victim].setHP(hp);

}
//---------------------------------

void Game::ParseMessagesServerGot()
{

    if (!serveris.storedPacketCount())
    {
        return;
    }

    std::lock_guard<std::mutex> lock(messageMutex);

    printf("client packets to parse %u\n", serveris.storedPacketCount());

    for (int msgIdx = serveris.storedPacketCount() - 1; msgIdx >= 0; --msgIdx)
    {

        Message* msg = serveris.fetchPacket(msgIdx);

        if (msg->parsed)
        {
            continue;
        }


        int clientIdx = serveris.findClientByAddress(msg->senderAddress);

        unsigned index = 0;

        while (index < msg->length)
        {

            NetworkCommands command = NET_NOP;

            if (msg->length - index >= 1)
            {
                memcpy(&command, &(msg->data)[index], 1);
            }

            switch(command)
            {

                case NET_CLIENT_MSG_CONNECT:
                    {
                        printf("CLIENT CONNECTED!\n");
                        ++index;

                        Dude naujas;
                        mapas.mons.add(naujas);
                        int zeroFrags = 0;
                        fragTable.add(zeroFrags);


                        ClientFootprint fp;
                        fp.address = msg->senderAddress;
                        serveris.addClient(fp);

                        mapas.mons[mapas.mons.count()-1].id = mapas.mons[mapas.mons.count()-2].id + 1;

                        for (int i=0; i < (int)serveris.clientCount(); i++)
                        {
                            SendMapInfo(i, mapas);
                        }

                        SendMapData(serveris.clientCount() - 1, mapas);

                    } break;

                case NET_CLIENT_MSG_CHARACTER_DATA:
                    {
                        ++index;
                        UpdateClientPosition(msg->data, &index, clientIdx);
                    } break;

                case NET_CLIENT_MSG_WEAPON_SHOT:
                    {
                        unsigned char cisMine=0;

                        ++index;
                        memcpy(&mapas.mons[mapas.enemyCount + 1 + clientIdx].ammo, &(msg->data)[index],sizeof(int));
                        index+=sizeof(int);
                        memcpy(&cisMine, &(msg->data)[index],sizeof(unsigned char));
                        index+=sizeof(unsigned char);
                        bool isMine = false;

                        if (cisMine)
                        {
                            isMine = true;
                        }

                        mapas.mons[mapas.enemyCount+1 + clientIdx].shoot(true,isMine,&bulbox);


                        for (unsigned int a = 0; a < serveris.clientCount(); a++)
                        {//isiunciam isovimo impulsa kitiems
                            if (a != (unsigned)clientIdx)
                            {
                                if ((a < (unsigned)clientIdx) && ( clientIdx > 0))
                                {
                                    SendBulletImpulse(255 + clientIdx, mapas.mons[mapas.enemyCount + clientIdx].ammo, a, isMine);
                                }
                                else
                                {
                                    SendBulletImpulse(256 + clientIdx, mapas.mons[mapas.enemyCount + clientIdx].ammo,a,isMine);
                                }

                            }
                        }
                    } break;

                case NET_CLIENT_MSG_ITEM:
                    {
                        ++index;
                        int itmindex = 0;
                        memcpy(&itmindex, &(msg->data)[index], sizeof(int));
                        index+=sizeof(int);

                        if ((mapas.items[itmindex].value < ITEM_AMMO_PACK) && (mapas.items[itmindex].value>0))
                        {
                            mustCollectItems--;
                        }

                        mapas.removeItem(itmindex);

                        for (unsigned int a = 0; a < serveris.clientCount(); a++)
                        {
                            if (a != (unsigned)clientIdx)
                            {
                                SendItemSRemove(itmindex,a,true);
                            }
                        }

                    } break;

                case NET_CLIENT_MSG_DOOR:
                    {
                        ++index;
                        int dx,dy;
                        unsigned char frame;
                        GetDoorInfo(msg->data, &index, &dx,&dy,&frame);

                        for (unsigned int a=0; a < serveris.clientCount(); a++)
                        {
                            if (a != (unsigned)clientIdx)
                            {
                                SendServerDoorState(a,dx,dy,frame);
                            }
                        }
                    } break;

                case NET_CLIENT_MSG_NEXT_LEVEL:
                    {
                        ++index;
                        mapai.current++;
                        int kiek = serveris.clientCount();

                        GoToLevel(mapai.current,kiek);

                        for (int a =0; a<(int)serveris.clientCount(); a++)
                        {
                            SendMapInfo(a, mapas);
                            SendMapData(a, mapas);
                        }


                    } break;

                case NET_CLIENT_MSG_MELEE_ATTACK:
                    {
                        ++index;
                        GetClientAtackImpulse(msg->data, &index, clientIdx);
                    } break;

                case NET_CLIENT_MSG_QUIT:
                    {
                        ++index;

                        for (unsigned i = 0; i < serveris.clientCount(); ++i)
                        {
                            if (i != (unsigned)clientIdx)
                            {
                                char data[MAX_MESSAGE_DATA_SIZE];
                                int len = 0;

                                memcpy(&data[len], "WD3", 3);
                                len += 3;

                                data[len] = NET_SERVER_MSG_REMOVE_CHARACTER;
                                ++len;
                                memcpy(&data[len], &mapas.mons[mapas.enemyCount + clientIdx].id, sizeof(int));
                                len += sizeof(int);
                                serveris.sendData(i, data, len);
                            }
                        }

                        serveris.removeClient(clientIdx);
                        fragTable.remove(clientIdx);
                        mapas.mons.remove(mapas.enemyCount + clientIdx + 1);
                    } break;

                case NET_CLIENT_MSG_PONG:
                    {
                        ++index;

                    } break;
                default:
                    {
                        ++index;
                    }
            }
        }

        msg->parsed = true;

    }

    for (int i = serveris.storedPacketCount() - 1; i >= 0 ; --i)
    {
        Message* msg = serveris.fetchPacket(i);
        if (msg->parsed)
        {
            serveris.discardPacket(i);
        }
    }

    //printf("packets after %u\n", serveris.storedPacketCount());


}
//------------------------------
void Game::ParseMessagesClientGot()
{
    if (!client.storedPacketCount())
    {
        return;
    }

    std::lock_guard<std::mutex> lock(messageMutex);

    printf("server packets to parse %u\n", client.storedPacketCount());

    for (unsigned msgIdx = 0; msgIdx < client.storedPacketCount(); ++msgIdx)
    {

        Message* msg = client.fetchPacket(msgIdx);

        if (msg->parsed)
        {
            continue;
        }


        int index = 0;

        while ((unsigned)index < msg->length)
        {
            NetworkCommands command = NET_NOP;
            char NetworkHeader[4] = {0};

            if (msg->length - index >= 3)
            {
                memcpy(&NetworkHeader, &(msg->data)[index], 3);
                index += 3;
                if (strcmp(NetworkHeader, "WD3") != 0)
                {
                    msg->parsed = true;
                    continue;
                }
            }

            if (msg->length - index >= 1)
            {
                memcpy(&command, &(msg->data)[index], 1);
            }

            switch(command)
            {

                case NET_SERVER_MSG_CHARACTER_DATA:
                    {
                        ++index;
                        GetCharData(msg->data, msg->length, &index);
                    } break;

                case NET_SERVER_MSG_SHUTTING_DOWN:
                    {
                        client.shutdown();
                        netMode = NETMODE_NONE;
                        state = GAMESTATE_TITLE;
                        mainmenu.activate();
                        PlayNewSong("evil.ogg");
                        return;
                    } break;

                case NET_SERVER_MSG_SERVER_INFO:
                    {
                        ++index;
                        GetMapInfo(msg->data, msg->length, &index);
                    } break;

                case NET_SERVER_MSG_MAP_DATA:
                    {
                        ++index;
                        GetMapData(msg->data, &index);
                    } break;

                case NET_SERVER_MSG_REMOVE_CHARACTER:
                    {
                        ++index;
                        int idToRemove;
                        memcpy(&idToRemove, &(msg->data)[index], sizeof(int));

                        int idxToRemove = -1;

                        for (unsigned i = mapas.enemyCount; i < mapas.mons.count(); ++i)
                        {
                            if (mapas.mons[i].id == idToRemove + 1)
                            {
                                idxToRemove = i;
                                break;
                            }
                        }

                        if (idxToRemove != -1)
                        {
                            mapas.removeMonster(idxToRemove);
                        }

                    } break;

                case NET_SERVER_MSG_KILL_CHARACTER:
                    {
                        ++index;
                        int victimID = -1;
                        memcpy(&victimID, &(msg->data)[index], sizeof(int));
                        index += sizeof(int);

                        if (victimID != -1)
                        {
                            KillEnemy(victimID);
                        }
                    } break;

                case NET_SERVER_MSG_FRAG:
                    {
                        ++index;
                        memcpy(&frags, &(msg->data)[index], sizeof(int));
                        index += sizeof(int);
                    } break;

                case NET_SERVER_MSG_WEAPON_SHOT:
                    {
                        ++index;
                        int ind = 0;
                        memcpy(&ind, &(msg->data)[index], sizeof(int));
                        index += sizeof(int);

                        if (ind < 254)
                        {
                            memcpy(&mapas.mons[ind].ammo, &(msg->data)[index],sizeof(int));
                            index+=sizeof(int);
                            unsigned char cisMine=0;
                            memcpy(&cisMine, &(msg->data)[index], sizeof(unsigned char));
                            index += sizeof(unsigned char);
                            bool isMine=false;

                            if (cisMine)
                            {
                                isMine=true;
                            }

                            mapas.mons[ind].shoot(true, isMine, &bulbox);
                        }
                        else
                        {
                            memcpy(&mapas.mons[mapas.enemyCount+(ind-254)].ammo, &(msg->data)[index], sizeof(int));
                            index += sizeof(int);
                            unsigned char cisMine=0;
                            memcpy(&cisMine, &(msg->data)[index], sizeof(unsigned char));
                            index += sizeof(unsigned char);
                            bool isMine=false;

                            if (cisMine)
                            {
                                isMine=true;
                            }

                            mapas.mons[mapas.enemyCount+(ind-254)].shoot(true, isMine, &bulbox);
                        }
                    } break;

                case NET_SERVER_MSG_ITEM:
                    {
                        ++index;
                        int itmindex=0;
                        memcpy(&itmindex, &(msg->data)[index], sizeof(int));
                        index+=sizeof(int);
                        unsigned char isPlayerTaked = 0;
                        memcpy(&isPlayerTaked, &(msg->data)[index], sizeof(unsigned char));
                        index++;

                        if ((mapas.items[itmindex].value < ITEM_AMMO_PACK) &&
                                (mapas.items[itmindex].value > 0) &&
                                (isPlayerTaked))
                        {
                            mustCollectItems--;
                        }

                        mapas.removeItem(itmindex);

                    } break;

                case NET_SERVER_MSG_DOOR:
                    {
                        ++index;
                        GetDoorInfo(msg->data, (unsigned*)&index, 0,0,0);
                    } break;

                case NET_SERVER_MSG_NEW_ITEM:
                    {
                        ++index;
                        GetNewItemInfo(msg->data, &index);
                    } break;

                case NET_SERVER_MSG_MELEE_ATTACK:
                    {
                        ++index;
                        GetAtackImpulse(msg->data, &index);

                    } break;

                case NET_SERVER_MSG_PING:
                    {
                        ++index;
                        char buf[2];
                        buf[0] = NET_CLIENT_MSG_PONG;
                        client.sendData(buf, 1);

                    } break;

                default:
                    {
                        index++;
                    }
            }

        } //while

        msg->parsed = true;

    } //for
    
    for (int i = client.storedPacketCount() - 1; i >= 0 ; --i)
    {
        Message* msg = client.fetchPacket(i);
        if (msg->parsed)
        {
            client.discardPacket(i);
        }
    }

    //printf("packets after %u\n", client.storedPacketCount());



}


//---------------------------------------
void Game::LoadShader(ShaderProgram* shader, const char* name)
{
    shader->create();

    char error[1024];
    char buf[512];

    Shader vert;
    Shader frag;

    printf("Loading vertex shader...\n");
    sprintf(buf, "shaders/%s.vert", name);
#ifdef __ANDROID__
    vert.load(GL_VERTEX_SHADER, buf, AssetManager);
#else
    vert.load(GL_VERTEX_SHADER, buf);
#endif

    printf("Loading fragment shader...\n");
    sprintf(buf, "shaders/%s.frag", name);
#ifdef __ANDROID__
    frag.load(GL_FRAGMENT_SHADER, buf, AssetManager);
#else
    frag.load(GL_FRAGMENT_SHADER, buf);
#endif

    shader->attach(vert);
    shader->attach(frag);
    shader->link();

    shader->getLog(error, 1024);
    if (strlen(error)) {
#ifdef __ANDROID__
        LOGI("--%s--", buf);
        LOGI("%s", error);
    }
    LOGI("---------------");
#else
        printf("--%s--\n", buf);
        puts(error);
    }
    puts("-----------");
#endif
}

//----------------------
void Game::loadConfig()
{
    char buf[1024];
    sprintf(buf, "%s/settings.cfg", DocumentPath);
    sys.load(buf);

    ScreenWidth = sys.ScreenWidth;
    ScreenHeight = sys.ScreenHeight;
    windowed = sys.useWindowed;

    sys.write(buf);
}


//-----------------------------------------
void Game::init()
{

   srand(time(0));

#ifndef __ANDROID__
    LoadExtensions();
#endif
    printf("Creating shaders...\n");
   
    LoadShader(&defaultShader, "default");
    LoadShader(&colorShader, "justcolor");

    colorShader.use();



    glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LEQUAL);

    glEnable (GL_BLEND);


    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    MatrixOrtho(0.0, ScreenWidth, ScreenHeight, 0.0, -400, 400, OrthoMatrix);

    pics.load("pics/imagesToLoad.xml"); 


    Smenu menu;
    strcpy(menu.opt[0],"Single Player");
    strcpy(menu.opt[1],"Network Game");
    strcpy(menu.opt[2],"Options");
    strcpy(menu.opt[3],"Exit");
    menu.count=4;
    mainmenu.init(0,sys.ScreenHeight-150,"",menu,0);
    mainmenu.activate();
    strcpy(menu.opt[0],"Start server");
    strcpy(menu.opt[1],"Join server");
    menu.count=2;
    netmenu.init(0,sys.ScreenHeight-100,"Network Game:",menu,0);

    strcpy(menu.opt[0],"Coop");
    strcpy(menu.opt[1],"DeathMatch");
    menu.count=2;
    netgame.init(0,sys.ScreenHeight-100,"Game Type:",menu,0);

    for (int i = 0; i < mapai.count(); ++i)
    {
        mapai.getMapName(i, menu.opt[i]);
    }

    menu.count = mapai.count();
    mapmenu.init(0, sys.ScreenHeight - mapai.count() * 20 - 32, "Select map:", menu, 0);

    strcpy(menu.opt[0],"Music Volume");
    strcpy(menu.opt[1],"Sound fx Volume");
    menu.count = 2;
    options.init(0,sys.ScreenHeight-100,"Options:",menu,0);

    ipedit.init(0,sys.ScreenHeight-100,"Enter Server's IP",20);

    SfxVolumeC.init(20,sys.ScreenHeight-100,"Sfx Volume:",0,10000,100);
    MusicVolumeC.init(20,sys.ScreenHeight-100,"Music Volume:",0,10000,100);


    LoadIntro();

    InitAudio();


    PlayNewSong("evil.ogg");

    Works = true;


}
//--------------------------------
void Game::destroy()
{

    switch(netMode)
    {
        case NETMODE_NONE: break;
        case NETMODE_SERVER: StopServer(); break;
        case NETMODE_CLIENT: QuitServer(); break;
    }


    music.release();
    SoundSystem::getInstance()->exit();

    mapas.destroy();
    mapai.Destroy();
    pics.destroy();

    bulbox.destroy();
}

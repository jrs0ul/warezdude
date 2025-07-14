#include "Game.h"


#include <cstdio>     //sprinf
#include <ctime>


#include "dude.h"
#include "maplist.h"
#include "BulletContainer.h"
#include "SelectMenu.h"
#include "EditBox.h"
#include "ScroollControl.h"
#include "WriteText.h"
#include "Usefull.h"
#include "TextureLoader.h"



//===================================GLOBALS

const int slime=35;

const unsigned NetPort=666;

Renderer3D render; //rendereris(D3D)

//inputs
XInput input;

OggPlayer music;

CBulletContainer bulbox; 

LPDIRECTSOUND8 pSound=0; 
LPDIRECTSOUND3DLISTENER8 listiner=0;



WNDCLASSEX windowsclass;
HWND hwndMain;
HINSTANCE hinstanceMain;



PicsContainer pics;
Gabalas* smotai; // wav samplai
unsigned int imgCount=0;
unsigned int maxwavs=0;


//=============================
const int GameKeyCount=10;
unsigned char Keys[GameKeyCount]={0}; //zaidimo mygtukai
unsigned char KeyCodes[GameKeyCount][2]={0};
//=============================


int goods=0;
int timeleft=0;
bool ext=false;
bool showdebugtext=false;
bool TitleScreen=true;  //rodome zaidimo TM picsa
bool IntroScreen=false; //rodome intro
bool HelpScreen=false; //rodome helpa
bool EndScreen=false;
bool FirstTime=true;
int ms=0;



MapList mapai;


//---
int pskx=22,psky=17;
int pushx=0; int pushy=0;
int scrx=22,scry=17;
int LeftBorder=0;
int RightBorder=0;
int UpBorder=0;
int DownBorder=0;// ar rodyti pakrasti ?
int posx=32, posy=32;
//---

int itmframe=0;
int itmtim=0;

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

DWORD tick=timeGetTime();

int door_tim=0;

bool fadein=true;
int fadetim=0;
int objectivetim=200;


SelectMeniu mainmenu,netmenu, netgame, options;
ScroollControl SfxVolumeC, MusicVolumeC;
EditBox ipedit;


unsigned char globalKey;


bool isServer=false;
bool isClient=false;
bool IsCoop=true;
bool IsDeathMatch=false;

bool killthread=false;

int klientai=0;

int leterKey;

bool Client_GotMapData=false;

long mousepowx,mousepowy;

bool noAmmo=false;
bool nextWepPressed=false;



//==================================================
void AdaptSoundPos(int soundIndex, float soundx,float soundy){

    SoundSystem* ss = SoundSystem::getInstance();
    Vector3D v = Vector3D(soundx, 0, soundy);
    ss->setSoundPos(soundIndex, v.v);
}
//-----------------------------------------------------------
int FPS (void){
    static int time = 0, FPS = 0, frames = 0, frames0 = 0;
    if ((int)timeGetTime() >= time) {
        FPS = frames-frames0;
        time = timeGetTime()+1000;
        frames0 = frames;
    }
    frames = frames+1;
    return FPS;
}


//-----------------------------------

HRESULT GetListener(LPDIRECTSOUND8 lpds, LPDIRECTSOUND3DLISTENER8* ppListener){
  DSBUFFERDESC             dsbd;
  LPDIRECTSOUNDBUFFER      lpdsbPrimary;  // Cannot be IDirectSoundBuffer8.
  LPDIRECTSOUND3DLISTENER8 lp3DListener = NULL;
  HRESULT hr;
 
  ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
  dsbd.dwSize = sizeof(DSBUFFERDESC);
  dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

  if (SUCCEEDED(hr = lpds->CreateSoundBuffer(&dsbd, &lpdsbPrimary, NULL)))
  {
    hr = lpdsbPrimary->QueryInterface(IID_IDirectSound3DListener8, (LPVOID *)ppListener);
    lpdsbPrimary->Release();
  }
  return hr;
  
}

//-------------------------------------------

void DeleteAudio(){

    if (maxwavs){
        for (unsigned int i =0;i<maxwavs;i++)
            smotai[i].release();
        delete []smotai;
    }


    music.Stop();
    music.Close();


    if (listiner)
        listiner->Release();

    if (pSound)
        pSound->Release(); 


}


//----------------------------------
HRESULT InitAudio()
{
    

    //dsoundas
    if (SUCCEEDED(DirectSoundCreate8( NULL, &pSound, NULL ))){

        if (SUCCEEDED(pSound->SetCooperativeLevel( hwndMain,  DSSCL_PRIORITY))){

            

            GetListener(pSound,&listiner);

        
            //loadinam wavus 

            FILE* list;

            char** mas=0;
            char buf[255];
            list=fopen("sfx/list.txt","rt");

            if (list){
                fscanf(list,"%u",&maxwavs);
                if (maxwavs){
                    smotai = new Gabalas[maxwavs];
                    fgetc(list);
                    char c=0;
                    int a;

                    mas=new char * [maxwavs];
                    for (unsigned int i=0; i<maxwavs; i++){
                        mas[i]=new char[255];
                        a=0;
                        while((c!='\n')&&(c!=EOF)){
                            c=fgetc(list); 
                            if ((c!='\n')&&(c!=EOF))
                                mas[i][a]=c;
                            else 
                                mas[i][a]='\0';
                            a++;
                        }
                        sprintf(buf,"sfx/%s",mas[i]);

                        smotai[i].open(pSound,buf,true);
                        smotai[i].setVolume(sys.soundFXVolume);
                        c=0;
                    }
                }
                fclose(list);
                for (unsigned int i=0; i<maxwavs; i++)
                    delete []mas[i];
                delete []mas;
            }

        }
    }



    return 0;
}

//----------------------------------------
void PlayNewSong(const char* songName){
    if (music.IsPlaying()){
        music.Stop();
        music.Close();
    }
    char buf[255];
    sprintf(buf,"music/%s",songName);
    music.OpenOgg(pSound,buf);
    music.setVolume(sys.musicVolume);
    music.Play(true);
}


//-------------------------------------


HRESULT LoadTextures(){

    HRESULT hr=D3D_OK;

    pics.initContainer("./pics/list.txt");

    for (int i=0;i<pics.count;i++)
        if (!pics.loadpicture(render.lpD3DDevice,i)){
            char buf[255];
            sprintf(buf,"File %s not found!",pics.list[i].name);
            MessageBox(hwndMain,buf,"Error",MB_OK);
            return D3DERR_NOTFOUND;
        }



        return D3D_OK;
}



//------------------

HRESULT InitD3D(){

    HRESULT hr;
    if (FAILED(hr=render.init())){
        MessageBox(hwndMain,"Failed to create D3D9 Interface!","Error",0);
        return hr;
    }

    if (sys.antialias>0)
        render.antialias=true;
    if (FAILED(hr=render.setmode(hwndMain,sys.width,sys.height,sys.bits,sys.windowed,sys.enableDepthStencil))){
        MessageBox(hwndMain,"Error while setting video mode!","Error",0);
        return hr;
    }

    if (FAILED(hr=LoadTextures())){
        MessageBox(hwndMain,"Error while loading textures!","Error",0);
        return hr;
    }


    return 0;

}




//-------------------------
void WINAPI DeleteInput(){
    input.deleteMouse();
    input.deleteJoystick();
    input.deleteKeyboard();
    input.deleteInput();    
}

//----------------------------------------

bool WINAPI InitInput(){

    if (input.initInput(hinstanceMain)){

        if (!(input.initKeyboard(hwndMain))){ 
            input.deleteInput();
            return false;
        }
        sys.joypresent=input.initJoystick(hwndMain);

        if (!input.initMouse(hwndMain)){
            input.deleteInput();
            return false;
        }



    } else 
        return false;

    return true;
}

//--------------------------------------
void LoadKeyData(){
    FILE* f;

    f=fopen("keys.cfg","rt");
    if (f){
     for (int i=0;i<4;i++){
      unsigned int tmp=0;
      fscanf(f,"%u",&tmp);
      KeyCodes[i][0]=(unsigned char)tmp;
     }
     fgetc(f);
     for (int i=0;i<4;i++){
      unsigned int tmp=0;
      fscanf(f,"%u",&tmp);
      KeyCodes[i][1]=(unsigned char)tmp;
     }
     fclose(f);
    } else
    {
      f= fopen("keys.cfg","wt+");
      fprintf(f,"200 208 205 203\n");
      fprintf(f,"17 31 32 30\n");
      KeyCodes[0][0]=200; 
      KeyCodes[1][0]=208; 
      KeyCodes[2][0]=205;
      KeyCodes[3][0]=203;
      
      KeyCodes[0][1]=17; 
      KeyCodes[1][1]=31; 
      KeyCodes[2][1]=32;
      KeyCodes[3][1]=30;

      fclose(f);
    }

}
//------------------------------------

void ReadKeys(){

    memset(Keys,0,GameKeyCount);
    mousepowx=mousepowy=0l;

    globalKey=0;

    input.updateKeyData();
    input.updateMouseData();


    if ((input.keydata[KeyCodes[0][0]]& 0x80)||
        (input.keydata[KeyCodes[0][1]]& 0x80)){
            Keys[0]=1; globalKey=38;}

    if ((input.keydata[KeyCodes[1][0]]& 0x80)||
        (input.keydata[KeyCodes[1][1]]& 0x80)){
            Keys[1]=1; globalKey=40;}

    if ((input.keydata[KeyCodes[2][0]]& 0x80)||
        (input.keydata[KeyCodes[2][1]]& 0x80)){
        Keys[2]=1; globalKey=VK_RIGHT;
    }

    if ((input.keydata[KeyCodes[3][0]]& 0x80)||
        (input.keydata[KeyCodes[3][1]]& 0x80)){
        Keys[3]=1; globalKey=VK_LEFT;
    }

    if ((input.keydata[DIK_RCONTROL]& 0x80)||
        (input.keydata[DIK_LCONTROL]& 0x80)){
            Keys[4]=1; mainkey=true; 
    }


    if (input.keydata[DIK_RETURN]& 0x80){
        globalKey=13;
    }
    if (input.keydata[DIK_ESCAPE]& 0x80){
        globalKey=27;
    }

    if (input.keydata[DIK_TAB]& 0x80) {Keys[5]=1; mapkey=true;}
    if (input.keydata[DIK_SPACE]& 0x80) {Keys[6]=1;}
    if (input.keydata[DIK_RSHIFT]& 0x80) Keys[7]=1;


    
    
    

    if (input.mstate.lX!=0) 
        mousepowx=input.mstate.lX;
  
    if (input.mstate.lY!=0) 
    mousepowy=input.mstate.lY;

    if (input.mstate.lZ<0) 
     Keys[7]=1;
  
 
    if (input.mstate.rgbButtons[0])
        Keys[4]=1;
    if (input.mstate.rgbButtons[1])
        Keys[6]=1;




    if (sys.joypresent){

        input.updateJoyData();

        if ( input.jstate.lY < 0 ) {Keys[0]=1; globalKey=38;}
        if ( input.jstate.lY > 0 ) {Keys[1]=1; globalKey=40;}
        if ( input.jstate.lX > 0 ) Keys[2]=1;
        if ( input.jstate.lX < 0 ) Keys[3]=1;

        if(input.jstate.rgbButtons[0]&0x80) {Keys[4]=1; globalKey=13; mainkey=true;}
        if(input.jstate.rgbButtons[1]&0x80) {Keys[5]=1; mapkey=true;}
        if(input.jstate.rgbButtons[2]&0x80) {Keys[6]=1;/* globalKey=27;*/}

    } 


    if ((mapkey)&&(!Keys[5])){
        mapkey=false;
        ShowMiniMap=!ShowMiniMap;

    }

    if ((mainkey)&&(!Keys[4])){
        mainkey=false;
        triger=true;
    }
    else triger=false;

}

//-------------------

void DrawSomeText(){

    char buf[80];

    //sprintf(buf,"AvailSysMem : %d KB",AvailSysMem()/1000);
    //WriteText(20,40,render.spraitas,&pics.images[10],buf, 0.8f,0.6f,0.6f);
    sprintf(buf,"FPS : %d",FPS());
    WriteText(20,20,render.spraitas,&pics.images[10],buf, 0.8f,0.6f,0.6f);
    if (isServer){
        sprintf(buf,"Client count : %d",serveris.clientCount());
        WriteText(20,40,render.spraitas,&pics.images[10],buf, 0.8f,1,1);
    }
    

}


//------------------
void DrawMap(float r=1.0f,float g=1.0f, float b=1.0f){

    int tmpy=0;


    for (int a=psky-scry; a<psky ;a++){
        int tmpx=0; 
        for (int i=pskx-scrx;i<pskx;i++){
            if (mapas.tiles)
                if (mapas.tiles[a])
                    pics.findpic("tileset.bmp")->Blt(render.spraitas,32*tmpx+pushx-posx,32*tmpy+pushy-posy,mapas.tiles[a][i]-1,
                    1.0f,1.0f,1.0f,0.0,r,g,b,true); 




            tmpx++;
        }

        tmpy++;

    }



    for (int i=0;i<mapas.decals.count();i++)
        mapas.decals[i].draw(pics.images[15],render,pskx,scrx,psky,scry,pushx,posx,pushy,posy);


    for (unsigned long i=0; i<mapas.items.count();i++){

        if (mapas.items[i].value<5){
            if ((round(mapas.items[i].y)<psky*32)&&(round(mapas.items[i].y)>=((psky-scry)*32))&&
                ((round(mapas.items[i].x)<pskx*32))&&(round(mapas.items[i].x)>=((pskx-scrx)*32)))

                pics.images[11].Blt(render.spraitas,(round(mapas.items[i].x))-((pskx-scrx)*32)+pushx-posx,(round(mapas.items[i].y))-((psky-scry)*32)+pushy-posy,(mapas.items[i].value-1)*4+itmframe,
                1.0f,1.0f,1.0f,0.0,r,g,b,true);
        }
        else
            if ((round(mapas.items[i].y)<psky*32)&&(round(mapas.items[i].y)>=((psky-scry)*32))&&
                ((round(mapas.items[i].x)<pskx*32))&&(round(mapas.items[i].x)>=((pskx-scrx)*32)))

                pics.images[7].Blt(render.spraitas,(round(mapas.items[i].x))-((pskx-scrx)*32)+pushx-posx,(round(mapas.items[i].y))-((psky-scry)*32)+pushy-posy,mapas.items[i].value-5,
                1.0f,1.0f,1.0f,0.0,r,g,b,true);

    }

    

    

    for (int z=0; z<bulbox.count(); z++){
        pics.images[6].Blt(render.spraitas,round(bulbox.buls[z].x)-((pskx-scrx)*32)+pushx-posx,
            round(bulbox.buls[z].y)-((psky-scry)*32)+pushy-posy,bulbox.buls[z].frame,1.0f,1.0f,1.0f,bulbox.buls[z].angle-(3.14f/2.0f),1,1,1,true); 
    }


    for (int i=0;i<klientai+2;i++){
        if (mapas.mons[mapas.enemyCount+i].alive)
            mapas.mons[mapas.enemyCount+i].draw(pics.images[5],render,pskx,scrx,psky,scry,pushx,posx,pushy,posy);
    }



    if (isServer){
        for (unsigned int i = 0; i < serveris.clientCount(); i++)
        {
            if (mapas.mons[mapas.enemyCount+i+1].alive)
            {
                mapas.mons[mapas.enemyCount+i+1].draw(pics.images[5],render,pskx,scrx,psky,scry,pushx,posx,pushy,posy);
            }
        }
    }

    if (mapas.enemyCount){
        for (int i=0; i<mapas.enemyCount; i++){
            float mong;
            if (mapas.mons[i].item)
                mong=0.0;
            else
                mong=g;
            if ((round(mapas.mons[i].y)<psky*32)&&(round(mapas.mons[i].y)>=((psky-scry)*32))&&
                ((round(mapas.mons[i].x)<pskx*32))&&(round(mapas.mons[i].x)>=((pskx-scrx)*32)))

                mapas.mons[i].draw(pics.images[mapas.mons[i].race+1],render,pskx,scrx,psky,scry,pushx,posx,pushy,posy);

        }
    }

    


}

//-------------------------------------------
void DrawMiniMap(int x, int y){

    pics.images[12].Blt(render.spraitas,x,y,0,0.6f,mapas.width*2.0f,mapas.height*2.0f);

    for (int i=0;i<mapas.height;i++)
        for (int a=0; a<mapas.width;a++){ 
            int frame=0;
            if ((mapas.colide)&&(mapas.tiles[i][a]!=65)&&(mapas.tiles[i][a]!=67)&&(mapas.tiles[i][a]!=69)
                &&(mapas.tiles[i][a]!=71))
                frame=mapas.colide[i][a];
            if (frame)
                pics.images[12].Blt(render.spraitas,a*4+x,i*4+y,frame,0.6f);
        }

        pics.images[12].Blt(render.spraitas,x+(round(mapas.mons[mapas.enemyCount].x/32.0f)*4),
                                            y+(round(mapas.mons[mapas.enemyCount].y/32.0f)*4),
                                            3,0.6f);

        for (int i=0;i<mapas.items.count();i++){
            pics.images[12].Blt(render.spraitas,x+(round(mapas.items[i].x/32.0f)*4),
                                            y+(round(mapas.items[i].y/32.0f)*4),
                                            4,0.6f);
        }


}

//---------------------
//sudelioja 3zenkli skaiciu is bmp
void DrawNum(int x, int y,int num){
    int arr[3]={0};

    int i=2;
    while (num!=0){
        arr[i]=num%10;
        num=num/10;
        i--;
    }

    for (int a=0; a<3; a++)
        pics.images[8].Blt(render.spraitas,x+a*16,y,arr[a],0.6f,0.98f,0.98f); 




}
//-------------------------------
void SendItemCreation(float x, float y, int value, unsigned int clientIndex){

    char buferis[256];
    int index=0;
    buferis[0]='i'; buferis[1]='t'; buferis[2]='c';
    index+=3;
    memcpy(&buferis[index],&x,sizeof(float));
    index+=sizeof(float);
    memcpy(&buferis[index],&y,sizeof(float));
    index+=sizeof(float);
    memcpy(&buferis[index],&value,sizeof(int));
    index+=sizeof(int);
    serveris.sendData(clientIndex,buferis,index);
    
}




//---------------------------
void KillPlayer(int index){
    mapas.mons[mapas.enemyCount+index].shot=true;
    mapas.mons[mapas.enemyCount+index].frame=mapas.mons[mapas.enemyCount+index].weaponCount*4;
    AdaptSoundPos(2,mapas.mons[mapas.enemyCount+index].x,mapas.mons[mapas.enemyCount+index].y);
    smotai[2].play();
    

    mapas.mons[mapas.enemyCount+index].stim=0;
    timeleft=mapas.timeToComplete;
    mapas.mons[mapas.enemyCount+index].hp=100;
    
    Decal decalas;
    decalas.x=round(mapas.mons[mapas.enemyCount+index].x);
    decalas.y=round(mapas.mons[mapas.enemyCount+index].y);
    decalas.r=1.0f;
    decalas.g=decalas.b=0.0f;
    decalas.frame=rand()%2;
    mapas.decals.add(decalas);
}

//------------------
void KillEnemy(int ID){
    if (ID<mapas.enemyCount){
        
        mapas.mons[ID].shot=true;

        AdaptSoundPos(3,mapas.mons[ID].x,mapas.mons[ID].y);
        smotai[3].play();

        Decal decalas;
        decalas.x=round(mapas.mons[ID].x);
        decalas.y=round(mapas.mons[ID].y);
        decalas.r=1.0f;
        decalas.g=decalas.b=0.0f;
        decalas.frame=rand()%2;
        mapas.decals.add(decalas);

        if (mapas.mons[ID].item){ //jei monstras turejo prarijes kazkoki daikta
            CItem swalenitem;
            swalenitem.value=mapas.mons[ID].item;
            swalenitem.x=mapas.mons[ID].x;
            swalenitem.y=mapas.mons[ID].y;
            mapas.addItem(swalenitem.x,swalenitem.y,swalenitem.value);
            mapas.mons[ID].item=0;
            if (isServer){
                for (unsigned int i=0;i<serveris.clientCount();i++)
                    SendItemCreation(swalenitem.x,swalenitem.y,swalenitem.value,i);
            }

        }
    
    }
    else
        KillPlayer(ID-254);



}



//--------------------
//jei kulka pataike i kazka
bool OnHit(Bullet& bul){

    int dmg=1;
    if (bul.isMine)
        dmg=3;
     
    

    bool hit=false;
    int tmpID=0;
    int players=1;
    if (isServer)
        players+=serveris.clientCount();
    if (isClient)
        players+=klientai;
    for (int i=0;i<mapas.enemyCount+players;i++){
        if (CirclesColide(mapas.mons[i].x,mapas.mons[i].y,8,bul.x,bul.y,8)){

            tmpID=mapas.mons[i].id;
            if ((bul.parentID!=tmpID)&&(!mapas.mons[i].shot)&&(!mapas.mons[i].spawn)){
                if (!hit)
                    hit=true;
                
                    mapas.mons[i].hit=true;
                    mapas.mons[i].hp-=dmg;
                
            }
        }
    }

    return hit;
}


//---------------------------
//nupaiso kiek turi gyvybiu soviniu etc
void DrawStats(){

    pics.images[9].Blt(render.spraitas,30,435,2,0.6f);
    DrawNum(58,440,mapas.mons[mapas.enemyCount].hp);


    pics.images[9].Blt(render.spraitas,120,440,0,0.6f); 
    DrawNum(155,440,mapas.mons[mapas.enemyCount].ammo);

    if (mapas.misionItems){
        pics.images[9].Blt(render.spraitas,220,440,1,0.6f);
        DrawNum(255,440,goods);
    }

    if (mapas.timeToComplete){
        int min=timeleft/60;
        int sec=timeleft-min*60;
        char buf[50];
        sprintf(buf,"%d:%d",min,sec);
        WriteText(450,20,render.spraitas,&pics.images[10],buf,0.7f,1.5f,1.5f,1.0f,0.5f,0.5f);
    }

}

//--------------------------
//intro screenas
void DrawTitleScreen(){

    pics.images[0].Blt(render.spraitas,0,0,0,1.0f,1.25f,1.9f);
    pics.images[16].Blt(render.spraitas,0,0,0);



    char buf[80];   

    sprintf(buf,"Jrs%dul",0);
    WriteText(540,10,render.spraitas,&pics.images[10],buf);
    sprintf(buf,"%d",2007);
    WriteText(550,30,render.spraitas,&pics.images[10],buf);

}


//------------------------
//the hero and camera movement
void Game::MoveDude(){


    /*if (keys[3])
        mapas.mons[mapas.enemyCount].rotate(0.1f);*/

    /*if (keys[2])
        mapas.mons[mapas.enemyCount].rotate(-0.1f);*/




    if ((Keys[0])||(Keys[1])||(Keys[2])||(Keys[3])){
        int dirx,diry;
        unsigned char cid=0;
        float speed=0.0f;
        float sspeed=0.0f;
        if (Keys[1])
            speed=-1.0f;
        if (Keys[0])
            speed=1.0f;
        if (Keys[2])
            sspeed=-1.0f;
        if (Keys[3])
            sspeed=1.0f;


        if (isServer)
            mapas.mons[mapas.enemyCount].move(speed, sspeed, 8.0f, mapas.colide,
                                              mapas.width, mapas.height, mapas.mons,
                                              mapas.enemyCount + serveris.clientCount() + 1, &dirx,&diry);
        else
            mapas.mons[mapas.enemyCount].move(speed,sspeed,8.0f,mapas.colide, mapas.width, mapas.height, mapas.mons,mapas.enemyCount+klientai+1,&dirx,&diry);

        //stumdom "kamera"
        if ((mapas.height>scry)&&(diry==1)){
            if ((psky>=scry)&&((mapas.mons[mapas.enemyCount].y+16)/32<=psky-scry/2)&&((DownBorder>0)||(UpBorder<64))){

                if ((UpBorder<32)&&(DownBorder<32))
                    pushy++; 

                if ((psky==scry)&&(UpBorder<64))
                    UpBorder++;
                else
                    if ((psky==mapas.height)&&(DownBorder>0))
                        DownBorder--; 
            }

            if ((pushy>31)&&(psky>scry)&&(DownBorder==0)){
                psky--;
                pushy=0;     
            }

        }
        //-----
        if ((mapas.height>scry)&&(diry==2)){      
            if ((psky<=mapas.height)&&((mapas.mons[mapas.enemyCount].y+16)/32>=psky-scry/2)&&((DownBorder<64)||(UpBorder>0))){

                if ((DownBorder<32)&&(UpBorder<32))
                    pushy--; 


                if ((psky==scry)&&(UpBorder>0))
                    UpBorder--;
                else
                    if ((psky==mapas.height)&&(DownBorder<64))
                        DownBorder++;

            }


            if ((pushy<-31)&&(psky<mapas.height)&&(UpBorder==0)){
                psky++;
                pushy=0;
            }

        }
        //---
        if ((mapas.width>scrx)&&(dirx==1)){
            if ((pskx>=scrx)&&((mapas.mons[mapas.enemyCount].x+16)/32<=pskx-scrx/2)&&((RightBorder>0)||(LeftBorder<64))){

                if ((LeftBorder<32)&&(RightBorder<32))
                    pushx++; 


                if ((pskx==scrx)&&(LeftBorder<64))
                    LeftBorder++;
                else
                    if ((pskx==mapas.width)&&(RightBorder>0))
                        RightBorder--; 

            }

            if ((pskx>scrx)&&(pushx>31)&&(RightBorder==0)){
                pskx--;
                pushx=0;
            }

        }

        //---
        if ((mapas.width>scrx)&&(dirx==2)){
            if ((pskx<=mapas.width)&&((mapas.mons[mapas.enemyCount].x+16)/32>=pskx-scrx/2)&&((RightBorder<64)||(LeftBorder>0))){

                if ((RightBorder<32)&&(LeftBorder<32))
                    pushx--;  

                if ((pskx==scrx)&&(LeftBorder>0))
                    LeftBorder--;
                else
                    if ((pskx==mapas.width)&&(RightBorder<64)){
                        RightBorder++;

                    }

                    if ((LeftBorder==0)&&(pskx<mapas.width)&&(pushx<-31)){
                        pskx++;
                        pushx=0;
                    }
            }
        }

    }

    listiner->SetPosition(mapas.mons[mapas.enemyCount].x,0,mapas.mons[mapas.enemyCount].y,DS3D_IMMEDIATE);
    
}



//--------------
void Game::AddaptMapView()
{
    pushx=0;
    pushy=0;

    if (mapas.width < 22)
    {
        scrx = mapas.width;
        posx = (sys.ScreenWidth / 2 - ((mapas.width*32)/2))*-1;
    }
    else{
        if (mapas.width > 22){
            scrx = sys.ScreenWidth / 32 + 2;
            posx = 16;
        }
    }

    if (mapas.height < 17)
    {
        scry = mapas.height;
        posy = (sys.ScreenHeight / 2 - ((mapas.height*32)/2))*-1;
    }
    else
    {
        if (mapas.height>17)
        {
            scry = sys.ScreenHeight / 32 + 2;
            posy = 16;
        }
    }

    pskx=scrx;
    psky=scry;
}

//---------------

void Game::findpskxy()
{
    UpBorder=0;
    DownBorder=0;
    LeftBorder=0;
    RightBorder=0;


    int pusesizx=scrx / 2; //puse matomo ekrano
    int pusesizy=scry / 2;
    //-
    pskx = round(mapas.mons[mapas.enemyCount].x/32.0f)+pusesizx; //mapas.mons[mapas.monscount]x+puseekrano.x
    if (pskx<scrx)
        pskx=scrx;
    if (pskx>mapas.width) 
        pskx=mapas.width;
    //-
    psky=round(mapas.mons[mapas.enemyCount].y/32.0f)+pusesizy;
    if (psky<scry)
        psky=scry;
    if (psky>mapas.height) 
        psky=mapas.height;




    if ((scry<mapas.height)&&(psky==mapas.height)){
        DownBorder=64;
        pushy=-32;
    }

    if ((scry<mapas.height)&&(psky==scry)){
        UpBorder=64;
        pushy=32;
    }

    if ((scrx<mapas.width)&&(pskx==scrx)){
        pushx=32;
        LeftBorder=64;
    }

    if ((scrx<mapas.width)&&(pskx==mapas.width)){
        RightBorder=64;
        pushx=32;
    }

}

//---------------------------
void Game::PutExit(){

    int ex = mapas.exit.x;
    int ey = mapas.exit.y;

    mapas.addItem(ex*32.0f,ey*32.0f,4);
    ext=true;
}

//----------------------------
void Game::GoToLevel(int level, int otherplayer){
    ext=false;
    bulbox.destroy();
    mapas.Destroy();
    char mapname[255];
    mapai.getMapName(level,mapname);
    mapas.Load(mapname,true,otherplayer);
    goods=mapas.misionItems;
    timeleft=mapas.timeToComplete;
    mapas.mons[mapas.enemyCount].x=(float)mapas.start.x;
    mapas.mons[mapas.enemyCount].y=(float)mapas.start.y;
    mapas.mons[mapas.enemyCount].id=254;
    AddaptMapView();
    findpskxy();
    fadein=true;
    objectivetim=200;
    if (ShowMiniMap)
        ShowMiniMap=false;
}

//-----------------------------------
//klientas servui isiuncia infa apie paimta daikta
void Game::SendItemCRemove(int itemIndex){
    char bufer[1024];
    int index=0;
    bufer[index]='i'; bufer[index+1]='t'; bufer[index+2]='m';
    index+=3;
    memcpy(&bufer[index],&itemIndex,sizeof(int));
    index+=sizeof(int);
    clientas.sendData(bufer,index);
}

//-----------------------------------
void Game::SendItemSRemove(int ItemIndex, int clientIndex, bool playerTaked){
    char bufer[1024];
    int index=0;
    bufer[0]='i';
    bufer[1]='t';
    bufer[2]='m';
    index+=3;
    memcpy(&bufer[index],&ItemIndex,sizeof(int));
    index+=sizeof(int);
    unsigned char plt=0;
    if (playerTaked)
        plt=1;
    memcpy(&bufer[index],&plt,sizeof(unsigned char));
    index++;
    serveris.sendData(clientIndex,bufer,index);
}

//-------------------------------------
//mapo pavadinimas, klientu skaicius
void Game::SendMapInfo(int clientIndex){
    char bufer[1024];
    int index=0;
    bufer[0]='s';bufer[1]='r';bufer[2]='v';

    index+=3;
    int len = (int)strlen(mapas.name); 
    int totalpacketlen=len+sizeof(int)+1;
    memcpy(&bufer[index],&totalpacketlen,sizeof(int)); //paketo dydis
    index+=sizeof(int);
    memcpy(&bufer[index],&len,sizeof(int)); //mapo pavadinimo ilgis
    index+=sizeof(int);
    memcpy(&bufer[index],mapas.name,strlen(mapas.name)); //mapo pavadinimas
    index+=(int)strlen(mapas.name);
    unsigned int kiekis=serveris.clientCount(); 
    memcpy(&bufer[index],&kiekis,sizeof(unsigned int)); //klientu skaicius
    index+=sizeof(unsigned int);

    serveris.sendData(clientIndex,bufer,index);

}
//------------------------------------------------
//monster races amd item positions
void Game::SendMapData(int clientIndex){

    char bufer[1024];
    int index=3;
    bufer[0]='d'; bufer[1]='a'; bufer[2]='t';
    memcpy(&bufer[index],&mapas.enemyCount,sizeof(int));
    index+=sizeof(int);
    for (int i=0;i<mapas.enemyCount;i++){
        memcpy(&bufer[index],&mapas.mons[i].race,sizeof(int));
        index+=sizeof(int);
    }
    
    int itmcount=(int)mapas.items.count();
    memcpy(&bufer[index],&itmcount,sizeof(int));
    index+=sizeof(int);
    for (unsigned long i=0;i<mapas.items.count();i++){
        memcpy(&bufer[index],&mapas.items[i].x,sizeof(float));
        index+=sizeof(float);
        memcpy(&bufer[index],&mapas.items[i].y,sizeof(float));
        index+=sizeof(float);
        memcpy(&bufer[index],&mapas.items[i].value,sizeof(int));
        index+=sizeof(int);
        
    }

    serveris.sendData(clientIndex,bufer,index);
}

//---------------------------------
//siuncia servui msg kad keistu mapa
void Game::SendWarpMessage()
{
    char buferis[32];
    int index=0;
    buferis[0]='n'; buferis[1]='x'; buferis[2]='t';
    index+=3;
    clientas.sendData(buferis,index);
}


//-------------------------
void Game::ItemPickup()
{

    for (unsigned long i=0; i<mapas.items.count(); i++)
    {
        if (CirclesColide(mapas.mons[mapas.enemyCount].x,mapas.mons[mapas.enemyCount].y,8,
            mapas.items[i].x,mapas.items[i].y,4)){

                int item=mapas.items[i].value;
                if ((item != 0) && ((item != 6) || (mapas.mons[mapas.enemyCount].hp < 100)))
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
                    if (isClient)
                        SendItemCRemove(i);
                    if (isServer){
                        for (unsigned int a=0;a<serveris.clientCount();a++)
                            SendItemSRemove(i,a,true);
                    }

                    //------------------------
                    mapas.removeItem(i);



                    if ((item<5)&&(item>0)){
                        goods--;
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
                    mapai.current++;
                    if (mapai.current==mapai.count()){
                        mapai.current=0;
                        EndScreen=true;
                        PlayNewSong("Crazy.ogg");

                    }

                    int kiek=0;
                    if (isServer)
                        kiek=serveris.clientCount();
                    if (!isClient)
                        GoToLevel(mapai.current,kiek);
                    else
                        SendWarpMessage();

                    if (isServer){
                        for (int a =0; a<(int)serveris.clientCount();a++){
                            SendMapInfo(a);
                            SendMapData(a);
                        }
                    }


                }

        }
    }

}



//---------------------------------
//serverio threadas laukiantis prisijungimu
long Game::Threadproc(void *param){
    while (!killthread)
    {
        int res=serveris.waitForClient();
        if (res){

            Dude naujas;
            mapas.mons.add(naujas);

            mapas.mons[mapas.mons.count()-1].id=mapas.mons[mapas.mons.count()-2].id+1;

            for (int i=0; i < (int)serveris.clientCount(); i++)
            {
                SendMapInfo(i);
            }

            SendMapData(serveris.clientCount()-1);
            //----end

        }
    }
    return 0;
}


//---------------------------------
void Game::InitServer()
{
    isServer=true;
    killthread=false;
    serveris.launch(NetPort);

    threadman.create(Threadproc);
}
//--------------------------------
void Game::StopServer()
{
    isServer=false;
    killthread=true;
    serveris.shutDown();
    threadman.close();
}
//---------------------------------
bool Game::JoinServer(const char* ip, unsigned port)
{
    if (clientas.connectServer(ip,port)){
        isClient=true;
        return true;
    }

    return false;
}
//--------------------------
void Game::QuitServer()
{
    isClient = false;
    char buf[4];
    buf[0]='q'; buf[1]='u'; buf[2]='t';
    clientas.sendData(buf,3);
    clientas.disconnectServer();
}

//--------------------------------
void Game::SendClientDoorState(int doorx,int doory, unsigned char doorframe)
{
    int index=0;
    char bufer[256];
    bufer[0]='d'; bufer[1]='o'; bufer[2]='r';
    index+=3;
    memcpy(&bufer[index],&doorx,sizeof(int));
    index+=sizeof(int);
    memcpy(&bufer[index],&doory,sizeof(int));
    index+=sizeof(int);
    memcpy(&bufer[index],&doorframe,sizeof(unsigned char));
    index+=sizeof(unsigned char);
    clientas.sendData(bufer,index);
}
//--------------------------------
void Game::SendServerDoorState(unsigned int clientIndex, int doorx,int doory, unsigned char doorframe)
{
    int index=0;
    char bufer[128];
    bufer[0]='d';
    bufer[1]='o';
    bufer[2]='r';
    index+=3;
    memcpy(&bufer[index],&doorx,sizeof(int));
    index+=sizeof(int);
    memcpy(&bufer[index],&doory,sizeof(int));
    index+=sizeof(int);
    memcpy(&bufer[index],&doorframe,sizeof(unsigned char));
    index+=sizeof(unsigned char);
    
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


        if (Keys[6])
        {
            if ((mapas.tiles[dry][drx]==67)
                ||(mapas.tiles[dry][drx]==65)
                ||(mapas.tiles[dry][drx]==69)
                ||(mapas.tiles[dry][drx]==71))
            {// atidarom
                mapas.tiles[dry][drx]++;
                mapas.colide[dry][drx]=false;
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
                    mapas.colide[dry][drx]=true;
                    door_tim=1;
                    AdaptSoundPos(9, mapas.mons[mapas.enemyCount].x, mapas.mons[mapas.enemyCount].y);
                    ss->playsound(9);
                }
            }

            if (isServer)
            {//isiunciam info visiems klientams
                for (unsigned int i=0;i<serveris.clientCount();i++)
                {
                    SendServerDoorState(i,drx,dry,mapas.tiles[dry][drx]);
                }
            }

            if (isClient)
            {//isiunciam infa servui
                SendClientDoorState(drx,dry,mapas.tiles[dry][drx]);
            }

        }
    } else{
        door_tim++;
        if (door_tim==30)
            door_tim=0;
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
        (slimtim==5)){
            mapas.mons[index].hp-=2;
            mapas.mons[index].hit=true;
    }
}
//-----------------------------
void Game::SendBulletImpulse(int monsterindex, int ammo, int clientIndex, bool isMine)
{
    int index=0;
    char buf[1024];
    buf[0]='s';
    buf[1]='h';
    buf[2]='t';
    index+=3;
    memcpy(&buf[index],&monsterindex,sizeof(int));
    index+=sizeof(int);
    memcpy(&buf[index],&ammo,sizeof(int));
    index+=sizeof(int);
    memcpy(&buf[index],&isMine,sizeof(unsigned char));
    index+=sizeof(unsigned char);

    serveris.sendData(clientIndex,buf,index);
}
//------------------------------------------
void Game::SendClientAtackImpulse(int victimID, int hp)
{
    char buferis[1024];
    int index=0;
    buferis[0]='a'; buferis[1]='t'; buferis[2]='k';
    index+=3;
    memcpy(&buferis[index],&victimID,sizeof(int));
    index+=sizeof(int);
    memcpy(&buferis[index],&hp,sizeof(int));
    index+=sizeof(int);
    clientas.sendData(buferis,index);
}
//----------------------------------------
void Game::SendAtackImpulse(unsigned int clientIndex, int victim, int hp)
{
    char buferis[1024];
    int index=0;
    
    buferis[0]='a';buferis[1]='t';buferis[2]='k';
    index+=3;
    memcpy(&buferis[index],&victim,sizeof(int));
    index+=sizeof(int);
    memcpy(&buferis[index],&hp,sizeof(int));
    index+=sizeof(int);
    serveris.sendData(clientIndex,buferis,index);
}

//----------------------------------
void Game::BeatEnemy(int aID, int damage)
{
    if (mapas.mons[aID].canAtack){
        mapas.mons[aID].atack(false,false,&bulbox);
        Vector3D vec = MakeVector(16.0f,0,mapas.mons[aID].angle);

        for (int i=0;i<mapas.mons.count();i++){

            if (mapas.mons[aID].hitIt(mapas.mons[i],vec.x,vec.y,damage)>-1){

                

                if (isServer){
                    //server stuff
                    for (int a=0;a<serveris.clientCount();a++){

                        int z=i;
                        if (i>=mapas.enemyCount){
                            if ((i-mapas.enemyCount-1)<a)
                                z=i+1;
                            if ((i-mapas.enemyCount-1)==a)
                                z=mapas.enemyCount;
                        }
                    
                        SendAtackImpulse(a,z,mapas.mons[i].hp);
                    }
                }
                if (isClient){//turbut atakavo playeris
                    
                    SendClientAtackImpulse(mapas.mons[i].id,mapas.mons[i].hp);
                }


                if (mapas.mons[i].hp<=0){
                    //if (i>mapas.enemyCount)
                    //  KillPlayer(i-mapas.enemyCount);
                    //else
                    //  if (i<mapas.enemyCount)
                    //      KillEnemy(i);
                }

            }

        }
    }
}



//-------------------------
void Game::MonsterAI(int index){

    SoundSystem * ss = SoundSystem::getInstance();

    int linijosIlgis=0;

    int tmpcnt=bulbox.count();

    if (mapas.mons[index].spawn){
        mapas.mons[index].respawn();
        if (!mapas.mons[index].spawn){
            for (int i=0; i< mapas.mons.count(); i++){
                if (i!=index){
                    if (CirclesColide(mapas.mons[index].x,mapas.mons[index].y,10.0f,mapas.mons[i].x,mapas.mons[i].y,8.0f)){
                        if (i<mapas.enemyCount)
                            KillEnemy(i);
                        else
                            KillPlayer(i-mapas.enemyCount);
                    }
                }
            }

        }
    }

    
    if ((!mapas.mons[index].shot)&&(!mapas.mons[index].spawn)){//jei nieks nepashove tai lets go :)

        int dx=0,dy=0;
        unsigned char cid=0;
        mapas.mons[index].move(1.0f,0.0f,8.0f,mapas.colide,mapas.width,mapas.height,mapas.mons,mapas.enemyCount+1+serveris.clientCount(),&dx,&dy);
        
        
        

        if ((dx==0)||(dy==0))
            mapas.mons[index].rotate(0.1f);




        //1 kas kolidina super duper figuroje
        int kiekplayeriu=1;
        if (isServer)
            kiekplayeriu+=serveris.clientCount();
        int victimindex=0;
        for (int i=mapas.enemyCount;i<mapas.enemyCount+kiekplayeriu;i++){
            if ((CirclesColide(mapas.mons[i].x,mapas.mons[i].y,16,mapas.mons[index].x,mapas.mons[index].y,128))
                &&(!mapas.mons[i].shot)&&(!mapas.mons[i].spawn)){
                victimindex=i;
                break;
            }
        }
        
        if (victimindex){
            //2 rask spindulio kelia iki pirmojo kolidinancio
            Vector3D * line = Line(round(mapas.mons[index].x),round(mapas.mons[index].y),
                round(mapas.mons[victimindex].x),round(mapas.mons[victimindex].y),
                linijosIlgis,32);

            if (linijosIlgis)
            {
                bool colide=false;
                for(int i=0;i<linijosIlgis;i++){
                    if (mapas.colide[(unsigned)line[i].y][(unsigned)line[i].x]){
                     colide=true;
                     
                     break;
                    }

                }
                if (!colide){
                    mapas.mons[index].enemyseen=true;
                //yessss!!!! veikia
                    mapas.mons[index].angle=-atan2(mapas.mons[victimindex].y -mapas.mons[index].y  , mapas.mons[victimindex].x - mapas.mons[index].x);
                //pakeiciam monstro kampa taip kad judetu link aukos
                }
                delete []line;
            }
            
        }
        



        
        if ((mapas.mons[index].enemyseen)){
            if (mapas.mons[index].race==3){//jei mentas, tai pasaudom
                mapas.mons[index].enemyseen=false;
                if (mapas.mons[index].canAtack){
                    mapas.mons[index].atack(true,false,&bulbox);
                    mapas.mons[index].ammo++;
                    mapas.mons[index].reloadtime=0;
                    if (isServer){ //pasiunciam klientui
                        for (unsigned int i=0;i<serveris.clientCount();i++){
                            SendBulletImpulse(index,mapas.mons[index].ammo,i,false);
                        }
                    }
                }
                else
                    mapas.mons[index].reload(50);
                

            }
            else{//baigiam saudima ;)
                if ((linijosIlgis<2)&&(linijosIlgis)){
                    BeatEnemy(index,10);
                    mapas.mons[index].enemyseen=false;
                    if (!mapas.mons[index].canAtack)
                        mapas.mons[index].reload(25);
                }
            }
        }
       

    }

    else 
        if (mapas.mons[index].shot){ //jei mus kazkas pasove shot=true
            mapas.mons[index].splatter();

            if (!mapas.mons[index].shot){
                mapas.mons[index].hp=30+rand()%10;
                mapas.mons[index].appearInRandomPlace(mapas.colide,mapas.width,mapas.height);
            }
        }

//visa sita slamasta reiks sukrauti i virsu kur tikrinama ar ne shot

    if ((!mapas.mons[index].shot)&&(!mapas.mons[index].spawn)){

        if (mapas.mons[index].hit){
            mapas.mons[index].damageAnim();
            if (!mapas.mons[index].hit)
            {
                AdaptSoundPos(11,mapas.mons[index].x,mapas.mons[index].y);
                ss->playsound(11);

                
            }
        }
        //else{
        //  BeatPlayer(index);
                
        //}
    

            //vagia daiktus

            if (mapas.mons[index].race==2){
                for (int i=0;i<mapas.items.count();i++){
                    if ((!mapas.mons[index].item)&&(mapas.items[i].value!=4)&&(mapas.items[i].value!=3)){
                        if (CirclesColide(mapas.items[i].x, mapas.items[i].y, 8.0f, mapas.mons[index].x,mapas.mons[index].y,16.0f)){
                            mapas.mons[index].item=mapas.items[i].value;
                            AdaptSoundPos(5,mapas.items[i].x,mapas.items[i].y);
                            ss->playsound(5);
                            
                            if (isServer){
                                for (int a=0;a<serveris.clientCount();a++){
                                    SendItemSRemove(i,a,false);
                                }
                            }
                            mapas.removeItem(i);
                        }
                    }

                }
            
            }

    }

    if (bulbox.count()>tmpcnt){
        AdaptSoundPos(0,mapas.mons[index].x,mapas.mons[index].y);
        ss->playsound(0);
    }

}
//--------------------------------
//kulku valdymas
void Game::HandleBullets()
{

    for (int i=0;i<bulbox.count();i++){
        bulbox.buls[i].ai(mapas.colide,mapas.width,mapas.height);

        if (OnHit(bulbox.buls[i]))
            if (!bulbox.buls[i].explode){
                bulbox.buls[i].explode=true;
                bulbox.buls[i].frame=2;
            }

        bulbox.removeDead(); //pasaliname neaktyvias kulkas
    }   
}
//-------------------------------
void AnimateSlime(){
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
void Game::LoadFirstMap()
{
    if (!isClient){
        char firstmap[255];
        mapai.getMapName(0,firstmap);
        mapas.Destroy();
        if (!mapas.Load(firstmap)){
            mapas.Destroy();
            mapai.Destroy();
            printf("Can't find first map!\n");
            Works = false;
        }
        goods=mapas.misionItems;
        timeleft=mapas.timeToComplete;
        mapas.mons[mapas.enemyCount].x=(float)mapas.start.x;
        mapas.mons[mapas.enemyCount].y=(float)mapas.start.y;
        mapas.mons[mapas.enemyCount].id=254;
        AddaptMapView();
        findpskxy();

    }
}

//-------------------------------------
void ResetVolume(){
        for (int i=0;i<maxwavs;i++)
                    smotai[i].setVolume(sys.soundFXVolume);
                music.setVolume(sys.musicVolume);
}


//-------------------------------------------------------------
void Game::TitleMeniuHandle()
{

    if (mainmenu.active()){
            if (isServer){
                StopServer();
            }
            if (isClient){
                QuitServer();
            }
            if (!mainmenu.selected)
                mainmenu.getInput(globalKey);
            else{
                switch(mainmenu.state){
                case 0:{
                    mainmenu.reset();
                    TitleScreen=false;
                    IntroScreen=true;
                    PlayNewSong("crazy.ogg");   
                       }break;
                case 1:{
                    netmenu.activate();
                    mainmenu.reset();
                    mainmenu.deactivate();
                       }break;
                case 2:{
                    options.activate();
                    mainmenu.reset();
                    mainmenu.deactivate();
                       }break;
                case 3:{
                    Works = false;
                    mainmenu.reset();
                       }break;
                }
            }
        }

    if (netmenu.active()){
        if (!netmenu.selected)
            netmenu.getInput(globalKey);
        else{
            switch(netmenu.state){
            case 0:{
                netgame.activate();
                netmenu.deactivate();
                netmenu.reset();
                } break;

            case 1:{
                if (netmenu.active())
                    netmenu.deactivate();
                    netmenu.reset();
                    ipedit.activate();  
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
        if (ipedit.active()){
            if (!ipedit.entered){
                
                ipedit.getInput(leterKey,globalKey);
                leterKey=0;
                
            }
            else{
                netmenu.reset();
                PlayNewSong("music.ogg");
                JoinServer(ipedit.text,NetPort);
                ipedit.deactivate();
                ipedit.reset();
            }

            if (ipedit.canceled){
                ipedit.deactivate();
                ipedit.reset();
                //netmenu.reset();
                netmenu.activate();
                
            }

        }
    //----------------
        if (netgame.active()){
            if (!netgame.selected)
                netgame.getInput(globalKey);
            else{
                if (netgame.state==1){
                  IsCoop=false;
                  IsDeathMatch=true;
                }

                if (netgame.state==1){
                  IsCoop=true;
                  IsDeathMatch=false;
                }

                netgame.reset();
                netgame.deactivate();
                TitleScreen=false;
                PlayNewSong("music.ogg");
                InitServer();
                LoadFirstMap();
            }

            if (netgame.canceled){
                netmenu.activate();
                netgame.reset();
                netgame.deactivate();
            }
        }

        if (options.active()){
            if (!options.selected)
                options.getInput(globalKey);
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
                MusicVolumeC.getInput(globalKey);
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
                SfxVolumeC.getInput(globalKey);
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
void IntroScreenHandle(){
    if ((triger)||(!FirstTime)){
        IntroScreen=false;
        HelpScreen=true;
        intro_cline=0;
        intro_cchar=0;
        intro_gline=0;
    }

    if (intro_gline<=lin){
        intro_tim++;
        if (intro_tim==5){
            intro_tim=0;
            intro_cchar++;
            if ((IntroText[intro_cline][intro_cchar]=='\0')){
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
void HelpScreenHandle(){
    itmtim++;
    if (itmtim>10){
        itmframe++; //animuoti daiktai
        if (itmframe>3)
            itmframe=0;
        itmtim=0;
    }
    if ((triger)||(!FirstTime)){
        HelpScreen=false;
        if (FirstTime)
            FirstTime=false;
        PlayNewSong("music.ogg");
        LoadFirstMap();

    }

}

//------------------------------
int PlayerCount(){
    int kiekplayeriu=1;
    if (isServer)
        kiekplayeriu+=serveris.clientCount();
    if (isClient)
        kiekplayeriu+=klientai;
    return kiekplayeriu;
}

//===============================================================

void Game::logic(){

    if ((mapas.timeToComplete)&&(!TitleScreen)&&(!IntroScreen)&&(!EndScreen))
    {
        ms+=10;
        if (ms>=1000){
            if (timeleft)
                timeleft--;
            else 
                KillPlayer(0);
            ms=0;
        }
    }

    if ((isServer)||(isClient))
    {
        SendData();
        GetData();
    }

    if (music.IsPlaying()) //updeitinam muzona
        music.Update();


    if (TitleScreen)
        TitleMeniuHandle();
    else
        if (IntroScreen)
            IntroScreenHandle();
        else
            if (HelpScreen)
                HelpScreenHandle();
            else
                if (EndScreen){
                    if (triger){
                        TitleScreen=true;
                        mainmenu.activate();    
                        FirstTime=true;
                        EndScreen=false;
                        PlayNewSong("Evil.ogg");
                    }
                }

                else
                    if (fadein){
                        fadetim++;
                        if (fadetim==160){
                            fadein=false;
                            fadetim=0;
                        }

                    }


                    else 
                    {   //------------------the game

                        mapas.fadeDecals();

                        if (objectivetim)//judinam taimeri
                            objectivetim--;

                        itmtim++;
                        if (itmtim>10){
                            itmframe++; //animuoti daiktai
                            if (itmframe>3)
                                itmframe=0;
                            itmtim=0;
                        }


                        if ((goods<1)&&(!ext))  //jei nebera itemu dedam exit
                            PutExit(); 


                        AnimateSlime();


                        //herojaus judejimas

                        if (mapas.mons[mapas.enemyCount].shot){ //isitaskymas
                            mapas.mons[mapas.enemyCount].splatter();

                            if (!mapas.mons[mapas.enemyCount].shot){

                                if ((!isClient)&&(!isServer)){
                                    mapas.mons[mapas.enemyCount].x=(float)mapas.start.x;
                                    mapas.mons[mapas.enemyCount].y=(float)mapas.start.y;
                                }
                                else{
                                    mapas.mons[mapas.enemyCount].appearInRandomPlace(mapas.colide,mapas.width,mapas.height);

                                }

                                AddaptMapView();
                                findpskxy();
                            }

                        }


                        if (mapas.mons[mapas.enemyCount].spawn){ //respawn      
                            mapas.mons[mapas.enemyCount].respawn();

                            if ((mapas.mons[mapas.enemyCount].ammo<1)&&(!mapas.mons[mapas.enemyCount].spawn))
                                mapas.mons[mapas.enemyCount].ammo=20;   
                        }



                        if ((mousepowx)&&(!mapas.mons[mapas.enemyCount].shot)&&(!mapas.mons[mapas.enemyCount].spawn))
                            mapas.mons[mapas.enemyCount].rotate(3.14f/(256.0f/-mousepowx));

                        if (!Keys[7])   
                            nextWepPressed=false;

                        if ((Keys[7])&&(!Keys[4])&&(!nextWepPressed)){
                            nextWepPressed=true;
                            mapas.mons[mapas.enemyCount].chageNextWeapon();
                        }


                        if ((Keys[0]||Keys[1]||Keys[2]||Keys[3])&&(!mapas.mons[mapas.enemyCount].shot)
                                &&(!mapas.mons[mapas.enemyCount].spawn)&&(mapas.mons[mapas.enemyCount].canAtack))
                            MoveDude();



                        // jei uzeina ant daikto, ji pasiima
                        ItemPickup();

                        //saunam
                        if ((Keys[4])&&(!mapas.mons[mapas.enemyCount].shot)&&(mapas.mons[mapas.enemyCount].alive)&&(!mapas.mons[mapas.enemyCount].spawn))
                            if (mapas.mons[mapas.enemyCount].canAtack){

                                bool res=false;

                                switch(mapas.mons[mapas.enemyCount].currentWeapon){
                                    case 1: res=mapas.mons[mapas.enemyCount].atack(true,false,&bulbox); break;
                                    case 2: res=mapas.mons[mapas.enemyCount].atack(true,true,&bulbox); break;
                                    case 0: BeatEnemy(mapas.enemyCount,20); break;
                                }


                                if ((mapas.mons[mapas.enemyCount].currentWeapon>0)&&(res)){
                                    if (mapas.mons[mapas.enemyCount].currentWeapon==1){

                                        AdaptSoundPos(0,mapas.mons[mapas.enemyCount].x,mapas.mons[mapas.enemyCount].y); 
                                        smotai[0].play(); 

                                    }
                                    if (mapas.mons[mapas.enemyCount].currentWeapon==2){
                                        AdaptSoundPos(12,mapas.mons[mapas.enemyCount].x,mapas.mons[mapas.enemyCount].y);
                                        smotai[12].play();          
                                    }

                                    if (isClient){
                                        char buf[10];
                                        buf[0]='s'; buf[1]='h'; buf[2]='t';
                                        memcpy(&buf[3],&mapas.mons[mapas.enemyCount].ammo,sizeof(int));
                                        unsigned char isMine=0;
                                        if (mapas.mons[mapas.enemyCount].currentWeapon==2)
                                            isMine=1;
                                        memcpy(&buf[7],&isMine,sizeof(unsigned char));
                                        clientas.sendData(buf,8);
                                    }
                                    bool isMine=false;
                                    if (mapas.mons[mapas.enemyCount].currentWeapon==2)
                                        isMine=true;
                                    if (isServer){
                                        for (int i=0;i<(int)serveris.clientCount();i++)
                                            SendBulletImpulse(255,mapas.mons[mapas.enemyCount].ammo,i,isMine);
                                    }

                                }
                                else
                                    if (mapas.mons[mapas.enemyCount].currentWeapon>0){
                                        if (noAmmo){
                                            mapas.mons[mapas.enemyCount].chageNextWeapon();
                                            noAmmo=false;
                                        }
                                        else{
                                            AdaptSoundPos(4,mapas.mons[mapas.enemyCount].x,mapas.mons[mapas.enemyCount].y);
                                            smotai[4].play();
                                            if (!noAmmo)
                                                noAmmo=true;
                                        }
                                    }


                            }


                        if (!mapas.mons[mapas.enemyCount].canAtack)
                            mapas.mons[mapas.enemyCount].reload(25);


                        for (int i=mapas.enemyCount;i<mapas.enemyCount+PlayerCount();i++){
                            if (mapas.mons[i].hit){
                                mapas.mons[i].damageAnim();
                                if (!mapas.mons[i].hit){
                                    AdaptSoundPos(10,mapas.mons[i].x,mapas.mons[i].y);
                                    smotai[10].play();
                                }
                            }
                        }


                        SlimeReaction(mapas.enemyCount); // herojaus reakcija i slime

                        DoorsInteraction();//open doors

                        HandleBullets();//kulku ai-------------

                        //kaip ir monstru ai
                        if (mapas.enemyCount){      
                            for (int i=0; i<mapas.enemyCount; i++){
                                if (!isClient)
                                    MonsterAI(i);
                            }       
                        }
                        //zudom playerius jei pas juos nebeliko gifkiu

                        for (int i=0;i<mapas.mons.count();i++){
                            if ((mapas.mons[i].hp<=0)&&(!mapas.mons[i].shot))
                                KillEnemy(mapas.mons[i].id);
                        }


                    }


}
//------------------------------------
void DrawHelp(){
    pics.draw(13, 0,0,0,1.0f,1.25f,1.9f);
    WriteText(30,30,render.spraitas,&pics.images[10],"Colect these and...");
    pics.images[11].Blt(render.spraitas,50,50,itmframe);
    pics.images[11].Blt(render.spraitas,100,50,itmframe+4);
    WriteText(200,80,render.spraitas,&pics.images[10],"Save those guys...");
    pics.images[11].Blt(render.spraitas,200,100,itmframe+8);
    WriteText(200,150,render.spraitas,&pics.images[10],"to unlock exit ");
    pics.images[11].Blt(render.spraitas,420,150,itmframe+12);

    pics.images[7].Blt(render.spraitas,100,210,0);
    WriteText(140,220,render.spraitas,&pics.images[10],"Ammo");
    pics.images[7].Blt(render.spraitas,100,230,1);
    WriteText(140,240,render.spraitas,&pics.images[10],"Health Up");

    WriteText(140,320,render.spraitas,&pics.images[10],"Controls:");
    WriteText(140,340,render.spraitas,&pics.images[10],"Arrow keys: controls character");
    WriteText(140,355,render.spraitas,&pics.images[10],"Tab: minimap");
    WriteText(140,370,render.spraitas,&pics.images[10],"CTRL: fire");
    WriteText(140,385,render.spraitas,&pics.images[10],"SPACE: opens door");
    int monframe=itmframe;
    if (monframe==3)
        monframe=0;
    pics.images[3].Blt(render.spraitas,100,270,monframe);
    WriteText(140,270,render.spraitas,&pics.images[10],"These monsters can eat items");
    WriteText(140,290,render.spraitas,&pics.images[10],"kill them to retrieve items back.");

    WriteText(300,460,render.spraitas,&pics.images[10],"press CTRL to play...");


}
//-------------------------------------
void DrawIntro(){
    pics.images[13].Blt(render.spraitas,0,0,0,1.0f,1.25f,1.9f);

    for (int i=0;i<intro_cline;i++)
        WriteText(30,25*i+20,render.spraitas,&pics.images[10],IntroText[i+18*(intro_gline/18)],1.0f,1.0f, 1.0f,1.0f,1.0f,1.0f);

    for (int a=0;a<intro_cchar;a++){
        pics.images[10].Blt(render.spraitas,30+a*11,25*intro_cline+20,(IntroText[intro_gline][a])-32,1.0,1.0f,1.0f,0.0f,1.0f,1.0f,1.0f);
    }

    WriteText(30,450,render.spraitas,&pics.images[10],"Press CTRL to skip this boring stuff...");
}



//---------------------------
void Game::DrawMissionObjectives()
{
    char buf[50];
    if (mapas.misionItems){
        sprintf(buf, "Collect %d Items",mapas.misionItems);
        WriteText(sys.width/2-100,sys.height/2+10,render.spraitas,&pics.images[10],buf,1.0f,1.0f,1.0f,0.0,0.0);
    }
    WriteText(sys.width/2-100,sys.height/2+30,render.spraitas,&pics.images[10],"Locate exit!",1.0f,1.0f,1.0f,0.0,0.0);
    sprintf(buf,"Time remaining:%d:%d",mapas.timeToComplete/60,mapas.timeToComplete-60*(mapas.timeToComplete/60));
    WriteText(sys.width/2-100,sys.height/2+50,render.spraitas,&pics.images[10],buf,1.0f,1.0,1.0,0.0,0.0);


}

//---------------------------

void Game::render(){

    if (render.lpD3DDevice){

        if (SUCCEEDED(render.lpD3DDevice->BeginScene())){  

            render.lpD3DDevice->Clear( 0, 0, D3DCLEAR_TARGET,D3DXCOLOR(0,0,0,0), 0, 0 );

            render.spraitas->Begin(D3DXSPRITE_ALPHABLEND);


            if (TitleScreen){
                DrawTitleScreen();  
                if (mainmenu.active())
                    mainmenu.draw(*pics.findpic("pointer.tga"),pics.findpic("charai.tga"),render.spraitas,render.lpD3DDevice);

                if (netmenu.active())
                    netmenu.draw(*pics.findpic("pointer.tga"),pics.findpic("charai.tga"),render.spraitas,render.lpD3DDevice);
                if (netgame.active())
                    netgame.draw(*pics.findpic("pointer.tga"),pics.findpic("charai.tga"),render.spraitas,render.lpD3DDevice);
                if (options.active())
                    options.draw(*pics.findpic("pointer.tga"),pics.findpic("charai.tga"),render.spraitas,render.lpD3DDevice);
                if (ipedit.active())
                    ipedit.draw(render.lpD3DDevice,render.spraitas,pics.findpic("charai.tga"));

                if (SfxVolumeC.active())
                    SfxVolumeC.draw(*pics.findpic("pointer.tga"),*pics.findpic("pointer.tga"),pics.findpic("charai.tga"),render.spraitas,render.lpD3DDevice);
                if (MusicVolumeC.active())
                    MusicVolumeC.draw(*pics.findpic("pointer.tga"),*pics.findpic("pointer.tga"),pics.findpic("charai.tga"),render.spraitas,render.lpD3DDevice);
            }
            else
                if (IntroScreen){
                    DrawIntro();
                }
                else
                    if (HelpScreen){
                        DrawHelp();
                    }
                    else 
                        if (EndScreen){
                            pics.images[15].Blt(render.spraitas,0,0,0,1.0f,1.25f,1.9f);
                            WriteText(260,430,render.spraitas,&pics.images[10],"The End...to be continued ?");
                        }


                        else{


                            
                            if ((mapas.width>0)&&(mapas.height>0)){

                                if (fadein)
                                    DrawMap(fadetim/160.0f,fadetim/160.0f,fadetim/160.0f);
                                else
                                    DrawMap();

                                
                            }

                            if (fadein)
                                WriteText(sys.width/2-150,sys.height/2-64,render.spraitas,&pics.images[10],"Get Ready!",1.0f,2,2);

                            if (objectivetim)
                                DrawMissionObjectives();

                            if (mapas.mons.count())
                                DrawStats();

                            if ((mapas.width>scrx)&&(mapas.height>scry)&&(ShowMiniMap))
                                DrawMiniMap(sys.width-mapas.width*4,sys.height-mapas.height*4);




                            if (showdebugtext)
                                DrawSomeText();  


                        }

                        render.spraitas->End();


                        render.lpD3DDevice->EndScene();

                        render.lpD3DDevice->Present( NULL, NULL, NULL, NULL );  

        }
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

//------------------------------------

void QuitApp(){
    if (isServer){
        StopServer();
    }
    if (isClient){
        QuitServer();
    }
    DeleteAudio();
    DeleteInput();
    pics.deleteContainer();
    render.destroy();
    PostQuitMessage(0);
}

//-----------------------------------

/*LRESULT CALLBACK MainWinProc( HWND hwnd,
                             UINT msg,
                             WPARAM wparam,
                             LPARAM lparam)
{
    switch(msg)
    {
    case WM_DESTROY:{
        QuitApp(); 
        break;
    }


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
    f=fopen("intro.itf","rt");
    if (f){


        int pos=0;
        int c=';';
        while (c!=EOF){
            c=fgetc(f);
            if ((c!=EOF)||(lin<100)){
                if ((c!='\n')&&(pos<100)){
                    IntroText[lin][pos]=c;
                    pos++;
                }
                else{
                    IntroText[lin][pos]='\0';
                    lin++;
                    pos=0;
                }


            }
        }
        fclose(f);
    }

}

//-------------------------

void LoadMap(const char* mapname, int otherplayers){
    bulbox.destroy();
    mapas.Destroy();
    mapas.Load(mapname,false, otherplayers);
    goods=mapas.misionItems;
    timeleft=mapas.timeToComplete;
    if ((!isClient)&&(!isServer)){
        mapas.mons[mapas.enemyCount].x=(float)mapas.start.x;
        mapas.mons[mapas.enemyCount].y=(float)mapas.start.y;
    }
    else{
        mapas.mons[mapas.enemyCount].appearInRandomPlace(mapas.colide,mapas.width,mapas.height);
    }

    AddaptMapView();
    findpskxy();
}
//----------------------------------------
//siuncia kliento info i serva
void Game::SendClientCoords()
{
    char coords[1024];
    int cnt=0;
    coords[0]='c'; coords[1]='h'; coords[2]='r';
    cnt+=3;
    memcpy(&coords[cnt],&mapas.mons[mapas.enemyCount].x,sizeof(float));
    cnt+=sizeof(float);
    memcpy(&coords[cnt],&mapas.mons[mapas.enemyCount].y,sizeof(float));
    cnt+=sizeof(float);
    memcpy(&coords[cnt],&mapas.mons[mapas.enemyCount].angle,sizeof(float));
    cnt+=sizeof(float);
    memcpy(&coords[cnt],&mapas.mons[mapas.enemyCount].frame,sizeof(unsigned char));
    cnt+=sizeof(unsigned char);
    unsigned char stats=0x0;
    if (mapas.mons[mapas.enemyCount].shot)
        stats|=0x80;
    if (mapas.mons[mapas.enemyCount].spawn)
        stats|=0x40;
    coords[cnt]=stats;
    cnt++;


    clientas.sendData(coords,cnt);
}
//-----------------------------------------
void SendPlayerInfoToClient(int clientindex){
    unsigned char z=0;
    for (int i=0;i<mapas.enemyCount+(int)serveris.clientCount()+1;i++){


        if (i-mapas.enemyCount-1!=clientindex){

            if ((i>=mapas.enemyCount)&&((i-mapas.enemyCount-1)<clientindex))
                z=(unsigned char)(i+1);
            else
                z=(unsigned char)i;



            char coords[1024];
            int cnt=0;  
            coords[cnt]='c';coords[cnt+1]='h';coords[cnt+2]='r';
            cnt+=3;
            memcpy(&coords[cnt],&z,sizeof(unsigned char));
            cnt+=sizeof(unsigned char);
            memcpy(&coords[cnt],&mapas.mons[i].x, sizeof(float));
            cnt+=sizeof(float);
            memcpy(&coords[cnt],&mapas.mons[i].y, sizeof(float));
            cnt+=sizeof(float);
            memcpy(&coords[cnt],&mapas.mons[i].angle, sizeof(float));
            cnt+=sizeof(float);
            memcpy(&coords[cnt],&mapas.mons[i].frame, sizeof(unsigned char));
            cnt+=sizeof(unsigned char);
            unsigned char stats=0x0;
            if (mapas.mons[i].shot)
                stats|=0x80;
            if (mapas.mons[i].spawn)
                stats|=0x40;
            coords[cnt]=stats;
            cnt++;
            if (mapas.mons[i].spawn){
                memcpy(&coords[cnt],&mapas.mons[i].hp, sizeof(int));
                cnt+=sizeof(int);
            }
            serveris.sendData(clientindex,coords,cnt);  
        }
    }
}
//--------------------------------------
void SendData(){

    /*
    float floatas=1023.4563; 
    short toShort = (short)floorf( 12.0f * floatas + 0.5f );
    float toFloat = toShort / 12.0f;
    */
    if (isClient){
        if (Client_GotMapData){
            SendClientCoords();
        }
    }

    else 
        if (isServer){
            
            for (int a=0;a<(int)serveris.clientCount();a++){
                char buf[4];
                buf[0]='p';buf[1]='i';buf[2]='n';
                serveris.sendData(a,buf,3);
                SendPlayerInfoToClient(a);
    
            }
        }

}
//-----------------------------------
void GetCharData(const char* bufer, int bufersize, int* index ){
    if ((bufersize-(*index))>=14){

        unsigned char monum=0;
        memcpy(&monum,&bufer[*index],sizeof(unsigned char));
        if ((monum>=0)){//aga toks yra
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
                mapas.mons[monum].spawn=true;
            else
                mapas.mons[monum].spawn=false;
            (*index)++;
            if (mapas.mons[monum].spawn){
                memcpy(&mapas.mons[monum].hp,&bufer[*index],sizeof(int));
                *index+=sizeof(int);
            }

        }

    }
}
//---------------------------------------------
void GetMapInfo(const char* bufer, int bufersize, int* index){

    int mapnamelen=0;
    if (bufersize-(*index)>=sizeof(int)){
        int totallen=0;
        memcpy(&totallen,&bufer[*index],sizeof(int));
        *index+=sizeof(int);
        if ((bufersize-(*index)>=totallen)&&(totallen>0)){
            memcpy(&mapnamelen,&bufer[*index],sizeof(int));
            if (mapnamelen){
                *index+=sizeof(int);
                char mapname[255];
                if (bufersize-(*index)>=mapnamelen)
                    memcpy(mapname,&bufer[*index],mapnamelen);
                *index+=mapnamelen;
                mapname[mapnamelen]=0;


                int klientaiold=klientai;
                //gaunam klientu skaiciu
                if (bufersize-(*index)>=sizeof(int))
                    memcpy(&klientai,&bufer[*index],sizeof(int));
                *index+=sizeof(unsigned int);


                if (strcmp(mapname,mapas.name)!=0){//jei mapas ne tas pats tai uzloadinam
                    LoadMap(mapname,klientai);
                    Client_GotMapData=true;
                    TitleScreen=false;
                }
                else{

                    if (klientai-klientaiold){
                        Dude naujas;
                            for (int i=0;i<(klientai-klientaiold);i++){
                                mapas.mons.add(naujas);
                                mapas.mons[mapas.mons.count()-1].id=mapas.mons[mapas.mons.count()-2].id+1;
                            }
                        
                    }

                }


            }
        }
    }
}
//------------------------------------
void GetMapData(const char* bufer, int bufersize, int* index){


    int moncount=0;
    memcpy(&moncount,&bufer[*index],sizeof(int));
    *index+=sizeof(int);

    for (int i=0;i<moncount;i++){
        memcpy(&mapas.mons[i].race,&bufer[*index],sizeof(int));
        *index+=sizeof(int);
    }
    
    int itmcount=0;
    memcpy(&itmcount,&bufer[*index],sizeof(int));
    *index+=sizeof(int);
    for (int i=0;i<itmcount;i++){
        CItem itm;
        
        memcpy(&itm.x,&bufer[*index],sizeof(float));
        *index+=sizeof(float);
        memcpy(&itm.y,&bufer[*index],sizeof(float));
        *index+=sizeof(float);
        memcpy(&itm.value,&bufer[*index],sizeof(int));
        *index+=sizeof(int);
        mapas.addItem(itm.x,itm.y,itm.value);
        
    }

}
//----------------------------------
void GetClientCoords(const char* bufer, int * buferindex, unsigned int clientIndex){
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
        
        
    *buferindex++;
    
}

//--------------------------------------------------------------
void GetDoorInfo(const char* bufer,int * index, int* dx, int* dy, unsigned char* frame){

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
        mapas.colide[doory][doorx]=true;
    else
        mapas.colide[doory][doorx]=false;
    if (dx)
        *dx=doorx;
    if (dy)
        *dy=doory;
    if (frame)
        *frame=doorframe;
}
//---------------------------------------
void GetNewItemInfo(char* bufer, int* index){
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
void GetClientAtackImpulse(const char* buf, int * index, int ClientIndex){
 int victim;
 int hp;
 memcpy(&victim,&buf[*index],sizeof(int));
 *index+=sizeof(int);
 memcpy(&hp,&buf[*index],sizeof(int));
 *index+=sizeof(int);



  int playerID=0;

 if (victim>254){
     int tmpID=victim-254;
     if (ClientIndex>=tmpID)
         playerID=tmpID-1;
     playerID=mapas.enemyCount+playerID;
 }
 else
     playerID=victim;
        
     mapas.mons[playerID].hit=true;
     mapas.mons[playerID].hp=hp;
    
 

 for (int a=0;a<serveris.clientCount();a++){

     int z=playerID;
     if ((playerID>=mapas.enemyCount)&&((playerID-mapas.enemyCount-1)<a))
         z= playerID+1;

     if (ClientIndex!=a)
        SendAtackImpulse(a,z,hp);
 }

}
//----------------------------------
//klientas paima infa apie atka
void GetAtackImpulse(const char* buf,int* index){
 int victim;
 int hp;
 memcpy(&victim,&buf[*index],sizeof(int));
 *index+=sizeof(int);
 memcpy(&hp,&buf[*index],sizeof(int));
 *index+=sizeof(int);
 
  mapas.mons[victim].hit=true;
  mapas.mons[victim].hp=hp;
  
}
//---------------------------------
void GetData(){
    if (isServer){

        char bufer[1024];

        for (unsigned int i=0;i<serveris.clientCount();i++){
            int siz=serveris.getData(i,bufer);
            if (siz){
                int index=0;
                while (index<siz){
                    
                    char hdr[4];
                    strcpy(hdr,"nop"); //pradine reiksme
                    if (siz-index>3){
                        memcpy(&hdr,&bufer[index],3);
                        hdr[3]=0;
                    }

                    if(strcmp(hdr,"chr")==0){
                        index+=3;
                        GetClientCoords(bufer,&index,i);
                    }
                    else
                        if (strcmp(hdr,"sht")==0){
                            unsigned char cisMine=0;
                            
                            index+=3;
                            memcpy(&mapas.mons[mapas.enemyCount+1+i].ammo,&bufer[index],sizeof(int));
                            index+=sizeof(int);
                            memcpy(&cisMine,&bufer[index],sizeof(unsigned char));
                            index+=sizeof(unsigned char);
                            bool isMine=false;
                            if (cisMine)
                                isMine=true;
                            mapas.mons[mapas.enemyCount+1+i].atack(true,isMine,&bulbox);

                            

                            for (unsigned int a=0;a<serveris.clientCount();a++){//isiunciam isovimo impulsa kitiems
                                if (a!=i){
                                    if ((a<i)&&(i>0))
                                        SendBulletImpulse(255+i,mapas.mons[mapas.enemyCount+i].ammo,a,isMine);
                                    else
                                        SendBulletImpulse(256+i,mapas.mons[mapas.enemyCount+i].ammo,a,isMine);

                                }

                            }
                        }
                        else
                            if (strcmp(hdr,"itm")==0){
                                index+=3;
                                int itmindex=0;
                                memcpy(&itmindex,&bufer[index],sizeof(int));
                                index+=sizeof(int);
                                if ((mapas.items[itmindex].value<5)&&(mapas.items[itmindex].value>0))
                                    goods--;
                                mapas.removeItem(itmindex);
                                for (unsigned int a=0;a<serveris.clientCount();a++){
                                    if (a!=i)
                                        SendItemSRemove(itmindex,a,true);
                                }

                            }
                            else
                                if (strcmp(hdr,"dor")==0){
                                    index+=3;
                                    int dx,dy;
                                    unsigned char frame;
                                    GetDoorInfo(bufer,&index,&dx,&dy,&frame);
                                    for (unsigned int a=0;a<serveris.clientCount();a++){
                                    if (a!=i)
                                        SendServerDoorState(a,dx,dy,frame);
                                    }
                                }
                                else
                                    if (strcmp(hdr,"nxt")==0){
                                        index+=3;
                                        mapai.current++;
                                        int kiek=serveris.clientCount();

                                        GoToLevel(mapai.current,kiek);
                                        for (int a =0; a<(int)serveris.clientCount();a++){
                                            SendMapInfo(a);
                                            SendMapData(a);
                                        }
                                        

                                    }
                                else
                                    if (strcmp(hdr,"atk")==0){
                                        index+=3;
                                        GetClientAtackImpulse(bufer,&index,i);
                                    }

                                    else
                                        if (strcmp(hdr,"qut")==0){
                                            index+=3;
                                            serveris.removeClient(i);
                                            mapas.mons.remove(mapas.enemyCount+i);
                                        }

                                    else
                                    if (strcmp(hdr,"pon")==0){
                                        index+=3;

                                    }
                                    else

                            index++;
                }
            }
        }
    }

    else//----------------------------------------------------------------------------
        if (isClient){ //jei klientas
            char bufer[1024];
            int siz=clientas.getData(bufer);


            if (siz){

                int index=0;

                while (index<siz){
                    char hdr[4];
                    strcpy(hdr,"nop"); //pradine reiksme

                    if (siz-index>3){
                        memcpy(&hdr,&bufer[index],3);
                        hdr[3]=0;
                    }

                    if (strcmp(hdr,"chr")==0){ 
                        index+=3;
                        GetCharData(bufer,siz,&index);                      
                    }
                    else
                        if (strcmp(hdr,"srv")==0){
                            index+=3;   
                            GetMapInfo(bufer,siz,&index);
                        }
                        else
                            if (strcmp(hdr,"dat")==0){
                                index+=3;
                                GetMapData(bufer,siz,&index);
                            }
                            else 
                                if (strcmp(hdr,"sht")==0){
                                    index+=3;
                                    int ind=0;
                                    memcpy(&ind,&bufer[index],sizeof(int));
                                    index+=sizeof(int);
                                    
                                    if (ind<254){
                                        memcpy(&mapas.mons[ind].ammo,&bufer[index],sizeof(int));
                                        index+=sizeof(int);
                                        unsigned char cisMine=0;
                                        memcpy(&cisMine,&bufer[index],sizeof(unsigned char));
                                        index+=sizeof(unsigned char);
                                        bool isMine=false;
                                        if (cisMine)
                                            isMine=true;
                                        mapas.mons[ind].atack(true,isMine,&bulbox);
                                    }
                                    else{
                                        memcpy(&mapas.mons[mapas.enemyCount+(ind-254)].ammo,&bufer[index],sizeof(int));
                                        index+=sizeof(int);
                                        unsigned char cisMine=0;
                                        memcpy(&cisMine,&bufer[index],sizeof(unsigned char));
                                        index+=sizeof(unsigned char);
                                        bool isMine=false;
                                        if (cisMine)
                                            isMine=true;
                                        mapas.mons[mapas.enemyCount+(ind-254)].atack(true,isMine,&bulbox);
                                    }
                                    


                                }

                            else
                                if (strcmp(hdr,"itm")==0){
                                    index+=3;
                                    int itmindex=0;
                                    memcpy(&itmindex,&bufer[index],sizeof(int));
                                    index+=sizeof(int);
                                    unsigned char isPlayerTaked=0;
                                    memcpy(&isPlayerTaked,&bufer[index],sizeof(unsigned char));
                                    index++;
                                    if ((mapas.items[itmindex].value<5)&&
                                        (mapas.items[itmindex].value>0)&&
                                        (isPlayerTaked))
                                        goods--;
                                    mapas.removeItem(itmindex);
                                        
                                    

                                }
                                else

                                    if (strcmp(hdr,"dor")==0){
                                        index+=3;
                                        GetDoorInfo(bufer,&index,0,0,0);
                                    }

                                    else
                                        if (strcmp(hdr,"itc")==0){
                                            index+=3;
                                            GetNewItemInfo(bufer,&index);
                                        }

                                    else
                                        if(strcmp(hdr,"atk")==0){
                                            index+=3;
                                            GetAtackImpulse(bufer,&index);

                                        }
                                        else
                                        if(strcmp(hdr,"pin")==0){
                                            index+=3;
                                            clientas.sendData("pon",3);

                                        }
                                        else

                                index++;

             }

            }
        }

}



//-----------------------------------------
void Game::init()
{

    sys.load("config.cfg",false);
    LoadKeyData();


    Smenu menu;
    strcpy(menu.opt[0],"Single Player");
    strcpy(menu.opt[1],"Network Game");
    strcpy(menu.opt[2],"Options");
    strcpy(menu.opt[3],"Exit");
    menu.count=4;
    mainmenu.init(0,sys.height-150,"",menu,0);
    mainmenu.activate();
    strcpy(menu.opt[0],"Start server");
    strcpy(menu.opt[1],"Join server");
    menu.count=2;
    netmenu.init(0,sys.height-100,"Network Game:",menu,0);

    strcpy(menu.opt[0],"Coop");
    strcpy(menu.opt[1],"DeathMatch");
    menu.count=2;
    netgame.init(0,sys.height-100,"Game Type:",menu,0);

    strcpy(menu.opt[0],"Music Volume");
    strcpy(menu.opt[1],"Sound fx Volume");
    menu.count=2;
    options.init(0,sys.height-100,"Options:",menu,0);

    ipedit.init(0,sys.height-100,"Enter Server's IP",20);

    SfxVolumeC.init(20,sys.height-100,"Sfx Volume:",0,10000,100);
    MusicVolumeC.init(20,sys.height-100,"Music Volume:",0,10000,100);


    LoadIntro();

    InitAudio();


    PlayNewSong("Evil.ogg");

    Works = true;


}

void Game::destroy()
{
    mapas.Destroy();
    mapai.Destroy();

    bulbox.destroy();
}

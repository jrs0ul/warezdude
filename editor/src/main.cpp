#ifdef WIN32
#pragma comment(lib,"SDL.lib")
#pragma comment(lib,"SDLmain.lib")
#pragma comment(lib,"OpenGl32.lib")
#pragma comment(lib,"GLU32.lib")
#endif
//--------------------------------------





#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cassert>

#include "../../src/SDLVideo.h"
#include "../../src/Consts.h"
#include "../../src/Image.h"
#include "../../src/TextureLoader.h"
#include "../../src/map.h"
#include "../../src/ShaderProgram.h"
#include "../../src/Matrix.h"
#include "Utils.h"
#include "Button.h"
#include "CIniFile.h"


#ifdef WIN32
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#endif



int SCREENW = 640;
int SCREENH = 480;

const unsigned PANEL_HEIGHT = 100;
const unsigned MAX_BUTTON = 100;
const unsigned TILE_STEP = 32;

const unsigned BUTTON_TILES_DEC = 25;
const unsigned BUTTON_TILES_INC = 26;
const unsigned BUTTON_FIRST_TILE = 27;
const unsigned BUTTON_SAVE = 18;
const unsigned BUTTON_SHOW_COLLISION = 11;

const unsigned BUTTON_PALETTE_0 = 8;
const unsigned BUTTON_PALETTE_1 = 15;
const unsigned BUTTON_PALETTE_2 = 16;
const unsigned BUTTON_PALETTE_3 = 17;

const unsigned BUTTON_SHOW_GRID = 2;

const unsigned TILE_BUTTONS_ORIGIN_X = 300;


const unsigned BUTTON_IMG = 1;

const char CHR_BASEPATH[] = "../src/";


Uint32 tick = 0;


PicsContainer pics;


bool _QuitApp = false;
float   OrthoMatrix[16];
float   ViewMatrix[16];

ShaderProgram        defaultShader;
ShaderProgram        colorShader;


CMap map;


SDLVideo video;

COLOR tempAttributeColors[4] = {COLOR(1.f, 1.f, 0, 0.2f),
                                COLOR(0, 0, 1.f, 0.2f),
                                COLOR(0, 1.f, 0, 0.2f),
                                COLOR(0, 1.f, 1.f, 0.2f)};


Mygtas mygtai[MAX_BUTTON];

Vector3D Cross;

int tilex,tiley;
bool SHOW_LEV1= true;
bool SELECT_LEV1= true;
bool SHOW_COLISSION= true;
bool SELECT_COLISSION= false;
bool SHOW_ATTRIBUTES = true;
bool SELECT_ATTRIBUTES = false;
bool SHOW_GRID = true;


unsigned char GlobalKey;
int oldmousekey,bm;
int mx,my;

unsigned char currenttile=0;
int currentchar = 0;
unsigned char firsttile = 0;

unsigned char currentPalette = 0;

int charsIDs[]={1,4,7,8};

CIniFile INI;

char MapTiles[256];
char Tileset[256];

const unsigned char NESColors[] = {
                            124,124,124,
                            0,0,252,
                            0,0,188,
                            68,40,188,
                            148,0,132,
                            168,0,32,
                            168,16,0,
                            136,20,0,
                            80,48,0,
                            0,120,0,
                            0,104,0,
                            0,88,0,
                            0,64,88,
                            0,0,0,

                            188,188,188,
                            0,120,248,
                            0,88,248,
                            104,68,252,
                            216,0,204,
                            228,0,88,
                            248,56,0,
                            228,92,16,
                            172,124,0,
                            0,184,0,
                            0,168,0,
                            0,168,68,
                            0,136,136,

                            248,248,248,
                            60,188,252,
                            104,136,252,
                            152,120,248,
                            248,120,248,
                            248,88,152,
                            248,120,88,
                            252,160,68,
                            248,184,0,
                            184,248,24,
                            88,216,84,
                            88,248,152,
                            0,232,216,
                            120,120,120,

                            252,252,252,
                            164,228,252,
                            184,184,248,
                            216,184,248,
                            248,184,248,
                            248,164,192,
                            240,208,176,
                            252,224,168,
                            248,216,120,
                            216,248,120,
                            184,248,184,
                            184,248,216,
                            0,252,252,
                            248,216,248,
                        };
const unsigned char ColorIndexes[55] = {
                                     0,    3,    6,    9,   12,   15,   18,   21,   24,   27,   30,   33,   36,   39,
                                    42,   45,   48,   51,   54,   57,   60,   63,   66,   69,   72,   75,   78,
                                    81,   84,   87,   90,   93,   96,   99,  102,  105,  108,  111,  114,  117,  120,
                                   123,  126,  129,  132,  135,  138,  141,  144,  147,  150,  153,  156,  159,  162
                                 };
const char* ColorNames[] =       {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0F",
                                  "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C",
                                  "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D",
                                  "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D"};

unsigned char Palettes[48] = {0};


//=======================================================================

static void QuitApp()
{

    printf("Quiting\n");
    _QuitApp = true;

}

//----------------------------------------------------------

static void process_events ( void ){

    SDL_Event event;

    while ( SDL_PollEvent ( &event ) )
    {

        switch ( event.type )
        {
            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym ){
                    case SDLK_ESCAPE:{
                        QuitApp();
                    }
                    break;
                    default:{}
                }
                break;
            case SDL_QUIT:

                QuitApp();
                break;
            default:{}
        }

    }

}

//------------------------------------------------------
int FPS ( void )
{
    static int time = 0, FPS = 0, frames = 0, frames0 = 0;
    if ( ( int ) SDL_GetTicks() >= time )
    {
        FPS = frames-frames0;
        time = SDL_GetTicks() +1000;
        frames0 = frames;
    }
    frames = frames+1;
    return FPS;
}

//-------------------------------------------
void DrawPanel(){

    DrawBlock (pics, 0, 0, SCREENW, 100,
                COLOR(0, 0, 1, 0.5f),
                COLOR(0, 0, 1, 0.5f)
                );
    char buf[100];
    sprintf ( buf,"FPS: %d ",FPS() );
    DrawText ( SCREENW-128,10,buf,pics,0,0.7f );

    sprintf ( buf,"tilex:%d tiley:%d ",tilex, tiley );
    DrawText ( 10,10,buf,pics,0, 0.7f );
    DrawText (130, 92, "Grid", pics, 0, 0.7f);

    if ( SHOW_LEV1 )
    {
        mygtai[0].draw ( pics, BUTTON_IMG, 5 );
    }
    else
    {
        mygtai[0].draw ( pics, BUTTON_IMG, 4 );
    }

    if ( SHOW_ATTRIBUTES )
    {
        mygtai[1].draw ( pics, BUTTON_IMG, 5 );
    }
    else
    {
        mygtai[1].draw ( pics, BUTTON_IMG, 4 );
    }

    
    if ( SHOW_GRID )
        mygtai[BUTTON_SHOW_GRID].draw ( pics, BUTTON_IMG, 5 );
    else
        mygtai[BUTTON_SHOW_GRID].draw ( pics, BUTTON_IMG, 4 );

    if ( SHOW_COLISSION )
    {
        mygtai[BUTTON_SHOW_COLLISION].draw ( pics, BUTTON_IMG, 5 );
    }
    else
    {
        mygtai[BUTTON_SHOW_COLLISION].draw ( pics, BUTTON_IMG, 4 );
    }

   /* if ( SHOW_DUDE )
        mygtai[19].draw ( pics,5,5 );
    else
        mygtai[19].draw ( pics,5,4 );

    if ( SHOW_PD )
        mygtai[20].draw ( pics,5,5 );
    else
        mygtai[20].draw ( pics,5,4 );

    if ( SHOW_SLAUGHTER )
        mygtai[21].draw ( pics,5,5 );
    else
        mygtai[21].draw ( pics,5,4 );

    if ( SELECT_DUDE )
        mygtai[22].draw ( pics,3,0,1.0f,0,0 );
    else
        mygtai[22].draw ( pics,3,0 );

    if ( SELECT_PD )
        mygtai[23].draw ( pics,5,10,1.0f,0,0 );
    else
        mygtai[23].draw ( pics,5, 10 );

    if ( SELECT_SLAUGHTER )
        mygtai[24].draw ( pics,5,11,1.0f,0,0 );
    else
        mygtai[24].draw ( pics,5,11 );

    if ( SHOW_ENTITIES )
        mygtai[13].draw ( pics,5,5 );
    else
        mygtai[13].draw ( pics,5,4 );*/


    mygtai[BUTTON_PALETTE_0].draw(pics, -1, 0, 
                                  tempAttributeColors[0].c[0],
                                  tempAttributeColors[0].c[1],
                                  tempAttributeColors[0].c[2]);
    mygtai[BUTTON_PALETTE_1].draw(pics, -1, 0, 
                                  tempAttributeColors[1].c[0],
                                  tempAttributeColors[1].c[1],
                                  tempAttributeColors[1].c[2]);
    mygtai[BUTTON_PALETTE_2].draw(pics, -1, 0, 
                                  tempAttributeColors[2].c[0],
                                  tempAttributeColors[2].c[1],
                                  tempAttributeColors[2].c[2]);
    mygtai[BUTTON_PALETTE_3].draw(pics, -1, 0, 
                                  tempAttributeColors[3].c[0],
                                  tempAttributeColors[3].c[1],
                                  tempAttributeColors[3].c[2]);




    if ( SELECT_LEV1 )
        mygtai[6].draw ( pics, BUTTON_IMG, 0, 1.0f, 0,0 );
    else
        mygtai[6].draw ( pics, BUTTON_IMG, 0 );

    if ( SELECT_ATTRIBUTES )
    {
        mygtai[7].draw ( pics, BUTTON_IMG,1,1.0f,0,0 );
    }
    else
    {
        mygtai[7].draw ( pics, BUTTON_IMG,1 );
    }

    /*if ( SELECT_LEV3 )
        mygtai[8].draw ( pics,5,2,1.0f,0,0 );
    else
        mygtai[8].draw ( pics,5,2 );*/

    if ( SELECT_COLISSION )
        mygtai[12].draw ( pics, BUTTON_IMG, 3, 1.0f, 0, 0 );
    else
        mygtai[12].draw ( pics, BUTTON_IMG, 3 );

   /* if ( SELECT_ENT )
        mygtai[14].draw ( pics,5,8,1.0f,0,0 );
    else
        mygtai[14].draw ( pics,5,8 );*/

    for (unsigned i = BUTTON_FIRST_TILE; i < BUTTON_FIRST_TILE + TILE_STEP; ++i)
    {
        mygtai[i].draw ( pics, 3 + currentPalette, firsttile + (i - BUTTON_FIRST_TILE));
    }

    mygtai[BUTTON_TILES_DEC].draw ( pics, BUTTON_IMG, 6 );
    mygtai[BUTTON_TILES_INC].draw ( pics, BUTTON_IMG, 7 );


    /*mygtai[15].draw ( pics,5,6 );
    mygtai[16].draw ( pics,charsIDs[currentchar],0 );
    mygtai[17].draw ( pics,5,7 );*/


    mygtai[BUTTON_SAVE].draw ( pics, BUTTON_IMG, 9 );

}

//---------------------------------------------

void DrawGrid2D(CMap& GMap, int shiftX, int shiftY)
{
         glDisable(GL_TEXTURE_2D);
         glLineWidth(1.f);

         int count = 0;

         float * vertices = 0;

         float * colors = 0;


         vertices = new float[(GMap.width() + 1) * 4 + (GMap.height() + 1) * 4];
         int colorCount = (GMap.width() + 1) * 8 + (GMap.height() + 1) * 8;
         colors = new float[colorCount];
         for (int i = 0; i < colorCount; ++i)
         {
             colors[i] = 1.f;
         }


         for (unsigned i = 0; i < GMap.width() + 1; i++){

            vertices[i * 4    ] = i * TILE_WIDTH - shiftX;
            vertices[i * 4 + 1] = -shiftY;

            vertices[i * 4 + 2] = i * TILE_WIDTH - shiftX;
            vertices[i * 4 + 3] = GMap.height() * TILE_WIDTH - shiftY;

            count ++;
         }

         for (unsigned i = 0; i < GMap.height() + 1; i++){

            int index = (GMap.width() + 1) * 4 + i * 4;

            vertices[index] = -shiftX;
            vertices[index + 1] = i * TILE_WIDTH - shiftY;

            vertices[index + 2] = GMap.width() * TILE_WIDTH - shiftX;
            vertices[index + 3] = i * TILE_WIDTH - shiftY;

            count ++;


         }

        int attribID = colorShader.getAttributeID("aPosition"); 
        int ColorAttribID = colorShader.getAttributeID("aColor");
        glVertexAttribPointer(attribID, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glEnableVertexAttribArray(attribID);
        glVertexAttribPointer(ColorAttribID, 4, GL_FLOAT, GL_FALSE, 0, colors);
        glEnableVertexAttribArray(ColorAttribID);


        glDrawArrays(GL_LINES, 0, count*2 );

        glDisableVertexAttribArray(ColorAttribID);
        glDisableVertexAttribArray(attribID);

        delete []colors;
        delete []vertices;


        glEnable(GL_TEXTURE_2D);

    }



//-----------------------------------------------------------

static void RenderScreen ( void ){


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    FlatMatrix identity;
    MatrixIdentity(identity.m);

    FlatMatrix finalM = identity * OrthoMatrix;
    defaultShader.use();
    int MatrixID = defaultShader.getUniformID("ModelViewProjection");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, finalM.m);
    colorShader.use();
    int MatrixID2 = colorShader.getUniformID("ModelViewProjection");
    glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, finalM.m);



    if ( SHOW_LEV1 )
    {
        map.draw(pics, 1,1,1, map.width(), map.height(), map.width(), map.height(), 0, 0, map.getPos().x, map.getPos().y);
    }

    if ( SHOW_COLISSION )
    {
        for (unsigned a = 0; a < map.height(); a++ )
        {
            for (unsigned i = 0; i < map.width(); i++ )
            {
                if ( map.colide ( i,a ) )
                {
                    DrawBlock (pics, (int)(map.getPos().x)-16 + i * 32,
                                (int)(map.getPos().y)-16 + a * 32, 
                                32, 32,
                                COLOR(1.0f, 0, 0, 0.5f),
                                COLOR(1.0f, 0, 0, 0.5f)
                                );
                }
            }
        }
    }

    if (SHOW_GRID)
    {
        pics.drawBatch(&colorShader, &defaultShader, 666);

        colorShader.use();
        DrawGrid2D(map, -1 * (int)map.getPos().x + 16, -1 * (int)map.getPos().y + 16);
    }

    DrawPanel();

    pics.draw ( 2, Cross.x, Cross.z );


    pics.drawBatch(&colorShader, &defaultShader, 666);


    //glDisable ( GL_BLEND );

    glFlush();

    //----------------------

    video.swap();
}



//---------------------------------------
void LoadShader(ShaderProgram* shader, const char* name)
{
    printf("shader load\n");
    shader->create();

    char error[1024];
    char buf[512];

    Shader vert;
    Shader frag;

    printf("Loading vertex shader...\n");
    sprintf(buf, "../shaders/%s.vert", name);
#ifdef __ANDROID__
    vert.load(GL_VERTEX_SHADER, buf, AssetManager);
#else
    vert.load(GL_VERTEX_SHADER, buf);
#endif

    printf("Loading fragment shader...\n");
    sprintf(buf, "../shaders/%s.frag", name);
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



//--------------------------------------------------------
static void SetupOpengl ( int width, int height )
{

    LoadExtensions();
    LoadShader(&defaultShader, "default");
    LoadShader(&colorShader, "justcolor");
    
    glEnable ( GL_TEXTURE_2D );

    MatrixOrtho(0.0, width, height, 0.0, -400, 400, OrthoMatrix);


    glClearColor ( 0, 0, 0, 1.0 );
    glEnable ( GL_BLEND );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );


    if (!pics.load ( "pics/clist.txt" )){
        puts("Cannot find picture list!");
        _QuitApp = true;
    }



}


//----------------------------------------------------------------

void CheckKeys()
{
    const Uint8 * keys = SDL_GetKeyboardState(NULL);

    oldmousekey = bm;
    bm = SDL_GetMouseState ( &mx,&my );

    //------------------------------
    GlobalKey = 0;
    if ( keys[SDL_SCANCODE_UP])
        GlobalKey = 38;
    if ( keys[SDL_SCANCODE_DOWN])
        GlobalKey = 40;
    if ( keys[SDL_SCANCODE_RETURN])
        GlobalKey = 13;


    //------------------------------

    if ( keys[SDL_SCANCODE_UP] )
    {
        Vector3D v ( 0, 1.0f, 0 );
        map.move ( v, 2.0f );
    }
    if ( keys[SDL_SCANCODE_LEFT] )
    {
        Vector3D v ( 1.0f,0,0 );
        map.move ( v, 2.0f );
    }
    if ( keys[SDL_SCANCODE_DOWN] )
    {
        Vector3D v ( 0,-1.0f, 0 );
        map.move ( v, 2.0f );
    }
    if ( keys[SDL_SCANCODE_RIGHT] )
    {
        Vector3D v ( -1.0f,0,0 );
        map.move ( v, 2.0f );
    }


    if ( ( mx<SCREENW ) && ( mx>0 )
            && ( my<SCREENH ) && ( my>0 ) )
    {
        Cross.set ( mx,0,my );
        if ( Cross.z > PANEL_HEIGHT )
        {
            tilex= ( (int)(Cross.x - map.getPos().x) +16 ) /32;
            tiley= ( (int)(Cross.z - map.getPos().y) +16 ) /32;
        }
    }

    if ( Cross.z > PANEL_HEIGHT )
    {
        if ( ( bm & SDL_BUTTON ( 1 ) ) )
        {
            if ( SELECT_LEV1 )
            {
                map.tiles[tiley][tilex] = currenttile + 1;
            }


            if ( SELECT_COLISSION )
            {
                map._colide[tiley][tilex] = true ;
            }

        }

        if ( ( bm & SDL_BUTTON ( 3 ) ) )
        {
            if ( SELECT_LEV1 )
            {
                map.tiles[tiley][tilex] = 0;
            }

        }

    }
    else
    {


        if ( ( !bm ) && ( oldmousekey & SDL_BUTTON ( 1 ) ) )
        {
            if ( ( mygtai[0].pointerOntop ( (int)Cross.x, (int)Cross.z ) ) )
            {
                SHOW_LEV1 = !SHOW_LEV1;
            }

            if ( ( mygtai[BUTTON_SHOW_GRID].pointerOntop ( (int)Cross.x, (int)Cross.z ) ) )
            {
                SHOW_GRID = !SHOW_GRID;
            }


            if ( ( mygtai[11].pointerOntop ( (int)Cross.x, (int)Cross.z ) ) )
            {
                SHOW_COLISSION = !SHOW_COLISSION;
            }

            for (unsigned i = BUTTON_FIRST_TILE; i < BUTTON_FIRST_TILE + TILE_STEP; ++i)
            {
                if ( ( mygtai[i].pointerOntop ( (int)Cross.x, (int)Cross.z ) ) )
                {
                    currenttile = firsttile + (i - BUTTON_FIRST_TILE);
                }
            }

            if ( ( mygtai[6].pointerOntop ( (int)Cross.x, (int)Cross.z ) ) )
            {
                SELECT_LEV1 = !SELECT_LEV1;
            }

            if ( ( mygtai[1].pointerOntop ( (int)Cross.x, (int)Cross.z ) ) )
            {
                SHOW_ATTRIBUTES = !SHOW_ATTRIBUTES;
            }
            

            if ( ( mygtai[7].pointerOntop ( (int)Cross.x, (int)Cross.z ) ) )
            {
                SELECT_ATTRIBUTES = !SELECT_ATTRIBUTES;
            }


            /*if (mygtai[BUTTON_PALETTE_0].pointerOntop((int)Cross.x, (int)Cross.z))
            {
                currentPalette = 0;
            }

            if (mygtai[BUTTON_PALETTE_1].pointerOntop((int)Cross.x, (int)Cross.z))
            {
                currentPalette = 1;
            }

            if (mygtai[BUTTON_PALETTE_2].pointerOntop((int)Cross.x, (int)Cross.z))
            {
                currentPalette = 2;
            }

            if (mygtai[BUTTON_PALETTE_3].pointerOntop((int)Cross.x, (int)Cross.z))
            {
                currentPalette = 3;
            }*/

            if ( ( mygtai[12].pointerOntop ( (int)Cross.x, (int)Cross.z ) ) )
            {
                SELECT_COLISSION = !SELECT_COLISSION;
            }

      
            if ( ( mygtai[BUTTON_TILES_DEC].pointerOntop ( (int)Cross.x, (int)Cross.z ) ) )
            {
                if (firsttile < TILE_STEP)
                {
                    firsttile = 0;
                }
                else
                {
                    //if ( firsttile - TILE_STEP >= 0 )
                        firsttile -= TILE_STEP;
                }
            }

            if ( ( mygtai[BUTTON_TILES_INC].pointerOntop ( (int)Cross.x, (int)Cross.z ) ) )
            {
                if ( firsttile + TILE_STEP <= 255 )
                {
                    firsttile += TILE_STEP;
                }
            }

            if ( ( mygtai[15].pointerOntop ( (int)Cross.x, (int)Cross.z ) ) )
            {
                if ( currentchar>0 )
                    currentchar--;
            }

            if ( ( mygtai[17].pointerOntop ( (int)Cross.x, (int)Cross.z ) ) )
            {
                if ( currentchar<3 )
                    currentchar++;
            }

            if ( ( mygtai[BUTTON_SAVE].pointerOntop ( (int)Cross.x,(int)Cross.z ) ) )
            {
                map.save(MapTiles);
            }

        }



    }

}


//-----------------------------------------------------------------
int main ( int argc, char* argv[] )
{

    srand ( time ( 0 ) );

    INI.read ( "duded.ini" );
    wchar_t buf[255];
    char abuf[255];
    INI.get ( L"width",buf );
    wcstombs(abuf,buf,255);
    SCREENW = atoi ( abuf );

    if ( !SCREENW )
    {
        SCREENW = 640;
    }

    INI.get ( L"height",buf );
    wcstombs(abuf,buf,255);
    SCREENH = atoi ( abuf );

    if ( !SCREENH )
    {
        SCREENH = 480;
    }


    INI.get ( L"mapTiles",buf );
    wcstombs ( MapTiles, buf, 255 );
  
    mygtai[0].init ( 10,20,32,32 );
    mygtai[1].init ( 46,20,32,32 );
    mygtai[BUTTON_SHOW_GRID].init ( 130,55,32,32 );
    mygtai[BUTTON_SHOW_COLLISION].init ( 82,20,32,32 );
    mygtai[13].init ( 154,20,32,32 );

    mygtai[6].init ( 10,55,32,32 );
    mygtai[7].init ( 46,55,32,32 );
    mygtai[14].init ( 154,55,32,32 );


    mygtai[BUTTON_PALETTE_0].init ( 130,20,32,32 );
    mygtai[BUTTON_PALETTE_1].init ( 166,20,32,32 );
    mygtai[BUTTON_PALETTE_2].init ( 202,20,32,32 );
    mygtai[BUTTON_PALETTE_3].init ( 238,20,32,32 );

    mygtai[BUTTON_SAVE].init ( SCREENW - 42, 20, 32, 32 );


    mygtai[19].init ( 460,20,32,32 );
    mygtai[20].init ( 496,20,32,32 );
    mygtai[21].init ( 532,20,32,32 );
    mygtai[22].init ( 460,55,32,32 );
    mygtai[23].init ( 496,55,32,32 );
    mygtai[24].init ( 532,55,32,32 );

    mygtai[BUTTON_TILES_DEC].init ( TILE_BUTTONS_ORIGIN_X, 55,32,32 );

    unsigned tileRow = 0;

    for (unsigned i = BUTTON_FIRST_TILE; i < BUTTON_FIRST_TILE + TILE_STEP; ++i)
    {
        mygtai[i].init(TILE_BUTTONS_ORIGIN_X + 40 + (i - BUTTON_FIRST_TILE - (16 * tileRow)) * 34, 10 + tileRow * 34, 32, 32);

        if (((i - BUTTON_FIRST_TILE) + 1) % 16 == 0 && (i - BUTTON_FIRST_TILE) > 0)
        {
            ++tileRow;
        }
    }

    mygtai[BUTTON_TILES_INC].init ( TILE_BUTTONS_ORIGIN_X + 48 + 16 * 34,55,32,32 );


    int width = 0;
    int height = 0;

    width = SCREENW;
    height = SCREENH;

    video.setMetrics(width, height);
    if (!video.InitWindow("DUDED", ""))
    {
        QuitApp();
    }

    SDL_ShowCursor ( 0 );

    SetupOpengl ( width, height );



    if ( !map.load(MapTiles, false) )
    {
        _QuitApp = true;
        printf("Error loading map %s !\n", MapTiles);
    }

    map.generate();

    printf("\nStarting loop %d\n", _QuitApp);
    while (!_QuitApp )
    {

        if ( SDL_GetTicks() > tick )
        {
            CheckKeys();
            RenderScreen();

            tick = SDL_GetTicks() +11;
        }


        else SDL_Delay ( 1 );

        process_events();
    }


    pics.destroy();
    map.destroy();

    video.Quit();

#ifdef WIN32
#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
#endif
#endif


    return 0;
}




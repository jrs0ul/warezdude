#ifdef _WIN32
    #ifdef  _MSC_VER
        #define _CRT_SECURE_NO_DEPRECATE 1
        #define CURL_STATICLIB
        #pragma comment(lib,"SDL2.lib")
        #pragma comment(lib,"SDL2main.lib")
        #pragma comment(lib,"OpenGl32.lib")
        //#pragma comment(lib,"GLU32.lib")
        #pragma comment(lib,"openal32.lib")
        #pragma comment(lib,"libcurl_a.lib")
        #pragma comment(lib,"Ws2_32.lib") 
        #pragma comment(lib,"Crypt32.lib")
        #pragma comment(lib,"Wldap32.lib")
        #pragma comment(lib,"Normaliz.lib")

        #ifdef _DEBUG
            #pragma comment(lib,"libogg_d.lib")
            #pragma comment(lib,"libvorbis_d.lib")
            #pragma comment(lib,"libvorbisfile_d.lib")
        #else
            #pragma comment(lib,"libogg.lib")
            #pragma comment(lib,"libvorbis.lib")
            #pragma comment(lib,"libvorbisfile.lib")
        #endif
    #endif
#endif



#include <ctime>
#include "Game.h"
#include "SDLVideo.h"
#include "OSTools.h"
#ifdef __APPLE__
#include <limits.h>
#include <unistd.h>
#include <CoreFoundation/CoreFoundation.h>
#endif
#include <curl/curl.h>
#include <string>
#include <thread>


SDLVideo SDL;
SDL_Joystick *Joy = 0;
int JoyX = 0;
int JoyY = 0;
int MouseX, MouseY; //relative mouse coords
int _MouseX, _MouseY;
unsigned long tick;

static std::string buffer;

Game Game;


void ConfGL()
{
    Game.init();
}
//-----------------
void RenderScreen()
{
    Game.render();
    glFlush();

    //SDL_GL_SwapBuffers( );
    SDL.swap();
}
//-----------------
void Logic(){
    Game.logic();
}
//-----------------
static void  process_events(){
    
    SDL_Event event;
    float scaleX = 1.f;
    float scaleY = 1.f;

    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {

        case SDL_KEYDOWN:{

            Game.globalKEY = (char)event.key.keysym.scancode;
            switch( event.key.keysym.sym ) {
                default:{} break;
                case SDLK_q: { Game.Works = false;} break;
                case SDLK_F1: {++Game.DebugMode; if (Game.DebugMode >= 2) Game.DebugMode = 0;} 
            }
        } break;
        case SDL_MOUSEBUTTONUP:{
            Vector3D pos(event.button.x * scaleX, event.button.y * scaleY, 0);
            //printf("up x:%f y:%f\n", pos.x() , pos.y());
            Game.touches.up.add(pos);
            Game.touches.allfingersup = true;
        } break;
        case SDL_MOUSEBUTTONDOWN:{
            Vector3D pos(event.button.x * scaleX, event.button.y * scaleY, 0);
            //printf("down x:%f y:%f\n", pos.x() , pos.y());
            Game.touches.down.add(pos);
            Game.touches.allfingersup = false;

        } break;
        case SDL_MOUSEMOTION:{
            if(SDL_GetMouseState(0, 0)&SDL_BUTTON_LMASK){
                Vector3D pos(event.button.x * scaleX, event.button.y * scaleY, 0);
                //printf("motion x:%f y:%f\n", pos.x() , pos.y());
                Game.touches.move.add(pos);
                Game.touches.allfingersup = false;
            }
        }break;


        case SDL_QUIT:{
            Game.Works = false;
        }break;
    
        }

    }
}
//--------------------
void CheckKeys()
{
    float scaleX = 1.f;
    float scaleY = 1.f;
    
    int JoyNum = 0;
    
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    JoyNum = SDL_NumJoysticks();

    if (JoyNum > 0) {

        SDL_JoystickOpen(0);

        SDL_JoystickUpdate ();
        JoyX = SDL_JoystickGetAxis(Joy, 0);
        JoyY = SDL_JoystickGetAxis(Joy, 1);
    }

    
    SDL_GetRelativeMouseState ( &MouseX,&MouseY );
    SDL_GetMouseState(&_MouseX, &_MouseY);

    Game.gamepad.v[0] = JoyX/ 1000.0f;
    Game.gamepad.v[1] = JoyY/ 1000.0f;
    
    Game.MouseX = _MouseX*scaleX;
    Game.MouseY = _MouseY*scaleY;

    //Game.gamepad.v[0] = MouseX * 10.0f;
    //Game.gamepad.v[1] = MouseY * 10.0f;


    memcpy(Game.OldKeys, Game.Keys, 20);
    memset(Game.Keys, 0, 20);

    if ( keys[SDL_SCANCODE_W] )    Game.Keys[0] = 1;
    if ( keys[SDL_SCANCODE_S] )    Game.Keys[1] = 1;
    if ( keys[SDL_SCANCODE_A] )    Game.Keys[2] = 1;
    if ( keys[SDL_SCANCODE_D] )    Game.Keys[3] = 1;
    if ( keys[SDL_SCANCODE_UP] )   Game.Keys[0] = 1;
    if ( keys[SDL_SCANCODE_DOWN])  Game.Keys[1] = 1;
    if ( keys[SDL_SCANCODE_LEFT])  Game.Keys[2] = 1;
    if ( keys[SDL_SCANCODE_RIGHT]) Game.Keys[3] = 1;
    if ( keys[SDL_SCANCODE_SPACE]) Game.Keys[4] = 1;
    if ( keys[SDL_SCANCODE_RETURN]) Game.Keys[5] = 1;
    if ( keys[SDL_SCANCODE_ESCAPE]) Game.Keys[6] = 1;
    if ( keys[SDL_SCANCODE_DELETE]) Game.Keys[7] = 1;

    if (JoyNum){
        if (SDL_JoystickGetButton (Joy, 0))
            Game.Keys[4] = 1;
        if (SDL_JoystickGetButton (Joy, 1))
            Game.Keys[5] = 1;

        SDL_JoystickClose(0);
    }
}

static int writer(char *data, size_t size, size_t nmemb,
                  std::string *buffer){

    int result = 0;
    if (buffer){
            buffer->append(data, size * nmemb);
            result = size * nmemb;
    }

        return result;
}

//--------------------
int main(){

    srand(time(0));

    char buf[255];
    GetHomePath(buf);
    sprintf(Game.DocumentPath, "%s.warezdude3", buf);
    MakeDir(Game.DocumentPath);
#ifdef __APPLE__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)){
        puts("Um...Error?");
    }
    CFRelease(resourcesURL);
    chdir(path);
#endif
    Game.loadConfig();

       

    printf("%d %d\n", Game.ScreenWidth, Game.ScreenHeight);
    SDL.setMetrics(Game.ScreenWidth, Game.ScreenHeight);


    const char* title = "Warez D00ds 3";

    if (!SDL.InitWindow(title, "icon1.ico", Game.windowed)){
        Game.Works = false;
    }

    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    if(SDL_NumJoysticks() > 0)
    {
        Joy = SDL_JoystickOpen(0);
    }

    ConfGL();

    Game.TimeTicks = SDL_GetTicks();
    
    SDL_ShowCursor(false);

   
    while (Game.Works){


        if ((SDL_GetTicks() > tick))
        {

            Game.DeltaTime = (SDL_GetTicks() - Game.TimeTicks) / 1000.0f;
            Game.TimeTicks = SDL_GetTicks();

            Game.Accumulator += Game.DeltaTime;

            while (Game.Accumulator >= Game.DT){
                Logic();
                Game.Accumulator -= Game.DT;
            }

            CheckKeys();
            RenderScreen();

            tick = SDL_GetTicks() + 1000/61;
        }

        SDL_Delay(0.6);

        process_events();




    }

    Game.destroy();



    SDL.Quit();

    return 0;
}


#ifdef _WIN32
    #ifdef  _MSC_VER
        #define _CRT_SECURE_NO_DEPRECATE 1
        #pragma comment(lib,"SDL2.lib")
        #pragma comment(lib,"vulkan-1.lib")
        #pragma comment(lib,"SDL2main.lib")
        #pragma comment(lib,"OpenGl32.lib")
        #pragma comment(lib,"openal32.lib")
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
#include <disarray/SDLVideo.h>
#include <disarray/OSTools.h>
#ifdef __APPLE__
#include <limits.h>
#include <unistd.h>
#include <CoreFoundation/CoreFoundation.h>
#endif
#include <string>
#include <thread>
#include <disarray/VulkanVideo.h>
#include <disarray/disarray.h>


SDLVideo SDL;
SDL_Joystick *Joy = 0;
SDL_GameControllerType controllerType = SDL_CONTROLLER_TYPE_UNKNOWN;
SDL_GameController* gamepad  = 0;

int JoyX = 0;
int JoyY = 0;
int MouseX, MouseY; //relative mouse coords
int _MouseX, _MouseY;
unsigned long tick;

static std::string buffer;

const char* GamePadTypes[] = {"Unknown", "XBOX 360", "XBOX One", "Playstation 3",
                              "Playstation 4", "Nintendo Switch PRO", "Virtual",
                              "Playstation 5", "Amazon Luna", "Google Stadia",
                              "NVIDIA Shield", "Nintendo Switch Joycon LEFT",
                              "Nintendo Switch Joycon RIGHT", "Nintendo Switch Joycon PAIR"};

Game game;

void ConfigureGraphicsLib(bool useVulkan)
{
    game.init(useVulkan);
}
//-----------------
static void  process_events()
{

    SDL_Event event;
    float scaleX = 1.f;
    float scaleY = 1.f;

    while( SDL_PollEvent( &event ) ) 
    {

        switch( event.type ) 
        {

        case SDL_TEXTINPUT:
        {
            strcpy(game.EditText, event.text.text);
        } break;

        case SDL_KEYUP:
        {
            game.globalKEY = 0;
            game.globalKeyUp = (char)event.key.keysym.scancode;
        } break;

        case SDL_KEYDOWN:{

            game.globalKEY = (char)event.key.keysym.scancode;
            switch( event.key.keysym.sym ) 
            {
                default:{} break;
                case SDLK_F1: {++game.DebugMode; if (game.DebugMode > 1) game.DebugMode = 0;} 
            }
        } break;
        case SDL_MOUSEBUTTONUP:{
            Vector3D pos(event.button.x * scaleX, event.button.y * scaleY, 0);
            game.touches->up.push_back(pos);
            game.touches->allfingersup = true;
        } break;
        case SDL_MOUSEBUTTONDOWN:{
            Vector3D pos(event.button.x * scaleX, event.button.y * scaleY, 0);
            game.touches->down.push_back(pos);
            game.touches->allfingersup = false;

        } break;

        case SDL_MOUSEWHEEL:
        {
            game.keys[7] = 1;
        } break;

        case SDL_MOUSEMOTION:{
            if(SDL_GetMouseState(0, 0)&SDL_BUTTON_LMASK){
                Vector3D pos(event.button.x * scaleX, event.button.y * scaleY, 0);
                //printf("motion x:%f y:%f\n", pos.x() , pos.y());
                game.touches->move.push_back(pos);
                game.touches->allfingersup = false;
            }
        }break;


        case SDL_QUIT:
        {
            game.works = false;
        }break;

        }

    }
}
//----------
void DoNetwork()
{
    game.network();
}
//-----------

class DS : public disarray
{
public:
    DS(GameProto* game, _SDL_GameController* gamepad) : disarray(game, gamepad){}
    void otherWhileLoopActivities() override { process_events(); DoNetwork();}
};

//--------------------
int main(int argc, char* argv[])
{

    if (argc)
    {
        printf("arg: %s/n", argv[0]);
    }

    srand(time(0));

    char buf[128];
    GetHomePath(buf);
    sprintf(game.documentPath, "%s.CartridgeQuest", buf);
    MakeDir(game.documentPath);
    game.loadConfig();

    printf("%d %d\n", game.screenWidth, game.screenHeight);
    SDL.setMetrics(game.screenWidth, game.screenHeight);


    const char* title = "CARTRIDGE QUEST";

    const bool USE_VULKAN = (bool)game.renderer;

    if (!SDL.initWindow(title, "icon1.bmp", game.windowed, USE_VULKAN))
    {
        game.works = false;
    }

    game.vk = SDL.getVkVideo();

    SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    if(SDL_NumJoysticks() > 0)
    {
        Joy = SDL_JoystickOpen(0);

        if (SDL_IsGameController(0))
        {
            controllerType = SDL_GameControllerTypeForIndex(0);
            gamepad = SDL_GameControllerOpen(0);
            printf("game controller type: %s\n", GamePadTypes[controllerType]);
        }
        else
        {
            printf("Generic DirectX Joystick\n");
        }
    }

    ConfigureGraphicsLib(USE_VULKAN);

    game.timeTicks = SDL_GetTicks();

    SDL_ShowCursor(false);

    DS ds(&game, gamepad);
    ds.whileLoopPC(&SDL);

    printf("QUITING!\n");

    game.destroy();

    SDL.quit(USE_VULKAN);

    return 0;
}


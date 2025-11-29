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
#include <disarray/disarray.h>


SDLVideo SDL;
Game game;

//----------
void DoNetwork()
{
    game.network();
}
//-----------

class DS : public disarray
{
public:
    DS(GameProto* game) : disarray(game){}
    void otherWhileLoopActivities() override { DoNetwork();}
};

//--------------------
int main()//int argc, char* argv[])
{

    /*if (argc)
    {
        printf("arg: %s/n", argv[0]);
    }*/

    srand(time(0));

    char doc[128];
    GetHomePath(doc);
    sprintf(game.documentPath, "%s.CartridgeQuest", doc);
    MakeDir(game.documentPath);

    char buf[1024];
    printf("Document path: %s\n", game.documentPath);
    sprintf(buf, "%s/settings.cfg", game.documentPath);


    DS ds(&game);
    ds.setupPC(&SDL, 640, 360, false, buf, "CARTRIDGE QUEST", true);
    SDL_ShowCursor(false);

    ds.runGamePC(&SDL);

    return 0;
}


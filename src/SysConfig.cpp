
#include "SysConfig.h"
#include <cwchar>
#include <cstdio>

#ifndef __ANDROID__
#include "Xml.h"


    bool SystemConfig::load(const char * config){

        Xml conf;


        if (!conf.load(config))
            return false;

        XmlNode * settings = 0;
        settings = conf.root.getNode(L"Settings");
        if (settings){

            wchar_t tmp[255];
            char ctmp[255];

            XmlNode * nod = 0;
            nod = settings->getNode(L"MusicVolume");
            if (nod){
                wcscpy(tmp, nod->getValue());
                wcstombs(ctmp, tmp, 100);
                musicVolume = atof(ctmp);
            }


            nod = 0;
            nod = settings->getNode(L"ScreenWidth");
            if (nod){
                wcscpy(tmp, nod->getValue());
                wcstombs(ctmp, tmp, 100);
                ScreenWidth = atoi(ctmp);
            }

            nod = 0;
            nod = settings->getNode(L"ScreenHeight");
            if (nod){
                wcscpy(tmp, nod->getValue());
                wcstombs(ctmp, tmp, 100);
                ScreenHeight = atoi(ctmp);
            }

            nod = 0;
            nod = settings->getNode(L"isWindowed");
            if (nod){
                wcscpy(tmp, nod->getValue());
                wcstombs(ctmp, tmp, 100);
                useWindowed = atoi(ctmp);
            }





            

        }

        conf.destroy();

        return true;
    }
//------------------------------------------
bool SystemConfig::write(const char * config)
{

    printf("writing: %s\n", config);

        char buf[255];
        wchar_t wbuf[255];
        Xml conf;

        XmlNode Settings;
        Settings.setName(L"Settings");
        Settings.setValue(L"\n");

        XmlNode _width;
        sprintf(buf, "%f", musicVolume);
        printf("%s\n", buf);
        mbstowcs(wbuf, buf, 255);
        _width.setName(L"MusicVolume");
        _width.setValue(wbuf);
        Settings.addChild(_width);

        XmlNode _ScreenWidth;
        sprintf(buf, "%d", ScreenWidth);
        mbstowcs(wbuf, buf, 255);
        _ScreenWidth.setName(L"ScreenWidth");
        _ScreenWidth.setValue(wbuf);
        Settings.addChild(_ScreenWidth);

        XmlNode _ScreenHeight;
        sprintf(buf, "%d", ScreenHeight);
        mbstowcs(wbuf, buf, 255);
        _ScreenHeight.setName(L"ScreenHeight");
        _ScreenHeight.setValue(wbuf);
        Settings.addChild(_ScreenHeight);

        XmlNode _isWindowed;
        sprintf(buf, "%d", useWindowed);
        mbstowcs(wbuf, buf, 255);
        _isWindowed.setName(L"isWindowed");
        _isWindowed.setValue(wbuf);
        Settings.addChild(_isWindowed);


       conf.root.addChild(Settings);


        if (!conf.write(config))
        {
            conf.destroy();
            return false;
        }
        conf.destroy();
        return true;
    }
#endif

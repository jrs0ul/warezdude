/***************************************************************************
 *   Copyright (C) 2008 by jrs0ul                                          *
 *   jrs0ul@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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
    bool SystemConfig::write(const char * config){

        char buf[255];
        wchar_t wbuf[255];
        Xml conf;

        XmlNode Settings;
        Settings.setName(L"Settings");
        Settings.setValue(L"\n");

        XmlNode _width;
        sprintf(buf, "%f", musicVolume);
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


        if (!conf.write(config)){
            conf.destroy();
            return false;
        
        }
        conf.destroy();
        return true;
    }
#endif

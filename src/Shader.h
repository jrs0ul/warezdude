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
#ifndef GLSL_SHADER_H
#define GLSL_SHADER_H

#ifdef __ANDROID__
    #include <GLES2/gl2.h>
    #include <android/asset_manager.h>
#else
    #include "Extensions.h"
#endif

class ShaderProgram;

class Shader{
        GLuint id;

public:

        Shader(){
            id = 0;
        }
        //loads and compiles shader
#ifndef __ANDROID__
        bool load(GLenum shaderType, const char* path);
#else
        bool load(GLenum shaderType, const char* path, AAssetManager* man);
#endif

        friend class ShaderProgram;

};

#endif //GLSL_SHADER

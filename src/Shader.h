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

/*    Copyright (C) 2017 by jrs0ul                                          *
 *   jrs0ul@gmail.com                                                      *
*/

#include "ShaderProgram.h"
#include <cstdio>

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

#endif

    void ShaderProgram::create(){
        program = glCreateProgram();
    }
//---------------------------
void ShaderProgram::destroy()
{
    glDeleteProgram(program);
}

void ShaderProgram::getLog(char* string, int len)
{
#ifndef __ANDROID__
    glGetInfoLogARB(program, len, 0, string);
#else
    glGetShaderInfoLog(program, len, 0, string);
#endif
}

void ShaderProgram::attach(Shader & sh)
{
    //printf("attaching shader:%d to program %d\n", sh.id, program);
    glAttachShader(program, sh.id);
}

void ShaderProgram::link()
{
    glLinkProgram(program);

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE)
    {
#ifdef __ANDROID__
        LOGI("Linking failed!");
#else
        printf("Linking failed!!!1\n");
#endif

    }
    else
    {
        //printf("All good\n");
    }
}

void ShaderProgram::use()
{
    glUseProgram(program);
    //printf("program id %u\n", program);
}

int ShaderProgram::getUniformID(const char* name)
{
    return  glGetUniformLocation(program, name);
}

int ShaderProgram::getAttributeID(const char* name)
{
    return glGetAttribLocation(program, name);
}

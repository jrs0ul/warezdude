#include "OSTools.h"
#include "Shader.h"
#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

#endif

#ifdef __ANDROID__
    bool Shader::load(GLenum shaderType, const char* path, AAssetManager* man)
#else
    bool Shader::load(GLenum shaderType, const char* path)
#endif
    {

        char * mbuf = 0;
#ifdef __ANDROID__
        if (!ReadFileData(path, &mbuf, man))
#else
        if (!ReadFileData(path, &mbuf))
#endif
            return false;


        char * backup = 0;
        backup = (char*)malloc(strlen(mbuf)+1);

        strcpy(backup, mbuf);
        //puts(backup);
#ifdef __ANDROID__
        //LOGI("\n[%s]\n", backup);
#endif

        char * tmp = 0;
        tmp = strtok(mbuf,"\n\r");
        int lineCount = 0;
        while (tmp){
            lineCount++;
            tmp = strtok(0, "\n\r");
        }
        free(mbuf);

        char ** code = 0;

        code = new char*[lineCount];

        //puts("---------------");
        tmp = 0;
        tmp = strtok(backup, "\n\r");
        if (tmp){
            code[0] = new char[strlen(tmp)+1];
            strcpy(code[0], tmp);
            //puts(tmp);
        }
        for (int i = 1; i < lineCount; i++){
            tmp = strtok(0, "\n\r");
            if (tmp){
                int len = (int)strlen(tmp);
                code[i] = new char[len + 2];
                strcpy(code[i], tmp);
                code[i][len] = '\n';
                code[i][len+1] = 0;
                //puts(tmp);
            }
        }



        id = glCreateShader(shaderType);

        printf("shader id: %d\n", id);

        glShaderSource(id, lineCount, (const GLchar**)code, 0);

        //const char* dummy = "void main(void){gl_Position = vector4(1,1,1,1);}";
        //glShaderSource(id, 1, &dummy, 0);
        glCompileShader(id);

        free(backup);

        GLint compiled = 0;
        glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
        if (!compiled) 
        {
            GLint infoLen = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLen);
#ifdef __ANDROID__
            LOGI("info length %d", infoLen);
#endif
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(id, infoLen, NULL, buf);
#ifdef __ANDROID__
                    LOGI("Could not compile %d:\n%s\n", shaderType, buf);
#else
                    printf("Could not compile %d:\n%s\n", shaderType, buf);
#endif
                    free(buf);
                }
                glDeleteShader(id);
            }
            else
            {
#ifdef __ANDROID__
                LOGI("Could not compile");
#endif
            }

        }


        for (int i = 0; i < lineCount; i++){
            //puts(code[i]);
            delete []code[i];
        }
        delete []code;

        
        return true;
    }


/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2025
 -------------------------------------------
 Sprite batcher
 mod. 2025.09.28
 */

#include <cstdio>
#include <cstring>
#include <cmath>
#include <cwchar>
#ifndef __ANDROID__
#include "Extensions.h"
#endif
#include "SpriteBatcher.h"
#include "Vectors.h"
#include "OSTools.h"
#include "Xml.h"

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#endif




GLuint PicsContainer::getname(unsigned long index){
    if (index < TexNames.count())
        return TexNames[index];
    return 0;
}
//-----------------------------

PicData* PicsContainer::getInfo(unsigned long index){
    if (index < TexNames.count())
        return &PicInfo[index];
    return 0;
}
//-----------------------------
#ifndef __ANDROID__
bool PicsContainer::load(const char* list)
#else
bool PicsContainer::load(const char* list, AAssetManager* assman)
#endif
{

#ifndef __ANDROID__
    if (!initContainer(list))
#else
    if (!initContainer(list, assman))
#endif
        return false;

    for (unsigned long i = 0; i < PicInfo.count(); i++){

        Image naujas;

        unsigned short imageBits = 0;

#ifdef __ANDROID__
        if (!naujas.loadTga(PicInfo[i].name, imageBits, assman))
            LOGI("%s not found or corrupted by M$\n",PicInfo[i].name);
#else
        if (!naujas.loadTga(PicInfo[i].name,imageBits))
            printf("%s not found or corrupted by M$\n",PicInfo[i].name);
#endif

        PicInfo[i].width = naujas.width;
        PicInfo[i].height = naujas.height;


        PicInfo[i].htilew = PicInfo[i].twidth/2.0f;
        PicInfo[i].htileh = PicInfo[i].theight/2.0f;
        PicInfo[i].vframes = PicInfo[i].height/PicInfo[i].theight;
        PicInfo[i].hframes = PicInfo[i].width/PicInfo[i].twidth;

        int filtras = GL_NEAREST;
        if (PicInfo[i].filter)
            filtras = GL_LINEAR;


        glBindTexture(GL_TEXTURE_2D, TexNames[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtras );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtras );

        if (imageBits > 24)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, naujas.width, naujas.height,
                         0, GL_RGBA, GL_UNSIGNED_BYTE,naujas.data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, naujas.width, naujas.height,
                         0, GL_RGB, GL_UNSIGNED_BYTE,naujas.data);

        naujas.destroy();

    }
    return true;
}

//---------------------------------------------------
void PicsContainer::draw(
                long textureIndex,
                float x, float y,
                unsigned int frame,
                bool useCenter,
                float scaleX, float scaleY,
                float rotationAngle,
                COLOR upColor,
                COLOR dwColor,
                bool flipColors
               )
{

    SpriteBatchItem nb;

    nb.x = x;
    nb.y = y;
    nb.textureIndex = textureIndex;
    nb.frame = frame;
    nb.useCenter = useCenter;
    nb.scaleX = scaleX;
    nb.scaleY = scaleY;
    nb.rotationAngle = rotationAngle;
    if (!flipColors){
        nb.upColor[0] = upColor;
        nb.upColor[1] = upColor;
        nb.dwColor[0] = dwColor;
        nb.dwColor[1] = dwColor;
    }
    else{
        nb.upColor[0] = upColor;
        nb.dwColor[0] = upColor;
        nb.upColor[1] = dwColor;
        nb.dwColor[1] = dwColor;
    }
        batch.add(nb);
}
//---------------------------------------------------
void PicsContainer::drawVA(void * vertices, 
                           void * uvs,
                           void *colors,
                           unsigned uvsCount,
                           unsigned vertexCount,
                           ShaderProgram* shader,
                           bool useVulkan)
{
    int attribID = 0;
    int ColorAttribID = 0;
    int UvsAttribID = 0;

    if (!useVulkan)
    {
        attribID = shader->getAttributeID("aPosition"); 
        ColorAttribID = shader->getAttributeID("aColor");
        glVertexAttribPointer(attribID, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glEnableVertexAttribArray(attribID);

        if (uvsCount)
        {
            UvsAttribID = shader->getAttributeID("aUvs");
            glVertexAttribPointer(UvsAttribID, 2, GL_FLOAT, GL_FALSE, 0, uvs);
            glEnableVertexAttribArray(UvsAttribID);
        }

        glVertexAttribPointer(ColorAttribID, 4, GL_FLOAT, GL_FALSE, 0, colors);
        glEnableVertexAttribArray(ColorAttribID);

        glDrawArrays(GL_TRIANGLES, 0, vertexCount / 2 );

        glDisableVertexAttribArray(ColorAttribID);

        if (uvsCount)
        {
            glDisableVertexAttribArray(UvsAttribID);
        }

        glDisableVertexAttribArray(attribID);
    }
}
//----------------------------------------------------------
Vector3D CalcUvs(PicData * p, unsigned frame)
{

    //printf("%d\n", p->hframes);
    float hf = frame / p->hframes;

    float startx =  (frame - p->hframes * hf) * p->twidth;
    float starty = hf * p->theight;

    Vector3D result = Vector3D(
                               (startx*1.0f)/(p->width*1.0f),
                               ((startx+p->twidth)*1.0f)/(p->width*1.0f),
                               (( p->height - starty) * 1.0f ) / ( p->height * 1.0f ),//- 0.0001f,
                               (( p->height - starty - p->theight ) * 1.0f) / (p->height * 1.0f)
                              );
    return result;
}

//----------------------------------------------------------
void PicsContainer::drawBatch(ShaderProgram * justColor,
                              ShaderProgram * uvColor,
                              int method,
                              bool useVulkan){

        switch(method){
              //TODO: complete VA
        default:{
                DArray<float> vertices;
                DArray<float> uvs;
                float * colors = 0;
                unsigned colorIndex = 0;

                if (batch.count())
                {
                    colors = (float*)malloc(sizeof(float) * 24 * batch.count());
                }

                Vector3D uv(0.0f, 0.0f, 0.0f, 0.0f);
                float htilew, htileh;
                float twidth, theight;


                long texIndex = -1;

                for (unsigned long i = 0; i < batch.count(); i++){

                    PicData * p = 0;

                    htilew = 0.5f; htileh = 0.5f;
                    twidth = 1; theight = 1;

                    if ((batch[i].textureIndex >= 0) && ( batch[i].textureIndex < (long)count()))
                    {
                        p = &PicInfo[batch[i].textureIndex];
                       // printf("tex index %ld\n", batch[i].textureIndex);
                    }

                    if (p)
                    {
                        if (p->sprites.count()) //iregular sprites in atlas
                        {
                            Sprite* spr = &(p->sprites[batch[i].frame]);

                            float startX = spr->startX / (p->width * 1.f);
                            float endX   = (spr->startX + spr->width) / (p->width * 1.f);
                            float startY =  ((p->height - spr->startY - spr->height) * 1.f) / (p->height * 1.f);
                            float endY = ((p->height - spr->startY) * 1.f) / (p->height * 1.f);

                            uv = Vector3D(startX, endX, endY, startY);
                            htilew = spr->width / 2.0f; 
                            htileh = spr->height / 2.0f;
                            twidth = spr->width; 
                            theight = spr->height;
                        }
                        else //all sprites of same size
                        {
                            uv = CalcUvs(p, batch[i].frame);
                            htilew = p->twidth / 2.0f; 
                            htileh = p->theight / 2.0f;
                            twidth = p->twidth; 
                            theight = p->theight;
                        }
                    }


                    ShaderProgram* currentShader = uvColor;

                    if ((texIndex != batch[i].textureIndex)
                        && (vertices.count() > 0))
                    {
                        //let's draw old stuff
                        if ((texIndex >= 0) && (texIndex < (long)count())){
                            glBindTexture(GL_TEXTURE_2D, TexNames[texIndex]);


                            if (uvColor)
                            {
                                uvColor->use();
                                //printf("drawing with uv color shader\n");
                            }
                        }

                        else{
                            glBindTexture(GL_TEXTURE_2D, 0);

                            if (justColor)
                            {
                                justColor->use();
                                currentShader = justColor;
                                uvs.destroy();
                                //printf("drawing with just color shader\n");
                            }
                        }

                        //SDL 1.2
                        //glLoadIdentity();
                    

                        drawVA(vertices.getData(), uvs.getData(), colors,
                               uvs.count(), vertices.count(), currentShader, useVulkan);

                        //printf("vertice count %d \n", (int)vertices.count());
                        //printf("uvs count %d \n", (int)uvs.count());


                        //printf("quad count %d \n",vertices.count());

                        vertices.destroy();
                        uvs.destroy();
                        colorIndex = 0;

                }
                texIndex = batch[i].textureIndex;

                //append to arrays
                uvs.add(uv.v[0]); uvs.add(uv.v[2]);
                uvs.add(uv.v[1]); uvs.add(uv.v[2]);
                uvs.add(uv.v[1]); uvs.add(uv.v[3]);
                uvs.add(uv.v[0]); uvs.add(uv.v[2]);
                uvs.add(uv.v[1]); uvs.add(uv.v[3]);
                uvs.add(uv.v[0]); uvs.add(uv.v[3]); 
                
                //----
                    memcpy(&colors[colorIndex], batch[i].upColor[0].c,
                           sizeof(float) * 4);
                    colorIndex += 4;
                    
                    memcpy(&colors[colorIndex], batch[i].upColor[1].c, 
                           sizeof(float) * 4);
                    colorIndex += 4;
                    
                    memcpy(&colors[colorIndex], batch[i].dwColor[1].c, 
                           sizeof(float) * 4);
                    colorIndex += 4;
                    
                    memcpy(&colors[colorIndex], batch[i].upColor[0].c, 
                           sizeof(float) * 4);
                    colorIndex += 4;
                    
                    memcpy(&colors[colorIndex], batch[i].dwColor[1].c, 
                           sizeof(float) * 4);
                    colorIndex += 4;
                    
                    memcpy(&colors[colorIndex], batch[i].dwColor[0].c, 
                           sizeof(float) * 4);
                    colorIndex += 4;
                    
                //---
                if (batch[i].rotationAngle == 0.0f){
                    if (batch[i].useCenter){
                        float hwidth = htilew * batch[i].scaleX;
                        float hheight = htileh * batch[i].scaleY;


                        vertices.add(batch[i].x - hwidth); 
                        vertices.add(batch[i].y - hheight);

                        vertices.add(batch[i].x + hwidth); 
                        vertices.add(batch[i].y - hheight);

                        vertices.add(batch[i].x + hwidth); 
                        vertices.add(batch[i].y + hheight);

                        vertices.add(batch[i].x - hwidth); 
                        vertices.add(batch[i].y - hheight);

                        vertices.add(batch[i].x + hwidth); 
                        vertices.add(batch[i].y + hheight);

                        vertices.add(batch[i].x - hwidth); 
                        vertices.add(batch[i].y + hheight);
                    }
                    else{

                        vertices.add(batch[i].x);
                        vertices.add(batch[i].y);

                        vertices.add(batch[i].x + twidth * batch[i].scaleX);
                        vertices.add(batch[i].y);

                        vertices.add(batch[i].x + twidth * batch[i].scaleX); 
                        vertices.add(batch[i].y + theight * batch[i].scaleY);

                        vertices.add(batch[i].x);
                        vertices.add(batch[i].y);

                        vertices.add(batch[i].x + twidth * batch[i].scaleX); 
                        vertices.add(batch[i].y + theight * batch[i].scaleY);


                        vertices.add(batch[i].x); 
                        vertices.add(batch[i].y + theight * batch[i].scaleY);
                    }
                
                }
                else{

                //TODO: non-centered rotation

                    float angle = batch[i].rotationAngle * 0.0174532925 + 3.14f;
                  
                    if (batch[i].useCenter){
                        float hwidth = htilew * batch[i].scaleX;
                        float hheight = htileh * batch[i].scaleY;

                        float co = cosf(angle);
                        float si = sinf(angle);
                        float cos_rot_w = co * hwidth;
                        float cos_rot_h = co * hheight;
                        float sin_rot_w = si * hwidth;
                        float sin_rot_h = si * hheight;


                        vertices.add(batch[i].x + cos_rot_w - sin_rot_h); 
                        vertices.add(batch[i].y + sin_rot_w + cos_rot_h);

                        vertices.add(batch[i].x - cos_rot_w - sin_rot_h); 
                        vertices.add(batch[i].y - sin_rot_w + cos_rot_h);

                        vertices.add(batch[i].x - cos_rot_w + sin_rot_h); 
                        vertices.add(batch[i].y - sin_rot_w - cos_rot_h);

                        vertices.add(batch[i].x + cos_rot_w - sin_rot_h); 
                        vertices.add(batch[i].y + sin_rot_w + cos_rot_h);

                        vertices.add(batch[i].x - cos_rot_w + sin_rot_h); 
                        vertices.add(batch[i].y - sin_rot_w - cos_rot_h);

                        vertices.add(batch[i].x + cos_rot_w + sin_rot_h); 
                        vertices.add(batch[i].y + sin_rot_w - cos_rot_h);
                    }
                    else{

                        float _width = twidth * batch[i].scaleX;
                        float _height = theight * batch[i].scaleY;

                        float co = cosf(angle);
                        float si = sinf(angle);
                        float cos_rot_w = co * _width;
                        float cos_rot_h = co * _height;
                        float sin_rot_w = si * _width;
                        float sin_rot_h = si * _height;

                        //TODO: fix this

                        vertices.add(batch[i].x); 
                        vertices.add(batch[i].y);

                        vertices.add(batch[i].x - cos_rot_w - sin_rot_h); 
                        vertices.add(batch[i].y - sin_rot_w + cos_rot_h);

                        vertices.add(batch[i].x - cos_rot_w + sin_rot_h); 
                        vertices.add(batch[i].y - sin_rot_w - cos_rot_h);

                        vertices.add(batch[i].x); 
                        vertices.add(batch[i].y);

                        vertices.add(batch[i].x - cos_rot_w + sin_rot_h); 
                        vertices.add(batch[i].y - sin_rot_w - cos_rot_h);

                        vertices.add(batch[i].x + cos_rot_w + sin_rot_h); 
                        vertices.add(batch[i].y + sin_rot_w - cos_rot_h);

                    }

                }
            } //for

            if (vertices.count() > 0){

                ShaderProgram* currentShader = uvColor;

                if ((texIndex >= 0) && (texIndex < (long)count()))
                {
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, TexNames[texIndex/*batch[i].textureIndex*/]);
                    //printf("tex index %u\n", TexNames[texIndex]);
                    if (uvColor)
                    {
                        uvColor->use();
                        //printf("drawing uvTex shader\n");
                    }
                }
                else{
                    glBindTexture(GL_TEXTURE_2D, 0);
                    glDisable(GL_TEXTURE_2D);
                    if (justColor)
                    {
                        justColor->use();
                        currentShader = justColor;
                        uvs.destroy();
                        //printf("drawing with just color shader\n");
                    }
                }

                //for SDL 1.2
                //glLoadIdentity();

              /*  if (texIndex == 1){
                    for(unsigned z = 0; z < vertices.count(); z++){
                        printf("%f ", vertices[z]);
                    }
                    puts(" ");
                    for(unsigned z = 0; z < uvs.count(); z++){
                        printf("%f ", uvs[z]);
                    }
                    puts(" ");
                }*/

                drawVA(vertices.getData(), uvs.getData(), colors,
                       uvs.count(), vertices.count(), currentShader, useVulkan);

                //printf("vertice count %d \n", (int)vertices.count());
                //printf("uvs count %d \n", (int)uvs.count());


                vertices.destroy();
                uvs.destroy();
                colorIndex = 0;
            }

            if (colors)
                free(colors);
        }
    }

    batch.destroy();
}
//-----------------------------------------------------
void PicsContainer::resizeContainer(unsigned long index,
                                    int twidth, int theight, int filter,
                                    const char * name,
                                    bool createTextures,
                                    GLuint texname)
{

    if (PicInfo.count() < index + 1)
    {

        GLuint glui = texname;
        PicData p;
        p.twidth = twidth;
        p.theight = theight;
        p.filter = filter;

        for (unsigned i = PicInfo.count(); i < index + 1; i++)
        {
            PicInfo.add(p);
            TexNames.add(glui);
        }

        if (createTextures)
            glGenTextures(1, ((GLuint *)TexNames.getData()) + index);

        char * copy = (char*)malloc(strlen(name)+1);
        strcpy(copy, name);
        char * res = 0;
        res = strtok(copy, "/");

        while (res)
        {
            strcpy(PicInfo[index].name, res);
            res = strtok(0, "/");
        }

        free(copy);

    }
    else
    {
        PicData * pp = &PicInfo[index];
        pp->twidth = twidth;
        pp->theight = theight;
        pp->filter = filter;
        char * copy = (char*)malloc(strlen(name)+1);
        strcpy(copy, name);
        char * res = 0;
        res = strtok(copy, "/");

        while (res)
        {
            strcpy(pp->name, res);
            res = strtok(0, "/");
        }

        free(copy);

        if (glIsTexture(TexNames[index]))
        {
            glDeleteTextures(1, ((GLuint *)TexNames.getData()) + index);
        }

        if (createTextures)
        {
            glGenTextures(1, ((GLuint *)TexNames.getData()) + index);
        }
        else
        {
            *(((GLuint *)TexNames.getData()) + index) = texname;
        }

    }


}

//-----------------------------------------------------
#ifndef __ANDROID__
bool PicsContainer::loadFile(const char* file,
                             unsigned long index,
                             int twidth,
                             int theight,
                             int filter)
#else
bool PicsContainer::loadFile(const char* file,
                             unsigned long index,
                             int twidth,
                             int theight,
                             int filter,
                             AAssetManager* man)
#endif
{

        Image naujas;

        unsigned short imageBits=0;





#ifdef __ANDROID__
        if (!naujas.loadTga(file, imageBits, man)){
            LOGI("%s not found or corrupted by M$\n", file);
#else

        if (!naujas.loadTga(file, imageBits)){
            printf("%s not found or corrupted by M$\n", file);
#endif
            return false;
        }


        resizeContainer(index, twidth, theight, filter, file);

        PicInfo[index].width = naujas.width;
        PicInfo[index].height = naujas.height;


        PicInfo[index].htilew =PicInfo[index].twidth/2.0f;
        PicInfo[index].htileh =PicInfo[index].theight/2.0f;
        PicInfo[index].vframes=PicInfo[index].height/PicInfo[index].theight;
        PicInfo[index].hframes=PicInfo[index].width/PicInfo[index].twidth;

        int filtras = GL_NEAREST;
        if (PicInfo[index].filter){
            filtras = GL_LINEAR;
        }


        glBindTexture(GL_TEXTURE_2D, TexNames[index]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtras );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtras );

        GLint border = 0;

        if (imageBits > 24)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, naujas.width, naujas.height,
                 border, GL_RGBA, GL_UNSIGNED_BYTE,naujas.data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, naujas.width, naujas.height,
                 border, GL_RGB, GL_UNSIGNED_BYTE,naujas.data);

        naujas.destroy();

        return true;

}
//---------------------------------------------------
void PicsContainer::makeTexture(Image& img,
                                const char * name,
                                unsigned long index,
                                int twidth,
                                int theight,
                                int filter)
{

    resizeContainer(index, twidth, theight, filter, name);

    PicInfo[index].width = img.width;
    PicInfo[index].height = img.height;


    PicInfo[index].htilew = PicInfo[index].twidth/2.0f;
    PicInfo[index].htileh = PicInfo[index].theight/2.0f;
    PicInfo[index].vframes = PicInfo[index].height/PicInfo[index].theight;
    PicInfo[index].hframes = PicInfo[index].width/PicInfo[index].twidth;

    int filtras = GL_NEAREST;
    if (PicInfo[index].filter){
        filtras = GL_LINEAR;
    }


    glBindTexture(GL_TEXTURE_2D, TexNames[index]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtras );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtras );

    GLint border = 0;

    if (img.bits > 24)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height,
                     border, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height,
                 border, GL_RGB, GL_UNSIGNED_BYTE, img.data);

}

//--------------------------------------------------
bool PicsContainer::loadFile(unsigned long index,
                             const char * BasePath)
{

    Image naujas;

    unsigned short imageBits = 0;


    char dir[512];
    char buf[1024];

    sprintf(dir, "%spics/", BasePath);
    sprintf(buf, "%s%s", dir, PicInfo[index].name);

#ifndef __ANDROID__
    if (!naujas.loadTga(buf, imageBits)){
#else
    if (!naujas.loadTga(buf, imageBits, 0)){
#endif

        sprintf(buf, "base/pics/%s", PicInfo[index].name);
        puts("Let's try base/");
#ifndef __ANDROID__
        if (!naujas.loadTga(buf, imageBits)){
#else
        if (!naujas.loadTga(buf, imageBits, 0)){
#endif
            printf("%s not found or corrupted by M$\n", buf);
            return false;
        }

    }

    PicInfo[index].width = naujas.width;
    PicInfo[index].height = naujas.height;


    PicInfo[index].htilew = PicInfo[index].twidth / 2.0f;
    PicInfo[index].htileh = PicInfo[index].theight / 2.0f;
    PicInfo[index].vframes = PicInfo[index].height / PicInfo[index].theight;
    PicInfo[index].hframes = PicInfo[index].width / PicInfo[index].twidth;

    int filtras = GL_NEAREST;
    if (PicInfo[index].filter)
        filtras = GL_LINEAR;


    glBindTexture(GL_TEXTURE_2D, TexNames[index]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtras );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtras );

    if (imageBits > 24)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, naujas.width, naujas.height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE,naujas.data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, naujas.width, naujas.height,
                     0, GL_RGB, GL_UNSIGNED_BYTE,naujas.data);

    naujas.destroy();

    return true;

}
//---------------------------------
void PicsContainer::attachTexture(GLuint textureID, unsigned long index,
                                  int width, int height,
                                 int twidth, int theight, int filter)
{

    resizeContainer(index, twidth, theight, filter, "lol", false, textureID);
    PicInfo[index].width = width;
    PicInfo[index].height = height;


    PicInfo[index].htilew = PicInfo[index].twidth / 2.0f;
    PicInfo[index].htileh = PicInfo[index].theight / 2.0f;
    PicInfo[index].vframes = PicInfo[index].height / PicInfo[index].theight;
    PicInfo[index].hframes = PicInfo[index].width / PicInfo[index].twidth;



    int filtras = GL_NEAREST;
    if (PicInfo[index].filter)
        filtras = GL_LINEAR;

    glBindTexture(GL_TEXTURE_2D, TexNames[index]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtras );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtras );
    glBindTexture(GL_TEXTURE_2D, 0);

}


//--------------------------------
int PicsContainer::findByName(const char* picname, bool debug){
    unsigned long start = 0;

    if (!PicInfo.count())
    {
        return -1;
    }

    while ((strcmp(PicInfo[start].name, picname) != 0) && (start < PicInfo.count()))
    {
        if (debug)
            puts(PicInfo[start].name);
        start++;
    }

    if (start == PicInfo.count())
    {
        return -1;
    }

    return start;
}
//---------------------------------------
#ifndef __ANDROID__
bool PicsContainer::initContainer(const char *list)
#else
bool PicsContainer::initContainer(const char *list, AAssetManager* assman)
#endif
{


    Xml pictureList;

#ifndef __ANDROID__
    bool result = pictureList.load(list);
#else


    bool result = pictureList.load(list, assman);

#endif

    if (result)
    {
        XmlNode *mainnode = pictureList.root.getNode(L"Images");

        int imageCount = 0;

        if (mainnode)
        {
            imageCount = mainnode->childrenCount();
        }

        for (int i = 0; i < imageCount; ++i)
        {
            PicData data;

            XmlNode *node = mainnode->getNode(i);

            if (node)
            {
                for (int j = 0; j < (int)node->attributeCount(); ++j)
                {
                    XmlAttribute *atr = node->getAttribute(j);

                    if (atr)
                    {
                        if (wcscmp(atr->getName(), L"src") == 0)
                        {
                            wchar_t* value = atr->getValue();

                            if (value)
                            {
                                sprintf(data.name, "%ls", value);
                            }
                        }
                        else if (wcscmp(atr->getName(), L"width") == 0)
                        {
                            char buffer[100];
                            wchar_t* value = atr->getValue();

                            if (value)
                            {
                                sprintf(buffer, "%ls", value);
                                data.twidth = atoi(buffer);
                            }
                        }
                        else if (wcscmp(atr->getName(), L"height") == 0)
                        {
                            char buffer[100];
                            wchar_t* value = atr->getValue();

                            if (value)
                            {
                                sprintf(buffer, "%ls", value);
                                data.theight = atoi(buffer);
                            }
                        }
                        else if (wcscmp(atr->getName(), L"filter") == 0)
                        {
                            char buffer[100];
                            wchar_t* value = atr->getValue();

                            if (value)
                            {
                                sprintf(buffer, "%ls", value);
                                data.filter = atoi(buffer);
                            }
                        }
                    }
                }

                XmlNode* pathNode = node->getNode(L"Path");

                if (pathNode)
                {
                    sprintf(data.name, "%ls", pathNode->getValue());
                }

                XmlNode* spritesNode = node->getNode(L"Sprites");

                if (spritesNode)
                {
                    for (int j = 0; j < (int)spritesNode->childrenCount(); ++j)
                    {
                        XmlNode* sprite = spritesNode->getNode(j);
                        if (sprite)
                        {
                            Sprite spr;

                            for (int h = 0; h < (int) sprite->attributeCount(); ++h)
                            {

                                XmlAttribute* at = sprite->getAttribute(h);

                                char buffer[100];
                                sprintf(buffer, "%ls", at->getName());

                                if (strcmp(buffer, "name") == 0)
                                {
                                    sprintf(spr.name, "%ls", at->getValue());
                                }
                                else if (strcmp(buffer, "x") == 0)
                                {
                                    sprintf(buffer, "%ls", at->getValue());
                                    spr.startX = atoi(buffer);
                                }
                                else if (strcmp(buffer, "y") == 0)
                                {
                                    sprintf(buffer, "%ls", at->getValue());
                                    spr.startY = atoi(buffer);
                                }
                                else if (strcmp(buffer, "width") == 0)
                                {
                                    sprintf(buffer, "%ls", at->getValue());
                                    spr.width = atoi(buffer);
                                }
                                else if (strcmp(buffer, "height") == 0)
                                {
                                    sprintf(buffer, "%ls", at->getValue());
                                    spr.height = atoi(buffer);
                                }

                            }

                            data.sprites.add(spr);

                        }
                    }

                }

            }

#ifdef __ANDROID__
//            LOGI(">>>Image: name:%s width:%d height:%d filter%d\n", data.name, data.twidth, data.theight, data.filter);
#else

//            printf(">>>Image: name:%s width:%d height:%d filter%d\n", data.name, data.twidth, data.theight, data.filter);
#endif
            PicInfo.add(data);
        }

        for (unsigned long i = 0; i < PicInfo.count(); i++) {
            GLuint glui = 0;
            TexNames.add(glui);
        }

        glGenTextures(PicInfo.count(), (GLuint *) TexNames.getData());
    }

    pictureList.destroy();

    return result;

}

//----------------------------------
void PicsContainer::destroy(){
    for (unsigned long i = 0; i < TexNames.count(); i++){
        if (glIsTexture(TexNames[i]))
            glDeleteTextures(1, ((GLuint *)TexNames.getData()) + i);
    }
    TexNames.destroy();
    batch.destroy();

    for (unsigned long i = 0; i < PicInfo.count(); ++i)
    {
        PicInfo[i].sprites.destroy();
    }

    PicInfo.destroy();
}

//-------------------------------
void PicsContainer::remove(unsigned long index){
    if (index < TexNames.count()){
        if (glIsTexture(TexNames[index]))
            glDeleteTextures(1, ((GLuint *)TexNames.getData()) + index);

        TexNames.remove(index);
        PicInfo.remove(index);
    }

}


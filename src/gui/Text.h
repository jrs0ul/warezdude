#ifndef _BITMAP_TEXT_H
#define _BITMAP_TEXT_H

#include "../Colors.h"
#include "../TextureLoader.h"


//writes text
void WriteText(unsigned int x, unsigned int y,
               PicsContainer& pics, int font, const char* s,
               float scalex=1.0f, float scaley=1.0f,
               COLOR c1 = COLOR(1.0f, 1.0f, 1.0f, 1.0f),
               COLOR c2 = COLOR(1.0f, 1.0f, 1.0f, 1.0f));
void WriteShadedText(unsigned int x, unsigned int y,
               PicsContainer& pics, int font, const char* s,
               float scalex=1.0f, float scaley=1.0f,
               COLOR c1 = COLOR(1.0f, 1.0f, 1.0f, 1.0f),
               COLOR c2 = COLOR(1.0f, 1.0f, 1.0f, 1.0f),
               COLOR shade = COLOR(0.0, 0.0, 0.0, 1.0f));

void DrawNumber(int x, int y, int number, PicsContainer& pics, unsigned index);

#endif //_BITMAP_TEXT_H


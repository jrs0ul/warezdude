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
#include "Text.h"


void WriteText(unsigned int x, unsigned int y,
               PicsContainer& pics, int font,
               const char* s,
               float scalex, float scaley, COLOR c1, COLOR c2){

    for (unsigned int i = 0; i < strlen(s); i++){
        pics.draw(font, x+(9*scalex)*i, y, ((unsigned char)s[i])-32,
                  false, scalex, scaley, 0.0f, c1, c2);
    }
}
//-----------------------------------------------------
void WriteShadedText(unsigned int x, unsigned int y,
                     PicsContainer& pics, int font, const char* s,
                     float scalex, float scaley,
                     COLOR c1,
                     COLOR c2,
                     COLOR shade){
    WriteText(x - 1, y - 1, pics, font, s, scalex, scaley, shade, shade);
    WriteText(x, y, pics, font, s, scalex, scaley, c1, c2);
    
    
    
}
//------------------------------------------------
void DrawNumber(int x, int y, int number, PicsContainer& pics, unsigned index){
    int arr[7] = {0};

    int i = 6;
    while (number != 0){
        arr[i] = number % 10;
        number = number / 10;
        i--;
    }

    for (int a=0; a < 7; a++)
        pics.draw(index, x + a * 16, y, arr[a], false, 0.98f,0.98f, 0, COLOR(1,1,1,0.6f), COLOR(1,1,1,0.6f)); 




 
}


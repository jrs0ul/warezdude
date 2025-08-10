#include "Intro.h"
#include "TextureLoader.h"
#include "gui/Text.h"


void Intro::reset()
{
    memset(IntroText, 0, sizeof(IntroText));
    cline = 0;
    cchar = 0;
    gline = 0;
}

void Intro::logic()
{
    if (gline <= lin)
    {
        tim++;

        if (tim == 5)
        {
            tim = 0;
            cchar++;

            if ((IntroText[cline][cchar] == '\0'))
            {
                cline++;
                gline++;

                if (cline == 18)
                {
                    cline = 0;
                }

                cchar = 0;

            }
        }
    }

}


void Intro::load(const char* filename)
{
    FILE* f;
    puts("Loading intro text...");
    f = fopen(filename,"rt");

    if (!f)
    {
        printf("bumer,could not open %s\n", filename);
        return;
    }


    int pos = 0;
    int c = ';';
    while (c != EOF)
    {
        c = fgetc(f);

        if ((c != EOF) || (lin < 100))
        {
            if ((c != '\n') && (pos < 100))
            {
                IntroText[lin][pos] = c;
                pos++;
            }
            else
            {
                IntroText[lin][pos]='\0';
                puts(IntroText[lin]);
                lin++;
                pos = 0;
            }

        }
    }

    fclose(f);

}


void Intro::draw(PicsContainer& pics)
{
    pics.draw(13, 320, 240, 0, true);

    char buf[2];

    for (int i = 0; i < cline; i++)
    {
        WriteShadedText(30, 25 * i + 20, pics, 10, IntroText[i + 18 * (gline / 18)]);
    }

    for (int a=0; a < cchar; a++)
    {
        sprintf(buf, "%c", IntroText[gline][a]);
        WriteShadedText(
                  30 + a * 11, 
                  25 * cline + 20,
                  pics,
                  10,
                  buf
                  );
    }

    WriteShadedText(30,450, pics, 10, "hit RETURN to skip ...");
}

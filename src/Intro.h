#pragma once


const int INTRO_TEXT_LINE_WIDTH = 100;
const int INTRO_TEXT_LINE_COUNT = 100;

class PicsContainer;

class Intro
{
    char IntroText[INTRO_TEXT_LINE_COUNT][INTRO_TEXT_LINE_WIDTH];
    int lin;
    int tim;
    int cline;
    int gline;
    int cchar;


public:
    Intro(){ reset();}
    void logic();
    void reset();
    void load(const char* filename);
    void draw(PicsContainer& pics);
};

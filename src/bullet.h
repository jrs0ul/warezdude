#ifndef BULLET_H
#define BULLET_H


class Bullet{
 
public:
 int tim;
 int explodetim;
 float x,y;
 float angle;
 int parentID;
 int frame;
 bool explode;
 bool exists;
 bool isMine;



 Bullet(){
	x=0;y=0;tim=0;
    angle=0.0f; parentID=0; frame=0;
	explodetim=0;
	explode=false;
	exists=true;
	isMine=false;
 }
 void ai(bool** map, int width, int height);
};





#endif //BULLET_H

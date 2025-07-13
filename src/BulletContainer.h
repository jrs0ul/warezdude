#ifndef CBULL_CON_H
#define CBULL_CON_H

#include "bullet.h"

class CBulletContainer{
 int _count;
 public:
  Bullet* buls;
 // FILE* fil;
  
  CBulletContainer(){ buls = 0; _count=0;}
  int count(){return _count;};
  void add(Bullet& newbulet);
  void removeDead();
  void destroy();
  ~CBulletContainer(){}


 };

 

#endif // CBULL_CON_H


#ifndef ACTIVE_CONTROL_H
#define ACTIVE_CONTROL_H

#include "BasicControl.h"

 class ActiveControl:public BasicControl{
  bool activated; //ar aktyvuotas 
 public:
  void activate(){if (!activated) activated=true;}
  bool active(){return activated;}
  void deactivate(){if (activated) activated=false;}
 };

#endif
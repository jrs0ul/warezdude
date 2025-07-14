#ifndef USEFULL_H
#define USEFULL_H



//apvalina floata
int round(double x);
//ar kolidina du apskritimai?
bool CirclesColide(float x1,float y1,float radius1, float x2, float y2, float radius2);


Vector3D* Line(int x1, int y1, int x2, int y2, int& count, int gridw);
Vector3D MakeVector(float speedx, float speedy, float _angle );



#endif// USEFULL_H

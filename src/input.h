/*
input class
jrs0ul
2007 02 14*/

#define DIRECTINPUT_VERSION 0x0800

#ifndef XINPUT_H
#define XINPUT_H


#include <dinput.h>



#define JOYMIN -10000
#define JOYMAX 10000
#define JOYDEAD 2000

#define MOUSE_BUFFER_SIZE 16

class XInput{
public:
 LPDIRECTINPUT8 pInput;
 LPDIRECTINPUTDEVICE8 pKeyboard;
 LPDIRECTINPUTDEVICE8 pJoystick;
 LPDIRECTINPUTDEVICE8 pMouse;

 char keydata[256];
 DIJOYSTATE jstate;
 HANDLE hMouseEvent;
 DIMOUSESTATE mstate;

 XInput(){pInput=0; pKeyboard=0; pJoystick=0; pMouse=0; hMouseEvent=0;}
 bool initInput(HINSTANCE& mHinstance);
 bool initKeyboard(HWND& hwnd);
 bool initJoystick(HWND& hwnd);
 bool initMouse(HWND& hwnd);

 void updateJoyData();
 void updateKeyData();
 void updateMouseData();

 void deleteKeyboard();
 void deleteJoystick();
 void deleteMouse();
 void deleteInput();
};


#endif //XINPUT_H

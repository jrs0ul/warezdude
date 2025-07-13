/*
input class
jrs0ul
2007 03 15*/
#include "Input.h"





bool XInput::initInput(HINSTANCE& mHinstance){
  if (FAILED(DirectInput8Create(mHinstance, DIRECTINPUT_VERSION ,IID_IDirectInput8,(void**)&pInput,NULL)))
  return false;
  else 
	  return true;
}
//----------------------------------
bool XInput::initKeyboard(HWND& hwnd){

	if (FAILED(pInput->CreateDevice(GUID_SysKeyboard, &pKeyboard, NULL))) return false;
  
	if (FAILED(pKeyboard -> SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))){
		pKeyboard->Release();
		return false;
	}	

	if(FAILED(pKeyboard -> SetDataFormat(&c_dfDIKeyboard))){
		pKeyboard->Release();
		return false;
    } 

	if(FAILED(pKeyboard -> Acquire())){
		pKeyboard->Release();
		return false;
	}
	
    return true;
}
//-----------------------------
bool XInput::initMouse(HWND& hwnd){

 if (FAILED(pInput->CreateDevice(GUID_SysMouse, &pMouse, NULL))) return false;
 
 if(FAILED(pMouse -> SetDataFormat(&c_dfDIMouse))){
		pMouse->Release();
		return false;
    } 

 if (FAILED(pMouse->SetCooperativeLevel(hwnd,
	 DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))){
    pMouse->Release();
	return false;
 }




 hMouseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

if (hMouseEvent == NULL) {
    pMouse->Release();
	return false;
}

 if (FAILED(pMouse->SetEventNotification(hMouseEvent))){
  pMouse->Release();
  return false;
 }
 
 DIPROPDWORD dipdw;
    // the header
 dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
 dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
 dipdw.diph.dwObj        = 0;
 dipdw.diph.dwHow        = DIPH_DEVICE;
 // the data
 dipdw.dwData            = MOUSE_BUFFER_SIZE;
 
 if (FAILED(pMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph))){
    pMouse->Release();
    return false; 
 }
 

 if(FAILED(pMouse -> Acquire())){
		pMouse->Release();
		return false;
	}


 return true;

}


//------------------------------------
bool XInput::initJoystick(HWND& hwnd){

 if (FAILED(pInput->CreateDevice(GUID_Joystick,&pJoystick,NULL))) return false;
  
 if (FAILED(pJoystick -> SetDataFormat(&c_dfDIJoystick))){ 
	 pJoystick->Release();
	 pJoystick=0;
     return false; 
  }
 
  if (FAILED(pJoystick->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))){
	  pJoystick->Release();
	  pJoystick=0;
	  return false; 
   } 

   if (FAILED(pJoystick -> Acquire())){ 
	   pJoystick->Release();
	   pJoystick=0;
       return false;		
	} 
   
    DIPROPRANGE diprg; 
 
    diprg.diph.dwSize       = sizeof( diprg ); 
    diprg.diph.dwHeaderSize = sizeof( diprg.diph ); 
    diprg.diph.dwObj        = 0; 
    diprg.diph.dwHow        = DIPH_DEVICE; 
    diprg.lMin              = JOYMIN; 
    diprg.lMax              = JOYMAX; 
 
    if ( FAILED( pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
	{
		pJoystick->Unacquire();
		pJoystick->Release();
		pJoystick=0;
        return FALSE; 
	}
    // deadzone
    DIPROPDWORD dipdw;

    dipdw.diph.dwSize       = sizeof( dipdw ); 
    dipdw.diph.dwHeaderSize = sizeof( dipdw.diph ); 
    dipdw.diph.dwObj        = 0;
    dipdw.diph.dwHow        = DIPH_DEVICE;
    dipdw.dwData            = JOYDEAD;

	if ( FAILED( pJoystick->SetProperty( DIPROP_DEADZONE, &dipdw.diph ) ) ){
        pJoystick->Unacquire();
		pJoystick->Release();
		pJoystick=0; 
        return FALSE;
	}

	return true;
}
//-------------------------------------
void XInput::updateJoyData(){
 if (FAILED(pJoystick->Poll())) return;

 pJoystick -> GetDeviceState(sizeof(DIJOYSTATE),(LPVOID)&jstate);

}

//--------------------------------------

void XInput::updateKeyData(){
 HRESULT res;
 res=pKeyboard -> GetDeviceState(sizeof(keydata),(LPVOID)&keydata);
 while (res==DIERR_INPUTLOST)
	 res=pKeyboard->Acquire();

	
 
 
}

//--------------------------------------
void XInput::updateMouseData(){
    HRESULT hr;
	DWORD dwElements = 1;
	hr = pMouse->GetDeviceState(sizeof(DIMOUSESTATE), 
                                     &mstate);
	while (hr==DIERR_INPUTLOST)
	 hr=pMouse->Acquire();
  
    

}

//-------------------------------
void XInput::deleteJoystick(){
  if (pJoystick!=0){
			pJoystick->Unacquire();
			pJoystick->Release();
			pJoystick=0;
		}
}

//-------------------------------

void XInput::deleteKeyboard(){
	if (pKeyboard!=0){
		pKeyboard->Unacquire();
		pKeyboard->Release();
		pKeyboard=0;
	}

}
//------------------------------

void XInput::deleteMouse(){
 if (pMouse!=0){
		pMouse->Unacquire();
		pMouse->Release();
		pMouse=0;
	}
}



//-------------------------------
void XInput::deleteInput(){
	if (pInput!=0){
		pInput->Release();
		pInput=0;
	}
}
//------





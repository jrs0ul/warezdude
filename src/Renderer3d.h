#ifndef RENDERER3D_H
#define RENDERER3D_H


/*
jrs0ul
2007 01 27
*/



#include <d3d9.h>
#include <d3dx9.h>



int AvailVidMem();



class Renderer3D{
  public:
   LPDIRECT3DDEVICE9           lpD3DDevice;
   LPDIRECT3D9					lpD3D;
   LPD3DXSPRITE spraitas;
   
   int deviceIndex;
   int colorBitCount;
   bool tnl;
   bool antialias;
   bool isHAL;
  
   Renderer3D(){ lpD3DDevice=0;
				lpD3D=0;
				spraitas=0;
				deviceIndex=-1;
				tnl=false;
				antialias=false;
				isHAL=false;
	}
   HRESULT init();
   void destroy();
   HRESULT setmode(HWND mainwindow,UINT width,UINT height,UINT bits,bool windowed, bool depthstencil);
   //padaro bmp screenshota
   int writeScreenshot(char* FileName);

private:
	bool findDevice(LPDIRECT3D9& d3d,UINT bits,bool windowed);
  
  };


#endif //RENDERER3D_H
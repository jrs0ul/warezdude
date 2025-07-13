#include "Renderer3D.h"
#include <cstdio>

/*
Jrs0ul
2007 03 20
*/


HRESULT Renderer3D::init(){
 lpD3D=Direct3DCreate9(D3D_SDK_VERSION);
 if (!lpD3D)
  return E_FAIL;
 return D3D_OK;
}

void Renderer3D::destroy(){

	if (spraitas){
	 spraitas->Release();
	 spraitas=0;
	}

	if (lpD3DDevice){
	 lpD3DDevice->Release();
	 lpD3DDevice=0;
	}


	if (lpD3D){
	   lpD3D->Release();
	   lpD3D=0;
	}

  

}
//-----------------------------------------------------------------
bool Renderer3D::findDevice(LPDIRECT3D9& d3d,UINT bits,bool windowed){

	DWORD count=d3d->GetAdapterCount();

	DWORD i=0;
	int answer=0;
	D3DCAPS9 caps;

	D3DFORMAT adapterFormat;
	D3DFORMAT backbuferFormat;

	if (bits==32){
		adapterFormat=D3DFMT_X8R8G8B8;
		backbuferFormat=D3DFMT_A8R8G8B8;
	}
	else{
		adapterFormat=D3DFMT_R5G6B5;  
		backbuferFormat=D3DFMT_R5G6B5; 
	}

	while ((answer==0)&&(i<count)){
		if (D3D_OK==d3d->CheckDeviceType(i,D3DDEVTYPE_HAL,adapterFormat,backbuferFormat,windowed)){
			 d3d->GetDeviceCaps(i,D3DDEVTYPE_HAL,&caps);
			if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
				tnl=true;
			else
				tnl=false;
			answer=1;
		}
		else
			answer=0; 
		i++;
	}

	
	if ((i>=count)&&(answer==0)){
		i=0;
		answer=0;
		isHAL=false;
	}
	else{
		deviceIndex=i-1;
		isHAL=true;
		return true;
	}
	
	
	while ((answer==0)&&(i<count)){
		if (D3D_OK==d3d->CheckDeviceType(i,D3DDEVTYPE_REF,adapterFormat,backbuferFormat,windowed)){
			d3d->GetDeviceCaps(i,D3DDEVTYPE_REF,&caps);

			if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
				tnl=true;
			else
				tnl=false;

			answer=1;
		}
		else
			answer=0; 
		i++;
	}



	if ((i>=count)&&(answer==0))
		return false;
	

	deviceIndex=i-1;

	return true;
}


//-----------------------------

HRESULT Renderer3D::setmode(HWND mainwindow,UINT width,UINT height,UINT bits,bool windowed, bool depthstencil){

 DWORD lev=0;	
 HRESULT hr;
   
 if (!findDevice(lpD3D,bits,windowed)){
	if (windowed)
		MessageBox(mainwindow,"Can't find suitable video device!","Error",0);
	return -1;
 }

D3DPRESENT_PARAMETERS params;
ZeroMemory(&params,sizeof(D3DPRESENT_PARAMETERS));


if (bits==32)
 params.BackBufferFormat=D3DFMT_A8R8G8B8;
else
 params.BackBufferFormat=D3DFMT_R5G6B5;

params.Windowed=windowed;
params.BackBufferCount=1;
params.BackBufferHeight=height;
params.BackBufferWidth=width;

if (antialias)
 params.MultiSampleType=D3DMULTISAMPLE_2_SAMPLES;

params.hDeviceWindow=mainwindow;
if (depthstencil)
 params.EnableAutoDepthStencil=true;
params.AutoDepthStencilFormat=D3DFMT_D24S8;
params.Flags=0;//D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
params.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;


params.SwapEffect=D3DSWAPEFFECT_DISCARD;
params.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;


DWORD ptnl; 
if (tnl)
 ptnl=D3DCREATE_HARDWARE_VERTEXPROCESSING;
else
 ptnl=D3DCREATE_SOFTWARE_VERTEXPROCESSING;

D3DDEVTYPE devtype=D3DDEVTYPE_HAL;

if (!isHAL)
  devtype=D3DDEVTYPE_REF;


if (FAILED(hr=lpD3D->CreateDevice(deviceIndex,devtype,
	                              mainwindow,ptnl,
								  &params,&lpD3DDevice))){    
 char buf[50];
 sprintf(buf,"code: %x, deviceIndex:%d, isHAL:%d",hr,deviceIndex,isHAL);
 MessageBox(mainwindow,buf,"Error!",MB_OK);
 return hr;
 }
 
 
  D3DVIEWPORT9 vp = {0,0, width, height, 0.0f, 1.0f };
  hr = lpD3DDevice->SetViewport(&vp);
  if (FAILED(hr))
	return hr;

  if (FAILED(hr=D3DXCreateSprite(lpD3DDevice,&spraitas)))
	 return hr;
  

 return D3D_OK;
}

//-----------------------------

int Renderer3D::writeScreenshot(char* Filename){
	 LPDIRECT3DSURFACE9 lpDDS;
	
	if (FAILED(lpD3DDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&lpDDS)))
      return -1;
    
    D3DXSaveSurfaceToFile(Filename,D3DXIFF_BMP,lpDDS,NULL,NULL);

	
 return 1;
}
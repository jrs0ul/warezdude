#ifndef SYSCONF_H
#define SYSCONF_H

/*

Jrs0ul
2007 03 23

*/


struct SystemConf{

 bool usespecular;
 bool enableDepthStencil;
 int antialias;
 bool tnl;

 WORD width;
 WORD height;
 WORD bits;
 bool windowed;

 bool shutdown;
 int maxmodes;
 int currmode;
 int devindex;

 long soundFXVolume;
 long musicVolume;

 bool soundCardWorks;
 bool joypresent;

 
 SystemConf(){
  usespecular=false;
  antialias=0;
  width=640;
  height=480;
  bits=16;
  soundCardWorks=false;
  shutdown=false;
  maxmodes=0;
  currmode=0;
  devindex=0;
  soundFXVolume=0;
  musicVolume=0;
  windowed=true;
  joypresent=false;
  enableDepthStencil=false;
 }
//------------------------------------
 bool load(const char* path, bool use3D){
	 FILE* conf;

 int  antiAlias=0;
 int isWindowed=1;
 int useZbuff=0;

 fopen_s(&conf,path,"rt");
 if (!conf)
  return false;
  
  fscanf_s(conf,"%d %d %d %d\n",&width, &height, &bits, &isWindowed);

  (isWindowed>0) ? windowed=true : windowed=false;

  if (use3D){
	  fscanf_s(conf,"%d %d\n",&antialias, &useZbuff);
	  if (antiAlias>0)
		  antialias=antiAlias;
	  (useZbuff>0) ? enableDepthStencil=true : enableDepthStencil=false;
	 
  }
  fscanf_s(conf,"%d %d",&soundFXVolume, &musicVolume);
  
  
	  
	  
  fclose(conf);
  return true;
 }
};



#endif //SYSCONF_H
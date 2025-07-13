#ifndef _SOUNDFX_H_
#define _SOUNDFX_H_

#include <dsound.h>
#include <mmsystem.h>

class Gabalas{
    LPDIRECTSOUNDBUFFER sample;
	LPDIRECTSOUND3DBUFFER sample3d;

	MMCKINFO ck;
	HMMIO hmmio;
	MMCKINFO ckriff;
	
public:
	bool ready;
	Gabalas():ready(false){}
	void open(LPDIRECTSOUND8& snd,char * name, bool use3D=false, bool headRelative=false);
	void play(bool loop=false);
	void setSoundPos(float x, float y, float z);
	void setVolume(LONG vol);
	bool isPlaying();
	void duplicate(LPDIRECTSOUND8 snd,LPDIRECTSOUNDBUFFER* ref);
	void release();

};
#endif //_SOUNDFX_H_
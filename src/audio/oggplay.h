#ifndef _OGG_PLAY_H_
#define _OGG_PLAY_H_

#include    <windows.h>                 // from your fave include dir ;)

#include    <dsound.h>                  // from the directx 8 sdk

#include    <vorbis/codec.h>            // from the vorbis sdk
#include    <vorbis/vorbisfile.h>       // also :)

#define     BUFSIZE    65536*10         // buffer length

class OggPlayer
{
protected:

    bool        bFileOpened;            // have we opened an ogg yet?
   
    LPDIRECTSOUNDBUFFER pDSB;                   // the buffer

    OggVorbis_File vf;                     // for the vorbisfile interface

    int         nLastSection,           // which half of the buffer are/were
                nCurSection;            // we playing?

    bool        bAlmostDone;            // only one half of the buffer to play
    bool        bDone;                  // done playing
    bool        bLoop;                  // loop?

public:

    OggPlayer();
    ~OggPlayer();

    bool OpenOgg(LPDIRECTSOUND8 pDS, const char *filename ); // this opens an oggvorbis for playing
    void Close();                                // and this one closes it :)
    void Play(bool loop = false);                                // play it again sam     
    void Stop();                                // stop it
    void Update();                               // be sure to call this from time to time
    inline bool IsPlaying(){
        return !bDone;
    }
	void setVolume(LONG vol);
	
};

#endif //_OGG_PLAY_H_
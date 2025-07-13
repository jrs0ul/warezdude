#include "oggplay.h"


OggPlayer::OggPlayer(){
    bFileOpened     = false;
    pDSB            = 0;
    bLoop           = false;
    bDone           = false;
    bAlmostDone     = false;
}
//----------------------------------------------
OggPlayer::~OggPlayer(){
    if (bFileOpened)
        Close();
}
//--------------------------------------------------------

bool OggPlayer::OpenOgg(LPDIRECTSOUND8 pDS, const char *filename ){

    if (bFileOpened)
        Close();

    FILE    *f;

    f = fopen(filename, "rb");
    if (!f) return false;

    ov_open(f, &vf, NULL, 0);
	

    // the vorbis_info struct keeps the most of the interesting format info
    vorbis_info *vi = ov_info(&vf,-1);

    // set the wave format
    WAVEFORMATEX        wfm;

    memset(&wfm, 0, sizeof(wfm));

    wfm.cbSize          = sizeof(wfm);
    wfm.nChannels       = vi->channels;
    wfm.wBitsPerSample  = 16;                    // ogg vorbis is always 16 bit
    wfm.nSamplesPerSec  = vi->rate;
    wfm.nAvgBytesPerSec = wfm.nSamplesPerSec*wfm.nChannels*2;
    wfm.nBlockAlign     = 2*wfm.nChannels;
    wfm.wFormatTag      = 1;


    // set up the buffer
    DSBUFFERDESC desc;

    desc.dwSize         = sizeof(desc);
    desc.dwFlags        = DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRLVOLUME; //kad ir praradus fokusa grotu
    desc.lpwfxFormat    = &wfm;
    desc.dwReserved     = 0;

    desc.dwBufferBytes  = BUFSIZE*2;
    pDS->CreateSoundBuffer(&desc, &pDSB, NULL );

    // fill the buffer

    DWORD   pos = 0;
    int     sec = 0;
    int     ret = 1;
    DWORD   size = BUFSIZE*2;

    char    *buf;

    pDSB->Lock(0, size, (LPVOID*)&buf, &size, NULL, NULL, DSBLOCK_ENTIREBUFFER);
    
    // now read in the bits
    while(ret && pos<size){
        ret = ov_read(&vf, buf+pos, size-pos, 0, 2, 1, &sec);
        pos += ret;
    }

    pDSB->Unlock( buf, size, NULL, NULL );

    nCurSection =  nLastSection        = 0;


    return bFileOpened = true;
}
//-------------------------------------------------------------
void OggPlayer::Close()
{
    bFileOpened = false;

	ov_clear(&vf);

    if (pDSB)
        pDSB->Release();

	pDSB=0;
}

//---------------------------------------------------

void OggPlayer::Play(bool loop)
{

    if (!bFileOpened)
        return;

    // play looping because we will fill the buffer
    pDSB->Play(0,0,DSBPLAY_LOOPING);    

    bLoop = loop;
    bDone = false;
    bAlmostDone = false;
}
//---------------------------------------------
void OggPlayer::Stop(){

    if (!bFileOpened)
        return;

	if (pDSB)
     pDSB->Stop();
}
//------------------------------------------
void OggPlayer::Update(){
	DWORD pos;

	if (pDSB){

		pDSB->GetCurrentPosition(&pos, NULL);

		nCurSection = pos<BUFSIZE?0:1;

		// section changed?
		if (nCurSection != nLastSection)
		{
			if (bDone && !bLoop)
				Stop();

			// gotta use this trick 'cause otherwise there wont be played all bits
			if (bAlmostDone && !bLoop)
				bDone = true;

			DWORD   size = BUFSIZE;
			char    *buf;

			// fill the section we just left
			pDSB->Lock( nLastSection*BUFSIZE, size, (LPVOID*)&buf, &size, NULL, NULL, 0 );

			DWORD   pos = 0;
			int     sec = 0;
			int     ret = 1;

			while(ret && pos<size){
				ret = ov_read(&vf, buf+pos, size-pos, 0, 2, 1, &sec);
				pos += ret;
			}

			// reached the and?
			if (!ret && bLoop)
			{
				// we are looping so restart from the beginning
				// NOTE: sound with sizes smaller than BUFSIZE may be cut off

				ret = 1;
				ov_pcm_seek(&vf, 0);
				while(ret && pos<size)
				{
					ret = ov_read(&vf, buf+pos, size-pos, 0, 2, 1, &sec);
					pos += ret;
				}
			}
			else if (!ret && !(bLoop))
			{
				// not looping so fill the rest with 0
				while(pos<size)
					*(buf+pos)=0; pos ++;

				// and say that after the current section no other sectin follows
				bAlmostDone = true;
			}

			pDSB->Unlock( buf, size, NULL, NULL );

			nLastSection = nCurSection;
		}
	}
}

//--------------------------------------
void OggPlayer::setVolume(LONG vol){
	
	pDSB->SetVolume(vol);
	
	
}
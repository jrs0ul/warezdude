#include "soundfx.h"


void Gabalas::open(LPDIRECTSOUND8& snd,char * name, bool use3D,bool headRelative){ 
  
  HRESULT hr;

  WAVEFORMATEX* pwfx;
   
  hmmio = mmioOpen(name,NULL, MMIO_ALLOCBUF | MMIO_READ);

   PCMWAVEFORMAT pcmwf;
   MMCKINFO ckin;
   //readmmio
   mmioDescend(hmmio,&ckriff,0,0);
   ckin.ckid = mmioFOURCC('f', 'm', 't', ' ');
   mmioDescend( hmmio, &ckin, &ckriff, MMIO_FINDCHUNK );
   mmioRead(hmmio,(HPSTR)&pcmwf,sizeof(pcmwf));
   
   pwfx = (WAVEFORMATEX*)new char[sizeof(WAVEFORMATEX)];
   memcpy(pwfx,&pcmwf,sizeof(pcmwf));
   pwfx->cbSize = 0;
   
   mmioAscend(hmmio,&ckin,0);
   //reset
   mmioSeek(hmmio,ckriff.dwDataOffset+sizeof(FOURCC), SEEK_SET);
   ck.ckid = mmioFOURCC('d','a','t','a');
   mmioDescend(hmmio,&ck,&ckriff,MMIO_FINDCHUNK);
   //
   
   DSBUFFERDESC dsbd;

   ZeroMemory(&dsbd,sizeof(DSBUFFERDESC));
   dsbd.dwSize = sizeof(DSBUFFERDESC);
   dsbd.dwBufferBytes = ck.cksize;
   dsbd.lpwfxFormat = pwfx;
   dsbd.dwFlags=DSBCAPS_CTRLVOLUME;
   if (use3D){
	dsbd.dwFlags|=DSBCAPS_CTRL3D ;

	//memcpy (&dsbd.guid3DAlgorithm,&DS3DALG_HRTF_FULL,sizeof(dsbd.guid3DAlgorithm));
    
   }



   

   if (SUCCEEDED(snd->CreateSoundBuffer(&dsbd, &sample,0))){
		ready=true;
		sample3d=0;



		if (use3D){
			hr=sample->QueryInterface(IID_IDirectSound3DBuffer,(VOID **)&sample3d);
			if (FAILED(hr))
				ready=false;

			if (headRelative)
				sample3d->SetMode(DS3DMODE_HEADRELATIVE,DS3D_IMMEDIATE);
			//sample3d->SetMaxDistance(1.0f,DS3D_IMMEDIATE);
			sample3d->SetMinDistance(64.0f,DS3D_IMMEDIATE);
			
		}
		sample->SetVolume(DSBVOLUME_MAX);
   }

   delete []pwfx;

}

//-----------------------
void Gabalas::play(bool loop){
	void* pDSlockedbuff = 0;
	DWORD dwlockedbuffsize =0;
	DWORD dwwavdataread = 0;
	DWORD status;

	if (ready){
		LPDIRECTSOUNDBUFFER& pDSB = sample;



		pDSB->GetStatus(&status);
		if (status==DSBSTATUS_BUFFERLOST)
			pDSB->Restore();



		//fillbuff
		pDSB->Lock(0,ck.cksize,&pDSlockedbuff,&dwlockedbuffsize,0,0,0L);


		//resetfile
		mmioSeek(hmmio,ckriff.dwDataOffset+sizeof(FOURCC), SEEK_SET);
		ck.ckid = mmioFOURCC('d','a','t','a');
		mmioDescend(hmmio,&ck,&ckriff,MMIO_FINDCHUNK);

		//readfile
		MMIOINFO mmioinfoIn;

		mmioGetInfo(hmmio,&mmioinfoIn,0);

		UINT cbdataIn = dwlockedbuffsize;

		if( cbdataIn > ck.cksize ) 
			cbdataIn = ck.cksize;      

		ck.cksize -= cbdataIn;

		for (DWORD cT=0; cT<cbdataIn; cT++ ){

			if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
			{
				mmioAdvance( hmmio, &mmioinfoIn, MMIO_READ );

			}

			*((BYTE*)pDSlockedbuff+cT) = *((BYTE*)mmioinfoIn.pchNext);
			mmioinfoIn.pchNext++;
		}

		dwwavdataread = cbdataIn;  





		pDSB->Unlock(pDSlockedbuff, dwlockedbuffsize,0,0);



		HRESULT hr;
		if (loop)
			hr=pDSB->Play(0,0,DSBPLAY_LOOPING);
		else
			hr=pDSB->Play(0,0,0);

	}
}
//-----------------------
void Gabalas::release(){
    
	if (hmmio){
	 mmioClose(hmmio,0);
	 hmmio=0;
	}
    

	if (sample){

		sample->Stop();
		sample->Release();
		if (sample3d)
			sample3d->Release();
	}
}


//----------------------------
void Gabalas::setSoundPos(float x, float y, float z){
  sample3d->SetPosition(x,y,z,DS3D_IMMEDIATE);
}
//----------------------------
void Gabalas::setVolume(LONG vol){
	
	sample->SetVolume(vol);
	
}
//-----------------------------
bool Gabalas::isPlaying(){
	DWORD status;
	sample->GetStatus(&status);
	if (status & DSBSTATUS_PLAYING)
		return true;
	else
		return false;

}
//------------------------
void Gabalas::duplicate(LPDIRECTSOUND8 snd,LPDIRECTSOUNDBUFFER* ref){
	snd->DuplicateSoundBuffer(sample,ref);	
}
  

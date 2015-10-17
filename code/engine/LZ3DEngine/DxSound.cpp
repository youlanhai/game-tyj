//DxSound.cpp 

#include "Commen.h"
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#pragma comment(lib,"dxguid")
#pragma comment(lib,"dsound")
#pragma comment(lib,"winmm")

#include "App.h"

#include "DxSound.h"

#include <fstream>
#include <cassert>
 

cSoundPlayer::cSoundPlayer(void) : m_lpDS(0)
{
	
}

cSoundPlayer::~cSoundPlayer(void)
{
	destroy();
}

bool cSoundPlayer::init(HWND hWnd)
{
    if (hWnd == NULL)
    {
        hWnd = getApp()->getHWnd();
    }
	if(FAILED(DirectSoundCreate8(NULL,&m_lpDS,NULL)))
	{
		return false;
	}
	if(FAILED(m_lpDS->SetCooperativeLevel(hWnd,DSSCL_NORMAL)))
	{
		return false;
	}

    m_vecDSBuffer.resize(50, NULL);
    m_vecDSCopyBuffer.reserve(50);

	return true;
}

void cSoundPlayer::releaseUnuseCopyBuffer()
{
    for(size_t i=0; i<m_vecDSCopyBuffer.size(); ++i)
    {
        if(m_vecDSCopyBuffer[i].pDSBuffer != NULL)
        {
            //判断是否正在播放
            DWORD status;
            if(FAILED(m_vecDSCopyBuffer[i].pDSBuffer->GetStatus(&status)))
            {
                continue;
            }
            if(status!=DSBSTATUS_LOOPING && status!=DSBSTATUS_PLAYING)
            {
                m_vecDSCopyBuffer[i].pDSBuffer->Release();
                m_vecDSCopyBuffer[i].pDSBuffer = NULL;
            }
        }
    }
}

int cSoundPlayer::copyBuffer(int index)
{
    if (m_vecDSBuffer.at(index) == NULL)
    {
        return -1;
    }

    releaseUnuseCopyBuffer();

    IDirectSoundBuffer8 *pBuffer8 = NULL;
	if(FAILED(m_lpDS->DuplicateSoundBuffer(m_vecDSBuffer[index],
        (IDirectSoundBuffer**)&pBuffer8)))
	{
		return -1;
    }

    size_t i;
    for(i=0; i<m_vecDSCopyBuffer.size(); ++i)
    {
        if(m_vecDSCopyBuffer[i].pDSBuffer == NULL)
        {
            break;
        }
    }
    if(i == m_vecDSCopyBuffer.size())
    {
        m_vecDSCopyBuffer.push_back(CopyDSBuffer(index,pBuffer8));
    }
    else
    {
        m_vecDSCopyBuffer[i] = CopyDSBuffer(index,pBuffer8);
    }
	return i;
}

bool cSoundPlayer::isPlaying(int index)
{
    if (index<0 || (size_t)index>=m_vecDSBuffer.size())
    {
        return false;
    }
	DWORD status;
	if(FAILED(m_vecDSBuffer[index]->GetStatus(&status)))
	{
		return false;
	}
	if(status==DSBSTATUS_LOOPING || status==DSBSTATUS_PLAYING)
	{
		return true;
	}
	return false;
}

///简单播放
bool cSoundPlayer::playSimple(LPCSTR name)
{
    return play(name, false, true, true);
}

bool cSoundPlayer::play(LPCSTR name, bool loop, bool bfromhead, bool bCopy)
{	
    IDirectSoundBuffer8* pSound = getSound(name);
    if (NULL == pSound)
    {
        return false;
    }
	DWORD dwFlags=0;
	if(loop)
	{
		dwFlags=DSBPLAY_LOOPING;
	}
	if(!isPlaying(pSound))//当前索引没有播放
	{
		if(FAILED(pSound->Play(0,0,dwFlags)))
		{
			return false;
		}
	}
	else if(bfromhead)//重头播放
	{
		stop(pSound);
		if(FAILED(pSound->Play(0,0,dwFlags)))
		{
			return false;
		}
	}
	else if(bCopy)//复制副本播放
	{
		int r = copyBuffer(pSound);
		if(-1 == r)
		{
			return false;
		}
        else
        {
            if(FAILED(m_vecDSCopyBuffer[r].pDSBuffer->Play(0,0,0)))
            {
                return false;
            }
        }
	}
	return true;
}

bool cSoundPlayer::setVolum(int index,long volume)
{
    if (index<0 || (size_t)index>=m_vecDSBuffer.size())
    {
        return false;
    }
    if(NULL == m_vecDSBuffer[index])
    {
        return false;
    }
	if(volume<0)
	{
		volume=0;
	}
	if(volume>100)
	{
		volume=100;
	}
	if(FAILED(m_vecDSBuffer[index]->SetVolume(-10*(100-volume))))
	{
		return false;
	}
	return true;
}


bool cSoundPlayer::setFrequncy(int index,long fre)
{
    if (index<0 || (size_t)index>=m_vecDSBuffer.size())
    {
        return false;
    }
    if(NULL == m_vecDSBuffer[index])
    {
        return false;
    }
	if(FAILED(m_vecDSBuffer[index]->SetFrequency(fre)))
	{
		return false;
	}
	return true;
}


bool cSoundPlayer::setPan(int index,long lpan)
{
    if (index<0 || (size_t)index>=m_vecDSBuffer.size())
    {
        return false;
    }
    if(NULL == m_vecDSBuffer[index])
    {
        return false;
    }
	if(FAILED(m_vecDSBuffer[index]->SetPan(lpan)))
	{
		return false;
	}
	return true;
}


bool cSoundPlayer::stop(int index)
{
    if (index<0 || (size_t)index>=m_vecDSBuffer.size())
    {
        return false;
    }
    if(NULL == m_vecDSBuffer[index])
    {
        return false;
    }
	if(FAILED(m_vecDSBuffer[index]->Stop()))
	{
		return false;
	}
	return true;
}


void cSoundPlayer::stopAll()
{
	for(size_t i=0;i<m_vecDSBuffer.size();i++)
	{
		stop(i);
	}
    for(size_t i=0;i<m_vecDSCopyBuffer.size();i++)
    {
        if(m_vecDSCopyBuffer[i].pDSBuffer != NULL)
        {
            m_vecDSCopyBuffer[i].pDSBuffer->Stop();
        }
    }
}


void cSoundPlayer::deleteSound(int index)
{
    if (index<0 || (size_t)index>=m_vecDSBuffer.size())
    {
        return ;//false;
    }
    if(NULL != m_vecDSBuffer[index])
    {
        m_vecDSBuffer[index]->Release();
        m_vecDSBuffer[index] = NULL;
	    for(size_t i=0;i<m_vecDSCopyBuffer.size();i++)
	    {
		    if(m_vecDSCopyBuffer[i].copyIndex==index &&
                m_vecDSCopyBuffer[i].pDSBuffer!=NULL)
		    {
                m_vecDSCopyBuffer[i].pDSBuffer->Release();
			    m_vecDSCopyBuffer[i].pDSBuffer=NULL;
		    }
        }
    }
}


void cSoundPlayer::deleteAllSound()
{
	for(size_t i=0; i<m_vecDSBuffer.size(); ++i)
	{
		deleteSound(i);
	}
}


void cSoundPlayer::destroy()
{
    stopAll();
    deleteAllSound();
	if(m_lpDS != NULL)
	{
		m_lpDS->Release();
	}
	m_lpDS=NULL;
}

/*从给定文件中加载wav文件。*/
bool cSoundPlayer::preLoadFromFile(char *szFileName)
{
    assert(m_lpDS!=NULL && szFileName!=NULL && "wav设备未初始化");

    std::ifstream fin(szFileName);
    if (!fin.good())
    {
        return false;
    }
    int count;
    fin>>count;
    char szBuffer[256];

    for (int i=0; i<count; ++i)
    {
        fin>>szBuffer;
        if(!loadWave(i,szBuffer))
        {
            WRITEX_LOG("加载wav文件[%s]失败！", szBuffer);
        }
    }

    fin.close();

    return true;
}

IDirectSoundBuffer8* cSoundPlayer::getSound(LPCSTR szName)
{
    SoundIter it = m_sounds.find(szName);
    if (it != m_sounds.end())
    {
        return it->second;
    }
    IDirectSoundBuffer8* buffer = loadWave(szName);
    m_sounds[szName] = buffer;
    return buffer;
}

IDirectSoundBuffer8* cSoundPlayer::loadWave(LPCSTR szName)
{
	HMMIO hwave;
	MMCKINFO parent,child;
	WAVEFORMATEX wmfx;

	parent.ckid=(FOURCC)0;
	parent.cksize=0;
	parent.dwDataOffset=0;
	parent.fccType=(FOURCC)0;
	parent.dwFlags=0;
	child = parent;
	if((hwave=mmioOpenA(szName,NULL,MMIO_READ|MMIO_ALLOCBUF))==NULL)
	{
		return NULL;
	}
	parent.fccType=mmioFOURCC('W','A','V','E');
	if(mmioDescend(hwave,&parent,NULL,MMIO_FINDRIFF))
	{
		mmioClose(hwave,0);
		return NULL;
	}
	child.ckid=mmioFOURCC('f','m','m',' ');
	if(mmioDescend(hwave,&child,&parent,0))
	{
		mmioClose(hwave,0);
		return NULL;
	}
	if(mmioRead(hwave,(char*)&wmfx,sizeof(wmfx))!=sizeof(wmfx))
	{
		mmioClose(hwave,0);
		return NULL;
	}
	if(wmfx.wFormatTag!=WAVE_FORMAT_PCM)
	{
		mmioClose(hwave,0);
		return NULL;
	}
	if(mmioAscend(hwave,&child,0))
	{
		mmioClose(hwave,0);
		return NULL;
	}
	child.ckid=mmioFOURCC('d','a','t','a');
	if(mmioDescend(hwave,&child,&parent,MMIO_FINDCHUNK))
	{
		mmioClose(hwave,0);
		return NULL;
	}
	UCHAR *snd_buffer=new UCHAR[sizeof(UCHAR)*child.cksize];
	if(mmioRead(hwave,(char*)snd_buffer,child.cksize)==-1)
	{
		mmioClose(hwave,0);
		return NULL;
	}
	mmioClose(hwave,0);

	//创建缓冲区	
	DWORD dwBufferSize=1000000;
	DSBUFFERDESC dsbd;
	memset(&dsbd,0,sizeof(dsbd));
	dsbd.dwSize=sizeof(dsbd);
	dsbd.dwFlags=DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME
		|DSBCAPS_STATIC|DSBCAPS_LOCSOFTWARE;
	if(dwBufferSize>child.cksize)
		dwBufferSize=child.cksize;
	dsbd.dwBufferBytes=dwBufferSize;
	dsbd.lpwfxFormat=&wmfx;

    IDirectSoundBuffer8 *pBuffer8 = NULL;
	if(FAILED(m_lpDS->CreateSoundBuffer(&dsbd,
        (LPDIRECTSOUNDBUFFER*)&pBuffer8,NULL)))
	{
		return NULL;
	}

	//锁定缓冲区
	UCHAR *audio_ptr1,*audio_ptr2;
	DWORD audio_len1,audio_len2;
	if(FAILED(pBuffer8->Lock(0,child.cksize,(void**)&audio_ptr1,&audio_len1,
		(void**)&audio_ptr2,&audio_len2,DSBLOCK_ENTIREBUFFER)))
	{
		return NULL;
	}
	//向缓冲区写入数据
	memcpy(audio_ptr1,snd_buffer,audio_len1);
	memcpy(audio_ptr2,snd_buffer+audio_len1,audio_len2);
	//解除锁定
	pBuffer8->Unlock(audio_ptr1,audio_len1,audio_ptr2,audio_len2);

	delete [] snd_buffer;

    return pBuffer8;
}
//////////////////////////////////////////
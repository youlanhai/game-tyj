#pragma once

#include <dsound.h>

#include "Base.h"

/*音效类。*/
class LZDLL_API cSoundPlayer
{
public:

    typedef IDirectSoundBuffer8*  LPDSBuffer;
    typedef std::map<std::string, LPDSBuffer> SoundMap;
    typedef SoundMap::iterator SoundIter;

    struct CopyDSBuffer
    {
        std::string name;  //复制来源的索引
        LPDSBuffer  pDSBuffer;
        CopyDSBuffer(void) : pDSBuffer(0)  {  }
        CopyDSBuffer(LPCSTR name_, LPDSBuffer p)
            : name(name_), pDSBuffer(p) 
        {
        }
    };

	cSoundPlayer(void);
	~cSoundPlayer(void);

    /*初始化音效。*/
	bool init(HWND hWnd=NULL);

    /*从给定文件中加载wav文件。*/
    bool preLoadFromFile(char *szFileName);

    ///简单播放
    bool playSimple(LPCSTR name);

    /*播放。*/
	bool play(int index,bool loop=false,bool bfromhead=false,bool bCopy=true);

    /*是否正在播放。*/
	bool isPlaying(int index);

    /*设置频率。*/
	bool setFrequncy(int index,long fre);

    /*设置平衡。*/
	bool setPan(int index,long lpan);

    /*设置音量。*/
	bool setVolum(int index,long volume);

    /*停止播放。*/
	bool stop(int index);

    /*停止所有播放。*/
	void stopAll();

    /*释放资源。*/
	void deleteSound(int index);

protected:
    /*加载wav文件。*/
    LPDSBuffer loadWave(LPCSTR szName);

    LPDSBuffer getSound(LPCSTR szName);

    void deleteAllSound();

    void destroy();

    int copyBuffer(int index);

    void releaseUnuseCopyBuffer();

private:
    IDirectSound8               *m_lpDS;
    SoundMap                    m_sounds;
    std::vector<CopyDSBuffer>   m_vecDSCopyBuffer;  //拷贝缓冲区
};
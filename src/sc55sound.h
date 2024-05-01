#ifndef __SC55SOUND_H__
#define __SC55SOUND_H__

#ifdef __cplusplus
extern "C"
{
#endif


int WINAPI
SC55Sound_Open(int sample_rate);

int WINAPI
SC55Sound_Close(void);

int WINAPI
SC55Sound_Write(unsigned char data);

int WINAPI
SC55Sound_Mix(short *buff, unsigned long samples);


#ifdef __cplusplus
}
#endif

#endif /* __SC55SOUND_H__ */

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include "mcu.h"
#include "sc55sound.h"

#define INTERP_SHIFT	(14)
#define INTERP_SIZE		(1 << INTERP_SHIFT)

static short interp_prev_sample[2];
static short interp_cur_sample[2];
static unsigned int interp_pos;
static unsigned int interp_ratio;


int WINAPI
SC55Sound_Open(int sample_rate)
{
	int ret = -1;

	ret = SC55_Open();

	::ZeroMemory(interp_prev_sample, sizeof(interp_prev_sample));
	::ZeroMemory(interp_cur_sample, sizeof(interp_cur_sample));
	interp_pos = INTERP_SIZE;
	interp_ratio = SC55_SampleFreq() * INTERP_SIZE / sample_rate;

	for (int i = 0; i < SC55_SampleFreq() * 3; i++)
	{
		SC55_Update(interp_prev_sample);
	}

	return ret;
}


int WINAPI
SC55Sound_Close(void)
{
	int ret = -1;

	SC55_Close();

	ret = 0;

	return ret;
}


int WINAPI
SC55Sound_Write(unsigned char data)
{
	int ret = -1;

	SC55_Write(data);

	ret = 0;

	return ret;
}


int WINAPI
SC55Sound_Mix(short *buff, unsigned long samples)
{
	int ret = -1;

	while (samples--)
	{
		while (interp_pos >= INTERP_SIZE)
		{
			interp_prev_sample[0] = interp_cur_sample[0];
			interp_prev_sample[1] = interp_cur_sample[1];
			SC55_Update(interp_cur_sample);
			interp_pos -= INTERP_SIZE;
		}
		interp_pos += interp_ratio;
#if 1
		*buff++ = ((int)interp_prev_sample[0] * (INTERP_SIZE - interp_pos)
				   + (int)interp_cur_sample[0] * (interp_pos)) / INTERP_SIZE;
		*buff++ = ((int)interp_prev_sample[1] * (INTERP_SIZE - interp_pos)
				   + (int)interp_cur_sample[1] * (interp_pos)) / INTERP_SIZE;
#else
		*buff++ = interp_cur_sample[0];
		*buff++ = interp_cur_sample[1];
#endif
	}

	ret = 0;

	return ret;
}


extern "C"
BOOL APIENTRY
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
#ifdef DEBUG_OUTPUT_LOG
		{
			static char buff[1024];
			::OutputDebugString("sc55sound.dll attached\n");
			::snprintf(buff, sizeof(buff), "Directory: %s\n", moduledir);
			::OutputDebugString(buff);
		}
#endif /* DEBUG_OUTPUT_LOG */
		break;

	case DLL_PROCESS_DETACH:
#ifdef DEBUG_OUTPUT_LOG
		{
			::OutputDebugString("mt32sound.dll detached\n");
		}
#endif /* DEBUG_OUTPUT_LOG */
		break;
	}

	return TRUE;
}

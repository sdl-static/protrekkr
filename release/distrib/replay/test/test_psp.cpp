/*
   How to use:

   1. Copy the .ptp and the Ptk_Properties.h file into the "Replay" directory.
   2. Edit the module.asm file for visual C or module.s for MingW
      and replace the ../YOUR_MODULE.PTP by the filename of your module file.
   3. Compile the replay routine (which will be fine tuned for your module).
   4. Compile this test example.

   Note: LATENCY should be increased if the cpu is extensively used for other purposes
         to avoid lags and stuttering.
*/

#include <pspkernel.h>
#include <pspctrl.h>
#include <psppower.h>

#include "../lib/include/ptkreplay.h"

PSP_MODULE_INFO("", PSP_MODULE_KERNEL, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU);

#include <stdio.h>

extern "C"
{
    extern unsigned int _PTK_MODULE;
    void _start(void);
    int sceKernelPowerTick(int type);
}

#define DISPLAY_FPS

#ifdef DISPLAY_FPS
    #include <psprtc.h>
    float curr_ms = 1.0f;
    float curr_fps;
    double time_span;
    u32 tick_res;
    u64 last_tick;
    u64 curr_tick;
#endif
extern int AUDIO_Samples;

// Initialize with 500 milliseconds of latency
// (the PSP needs much more time than a PC)
#define LATENCY 300

void ptk_start(void)
{
    SceCtrlData Ctrl_Buf;

	pspDebugScreenInit();
    pspDebugScreenSetOffset((int) 0);

    if(Ptk_InitDriver(LATENCY))
    {
        // Load it
        if(Ptk_InitModule((unsigned char *) &_PTK_MODULE, 0))
        {
            // Start playing it
            Ptk_Play();
        
#ifdef DISPLAY_FPS
    sceRtcGetCurrentTick(&last_tick);
    tick_res = sceRtcGetTickResolution();
#endif
            // Quit with home button
            while(1)
            {

#ifdef DISPLAY_FPS
                sceRtcGetCurrentTick(&curr_tick);
                curr_fps = 1.0f / curr_ms;
                pspDebugScreenSetXY(1, 32);
                pspDebugScreenPrintf("%d.%03d", (int) curr_fps, (int) ((curr_fps - (int) curr_fps) * 1000.0f));
                time_span = ((curr_tick - last_tick)) / (float) tick_res;
                curr_ms = time_span;
                sceRtcGetCurrentTick(&last_tick);
#endif
                sceCtrlPeekBufferPositive(&Ctrl_Buf, 1);
                if(Ctrl_Buf.Buttons & PSP_CTRL_HOME) break;

                sceKernelPowerTick(0);
            }
            Ptk_Stop();
        }
        Ptk_ReleaseDriver();
    }
    sceKernelExitGame();
}

void (*m)(void);
void _start(void)
{
    // Give us some boost
    scePowerSetClockFrequency(333, 333, 166);
    // Enable access to all the coprocessors and also obtain access to the media engine
    asm(
        "mfc0 $v0, $12\n"
        "or $v0, 0b11110000000000000000000000000000\n"
        "and $v0, 0b11111111111111111111111111100111\n"
        "mtc0 $v0, $12\n"
    );
    m = (void (*)()) ((u32) ptk_start | 0x80000000);
    m();
}
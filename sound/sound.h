#ifndef SOUND_H
#define SOUND_H

#include "wbuf.h"
#include "spi.h"

void pwm_init();
void dgen_init(); 
void play_music(void);
void stop_playing(void);

#endif //SOUND_H
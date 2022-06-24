/***********************************************************************
*	sound function to simplify the use of the openAL funcs
*	TouchHapsys Project
*	date : 17 July 2003
*	Author : LSC - Guillaume Bouyer & Vincent Maury
************************************************************************/


#ifndef Soundh
#define Soundh

#include <string.h>
#include <stdio.h>
#include <AL/ALut.h>
#include <AL/al.h>
#include <AL/alc.h>

#include <windows.h>
#include <AL/alut.h>
#include <AL/eax.h>



void alSettingUpOpenAL();
void alSettingASource(ALuint &inSoundBuffer,ALuint &inSoundSource, char *inNameOfFileContainingSound);

void alSetSourcePosition(ALuint &inSoundSource, float xPos, float yPos, float zPos);
void alSetSourceDirection(ALuint &inSoundSource, float xPos, float yPos, float zPos);
void alSetSourceVelocity(ALuint &inTriangleSoundSource, float xPos, float yPos, float zPos);
void alSetListenerPosition(float xPos, float yPos, float zPos);
void alSetListenerOrientation(float inAngle, float xPos, float yPos, float zPos);

char *chooseWav(int number);
long chooseEnv(int number);

#endif
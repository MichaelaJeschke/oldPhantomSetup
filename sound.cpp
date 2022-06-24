/***********************************************************************
*	sound function to simplify the use of the openAL funcs
*	TouchHapsys Project
*	date : 17 July 2003
*	Author : LSC - Guillaume Bouyer & Vincent Maury
************************************************************************/


#include "sound.h"



void alSettingUpOpenAL()
{
	alutInit (NULL, 0); 	// init OpenAL
	alGetError(); 	//checking for errors
}


void alSettingASource(ALuint &inSoundBuffer,ALuint &inSoundSource, char *inNameOfFileContainingSound)
{
	alGenBuffers(1, &inSoundBuffer);
	alGenSources(1, &inSoundSource);

	ALboolean bLooping;
	ALsizei size, freq;
	ALenum format;
	ALvoid *data;
   

	alutLoadWAVFile(inNameOfFileContainingSound, &format, &data, &size, &freq, &bLooping);
	alBufferData (inSoundBuffer, format, data, size, freq);

	alSourcei(inSoundSource, AL_BUFFER, inSoundBuffer); //attach the buffer to the sound source
}


void alSetSourcePosition(ALuint &inSoundSource, float xPos, float yPos, float zPos)
{
	alSource3f(inSoundSource,AL_POSITION,xPos,yPos,zPos);
}


void alSetSourceDirection(ALuint &inSoundSource, float xPos, float yPos, float zPos)
{
	alSource3f(inSoundSource,AL_DIRECTION,xPos,yPos,zPos);
}


void alSetSourceVelocity(ALuint &inTriangleSoundSource, float xPos, float yPos, float zPos)
{
	alSource3f(inTriangleSoundSource,AL_VELOCITY,xPos,yPos,zPos);
}


void alSetListenerPosition(float xPos, float yPos, float zPos)
{
	alListener3f(AL_POSITION, xPos,yPos,zPos); //Set the position from where you hear the sound
}


void alSetListenerOrientation(float inAngle, float xPos, float yPos, float zPos)
{
	float Orientation[] = {inAngle, xPos,yPos,zPos};
	alListenerfv(AL_ORIENTATION, Orientation);	//set the orientation from where your are listening
}



long chooseEnv(int number)
{
	long Val;

	switch (number)
	{
		case 1 : Val = EAX_ENVIRONMENT_GENERIC; break;
		case 2 : Val = EAX_ENVIRONMENT_PADDEDCELL; break;
		case 3 : Val = EAX_ENVIRONMENT_ROOM; break;
		case 4 : Val = EAX_ENVIRONMENT_BATHROOM; break;
		case 5 : Val = EAX_ENVIRONMENT_LIVINGROOM; break;
		case 6 : Val = EAX_ENVIRONMENT_STONEROOM; break;
		case 7 : Val = EAX_ENVIRONMENT_AUDITORIUM; break;
		case 8 : Val = EAX_ENVIRONMENT_CONCERTHALL; break;
		case 9 : Val = EAX_ENVIRONMENT_CAVE; break;
		case 10 : Val = EAX_ENVIRONMENT_ARENA; break;
		case 11 : Val = EAX_ENVIRONMENT_HANGAR; break;
		case 12 : Val = EAX_ENVIRONMENT_CARPETEDHALLWAY; break;
		case 13 : Val = EAX_ENVIRONMENT_HALLWAY; break;
		case 14 : Val = EAX_ENVIRONMENT_STONECORRIDOR; break;
		case 15 : Val = EAX_ENVIRONMENT_ALLEY; break;
		case 16 : Val = EAX_ENVIRONMENT_FOREST; break;
		case 17 : Val = EAX_ENVIRONMENT_CITY; break;
		case 18 : Val = EAX_ENVIRONMENT_MOUNTAINS; break;
		case 19 : Val = EAX_ENVIRONMENT_QUARRY; break;
		case 20 : Val = EAX_ENVIRONMENT_PLAIN; break;
		case 21 : Val = EAX_ENVIRONMENT_PARKINGLOT; break;
		case 22 : Val = EAX_ENVIRONMENT_SEWERPIPE; break;
		case 23 : Val = EAX_ENVIRONMENT_UNDERWATER; break;
		case 24 : Val = EAX_ENVIRONMENT_DRUGGED; break;
		case 25 : Val = EAX_ENVIRONMENT_DIZZY; break;
		case 26 : Val = EAX_ENVIRONMENT_PSYCHOTIC; break;
	}

	return Val;
}

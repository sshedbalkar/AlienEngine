#include"Audio.h"
#include "Core.h"
#include "Message.h"
#include<iostream>
//
namespace Framework
{
	Audio* AUDIO=NULL;

	Sound::Sound()
	{
		sound=NULL;
		channel=NULL;
		volume=1.0f;
	}

	Sound::~Sound()
	{
		if(sound!=NULL)
		{
			FMOD_Sound_Release(sound);
			sound=NULL;
		}
	}

	Audio::Audio():
		_paused(false)
	{
		system = NULL;
		AUDIO = this;
	}

	Audio::~Audio()
	{
		//release all sounds
		for(Iterator it= sounds.begin();it != sounds.end(); ++it)
		{delete(*it);					//#### added temporary line
		(*it)= NULL;}
		FMOD_System_Release(system);
	}

	void Audio::Update(float timeslice)
	{
		static_cast<void>(timeslice);
		{
			FMOD_System_Update(system);
		}
	}

	void Audio::Initialize()
	{
		std::cout<<"Loading Sounds..."<<std::endl;
		if(FMOD_System_Create(&system) != FMOD_OK)
			return;
		if (FMOD_System_Init(system,100,FMOD_INIT_NORMAL,NULL) != FMOD_OK)
			return;

		Load("Sounds/button.wav","button",false,0.4f);
		Load("Sounds/main.mp3","main",true,0.1f);
		/*Load("Sounds/maintrack.mp3","background",true,0.3f);
		Load("Sounds/20 - The Four Seasons, Summer, Concerto in G Minor Op 8 N2, RV 315.mp3","boss_fight_music",true,0.01f);
		Load("Sounds/teleport.wav","teleport",false,0.2f);
		Load("Sounds/laser.wav","laser",true,0.01f);
		Load("Sounds/Footstep_Boss.wav","bosswalk",true,0.4f);
		Load("Sounds/water.wav","water",false);
		Load("Sounds/death1.wav","death1",false,0.6f);
		Load("Sounds/death2.wav","death2",false,0.6f);
		
		Load("Sounds/door_deactivate.wav","door_deactivate",false,0.3f);
		Load("Sounds/door_activate.wav","door_activate",false,0.3f);
		Load("Sounds/boss_teleporting_hero.wav","boss_teleporting_hero",false,0.5f);
		Load("Sounds/danger.wav","danger",false,0.2f);
		Load("Sounds/beep.wav","beep",true,0.5f);
		Play("background");*/
		//boss_teleporting_hero
		return;
	}

	Sound* Audio::Load(string filename)
	{
		if(filename.length()==0) return false;

		Sound *sound= new Sound();
		FMOD_RESULT result;
		result= FMOD_System_CreateSound(system,				//fmod system
										filename.c_str(),   //filename
										FMOD_DEFAULT,		//default audio
										NULL,				//i dont know
										&sound->sound);		//pointer to the sound
		if(result != FMOD_OK) sound=NULL;
		return sound;
	}

	bool Audio::Load(string filename,string name,bool loop,float volume)
	{
		if (filename.length() == 0 || name.length() == 0) return false;
	
		Sound *sound= new Sound();
		sound->setName(name);
		sound->setVolume(volume);
		FMOD_RESULT result;
		if(loop)
		result= FMOD_System_CreateSound(system,				//fmod system
										filename.c_str(),   //filename
										FMOD_DEFAULT |		//default audio
										FMOD_LOOP_NORMAL,   //loop normal
										NULL,				//i dont know
										&sound->sound);		//pointer to the sound
		else
		result= FMOD_System_CreateSound(system,				//fmod system
										filename.c_str(),   //filename
										FMOD_DEFAULT,		//default audio
										NULL,				//i dont know
										&sound->sound);		//pointer to the sound
		
		if(result!=FMOD_OK)
		{
			delete sound;
			return false;
		}
		sounds.push_back(sound);
		return true;
	}

	bool Audio::isPlaying(string name)
	{
		Sound *sound=findSound(name);
		if(sound==NULL) return false;

		int index=0;
		FMOD_Channel_GetIndex(sound->channel, &index);
		return(index>0); //index=0 if not playing index=99 if playing
	}

	bool Audio::soundExists(string name)
	{
		for(Iterator it= sounds.begin();it != sounds.end(); ++it)
		{
			if((*it)->getname()==name) return true;
		}
		return false;
	}

	Sound* Audio::findSound(string name)
	{
		Sound *sound=NULL;
		for(Iterator it= sounds.begin();it != sounds.end(); ++it)
		{
			if((*it)->getname()==name.c_str()) 
			{
				sound=(*it);
				break;
			}
		}
		return sound;
	}

	bool Audio::Play(string name,int loopCount)
	{
		if(isPlaying(name)) 
			return true;
		FMOD_RESULT result;
		Sound *sound=findSound(name);
		if(sound->sound!=NULL)//check if the sound is present
		{
			result=FMOD_System_PlaySound(system,
										 FMOD_CHANNEL_FREE,
										 sound->sound,
										 true,
										 &sound->channel);
			if(result!=FMOD_OK) return false;
			FMOD_Channel_SetLoopCount(sound->channel,loopCount);
			FMOD_Channel_SetPaused(sound->channel,false);
			FMOD_Channel_SetVolume(sound->channel,sound->getVolume());
		}
		return true;
	}

	bool Audio::Play(Sound *sound, int loopCount)
	{
		FMOD_RESULT result;
		if(sound==NULL || sound->sound==NULL) return false;
		result=FMOD_System_PlaySound(system,
									 FMOD_CHANNEL_FREE,
									 sound->sound,
									 true,
									 &sound->channel);
		if(result!=FMOD_OK) return false;
		FMOD_Channel_SetLoopCount(sound->channel,loopCount);
		FMOD_Channel_SetPaused(sound->channel,false);
		return true;
	}

	void Audio::Stop(string name)
	{
		if(!isPlaying(name)) return;
		Sound *sound=findSound(name);
		if(sound==NULL) return;
		FMOD_Channel_Stop(sound->channel);
	}

	void Audio::stopAll()
	{
		for(Iterator it= sounds.begin();it != sounds.end(); ++it)
			FMOD_Channel_Stop((*it)->channel);
	}

	void Audio::stopAllExcept(string name)
	{
		for(Iterator it= sounds.begin();it != sounds.end(); ++it)
		{
			if((*it)->getname()!= name)
				FMOD_Channel_Stop((*it)->channel);
		}
	}

	void Audio::changeVolume(float volume)
	{
		//for(Iterator it= sounds.begin();it != sounds.end(); ++it)
			//FMOD_Channel_SetVolume((*it)->channel,volume);


		FMOD_CHANNELGROUP *maingrp = NULL;
		FMOD_System_GetMasterChannelGroup(system, &maingrp);
		if(maingrp==NULL) return;
		FMOD_ChannelGroup_SetVolume(maingrp,(float)max(0,min(volume,1)));
	}

	void Audio::changeVolume(string name,float volume)
	{
		if(!isPlaying(name)) return;
		Sound *sound=findSound(name);
		if(sound==NULL) return;
		FMOD_Channel_SetVolume(sound->channel,volume);
		sound->setVolume(volume);
	}

	void Audio::Pause(string name)
	{
		if(isPlaying(name))
			FMOD_Channel_SetPaused(findSound(name)->channel,true);
	}

	void Audio::resume(string name)
	{
		FMOD_Channel_SetPaused(findSound(name)->channel,false);
	}

	void Audio::pauseAll()
	{
		for(Iterator it= sounds.begin();it != sounds.end(); ++it)
		{
			FMOD_Channel_SetPaused((*it)->channel,true);
		}
	}

	void Audio::resumeAll()
	{
		for(Iterator it= sounds.begin();it != sounds.end(); ++it)
		{
			FMOD_Channel_SetPaused((*it)->channel,false);
		}
	}

	void Audio::reverse(string name)
	{
		//FMOD_Channel_SetPaused(findSound(name)->channel,false);
		float frequency;
		FMOD_Channel_GetFrequency(findSound(name)->channel,&frequency);

		FMOD_Channel_SetFrequency(findSound(name)->channel,-frequency*2);
	}

	void Audio::resetFrequency(string name)
	{
		//FMOD_Channel_SetPaused(findSound(name)->channel,false);
		//float frequency;
		
		FMOD_Channel_SetFrequency(findSound(name)->channel,44100);
	}
	//
	void Audio::SendMessage(Message *message)
	{
		if (message->MessageId == Mid::SystemPause)
		{
			const SystemPause *m = static_cast<const SystemPause*>(message);
			_paused = m->Pause;
		}
	}
}

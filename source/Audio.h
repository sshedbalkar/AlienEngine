#pragma once //Makes sure this header is only included once

#include "fmod/fmod.hpp"
#include <string>
#include <vector>
#include "Engine.h"
#include <Windows.h>
//#include "WindowsIncludes.h"
//#include "Message.h"
//
namespace Framework
{
	using namespace std;
	class Message;
	//
	class Sound
	{
	private:
		string name;
		float volume;
	public:
		Sound();
		~Sound();

		std::string getname() {return name;}
		void setName( string Name ){ name=Name; }
		void setVolume(float Volume){ volume=Volume; };
		float getVolume(){ return volume; }

		FMOD_SOUND *sound;
		FMOD_CHANNEL *channel;
	};

	class Audio : public ISystem
	{
	private:
		FMOD_SYSTEM *system;
		typedef std::vector<Sound*> Sounds;
		typedef std::vector<Sound*>::iterator Iterator;
		Sounds sounds;
		bool _paused;
		//
	public:
		Audio();
		~Audio();
		FMOD_SYSTEM* getSystem() { return system; }

		void Initialize();
		void Update(float timeslice);
		virtual std::string GetName(){return "Audio";}
		
		bool serialize();

		bool Load(string filename,string name,bool loop=false,float volume=1.0);
		Sound* Load(string filename);

		bool isPlaying(string name);
		bool soundExists(string name);

		Sound* findSound(string name);
	
		//loopcount=-1 for not loloping
		bool Play(Sound *sound, int loopCount=-1);
		bool Play(string name, int loopCount=-1);

		void Stop(string name);
		void stopAll();
		void stopAllExcept(string name);

		void Pause(string name);
		void pauseAll();

		void resume(string name);
		void resumeAll();

		void reverse(string name);
		void resetFrequency(string name);

		//volume 0 to 1
		void changeVolume(float volume);
		void changeVolume(string name,float volume);
		//
		virtual void SendMessage(Message *message);
	};

	extern Audio* AUDIO;
}


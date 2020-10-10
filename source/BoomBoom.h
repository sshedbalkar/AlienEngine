#pragma once //Makes sure this header is only included once
#include "PhysicsMath.h"
#include <vector>

namespace Framework
{
	typedef struct 
	{
		Vector3 pos;
		real distance;
		real force;
	}Bomb;

	class BoomBoom
	{
		//friend class GameLogic;
	public:
		BoomBoom(){}
		~BoomBoom(){}
		void Initialize();
		void Update();		
		void AddBomb(Bomb);		
	private:
		typedef std::vector<Bomb> Bombs;
		Bombs bomb_list;
	};
}
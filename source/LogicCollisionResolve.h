#pragma once //Makes sure this header is only included once
#include "Message.h"

namespace Framework
{
	class LogicCollisionResolve
	{
		//friend class RigidBody;
	public:
		LogicCollisionResolve();
		~LogicCollisionResolve(){}
		void Initialize();
		void Update(float &);	
	    void SendMessage(Message *);
	private:
		
		
	};
}
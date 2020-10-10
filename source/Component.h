///////////////////////////////////////////////////////////////////////////////////////
///
///	\file Component.h Defines GameComponent
///
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once //Makes sure this header is only included once

#include "Message.h"
#include "Serialization.h"
#include "ComponentTypeIds.h"

namespace Framework
{
	//Forward declaration of GOC class
	class GameObjectComposition;
	typedef GameObjectComposition GOC;
	
	///A component is an atomic piece of functionality that is
	///composed onto a GameObjectComposition to form game objects.
	class GameComponent 
	{
	public:
		friend class GameObjectComposition;

		///Signal that the component is now active in the game world.
		///See GameObjectComposition for details.
		virtual void Initialize(){};

		///GameComponents receive all messages send to their owning composition. 
		///See Message.h for details.
		virtual void SendMessage(Message *){};

		///Component Serialization Interface see Serialization.h for details.
		virtual void Serialize(ISerializer& str){static_cast<void>(str);}
		virtual bool Deserialize(ISerializer& str){ return true; }
		virtual void RemoveComponentFromGame(){};
		virtual void RestoreComponentToGame(){};
		virtual void Load(){};
		virtual void Unload(){};/// for restarting level
		virtual void Free(){};/// un-initialize

		///Get the GameObjectComposition this component is owned/composed.
		GOC * GetOwner(){ return Base; }

		ComponentTypeId TypeId;
		const char* GetName()
		{
			return ComponentIdText[ TypeId ];
		}
		static int GetIdFromName( char* name )
		{
			for( int i = 1; i < CT_MAX; ++i )
			{
				if( !std::strcmp(name, ComponentIdText[i]) )
				{
					return i;
				}
			}
			//
			return CT_INVALID;
		}
	protected:
		///Destroy the component.
		virtual ~GameComponent(){};
	private:
		///Each component has a pointer back to the base owning composition.
		GOC * Base;
	};

}

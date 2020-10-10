///////////////////////////////////////////////////////////////////////////////
///
///	\file Factory.h
///	Defines the GameObjectFactory system.
///	
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "System.h"
#include "Composition.h"
#include "VMath.h"

namespace Framework
{
	///Forward Declaration of Component Creator
	class ComponentCreator;
	class ISerializer;

	///Game Object Factory The game object factory creates composition objects 
	///from data streams and manages their lifetimes. As part of controlling the 
	///lifetimes of the GOC it also provides an integer based Id system for safe 
	///referencing of game objects through integer Id Handles.
	typedef std::map<unsigned,GOC*> GameObjectIdMapType;
	
	class GameObjectFactory : public ISystem
	{
		
	public:
		GameObjectFactory();
		~GameObjectFactory();


		///Create initialize and Id a GOC from the data file.
		//GOC * Create(const std::string& filename);
		GOC* Create(const ISerializer& stream);

		///Add a GOC to the destroy list for delayed destruction.
		void Destroy(GOC * gameObject);

		///Update the factory, destroying dead objects.
		virtual void Update(float dt);

		///Name of the system is factory.
		virtual std::string GetName(){return "Factory";}

		///Message Interface see Message.h
		virtual void SendMessage(Message * message);

		///Destroy all the GOCs in the world. Used for final shutdown.
		void DestroyAllObjects();

		///Create and Id a GOC at runtime. Used to dynamically build GOC.
		///After components have been added call GOC->Initialize().
		GOC * CreateEmptyComposition();

		///Build a composition and serialize from the data file but do not initialize the GOC.
		///Used to create a composition and then adjust its data before initialization
		///see GameObjectComposition::Initialize for details.
		//GOC * BuildAndSerialize(const std::string& filename);
		GOC * BuildAndSerialize(const ISerializer& stream);
		GOC* BuildFromArchetype(const char* archId);
		///Id object and store it in the object map.
		void IdGameObject(GOC* gameObject);

		///Add a component creator enabling data driven composition
		void AddComponentCreator(const std::string& name, ComponentCreator* creator);

		///Get the game object with given id. This function will return NULL if
		///the object has been destroyed.
		GOC * GetObjectWithId(GOCId id);
		const GameObjectIdMapType & GetAllObjects() const;
		std::vector<GOC *> getVectorOfGOCOfSpecificType(int type); 
		GOC * getGOCofSpecificType(int type); //returns the first it finds or NULL if not found, useful if we know there is only one instance of this GOC like the hero
		bool Deserialize( ISerializer* stream );
		int MoveToRemovedList( GOC* gameObject );
		GOC* GetRemovedGOCById( GOCId id );
		int RestoreRemovedGOC( GOC* gameObject );
	private:
		///Used to incrementally generate unique id's.
		unsigned LastGameObjectId;

		///Map of component creator used for data driven composition
		typedef std::map<std::string, ComponentCreator*> ComponentMapType;
		ComponentMapType ComponentMap;

		///Map of GOC to their Ids used for safe referencing of game objects
		
		GameObjectIdMapType GameObjectIdMap;

		///Objects to be deleted
		///this is a set to prevent problems when a game object
		///is deleted in multiple locations
		typedef std::set< GOC* > GOCSet;
		GOCSet ObjectsToBeDeleted;
		GOCSet ObjectsToBeRemoved;
		GOCSet m_removedList;
		bool _paused;
	};

	extern GameObjectFactory * FACTORY;
}

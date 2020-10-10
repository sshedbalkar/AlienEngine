#pragma once //Makes sure this header is only included once
#include "Physics.h"
#include "Variant.h"
#include "Transform.h"


namespace Framework
{

	enum HistoryDataType {RIGID_TYPE, COMPLEX_TYPE, STATIC_TYPE,ACTIVATION_TYPE,DOMINO_TYPE,NORIGID_TYPE};//add new types here


	class RigidBodyComplexData
	{
	public:
		Vector3 position;
		Quaternion orientation;
	    Vector3 velocity;
		Vector3 rotation;
		std::bitset<16> alivebitset;
		//Vector3 forceAccum;
		//Vector3 torqueAccum;
		//Vector3 acceleration;
		bool dying;
		float transparrency;
		bool isawake;
		bool previousSleeping;
	};


	class RigidBodyData
	{
	public:
		Vector3 position;
		Quaternion orientation;
	    Vector3 velocity;
		Vector3 rotation;
		//Vector3 forceAccum;
		//Vector3 torqueAccum;
		//Vector3 acceleration;
		bool dying;
		float transparrency;
		bool isawake;
		bool previousSleeping;
	};

	class DominoData
	{
	public:
		Vector3 position;
		Quaternion orientation;
	    Vector3 velocity;
		Vector3 rotation;
		bool sleeping;
		bool finished;
		bool isInitial;
	};

	class StaticBodyData
	{
	public:
		Vector3 position;
		Quaternion orientation;
		bool activated;
	};
	class NoRigid
	{
	public:
		Vec3 position;
		D3DXQUATERNION orientation;
	};
	class ActivationData
	{
	public:
		bool activated;
		float timer;
	};

	class HistoryData
	{
	public:
		HistoryDataType dataType;
		utility::Object data;
		int objectId;
		bool destroyed;

		HistoryData(){destroyed=false;}

		int GetBytesUsed()
		{
			int mem=0;
			mem += sizeof(HistoryData);
			if (dataType==RIGID_TYPE)
			{
				mem += sizeof(RigidBodyData);
			}
			else if (dataType==STATIC_TYPE)
			{
				mem += sizeof(StaticBodyData);
			}
			return mem;
		}

		~HistoryData()
		{
			if(data)
			{
				if (dataType==RIGID_TYPE)
				{
					RigidBodyData* temp = static_cast<RigidBodyData*>(data);
					delete temp;
				}
				else if (dataType==COMPLEX_TYPE)
				{
					RigidBodyComplexData* temp = static_cast<RigidBodyComplexData*>(data);
					delete temp;
				}
				else if (dataType==STATIC_TYPE)
				{
					StaticBodyData* temp = static_cast<StaticBodyData*>(data);
					delete temp;
				}
				else if (dataType==ACTIVATION_TYPE)
				{
					ActivationData* temp = static_cast<ActivationData*>(data);
					delete temp;
				}
				else if (dataType==DOMINO_TYPE)
				{
					DominoData* temp = static_cast<DominoData*>(data);
					delete temp;
				}
				else if (dataType==NORIGID_TYPE)
				{
					NoRigid* temp = static_cast<NoRigid*>(data);
					delete temp;
				}
			}
			else
			{
				std::cout<<"HISTORY DATA DESTRUCTOR PROBLEM"<<std::endl;
				std::cin.get();
			}
			
		}

	};
}

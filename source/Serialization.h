////////////////////////////////////////////////////////////////
//
//	Serialization.h
//	Provides interface for the serialization
//
//	Author: Santosh Shedbalkar
//	Copyright 2011, Digipen Institute of Technology
//
////////////////////////////////////////////////////////////////
#ifndef SERIALIZATION_H
#define SERIALIZATION_H
//#pragma message("Compiling: "__FILE__)
//
#include <string>
#include <vector>
#include "VMath.h"
//
namespace Framework
{
	class ISerializer;
	typedef std::vector<ISerializer*> StreamList;
	//
	class ISerializer
	{
//#pragma message("Including Class: ISerializer")
	public:
		virtual bool readProperty(const char* prop, int& i) const = 0;
		virtual bool readProperty(const char* prop, float& f) const = 0;
		virtual bool readProperty(const char* prop, std::string& str) const = 0;
		virtual bool readProperty(const char* prop, ISerializer& stream) = 0;
		virtual bool readProperty(const char* prop, StreamList& vec) const = 0;
		//virtual bool readProperty(const char* prop, Vec2& vec) const = 0;
		//virtual bool readProperty(const char* prop, Vec3& vec) const = 0;
		//virtual bool readProperty(const char* prop, Vec4& vec) const = 0;
		virtual bool readProperty(const char* prop, float* vec, int count ) const = 0;
		//
		virtual bool readValue(const char* prop, int& i) const = 0;
		virtual bool readValue(const char* prop, float& f) const = 0;
		virtual bool readValue(const char* prop, std::string& str) const = 0;
		//virtual bool readValue(const char* prop, Vec2& vec) const = 0;
		//virtual bool readValue(const char* prop, Vec3& vec) const = 0;
		//virtual bool readValue(const char* prop, Vec4& vec) const = 0;
		virtual bool readValue( const char* prop, float* vec, int count ) const = 0;
		virtual bool readSiblingNode(const char* prop, ISerializer& stream) = 0;
		//
		virtual bool goToParent() = 0;
		//
		virtual ISerializer* clone() = 0;
		virtual ISerializer* clone() const = 0;
		virtual bool isGood() const = 0;
		virtual char* name() const = 0;
		//
		virtual bool writeProperty( const char* key, const char* value ) = 0;
		virtual bool writeNodeAndValue( const char* node, const char* value ) = 0;
		virtual bool writeNode( const char* node ) = 0;
		virtual bool removeProperty( const char* key ) = 0;
		virtual bool removeNode( const char* key ) = 0;
		virtual bool appendNode( ISerializer& stream ) = 0;
		virtual void allocateNew() = 0;
	};

	//Serialization Operators
	//Base case of serialization is that the object serializes itself.
	template<typename type>
	inline void streamRead(ISerializer& stream, type& typeInstance)
	{
		typeInstance.Serialize(stream);
	}
}
//
#endif
/*
///////////////////////////////////////////////////////////////////////////////
///
///	\file Serialization.h
///	Defines ISerializer interface and basic stream opeartors.
///
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

namespace Framework
{
	///Base Serializer interface. Concrete serializers (Text, Xml, Binary) 
	///implement this interface. This separates the serialization process 
	///(what data needs to be read) from the serialization format (Text,Binary)
	///The serializers only need to implement the serialization of fundamental
	///type to be used by the engine.
	class ISerializer
	{
	public:
		///Open the serialization stream from a file.
		virtual bool Open(const std::string& file)=0;
		///Does the stream still have data?
		virtual bool IsGood()=0;
		virtual void ReadInt(int& i)=0;
		virtual void ReadFloat(float& f)=0;
		virtual void ReadString(std::string& str)=0;
	};

	//Serialization Operators 


	//Base case of serialization is that the object serializes itself.
	template<typename type>
	inline void StreamRead(ISerializer& stream,type& typeInstance)
	{
		typeInstance.Serialize(stream);
	}

	//Base non recursive stream operators of fundamental types.

	inline void StreamRead(ISerializer& stream,float& f)
	{
		stream.ReadFloat(f);
	}

	inline void StreamRead(ISerializer& stream,int& i)
	{
		stream.ReadInt(i);
	}

	inline void StreamRead(ISerializer& stream,std::string& str)
	{
		stream.ReadString(str);
	}


}
*/

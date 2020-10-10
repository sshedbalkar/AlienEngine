///////////////////////////////////////////////////////////////////////////////
///
///	\file Message.h
///	Object Messaging System used by engine for cross game object and system 
///	communication. Messages are passed as base class pointers and downcast 
///	to their derieved types by the receiver usually inside of a switch
///	block.
///
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once

namespace Framework
{
	namespace Mid // Message id
	{
		enum MessageIdType
		{
			None,
			Quit,
			Collide,
			EventTrigger,
			ToggleDebugInfo,
			CharacterKey,
			CharacterKeyRelease,
			VirtualKey,
			LMouseButton,
			RMouseButton,
			MouseMove,
			FileDrop,
			FrameByFrame,
			AdvanceFrame,
			MouseWheel,
			ActivateObj,
			GameWon,
			Menu,
			LeaveMenu,
			SystemPause,
			ToggleTime,
			TimeStateChange
		};
	}



	///Base message class. New message types are defined by deriving from this
	///class and mapping defined an element of the MessageIdType enumeration.
	class Message
	{
	public:
		Message(Mid::MessageIdType id) : MessageId(id){};
		Message(const Message& rhs): MessageId(rhs.MessageId){};
		//
		Mid::MessageIdType MessageId;
		virtual ~Message(){};
		virtual Message* clone(){return new Message(*this);};
	};

}

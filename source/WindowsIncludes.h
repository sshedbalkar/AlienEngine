///////////////////////////////////////////////////////////////////////////////////////
///
///	\file WindowsIncludes.h
///	Include the needed parts of the windows header.
///	
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
// prevent min/max from being #defined
#pragma once
#define NOMINMAX

// prevent winsock1 from being included
#define _WINSOCKAPI_
//#define WIN32_LEAN_AND_MEAN

// windows xp
//#define WINVER 0x0501
//#define _WIN32_WINNT 0x0501
//
//// internet explorer 6
//#define _WIN32_IE 0x0600
#include <windows.h>
#include <winsock2.h>
//#include <windows.h>
#include <mmsystem.h>
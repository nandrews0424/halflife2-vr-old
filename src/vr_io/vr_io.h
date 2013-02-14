#ifndef _VRIO_H
#define _VRIO_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define VRIOERR_NO_DATA_SOURCE        2
#define VRIOERR_SERVER_STOPPED        6
#define VRIOERR_SERVER_DISCONNECTED 109
#define VRIOERR_PIPE_IN_USE         231
#define VRIOERR_CLIENT_DISCONNECTED 232

enum VRIO_Channel
{
	HEAD = 0,
	WEAPON = 1,
	BODY = 2,
	RIGHT_HAND = 3,
	LEFT_HAND = 4,
	RIGHT_FOOT = 5,
	LEFT_FOOT = 6
};

#if defined(IS_DLL)	// inside DLL
#	define IMPORT_EXPORT   __declspec(dllexport)
#else						// outside DLL define
#	define IMPORT_EXPORT   __declspec(dllimport)
#endif  


// Maybe not a great name, but essentially the input data for a given channel (a particular target joint, etc)
struct VRIO_Message
{
	VRIO_Channel channel;
		
	// orientation
	float pitch;
	float yaw;
	float roll;

	//position
	float x;
	float y;
	float z;

	void init( )
	{
		pitch = 0;
		yaw = 0;
		roll = 0;
		x = 0;
		y = 0;
		z = 0;
	}
};


// Base interface for 
struct IVRIOClient
{
	virtual int initialize( void ) = 0;
	virtual int think( void ) = 0;
	virtual int getOrientation( VRIO_Channel, VRIO_Message& message ) = 0;
	virtual void dispose( void ) = 0;
};

extern "C" IMPORT_EXPORT IVRIOClient* APIENTRY _vrio_getInProcessClient();


#endif
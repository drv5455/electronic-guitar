#ifndef MULTI_TOUCH_H
#define MULTI_TOUCH_H

#include <map>
#include <stdint.h>
#include <cstring>

#define MT_OK		0	
#define MT_ERR_OPEN	-1
#define MT_ERR_DEV_NOT_OPEN -2

#define MT_WIDTH 4096
#define MT_HEIGHT 4096

typedef enum
{
	eeFingerDown,
	eeFingerUp
}teMtEvent;

static void eventToString(char* buf, teMtEvent type)
{
	switch(type)
	{
		case eeFingerDown: 
			strcpy(buf, "eeFingerDown");
			break;
		case eeFingerUp: 
			strcpy(buf, "eeFingerUp");
			break;
	}
}

typedef enum
{
	eeLeft,
	eeMiddle,
	eeRight
}teMtRegion;

static void regionToString(char* buf, teMtRegion region)
{
	switch(region)
	{
		case eeLeft: 
			strcpy(buf, "eeLeft");
			break;
		case eeMiddle: 
			strcpy(buf, "eeMiddle");
			break;
		case eeRight: 
			strcpy(buf, "eeRight");
			break;
	}
}

typedef struct
{
	teMtEvent type;
	teMtRegion region;
}tsMtEvent;

typedef struct
{
	uint16_t x;
	uint16_t y;
	bool justAdded;
	bool remove;
}tsPosition;

class MultiTouch
{
public:
	MultiTouch();
	~MultiTouch();

	int openDevice(char* device);
	int checkDevice();
	void setCallback(void (*handleEvent)(const tsMtEvent& event));

protected: // FUNCTIONS
	void handleABS(const uint32_t& code, const uint32_t& value);
	void handleSYN(const uint32_t& code, const uint32_t& value);
	teMtRegion calcRegion(const uint16_t &x, uint16_t &y);
	

protected:
	int m_descriptor;	
	std::map <int, tsPosition> m_posMap;
	std::map <int, int> m_trackMap;
	int m_curSlot;
	void (*m_handleEvent)(const tsMtEvent& event);
};

#endif

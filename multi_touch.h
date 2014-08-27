#ifndef MULTI_TOUCH_H
#define MULTI_TOUCH_H

#include <map>
#include <stdint.h>

#define MT_OK		0	
#define MT_ERR_OPEN	-1
#define MT_ERR_DEV_NOT_OPEN -2

struct tsPosition
{
	uint16_t x;
	uint16_t y;
	bool justAdded;
	bool remove;
};

class MultiTouch
{
public:
	MultiTouch();
	~MultiTouch();

	int openDevice(char* device);
	int checkDevice();

protected: // FUNCTIONS
	void handleABS(const uint32_t& code, const uint32_t& value);
	void handleSYN(const uint32_t& code, const uint32_t& value);

protected:
	int m_descriptor;	
	std::map <int, tsPosition> m_posMap;
	std::map <int, int> m_trackMap;
	int m_curSlot;

};

#endif

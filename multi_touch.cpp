#include "multi_touch.h"
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>

MultiTouch::MultiTouch()
: m_descriptor(-1)
{
}

MultiTouch::~MultiTouch()
{
}

int MultiTouch::openDevice(char* device)
{
	m_descriptor = open(device, O_RDONLY);	
	if(m_descriptor == -1)
		return MT_ERR_OPEN;

	// reset all variables
	m_posMap.clear();
	m_trackMap.clear();
	m_curSlot = -1;
}

int MultiTouch::checkDevice()
{
	if(m_descriptor == -1)
		return MT_ERR_DEV_NOT_OPEN;

	struct input_event ie;
	read(m_descriptor, &ie, sizeof(input_event));

	switch(ie.type)
	{
		case EV_SYN:
			handleSYN(ie.code, ie.value);
			break;
		case EV_ABS:
			handleABS(ie.code, ie.value);
			break;
		default:
			break;
	}

	return MT_OK;
}

void MultiTouch::handleABS(const uint32_t& code, const uint32_t& value)
{
		switch(code)
		{
			case ABS_MT_SLOT:
				m_curSlot = value;
				break;
			case ABS_MT_TRACKING_ID:
			{
				if(value == -1)
				{
					// finger lifted
					int track = m_trackMap[m_curSlot];
					m_posMap[track].remove = true;
					m_trackMap.erase(m_curSlot);
				}
				else if(m_posMap.count(value) == 0)
				{
					// finger added
					m_trackMap[m_curSlot] = value;
					m_posMap[value].justAdded = true;
					m_posMap[value].remove = false;
				}
				break;
			}
			case ABS_MT_POSITION_X:
			{
				int track = m_trackMap[m_curSlot];
				m_posMap[track].x = value;
				break;
			}
			case ABS_MT_POSITION_Y:
			{
				int track = m_trackMap[m_curSlot];
				m_posMap[track].y = value;
				break;
			}
			default:
				break;
		}
}

void MultiTouch::handleSYN(const uint32_t& code, const uint32_t& /*value*/)
{
	if(code == SYN_REPORT)
	{
		std::map<int, tsPosition>::iterator it;
		for(it = m_posMap.begin(); it != m_posMap.end(); it++)
		{
			if(it->second.justAdded)
			{
				//printf("x: %d, y: %d, new: %d\n", it->second.x, it->second.y, it->second.justAdded);
				it->second.justAdded = false;
				uint16_t y = it->second.y;

				tsMtEvent event;
				event.type = eeFingerDown;
				event.region = calcRegion(it->second.x, it->second.y);
				m_handleEvent(event);
			}
			else if(it->second.remove)
			{
				uint16_t y = it->second.y;

				tsMtEvent event;
				event.type = eeFingerUp;
				event.region = calcRegion(it->second.x, it->second.y);
				m_handleEvent(event);

				m_posMap.erase(it->first);
			}
		}
	}
}

void MultiTouch::setCallback(void (*handleEvent)(const tsMtEvent& event))
{
	m_handleEvent = handleEvent;
}

teMtRegion MultiTouch::calcRegion(const uint16_t &/*x*/, uint16_t &y)
{
	teMtRegion region;
	if(y >= 0 && y < MT_WIDTH/3)
		region = eeLeft;
	else if(y >= MT_WIDTH/3 && y < (MT_WIDTH/3)*2)
		region = eeMiddle;
	else
		region = eeRight;

	return region;
}

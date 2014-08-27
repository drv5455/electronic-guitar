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
				printf("x: %d, y: %d, new: %d\n", it->second.x, it->second.y, it->second.justAdded);
				it->second.justAdded = false;

				uint16_t y = it->second.y;
				if(y >= 0 && y < 4096/3)
				{
					printf("on 1\n");
				}
				else if(y >= 4096/3 && y < (4096/3)*2)
				{
					printf("on 2\n");
				}
				else
				{
					printf("on 3\n");
				}
			}
			else if(it->second.remove)
			{
				printf("finger lifted\n");
				uint16_t y = it->second.y;
				if(y >= 0 && y < 4096/3)
				{
					printf("off 1\n");
				}
				else if(y >= 4096/3 && y < (4096/3)*2)
				{
					printf("off 2\n");
				}
				else
				{
					printf("off 3\n");
				}
				m_posMap.erase(it->first);
			}
		}
	}
}

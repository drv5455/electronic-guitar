#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//#include <alsa/asoundlib.h>
#include "multi_touch.h"

static char *port_name = "default";

int main(int argc, char* argv[])
{

	MultiTouch mt;
	mt.openDevice(argv[1]);
	while(1)
		mt.checkDevice();
#if 0
	int fd = open(argv[1], O_RDONLY);	
	if(fd == -1)
		return -1;

	fd_set fs;
	uint t = 0;
	while(true)
	{
		usleep(100);
		FD_ZERO(&fs);
		FD_SET(fd, &fs);
		select(fd+1, &fs, NULL, NULL, NULL);
		if(FD_ISSET(fd, &fs))
			printf("Touched %u\n", t++);
	}
	static enum {
		STATE_UNKNOWN,
		STATE_1PARAM,
		STATE_1PARAM_CONTINUE,
		STATE_2PARAM_1,
		STATE_2PARAM_2,
		STATE_2PARAM_1_CONTINUE,
		STATE_SYSEX
	} state = STATE_UNKNOWN;
#endif

#if 0
	int err;
	snd_rawmidi_t *output, **outputp;
	outputp = &output;
	if ((err = snd_rawmidi_open(NULL, outputp, port_name, SND_RAWMIDI_NONBLOCK)) < 0) {
		printf("cannot open port \"%s\": %s", port_name, snd_strerror(err));
		return -1;
	}

	char midi_data[3];
	std::map <int, tsPosition> posMap;
	std::map <int, int> trackMap;

	struct input_event ie;
	int curSlot = -1;
	while(read(fd, &ie, sizeof(input_event)))
	{
		//if(ie.type
#if 0
		if(ie.type == EV_ABS || (ie.type == EV_SYN && (ie.code == SYN_REPORT || ie.code == SYN_MT_REPORT)))
			printf("type %04d\tcode %04X\tvalue %04d\n", ie.type, ie.code, ie.value);
#endif

		if(ie.type == EV_SYN)
		{
			if(ie.code == SYN_REPORT)
			{
				// PRINT
				std::map<int, tsPosition>::iterator it;
				for(it = posMap.begin(); it != posMap.end(); it++)
				{
					if(it->second.justAdded)
					{
						printf("x: %d, y: %d, new: %d\n", it->second.x, it->second.y, it->second.justAdded);
						it->second.justAdded = false;

						midi_data[0] = 0x90;
						midi_data[2] = 0x64;
						uint16_t y = it->second.y;
						if(y >= 0 && y < 4096/3)
						{
							midi_data[1] = 0x3D;
							printf("on 1\n");
						}
						else if(y >= 4096/3 && y < (4096/3)*2)
						{
							midi_data[1] = 0x3E;
							printf("on 2\n");
						}
						else
						{
							midi_data[1] = 0x3F;
							printf("on 3\n");
						}
						if ((err = snd_rawmidi_nonblock(output, 0)) < 0) {
							printf("cannot set blocking mode: %s", snd_strerror(err));
							return -2;
						}
						if ((err = snd_rawmidi_write(output, midi_data, 3)) < 0) {
							printf("cannot send data: %s", snd_strerror(err));
							return -3;
						}
					}
					else if(it->second.remove)
					{
						printf("finger lifted\n");
						midi_data[0] = 0x80;
						midi_data[2] = 0x64;
						uint16_t y = it->second.y;
						if(y >= 0 && y < 4096/3)
						{
							midi_data[1] = 0x3D;
							printf("off 1\n");
						}
						else if(y >= 4096/3 && y < (4096/3)*2)
						{
							midi_data[1] = 0x3E;
							printf("off 2\n");
						}
						else
						{
							midi_data[1] = 0x3F;
							printf("off 3\n");
						}
						if ((err = snd_rawmidi_nonblock(output, 0)) < 0) {
							printf("cannot set blocking mode: %s", snd_strerror(err));
							return -2;
						}
						if ((err = snd_rawmidi_write(output, midi_data, 3)) < 0) {
							printf("cannot send data: %s", snd_strerror(err));
							return -3;
						}
						posMap.erase(it->first);
					}
				}
			}
		}
		else if(ie.type == EV_ABS)
		{
			switch(ie.code)
			{
				case ABS_MT_SLOT:
					curSlot = ie.value;
					break;
				case ABS_MT_TRACKING_ID:
				{
					if(ie.value == -1)
					{
						// finger lifted
						int track = trackMap[curSlot];
						posMap[track].remove = true;
						trackMap.erase(curSlot);
#if 0
						posMap.erase(track);
						trackMap.erase(curSlot);
						printf("finger lifted\n");
#endif
					}
					else if(posMap.count(ie.value) == 0)
					{
						// finger added
						trackMap[curSlot] = ie.value;
						posMap[ie.value].justAdded = true;
						posMap[ie.value].remove = false;
					}
					break;
				}
				case ABS_MT_POSITION_X:
				{
					int track = trackMap[curSlot];
					posMap[track].x = ie.value;
					break;
				}
				case ABS_MT_POSITION_Y:
				{
					int track = trackMap[curSlot];
					posMap[track].y = ie.value;
					break;
				}
				default:
					break;
			}
		}
	}
#endif


	return 0;
}

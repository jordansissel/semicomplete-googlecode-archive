/*
 * Solaris Audio Mixer thingy :(
 *
 * $Id$
 */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/audio.h>
#include <sys/mixer.h>

struct options {
	uint_t	volume;        /* Master output volume */
	uchar_t	muted;         /* Muted? */
	uint_t	port;          /* Where is audio going? (headphones, etc) */
};

typedef struct options options_t;

void print_status();
void print_info(audio_info_t *info, audio_channel_t *ch);
void parse_args(int argc, char **argv);
void set_info(options_t *foo, audio_info_t *info, audio_channel_t *chan);
void usage(const char *err);

/* I'm lazy... */
static char            *me;
static audio_channel_t  ch;
static am_control_t    *ctl;
static int              num, audio_fd;

int main(int argc, char **argv) {
	audio_info_t	info;
	int i;

	audio_fd = open("/dev/audioctl", O_RDWR);
	me = *argv;

	if (audio_fd == -1) {
		printf("Error [/dev/audioctl]: %s\n", strerror(errno));
		exit(1);
	}

	if (ioctl(audio_fd, AUDIO_GET_NUM_CHS, &num) < 0) {
		printf("AUDIO_GET_NUM_CHS: %s\n", strerror(errno));
		exit(1);
	}

	ctl = (am_control_t *)malloc(AUDIO_MIXER_CTL_STRUCT_SIZE(num));

	if (ioctl(audio_fd, AUDIO_MIXERCTL_GETINFO, ctl) < 0) {
		printf("AUDIO_MIXERCTL_GETINFO: %s\n", strerror(errno));
		exit(1);
	}

	ch.info = &info;
	ch.info_size = sizeof (audio_info_t);

	if (argc == 1) {
		print_status();
	} else {
		parse_args(argc, argv);
		print_status();
	}

	free(ctl);
}

void print_status() {
	int i;
	audio_info_t maininfo;
	if (ioctl(audio_fd, AUDIO_GETINFO, &maininfo) >= 0) {
		print_info(&maininfo, NULL);
	}
	for (i = 0; i < num; i++) {
		if (ctl->ch_open[i] != 0) {
			ch.ch_number = i;
			if (ioctl(audio_fd, AUDIO_MIXERCTL_GET_CHINFO, &ch) < 0) {
				printf("Channel # %d isn't an audio/audioctl device\n", i);
			} else {
				audio_info_t *info = (audio_info_t *)ch.info;
				print_info(info, &ch);
			}
		}
	}
}

void print_info(audio_info_t *info, audio_channel_t *ch) {
	float vol = (float)(info->play.gain);

	if (ch == NULL)
		printf("Ch# Master");
	else 
		printf("Ch# %d, PID = %d, Type = %d",
				 ch->ch_number, ch->pid, ch->dev_type);

	if (info->output_muted) 
		printf(" [MUTED]\n");
	else
		printf("\n");

	printf("\tVolume: %.0f%%\n", (vol / AUDIO_MAX_GAIN) * 100.0);
}

void parse_args(int argc, char **argv) {
	audio_info_t maininfo;
	char **p = argv;
	int gain;
	char *param;
	char done;
	int i;
	options_t foo;

	foo.muted = 0;
	foo.volume = 0;
	foo.port = 0;

	while (*++p != NULL && **p == '-') {
		done = 0;
		while (!done && *++*p != '\0') {
			/* printf("Flag: %c\n", **p); */
			switch (**p) {
				case 'h':
					usage(NULL);
					break;
				case 'm':
					printf("Toggling mute\n");
					foo.muted = 1;
					break;
				case 'p':
					printf("Toggling Headphones\n");
					foo.port |= AUDIO_HEADPHONE;
					break;
				case 'l':
					printf("Toggling Line Out\n");
					foo.port |= AUDIO_LINE_OUT;
					break;
				case 's':
					printf("Toggling Internal Speaker\n");
					foo.port |= AUDIO_SPEAKER;
					break;
				case 'v':
					//printf("---> %c\n", 
					if (*(*p + 1) == NULL)
						if (*(p + 1) == NULL)
							usage("-v requires a value (0-100)");
						else {
							param = *(p + 1);
							/* Move to next word */
							p++;
						}
					else {
							param = *p + 1;
					}

					/* Move to the next word... */
					p++;

					printf("Word: %s\n", param);
					gain = atoi(param);
					if (gain < 0 || gain > 100)
						usage("Volume values are range 0-100 inclusive.");

					foo.volume = (int)((gain / 100.0) * 255.0);
					done = 1;
					break;
				default:
					/* Call usage() instead? */
					printf("Invalid argument: %c\n", **p);
					break;
			}

			for (i = 0; i < num; i++) {
				if (ctl->ch_open[i] != 0) {
					ch.ch_number = i;
					if (ioctl(audio_fd, AUDIO_MIXERCTL_GET_CHINFO, &ch) < 0) {
						printf("Channel # %d isn't an audio/audioctl device\n", i);
					} else {
						audio_info_t *info = (audio_info_t *)ch.info;

						set_info(&foo, info, &ch);
					}
				}
			}
			if (ioctl(audio_fd, AUDIO_GETINFO, &maininfo) >= 0) {
				set_info(&foo, &maininfo, NULL);
			}
		}
	}
}

void set_info(options_t *foo, audio_info_t *info, audio_channel_t *chan) {
	char chname[10] = "main";

	if (chan != NULL)
		sprintf(chname, "%d", chan->ch_number);
	else {
		if (foo->muted) {
			/* printf("Muting channel %s / %d / %d\n", chname, info->output_muted, foo->muted); */
			info->output_muted ^= foo->muted;
		}
	}

	if (foo->volume)
		info->play.gain = foo->volume;

	if (foo->port)
		info->play.port ^= foo->port;

	if (ioctl(audio_fd, AUDIO_SETINFO, info) < 0) {
		printf("Warning: Failed setting audio channel info for channel %s", chname);
		if (chan != NULL)
	  		printf("[PID: %d].\n", chan->pid);
	}

}

void usage(const char *err) {
	if (err != NULL)
		printf("Error: %s\n", err);

	printf("Usage: %s [ -mslp -v <0-100> ]\n", me);
	printf("\t-m     toggle mute\n");
	printf("\t-l     toggle line-out\n");
	printf("\t-p     toggle headphones\n");
	printf("\t-s     toggle internal speaker\n");
	printf("\t-v <0-100> set volume percentage level\n");
	exit(1);
}

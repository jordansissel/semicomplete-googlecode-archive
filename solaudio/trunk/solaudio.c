#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/audio.h>
#include <sys/mixer.h>

/*
 * audio_info_t    info;
 * AUDIO_INITINFO(&info);
 * info.play.port = AUDIO_HEADPHONE;
 * err = ioctl(audio_fd, AUDIO_SETINFO, &info);
 */

void print_status(audio_info_t *info);
void parse_args(int argc, char **argv, audio_info_t *info);
void usage(const char *err);

struct options {
	uint_t	volume;        /* Master output volume */
	uchar_t	muted;         /* Muted? */
	uint_t	port;          /* Where is audio going? (headphones, etc) */
};

typedef struct options options_t;

static char *me;
static audio_channel_t  ch;
static am_control_t     *ctl;
static int        num;
static int				err, audio_fd;

int main(int argc, char **argv) {
	audio_info_t	info;
	int i;

	audio_fd = open("/dev/audioctl", O_RDWR);
	me = *argv;

	if (audio_fd == -1) {
		printf("Error [/dev/audioctl]: %s\n", strerror(errno));
		exit(1);
	}

	if (ioctl(audio_fd, AUDIO_GETINFO, &info) == -1) {
		printf("Error trying to get info from the audio device.\n%s\n", strerror(errno));
		exit(1);
	}
	
	printf("Foo: 0x%08x\n", info.sw_features_enabled);

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
		print_status(&info);
	} else {
		parse_args(argc, argv, &info);

		//info.play.port ^= AUDIO_HEADPHONE;
		printf("Headphone: %d\n", info.play.port);
		printf("Volume: %d\n", info.play.gain);

		if (ioctl(audio_fd, AUDIO_SETINFO, &info) == -1) {
			printf("Error trying to set info on the audio device.\n%s\n", strerror(errno));
		}
		print_status(&info);
	}

}

void print_status(audio_info_t *info) {
	printf("Muted: %d\n", info->output_muted);
	printf("Gain: %u\n", info->monitor_gain);
	printf("Volume: %u\n", info->play.gain);
}

void parse_args(int argc, char **argv, audio_info_t *info) {
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
				case 'm':
					printf("Toggling mute\n");
					foo.muted = 1;
					//info->output_muted ^= 0x1;
					break;
				case 'p':
					printf("Toggling Headphones\n");
					//info->play.port ^= AUDIO_HEADPHONE;
					foo.port |= AUDIO_HEADPHONE;
					break;
				case 'l':
					printf("Toggling Line Out\n");
					//info->play.port ^= AUDIO_LINE_OUT;
					foo.port |= AUDIO_LINE_OUT;
					break;
				case 'v':
					if (**(p + 1) == NULL)
						if (*(p + 1) == NULL)
							usage("-v requires a value (0-100)");
						else {
							param = *p + 1;
						}
					else {
						param = *(p + 1);
						/* Move to next word */
						p++;
					}

					/* Move to the next word... */
					p++;

					printf("Word: %s\n", param);
					gain = atoi(param);
					foo.volume = gain;
					printf("%d Gain: %d\n", p, gain);
					//info->play.gain = gain;
					done = 1;
					break;
				default:
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
						printf("Ch# %d, PID = %d, Type = %d, Gain = %d\n",
								 i, ch.pid, ch.dev_type, ((audio_info_t *)ch.info)->play.gain);
						if (foo.muted)
							info->output_muted ^= foo.muted;

						if (foo.volume)
							info->play.gain = foo.volume;

						if (foo.port)
							info->play.port ^= foo.port;


					}
				}
			}
		}
	}
	if (*p != NULL) {
		printf("Current: %c / %d\n", **p, p - argv);
	}
}

void usage(const char *err) {
	printf("Usage: %s [ -m | -v <0-100> ]\n", me);
	printf("	Error: %s\n", err);
	exit(1);
}

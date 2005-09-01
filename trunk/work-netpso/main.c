/* 
 * This should be run from inetd (/etc/inetd.conf)
 *
 * It takes input from stdin and writes to stdout.
 */

#include <sys/types.h>
#include <sys/uio.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <stdarg.h>

/* Buffer sizes of page size is efficient */
#define BUFSIZE 4096

void readparameters(char *buf, int *steps, int *particles);
void readtabfile(char *buf);
void runtest(int steps, int particles);

int main(int argc, char *argv) {
	static int pid;
	int steps = 0, particles = 0;
	char buf[BUFSIZE];

	memset(buf, 0, BUFSIZE);

	chdir("/home/psionic/projects/work-netpso");

	/* Start logging */
	openlog("netpso", LOG_PID | LOG_PERROR, LOG_USER);

	/* Setup, we need to be told about things */
	readparameters(buf, &steps, &particles);
	readtabfile(buf);

	/* Turn the tabfile into a packed data file *sigh* */
	system("./packdata data.tab > data.packed");

	/* Run the test */
	runtest(steps, particles);


	syslog(LOG_DEBUG, "Sending endofoutput and arrow data");
	/* Done running the test, tell the windows client we're done */
	fflush(stdout);
	fwrite("ENDOFOUTPUT", 1, strlen("ENDOFOUTPUT"), stdout);
	fflush(stdout);

	system("cat Arrow_data.pa_*_0.dat");


	return 0;
}

void readparameters(char *buf, int *steps, int *particles) {
	int parms[2];
	int x = 0;
	char *p;
	int bytes = 0;

	syslog(LOG_DEBUG, "Starting parameter parsing");
	p = buf;

	/* Read null-delimited tokens until 'ENDOFPARAMETERS' */
	while (strncmp(buf, "ENDOFPARAMETERS", BUFSIZE) != 0) {

		do {
			bytes += read(STDIN_FILENO, buf + bytes, 1);
		} while (*(buf + bytes - 1) != '\0');

		syslog(LOG_DEBUG, "Got [%d] %s\n", bytes, buf);

		if (x < 2) 
			parms[x++] = atoi(buf);
		bytes = 0;
	}


	*steps = parms[0]; /* First number is number of steps */
	*particles = parms[1];

	syslog(LOG_DEBUG, "End of parameter parsing");
}

void readtabfile(char *buf) {
	FILE *tabfile;
	int bytes;

	syslog(LOG_DEBUG, "Starting tabfile reading");

	tabfile = fopen("data.tab", "w");
	if (NULL == tabfile) {
		syslog(LOG_DEBUG, "FATAL: data.tab: %m");
		exit(1);
	}

	while ((bytes = read(STDIN_FILENO, buf, BUFSIZE)) > 0) {
		int x = 0;
		int c = 0;

		/* Check 'buf' for a sequence of 5 \0's in a row */
		for (x = 0; x < bytes; x++) {
			if (buf[x] == '\0') {
				c++;
				syslog(LOG_DEBUG, "Got %d nulls", c);
			}
			else 
				c = 0;

			if (c == 5) {
				bytes = x - 5;
				break;
			}
		}

		syslog(LOG_DEBUG, "Read %d bytes\n", bytes);
		fwrite(buf, 1, bytes, tabfile);

		if (5 == c) {
			memcpy(buf, buf + bytes, BUFSIZE - bytes);
			break;
		}
	}

	syslog(LOG_DEBUG, "Finished reading tabfile: %m");

	fclose(tabfile);

	syslog(LOG_DEBUG, "Done with tabfile reading");
}

void runtest(int steps, int particles) {
	char *cmd;
	int len;
	pid_t pid;
	char dir[20];
	snprintf(dir, 20, "test_%d", getpid());

	if (-1 == mkdir(dir, 0755)) {
		/* Only care if this is an error *other* than EEXIST */
		if (EEXIST != errno) { 
			syslog(LOG_DEBUG, "FATAL: mkdir: %m");
			exit(1);
		}
	}

	if (-1 == chdir(dir)) {
		syslog(LOG_DEBUG, "FATAL: chdir: %m");
		exit(1);
	}

	system("cp ../data.packed .");
	system("cp ../woohoo .");

	/* mpirun -np PARTICLES ./woohoo STEPS PARTICLES 0 data.packed */
	len = asprintf(&cmd, "/opt/mpich/p4-gnu/bin/mpirun -np %d ./woohoo %d %d 0 data.packed | tee mpirunoutput", 
						particles, steps, particles);

	syslog(LOG_DEBUG, "running: '%s'", cmd);
	system(cmd);
	syslog(LOG_DEBUG, "Done with '%s'", cmd);
	free(cmd);
}

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

void readparameters(char *buf, int *steps, int *particles, int *bytes);
void readtabfile(char *buf, int bytes);
void runtest(int steps, int particles);

int main(int argc, char *argv) {
	static int pid;
	int steps, particles;
	char buf[BUFSIZE];
	int bytes;

	chdir("/home/psionic/projects/work-netpso");

	/* Start logging */
	openlog("netpso", LOG_PID | LOG_PERROR, LOG_USER);

	/* Setup, we need to be told about things */
	readparameters(buf, &steps, &particles, &bytes);
	readtabfile(buf, bytes);

	/* Turn the tabfile into a packed data file *sigh* */
	system("./packdata data.tab > data.packed");

	/* Run the test */
	runtest(steps, particles);

	/* Done running the test, tell the windows client we're done */
	fwrite("\0\0\0\0\0", 1, 5, stdout);


}

void readparameters(char *buf, int *steps, int *particles, int *bytes) {
	int parms[2];
	int x = 0;
	char *p;

	syslog(LOG_DEBUG, "Starting parameter parsing");
	*bytes = read(STDIN_FILENO, buf, BUFSIZE);
	p = buf;

	/* Read null-delimited tokens until 'ENDOFPARAMETERS' */
	while (strncmp(p, "ENDOFPARAMETERS", BUFSIZE) != 0) {
		syslog(LOG_DEBUG, "Got %s\n", p);
		parms[x++] = atoi(p);
		p += strlen(p) + 1;
	}

	*steps = parms[0]; /* First number is number of steps */
	*particles = parms[1];

	/* Shift past 'ENDOFPARAMETERS' (to the beginning of the tab-file data) */
	p += strlen(p) + 1;

	/* Move the parameters to the beginning of the buffer */
	strncpy(buf, p, BUFSIZE - (p - buf));
	bytes -= (p - buf);
	p = buf;

	syslog(LOG_DEBUG, "End of parameter parsing");
}

void readtabfile(char *buf, int bytes) {
	FILE *tabfile;

	syslog(LOG_DEBUG, "Starting tabfile reading");

	tabfile = fopen("data.tab", "w");
	if (NULL == tabfile) {
		syslog(LOG_DEBUG, "FATAL: data.tab: %m");
		exit(1);
	}

	while (bytes > 0) {
		syslog(LOG_DEBUG, "Read %d bytes\n", bytes);
		fwrite(buf, 1, bytes, tabfile);
		*(buf + bytes) = '\0';
		bytes = read(STDIN_FILENO, buf, BUFSIZE);
	}

	fclose(tabfile);

	syslog(LOG_DEBUG, "Done with tabfile reading");
}

void runtest(int steps, int particles) {
	char *cmd;
	int len;
	pid_t pid;

	if (-1 == mkdir("test", 0x755)) {
		/* Only care if this is an error *other* than EEXIST */
		if (EEXIST != errno) { 
			syslog(LOG_DEBUG, "FATAL: mkdir: %m");
			exit(1);
		}
	}

	/* mpirun -np PARTICLES ./woohoo STEPS PARTICLES 0 data.packed */
	len = asprintf(&cmd, "/opt/mpich/p4-gnu/bin/mpirun -np %d ./woohoo %d %d 0 data.packed", 
						particles, steps, particles);

	syslog(LOG_DEBUG, "running: '%s'", cmd);
	system("cmd");
	free(cmd);
}

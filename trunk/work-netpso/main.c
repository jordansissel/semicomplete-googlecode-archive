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

int main(int argc, char *argv) {
	char buf[BUFSIZE];
	char *p = buf;
	int parms[2], x = 0;
	int bytes;
	FILE *tabfile;

	/* Start logging */
	openlog("netpso", LOG_PID | LOG_PERROR, LOG_USER);

	bytes = read(STDIN_FILENO, buf, BUFSIZE);
	
	/* Read null-delimited things until 'ENDOFPARAMETERS' */
	while (strncmp(p, "ENDOFPARAMETERS", BUFSIZE) != 0) {
		syslog(LOG_DEBUG, "Got %s\n", p);
		parms[x++] = atoi(p);
		p += strlen(p) + 1;
	}

	syslog(LOG_DEBUG, "End of parameters");

	/* Shift past 'ENDOFPARAMETERS' */
	p += strlen(p) + 1;

	/* Move the parameters to the beginning of the buffer */
	strncpy(buf, p, BUFSIZE - (p - buf));
	bytes -= (p - buf);
	p = buf;

	/* At this point, we're at the beginning of the the tab-file data */
	chdir("/home/psionic/projects/work-netpso");

	tabfile = fopen("data.tab", "w");
	if (NULL == tabfile) {
		syslog(LOG_DEBUG, "data.tab: %m");
		exit(1);
	}

	while (bytes > 0) {
		syslog(LOG_DEBUG, "Read %d bytes\n", bytes);
		fwrite(buf, 1, bytes, tabfile);
		*(buf + bytes) = '\0';
		bytes = read(STDIN_FILENO, buf, BUFSIZE);
	}

	fclose(tabfile);

	syslog(LOG_DEBUG, "EOF");


}

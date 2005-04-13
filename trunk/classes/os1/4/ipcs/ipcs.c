#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>

#define	MAX(a,b)	(a > b ? a : b)
#define	MIN(a,b)	(a < b ? a : b)

/*
 * A mode type that includes a character string representation.
 */
typedef	struct {
	char string[12];
} strmode_t;

/*
 * Convert an octal mode to its character string representation.
 */
void
convert_mode(mode_t mode, strmode_t *strmode)
{
	int i;
	char *p = strmode->string;

	/*
	 * Not part of the octal mode.
	 */
	*p++ = '-';
	*p++ = '-';

	for(i = 0; i < 3; i++) {
		*p++ = (mode & (S_IREAD >> (i * 3))) ? 'r' : '-';
		*p++ = (mode & (S_IWRITE >> (i * 3))) ? 'w' : '-';
		*p++ = '-';	/* Unused by ipcs */
	}

	*p = '\0';
}

/*
 * Report on a segment.
 */
void
process_shmid(int shmid)
{
	struct shmid_ds info;

	/*
	 * Obtain segment details, if accessible.
	 */
	if (shmctl(shmid, IPC_STAT, &info) != 0) {
		if (EACCES == errno) {
			printf("%8d %8c %12c %8c %8c %8c Inaccessible\n",
					shmid, '-', '-', '-', '-', '-');
			return;
		} else {
			perror("shmctl");
			exit(1);
		}
	}

	/*
	 * Display segment details.
	 */
	strmode_t strmode;
	convert_mode(info.shm_perm.mode, &strmode);
	printf("%8d %8#x %12s %8d %8d %8d\n", shmid, info.shm_perm.key,
			strmode.string, info.shm_perm.uid, info.shm_segsz,
			info.shm_nattch);
}

/*
 * This method on loan from shmids(2).
 */
void
examine_shmids(int hi, int lo)
{
	int *ids = NULL;

	uint_t nids = 0;
	uint_t n;
	int i;

	/*
	 * Obtain shmids if possible.
	 */
	for (;;) {
		if (shmids(ids, nids, &n) != 0) {
			perror("shmids");
			exit(1);
		}
		if (n <= nids)     /* we got them all */
			break;

		/* we need a bigger buffer */
		ids = realloc(ids, (nids = n) * sizeof (int));
	}

	/*
	 * Display the details of each shmid.
	 */
	printf("%8s %8s %12s %8s %8s %8s\n",
			"ID", "KEY", "MODE", "OWNER", "SIZE", "NUMLOCKS");
	for (i = 0; i < n; i++)
		if (hi >= ids[i] && ids[i] >= lo)
			process_shmid(ids[i]);

	free(ids);
}

int
main(int argc, char **argv)
{
	int a, b;

	if (argc != 3) {
		printf("usage: %s bound-1 bound-2\n", argv[0]);
		exit(1);
	}

	a = atoi(argv[1]);
	b = atoi(argv[2]);

	examine_shmids(MAX(a,b), MIN(a,b));
}

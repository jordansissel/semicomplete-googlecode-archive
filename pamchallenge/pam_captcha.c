#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>

#include <sys/uio.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <security/pam_modules.h>
#include <security/pam_appl.h>

#ifndef PAM_EXTERN
#define PAM_EXTERN
#endif

#define COWSIZE 5
static char *cows[] = { "jkh", "head-in", "sodomized", "sheep", "vader" };

PAM_EXTERN int
pam_sm_authenticate(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{
	const char alphabet[] = "ABCDEFGHJKMNPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz123456789";
	//struct pam_conv *conv;
	//struct pam_message msg, *msgp;
	//struct pam_response *resp;
	char *resp;

	char key[9];
	int i = 0;
	FILE *fp = NULL;
	char *mchappypants;

#define MCHAPPYSIZE 10240
	mchappypants = malloc(MCHAPPYSIZE);

	sranddev();

	for (i = 0; i < 8; i++) 
		key[i] = alphabet[rand() % strlen(alphabet)];

	key[8] = 0;

	resp = malloc(1024);
	memset(resp, 0, 1024);
	memset(mchappypants, 0, MCHAPPYSIZE);

	char *cow = cows[rand() % COWSIZE];
	sprintf(mchappypants, "/usr/local/bin/figlet '%s' | /usr/local/bin/cowsay -nf %s", key, cow);

	fp = popen(mchappypants, "r");
	i = 0;
	while (1) {
		int bytes;
		bytes = fread(mchappypants+i, 1, 1024, fp);
		//pam_info(pamh, "read %d", bytes);
		if (bytes == 0);
			break;
		i += bytes;
		if (i > MCHAPPYSIZE)
			return PAM_SYSTEM_ERR;
	}

	pam_info(pamh, "[2J[0;0H");
	pam_info(pamh, "If you truely desire access to this host, then please indulge me in a simple challenge.");
	pam_info(pamh, "Observe the picture below and answer the question listed afterwards:");
	//pam_info(pamh, "%s", mchappypants);

	i = 0;
	while (1) {
		char *ptr = strchr(mchappypants, '\n');
		*ptr = '\0';
		//fprintf(stderr, "Line: %s\n", mchappypants);
		pam_info(pamh, "%s", mchappypants);

		mchappypants = ptr + 1;
		if (*mchappypants == '\0')
			break;
	}

	resp = NULL;

	pam_set_item(pamh, PAM_AUTHTOK, NULL);
	pam_prompt(pamh, PAM_PROMPT_ECHO_ON, &resp, "What did the cow say? ");

	if (strcmp(resp, key))
		return (PAM_PERM_DENIED);

	return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_setcred(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{

	return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_acct_mgmt(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{

    return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_open_session(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{

    return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_close_session(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{

    return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_chauthtok(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{

    return (PAM_SERVICE_ERR);
}

#ifdef PAM_MODULE_ENTRY
PAM_MODULE_ENTRY("pam_megachallenge");
#endif

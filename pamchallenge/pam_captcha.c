#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>

#include <time.h>
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
static char *cows[] = { "head-in", "sodomized", "sheep", "vader" };

PAM_EXTERN int
pam_sm_authenticate(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{
	const char alphabet[] = "ABCDEFGHJKMNPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz123456789";

	struct pam_conv *conv;
	struct pam_message msg, *msgp;
	struct pam_response *resp;
	//char *resp;

	char key[9];
	int i = 0;
	FILE *fp = NULL;
	char *mchappypants;

#define MCHAPPYSIZE 10240
	mchappypants = malloc(MCHAPPYSIZE);

	srand(time(NULL));

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

	pam_get_item(pamh, PAM_CONV, (const void **)&conv);
	msgp = &msg;

	msg.msg_style = PAM_TEXT_INFO;
	resp = NULL;

	msg.msg = "[2J[0;0H";
	(*conv->conv)(1, &msgp, &resp, conv->appdata_ptr);

	msg.msg = "If you truely desire access to this host, then please indulge me in a simple challenge.";
	(*conv->conv)(1, &msgp, &resp, conv->appdata_ptr);

	msg.msg = "Observe the picture below and answer the question listed afterwards:";
	(*conv->conv)(1, &msgp, &resp, conv->appdata_ptr);

	i = 0;
	while (1) {
		char *ptr = strchr(mchappypants, '\n');
		*ptr = '\0';
		//fprintf(stderr, "Line: %s\n", mchappypants);
		msg.msg = mchappypants;
		(*conv->conv)(1, &msgp, &resp, conv->appdata_ptr);
		//pam_info(pamh, "%s", mchappypants);

		mchappypants = ptr + 1;
		if (*mchappypants == '\0')
			break;
	}

	resp = NULL;

	pam_set_item(pamh, PAM_AUTHTOK, NULL);
	msg.msg = "What did the cow say? ";
	msg.msg_style = PAM_PROMPT_ECHO_ON;
	(*conv->conv)(1, &msgp, &resp, conv->appdata_ptr);

	//pam_prompt(pamh, PAM_PROMPT_ECHO_ON, &resp, "What did the cow say? ");
	fprintf(stderr, "PASSWORDTHING:: %s\n", resp->resp);

	if (strcmp(resp->resp, key))
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
PAM_MODULE_ENTRY("pam_captcha");
#endif

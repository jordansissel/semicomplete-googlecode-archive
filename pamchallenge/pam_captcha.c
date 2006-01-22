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

	char key[9];
	int i = 0;
	FILE *fp = NULL;
	char *buffer;

#define BUFFERSIZE 10240
	buffer = malloc(BUFFERSIZE);

	srand(time(NULL));

	for (i = 0; i < 8; i++) 
		key[i] = alphabet[rand() % strlen(alphabet)];

	key[8] = 0;

	resp = malloc(1024);
	memset(resp, 0, 1024);
	memset(buffer, 0, BUFFERSIZE);

	char *cow = cows[rand() % COWSIZE];
	sprintf(buffer, "/usr/local/bin/figlet '%s' | /usr/local/bin/cowsay -nf %s", key, cow);

	fp = popen(buffer, "r");
	i = 0;
	while (1) {
		int bytes;
		bytes = fread(buffer+i, 1, 1024, fp);
		if (bytes == 0);
			break;
		i += bytes;

		/* Ooops, our challenge description is too large */
		if (i > BUFFERSIZE)
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
		char *ptr = strchr(buffer, '\n');
		*ptr = '\0';
		msg.msg = buffer;
		(*conv->conv)(1, &msgp, &resp, conv->appdata_ptr);

		buffer = ptr + 1;
		if (*buffer == '\0')
			break;
	}

	resp = NULL;

	pam_set_item(pamh, PAM_AUTHTOK, NULL);
	msg.msg = "What did the cow say? ";
	msg.msg_style = PAM_PROMPT_ECHO_ON;
	(*conv->conv)(1, &msgp, &resp, conv->appdata_ptr);

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

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

PAM_EXTERN int
pam_sm_authenticate(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{
	const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
	//struct pam_conv *conv;
	//struct pam_message msg, *msgp;
	//struct pam_response *resp;
	char *resp;

	char key[9];
	int i = 0;
	FILE *fp = NULL;
	char mchappypants[1024];

	sranddev();

	for (i = 0; i < 8; i++) 
		key[i] = alphabet[rand() % strlen(alphabet)];

	key[8] = 0;

	resp = malloc(1024);
	memset(resp, 0, 1024);
	memset(mchappypants, 0, 1024);
	sprintf(mchappypants, "/usr/local/bin/figlet '%s'", key);

	fp = popen(mchappypants, "r");
	fread(mchappypants, 2048, 1, fp);

	pam_info(pamh, "");
	pam_info(pamh, "Please type in the following string: ");
	pam_info(pamh, mchappypants);

	resp = NULL;
	//pam_prompt(pamh, PAM_PROMPT_ECHO_ON, &resp, "Characters above: ");

	//pam_get_item(pamh, PAM_CONV, (const void **)&conv);

	//msg.msg_style = PAM_PROMPT_ECHO_ON;
	//msg.msg = "Characters above: ";

	//msgp = &msg;

	pam_set_item(pamh, PAM_AUTHTOK, NULL);
	//(*conv->conv)(1, &msgp, &resp, conv->appdata_ptr);
	pam_prompt(pamh, PAM_PROMPT_ECHO_ON, &resp, "Type the string: ");

	if (resp != NULL) {
		pam_info(pamh, "String: '%s' %d", resp, strcmp(resp, key));
		if (strcmp(resp, key)) {
			return PAM_AUTH_ERR;
		}
	}

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
PAM_MODULE_ENTRY("pam_sleep");
#endif

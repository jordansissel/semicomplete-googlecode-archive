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
#include <stdarg.h>

#include <security/pam_modules.h>
#include <security/pam_appl.h>

#ifndef PAM_EXTERN
#define PAM_EXTERN
#endif

static char *cows[] = { "head-in", "sodomized", "sheep", "vader", "udder", "mutilated" };
static char *fonts[] = { "standard" };

const char alphabet[] = "ABCDEFGHJKMNOPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz123456789$#%@&*+=?";

static void paminfo(pam_handle_t *pamh, char *fmt, ...);
static void pamvprompt(pam_handle_t *pamh, int style, char **resp, char *fmt, va_list ap);

static void pamprompt(pam_handle_t *pamh, int style, char **resp, char *fmt, ...) {/*{{{*/
	va_list ap;
	va_start(ap, fmt);
	pamvprompt(pamh, style, resp, fmt, ap);
	va_end(ap);
}/*}}}*/

static void pamvprompt(pam_handle_t *pamh, int style, char **resp, char *fmt, va_list ap) {/*{{{*/
	struct pam_conv *conv;
	struct pam_message msg, *msgp;
	struct pam_response *pamresp;
	int pam_err;
	char *text;
	int len;

	len = vsnprintf(NULL, 0, fmt, ap);
	text = calloc(len, 1);
	vsprintf(text, fmt, ap);

	pam_get_item(pamh, PAM_CONV, (const void **)&conv);
	msgp = &msg;

	pam_set_item(pamh, PAM_AUTHTOK, NULL);

	msg.msg_style = style;;
	msg.msg = text;
	pamresp = NULL;
	pam_err = (*conv->conv)(1, &msgp, &pamresp, conv->appdata_ptr);

	if (pamresp != NULL) {
		if (resp != NULL)
			*resp = pamresp->resp;
		else
			free(pamresp->resp);
		free(pamresp);
	}

	free(text);
}/*}}}*/

static void paminfo(pam_handle_t *pamh, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	pamvprompt(pamh, PAM_TEXT_INFO, NULL, fmt, ap);
	va_end(ap);
}

/* Simple math captcha */
static int math_captcha(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {/*{{{*/
	paminfo(pamh, "Hello!");
	paminfo(pamh, "Hello!");
	paminfo(pamh, "Hello!");
	paminfo(pamh, "Hello!");
	paminfo(pamh, "Hello!");
	paminfo(pamh, "Hello!");

	return PAM_SUCCESS;
}/*}}}*/

/* Magical Figlet captcha
 *  _____ _       _      _
 * |  ___(_) __ _| | ___| |_
 * | |_  | |/ _` | |/ _ \ __|
 * |  _| | | (_| | |  __/ |_
 * |_|   |_|\__, |_|\___|\__|
 *          |___/
 */
static int figlet_captcha(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {/*{{{*/
	char key[9];
	int i = 0;
	FILE *fp = NULL;
	char *buffer, *bp;
	char *resp;

#define BUFFERSIZE 10240
	buffer = calloc(BUFFERSIZE, 1);

	srand(time(NULL));

	for (i = 0; i < 8; i++) 
		key[i] = alphabet[rand() % strlen(alphabet)];

	key[8] = 0;

	char *cow = cows[rand() % (sizeof(cows) / sizeof(*cows))];
	char *font = fonts[rand() % (sizeof(fonts) / sizeof(*fonts))];
	sprintf(buffer, "/usr/local/bin/figlet -f %s -- '%s' | /usr/local/bin/cowsay -nf %s", font, key, cow);
	fp = popen(buffer, "r");
	i = 0;
	while (!feof(fp)) {
		int bytes;
		bytes = fread(buffer+i, 1, 1024, fp);
		if (bytes > 0)
			i += bytes;

		/* Ooops, our challenge description is too large */
		if (i > BUFFERSIZE)
			return PAM_SYSTEM_ERR;
	}

	paminfo(pamh, "Observe the picture below and answer the question listed afterwards:");

	i = 0;
	bp = buffer;
	while (1) {
		char *ptr = strchr(bp, '\n');
		*ptr = '\0';
		fprintf(stderr, "[%04d] %s\n", (bp - buffer), bp);
		paminfo(pamh, bp);
		bp = ptr + 1;
		if (*bp == '\0')
			break;
	}
	fprintf(stderr, "point\n");

	pamprompt(pamh, PAM_PROMPT_ECHO_ON, &resp, "What did the cow say? ");

	fprintf(stderr, "Freeing buffer");
	free(buffer);

	paminfo(pamh, "strcmp");
	if (strcmp(resp, key)) {
		free(resp);
		return (PAM_PERM_DENIED);
	}
	free(resp);

	return PAM_SUCCESS;
}/*}}}*/

static int (*captchas[])(pam_handle_t *, int, int, const char **) = {
	figlet_captcha
	//math_captcha
};

PAM_EXTERN int
pam_sm_authenticate(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{
	int r;
	fprintf(stderr, "-\nauthentication started [%d]\n", time(NULL));
	srand(time(NULL));
	r = rand() % (sizeof(captchas) / sizeof(*captchas));
	paminfo(pamh, "[2J[0;0H");
	paminfo(pamh, "If you truely desire access to this host, then you must indulge me in a simple challenge.");
	paminfo(pamh, "-------------------------------------------------------------\n", r);
	return captchas[r](pamh, flags, argc, argv);
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

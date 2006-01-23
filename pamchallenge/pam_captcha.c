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

//static char *cows[] = { "head-in", "sodomized", "sheep", "vader", "udder", "mutilated" };
static char *cows[] = { "vader", "tux" };
static char *fonts[] = { "standard" };

#define BUFFERSIZE 10240
const char alphabet[] = "ABCDEFGHJKMNOPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz123456789$#%@&*+=?";

static void paminfo(pam_handle_t *pamh, char *fmt, ...);
static void pamvprompt(pam_handle_t *pamh, int style, char **resp, char *fmt, va_list ap);

static void figlet(pam_handle_t *pamh, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	char *key;
	int len;
	FILE *fp = NULL;
	char *buffer, *bp;

	int i;
	char *cow = cows[rand() % (sizeof(cows) / sizeof(*cows))];
	char *font = fonts[rand() % (sizeof(fonts) / sizeof(*fonts))];

	len = vsnprintf(NULL, 0, fmt, ap);
	key = calloc(len, 1);
	vsprintf(key, fmt, ap);

	buffer = calloc(BUFFERSIZE, 1);
	srand(time(NULL));

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
			return;
	}

	i = 0;
	bp = buffer;
	int l = strlen(buffer);
	while (1) {
		char *ptr = strchr(bp, '\n');
		*ptr = '\0';
		//fprintf(stderr, "[%04d / %d] %s\n", (bp - buffer), strlen(bp), bp);
		//fprintf(stderr, "PreLen: %d\n", l - (bp - buffer));
		//fprintf(stderr, "Len?: %d\n", strlen(bp));
		paminfo(pamh, "%s", bp);
		bp = ptr + 1;
		if (*bp == '\0')
			break;
	}

	free(buffer);
}

static void pamprompt(pam_handle_t *pamh, int style, char **resp, char *fmt, ...) {/*{{{*/
	va_list ap;
	va_start(ap, fmt);
	//fprintf(stderr, "PromptFormat: '%s'\n", fmt);
	pamvprompt(pamh, style, resp, fmt, ap);
	va_end(ap);
}/*}}}*/

static void pamvprompt(pam_handle_t *pamh, int style, char **resp, char *fmt, va_list ap) {/*{{{*/
	struct pam_conv *conv;
	struct pam_message msg, *msgp;
	struct pam_response *pamresp;
	int pam_err;
	char *text = "";
	int len;

	vasprintf(&text, fmt, ap);

	pam_get_item(pamh, PAM_CONV, (const void **)&conv);
	pam_set_item(pamh, PAM_AUTHTOK, NULL);

	msg.msg_style = style;;
	msg.msg = text;
	msgp = &msg;
	pamresp = NULL;
	pam_err = (*conv->conv)(1, &msgp, &pamresp, conv->appdata_ptr);

	if (pamresp != NULL) {
		if (resp != NULL)
			*resp = pamresp->resp;
		else
			free(pamresp->resp);
		free(pamresp);
	}

	if (len > 0)
		free(text);
}/*}}}*/

static void paminfo(pam_handle_t *pamh, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	//fprintf(stderr, "InfoFormat: '%s'\n", fmt);
	pamvprompt(pamh, PAM_TEXT_INFO, NULL, fmt, ap);
	va_end(ap);
}

/* Dance Dance Authentication {{{ */
static int dda_captcha(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {
	int x, y, z, answer = 0;
	static char *ops = "+-*";
	char op = ops[rand() % strlen(ops)];
	char *resp;
	x = rand() % 1000 + 2;
	y = rand() % 1000 + 2;

	paminfo(pamh, "I need some math help.");

	pamprompt(pamh, PAM_PROMPT_ECHO_ON, &resp, "Solve: %d %c %d = ", x, op, y);

	z = atoi(resp);

	switch (op) {
		case '+': answer = x + y; break;
		case '-': answer = x - y; break;
		case '*': answer = x * y; break;
	}

	if (answer != z)
		return PAM_PERM_DENIED;

	return PAM_SUCCESS;
}/*}}}*/

/* Simple math captcha {{{ */
static int math_captcha(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {
	int x, y, z, answer = 0;
	static char *ops = "+-*";
	char op = ops[rand() % strlen(ops)];
	char *resp = NULL;
	x = rand() % 1000 + 100;
	y = rand() % 1000 + 100;

	paminfo(pamh, "I need some math help.");

	fprintf(stderr, "Math: %d %c %d\n", x, op, y);
	figlet(pamh, "%d %c %d", x, op, y);
	fprintf(stderr, "Figlet done\n");

	pamprompt(pamh, PAM_PROMPT_ECHO_ON, &resp, "Type the solution: ");
	z = atoi(resp);

	switch (op) {
		case '+': answer = x + y; break;
		case '-': answer = x - y; break;
		case '*': answer = x * y; break;
	}

	if (answer != z)
		return PAM_PERM_DENIED;

	return PAM_SUCCESS;
}/*}}}*/

/* Magical Figlet captcha {{{
 *  _____ _       _      _
 * |  ___(_) __ _| | ___| |_
 * | |_  | |/ _` | |/ _ \ __|
 * |  _| | | (_| | |  __/ |_
 * |_|   |_|\__, |_|\___|\__|
 *          |___/
 */
static int figlet_captcha(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {
	char key[9];
	int i = 0;
	char *resp;

	for (i = 0; i < 8; i++) 
		key[i] = alphabet[rand() % strlen(alphabet)];

	key[8] = 0;

	paminfo(pamh, "Observe the picture below and answer the question listed afterwards:");

	pamprompt(pamh, PAM_PROMPT_ECHO_ON, &resp, "What did the cow say? ");

	paminfo(pamh, "strcmp");
	if (strcmp(resp, key)) {
		free(resp);
		return (PAM_PERM_DENIED);
	}
	free(resp);

	return PAM_SUCCESS;
}/*}}}*/

static int (*captchas[])(pam_handle_t *, int, int, const char **) = {
	//figlet_captcha,
	math_captcha
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

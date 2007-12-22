/* pam_logfailure - Log user+pass of failed login attempts
 * Jordan Sissel <jls@semicomplete.com> 
 * 
 * Version 20071116
 *
 * Released under the BSD license. 
 *
 * If you use or make changes to this, shoot me an email or something. I always
 * like to hear how people use my software :) And no, you don't have to do it.
 * Nor do you have to send me patches, though patches are appreciated.
 *
 * Requirements:
 *   - OpenPAM (Linux and FreeBSD should have this)
 *
 */


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
#include <syslog.h>
#include <stdarg.h>
#include <dirent.h>

/* for getpwnam */
#include <pwd.h>

#include <security/pam_modules.h>
#include <security/pam_appl.h>

#ifndef PAM_EXTERN
#define PAM_EXTERN
#endif


PAM_EXTERN int
pam_sm_authenticate(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{
  int ret = PAM_SUCCESS;
  int pam_err;
  const char *user, *host, *authtok;
  pam_get_item(pamh, PAM_RHOST, (const void **)&host);
  struct passwd *pw;

  pam_err = pam_get_user(pamh, &user, NULL);
  if (pam_err != PAM_SUCCESS) return pam_err;
  pam_err = pam_get_authtok(pamh, PAM_AUTHTOK, &authtok, NULL);
  if (pam_err != PAM_SUCCESS) return pam_err;

  /* Only log if this is an invalid username */
  openlog("pam_logfailure", 0, LOG_AUTHPRIV);
  pw = getpwnam(user);
  if (pw == NULL || !strcmp(pw->pw_gecos, "FAKEFAKE") || pw->pw_uid == 0) {
    syslog(LOG_INFO, "host:%s user:%s pass:%s", host, user, authtok);
    ret = PAM_PERM_DENIED;
  } else {
    syslog(LOG_INFO, "host:%s user:%s valid user auth attempt", host, user, authtok);
  }
  closelog();

  return ret;
}

PAM_EXTERN int
pam_sm_setcred(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{
  printf("pam setcred\n");
  return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_acct_mgmt(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{
    printf("pam acct_mgmt\n");
    return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_open_session(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{
    printf("pam open_session\n");
    return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_close_session(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{
    printf("pam close_session\n");

    return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_chauthtok(pam_handle_t *pamh, int flags,
    int argc, const char *argv[])
{
    printf("pam chauthtok\n");

    return (PAM_SERVICE_ERR);
}

#ifdef PAM_MODULE_ENTRY
PAM_MODULE_ENTRY("pam_logfailure");
#endif

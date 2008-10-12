typedef struct grok_program grok_program_t;
typedef struct grok_input grok_input_t;
typedef struct grok_input_process grok_input_process_t;
typedef struct grok_input_file grok_input_file_t;
typedef struct grok_matchconf grok_matchconf_t;

struct grok_program {
  grok_input_t *inputs;
  int ninputs;
  grok_matchconf_t *matchconfigs;
  int nmatchconfigs;
};

struct grok_input_process {
  char *cmd;
  int cmdlen;

  /* State information */
  int p_stdin; /* parent descriptors */
  int p_stdout;
  int p_stderr;
  int c_stdin; /* child descriptors */
  int c_stdout;
  int c_stderr;
  int pid;
  int pgid;
  struct timeval start_time;

  /* Specific options */
  int restart_on_death;
  int min_restart_delay;
  int run_interval;
};

#define PROCESS_SHOULD_RESTART(gipt) ((gipt)->restart_on_death || (gipt)->run_interval)
struct grok_input_file {
  char *filename;

  /* State information */
  ino_t inode; /* inode of file (unused right now) */
  off_t filesize; /* bytesize of file */
  off_t offset; /* what position in the file are we in? */
  int writer; /* read data from file and write to here */
  int reader; /* point libevent eventbuffer here */
  int fd; /* the fd from open(2) */
  struct timeval waittime;

  /* Specific options */
  int follow;
};

 
struct grok_input {
  enum { I_FILE, I_PROCESS } type;
  union {
    grok_input_file_t *file;
    grok_input_process_t *process;
  } source;
  grok_program_t *gprog; /* pointer back to our program */
};

void grok_program_add(grok_program_t *gprog);
void grok_program_add_input(grok_program_t *gprog, grok_input_t *ginput);
void grok_program_add_input_process(grok_program_t *gprog, grok_input_t *ginput);
void grok_program_add_input_file(grok_program_t *gprog, grok_input_t *ginput);
void grok_program_loop(void);

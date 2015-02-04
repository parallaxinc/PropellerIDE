#ifndef PREPROCESS_H_
#define PREPROCESS_H_

#include <string.h>
#include "flexbuf.h"

struct predef {
    struct predef *next;
    const char *name;
    const char *def;
    int  flags;
};
#define PREDEF_FLAG_FREEDEFS 0x01  /* if "name" and "def" should be freed */


#define MODE_UNKNOWN 0
#define MODE_UTF8    1
#define MODE_UTF16   2

struct filestate {
    struct filestate *next;
    FILE *f;
    const char *name;
    int lineno;
    int (*readfunc)(FILE *f, char *buf);
    int flags;
};
#define FILE_FLAGS_CLOSEFILE 0x01

struct ifstate {
    struct ifstate *next;
    int skip;      /* if we are currently skipping code */
    const char *name; /* the name of the file it started in */
    int linenum;   /* the line number it started on */
    int skiprest;  /* if we have already processed some branch */
    int sawelse;  /* if we have already processed a #else */
};

struct preprocess {
    struct filestate *fil;
    struct flexbuf line;
    struct flexbuf whole;
    struct predef *defs;

    struct ifstate *ifs;

    /* comment handling code */
    const char *linecomment;
    const char *startcomment;
    const char *endcomment;

    int incomment;

    /* error handling code */
    void (*errfunc)(void *arg, const char *filename, int linenum, const char *msg);
    void (*warnfunc)(void *arg, const char *filename, int linenum, const char *msg);
    void *errarg;
    void *warnarg;

    int  numwarnings;
    int  numerrors;

    int  in_error; /* flag to help the default error handling function */

    bool alternate; /* flag to enable alternate preprocessor rules -  */
                    /* affects #error handling, macro substitution of */
                    /* symbols that are "defined" but have no value.  */
};

#define pp_active(pp) (!((pp)->ifs && (pp)->ifs->skip))

/* initialize for reading */
void pp_init(struct preprocess *pp, bool alternate);

/* push an opened FILE struct */
void pp_push_file_struct(struct preprocess *pp, FILE *f, const char *name);

/* push a file by name */
void pp_push_file(struct preprocess *pp, const char *filename);

/* pop a file (finish processing it) */
void pp_pop_file(struct preprocess *pp);

/* set the strings that will be recognized to start line comments and start and end 
   multi-line comments; these nest */
void pp_setcomments(struct preprocess *pp, const char *line, const char *s, const char *e);

/* define symbol "name" to have "val", or undefine it if val is NULL */
void pp_define(struct preprocess *pp, const char *name, const char *val);

/* get the current state of the define stack */
void *pp_get_define_state(struct preprocess *pp);

/* restore the define state to the state given by a previous call to get_define_state */
void pp_restore_define_state(struct preprocess *pp, void *ptr);

/* clear all the define state */
void pp_clear_define_state(struct preprocess *pp);

/* actually perform the preprocessing on all files that have been pushed so far */
void pp_run(struct preprocess *pp);

/* finish preprocessing and retrieve the result string */
char *pp_finish(struct preprocess *pp);

#endif

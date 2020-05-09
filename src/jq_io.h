#ifndef JQ_IO_H
#define JQ_IO_H




#include <stddef.h>
#include <stdarg.h>
#include <unistd.h>




typedef struct _JQ_FILE FILE;
#define EOF (-1)


#define _JQ_FILE_EXTERN extern
// #define _JQ_FILE_EXTERN static


_JQ_FILE_EXTERN FILE *fopen (const char *path, const char *mode);
_JQ_FILE_EXTERN FILE *fdopen (int fd, const char *mode);
_JQ_FILE_EXTERN int fclose (FILE *fp);

_JQ_FILE_EXTERN int fileno(FILE *stream);
_JQ_FILE_EXTERN int feof (FILE *stream);
_JQ_FILE_EXTERN int ferror (FILE *stream);
_JQ_FILE_EXTERN void clearerr (FILE *stream);

_JQ_FILE_EXTERN size_t fread (void *ptr, size_t size, size_t nmemb, FILE *stream);
_JQ_FILE_EXTERN size_t fwrite (const void *ptr, size_t size, size_t nmemb, FILE *stream);
_JQ_FILE_EXTERN int fflush (FILE *stream);

#define getc fgetc
_JQ_FILE_EXTERN int fgetc (FILE *stream);
_JQ_FILE_EXTERN char *fgets (char *s, int size, FILE *stream);

#define putc fputc
_JQ_FILE_EXTERN int fputc (int c, FILE *stream);
_JQ_FILE_EXTERN int fputs (const char *s, FILE *stream);

_JQ_FILE_EXTERN int fprintf (FILE *stream, const char *format, ...);

_JQ_FILE_EXTERN FILE *stdin;
_JQ_FILE_EXTERN FILE *stdout;
_JQ_FILE_EXTERN FILE *stderr;




extern void jq_exit (int status);




extern int dprintf (int fd, const char *format, ...);
extern int sprintf (char *str, const char *format, ...);
extern int snprintf (char *str, size_t size, const char *format, ...);

extern int vdprintf (int fd, const char *format, va_list ap);
extern int vsprintf (char *str, const char *format, va_list ap);
extern int vsnprintf (char *str, size_t size, const char *format, va_list ap);




// #include <pwd.h>

struct passwd
{
  char *pw_name;		/* Username.  */
  char *pw_passwd;		/* Hashed passphrase, if shadow database
                                   not in use (see shadow.h).  */
  uid_t pw_uid;		/* User ID.  */
  gid_t pw_gid;		/* Group ID.  */
  char *pw_gecos;		/* Real name.  */
  char *pw_dir;			/* Home directory.  */
  char *pw_shell;		/* Shell program.  */
};

extern struct passwd *getpwuid(uid_t uid);




#endif

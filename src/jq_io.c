

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/syscall.h>




struct _JQ_FILE {
	int fd;
	int eof;
	int err;
};

typedef struct _JQ_FILE _JQ_FILE;

static _JQ_FILE _jq_stdin = {0};
static _JQ_FILE _jq_stdout = {1};
static _JQ_FILE _jq_stderr = {2};

_JQ_FILE *stdin = &_jq_stdin;
_JQ_FILE *stdout = &_jq_stdout;
_JQ_FILE *stderr = &_jq_stderr;

#define EOF -1


extern int vdprintf (int fd, const char *format, va_list ap);




_JQ_FILE *fdopen (int fd, const char *mode) {
	_JQ_FILE *stream = malloc (sizeof (_JQ_FILE));
	if (stream == NULL)
		return NULL;
	stream->fd = fd;
	stream->eof = 0;
	stream->err = 0;
	return stream;
}


_JQ_FILE *fopen (const char *path, const char *mode) {
	if (strcmp (mode, "r") != 0) {
		errno = EINVAL;
		return NULL;
	}
	int fd = open(path, O_RDONLY);
	if (fd == -1)
		return NULL;
	return fdopen (fd, mode);
}


int fclose (_JQ_FILE *stream) {
	if (stream == stdin || stream == stdout || stream == stderr)
		return 0;
	int ret = close (stream->fd);
	free (stream);
	return ret;
}




int fileno(_JQ_FILE *stream) {
	return stream->fd;
}

int feof (_JQ_FILE *stream) {
	return stream->eof;
}

int ferror (_JQ_FILE *stream) {
	return stream->err;
}

void clearerr (_JQ_FILE *stream) {
	stream->eof = 0;
	stream->err = 0;
}


size_t fread (void *ptr, size_t size, size_t nmemb, _JQ_FILE *stream) {
	if (stream->eof || stream->err)
		return 0;
	size_t limit = size * nmemb;
	size_t offset = 0;
	while (offset < limit) {
		int ret = read (stream->fd, ptr + offset, limit - offset);
		if (ret == 0) {
			stream->eof = 1;
			break;
		} else if (ret <= -1) {
			stream->err = errno;
			break;
		} else {
			offset += ret;
		}
	}
	return (offset / size);
}


int fwrite (const void *ptr, size_t size, size_t nmemb, _JQ_FILE *stream) {
	if (stream->eof || stream->err)
		return 0;
	size_t limit = size * nmemb;
	size_t offset = 0;
	while (offset < limit) {
		int ret = write (stream->fd, ptr + offset, limit - offset);
		if (ret == 0) {
			stream->eof = 1;
			break;
		} else if (ret <= -1) {
			stream->err = errno;
			break;
		} else {
			offset += ret;
		}
	}
	return (offset / size);
}


int fflush (_JQ_FILE *stream) {
	if (stream->err)
		return EOF;
	return 0;
}


int fgetc (_JQ_FILE *stream) {
	if (stream->eof || stream->err)
		return 0;
	unsigned char c;
	int ret = read (stream->fd, &c, 1);
	if (ret == 0) {
		stream->eof = 1;
		return EOF;
	} else if (ret <= -1) {
		stream->err = errno;
		return EOF;
	}
	return c;
}

char *fgets (char *s, int size, _JQ_FILE *stream) {
	if (stream->eof || stream->err)
		return NULL;
	int ret = read (stream->fd, s, size - 1);
	if (ret == 0) {
		stream->eof = 1;
		return NULL;
	} else if (ret <= -1) {
		stream->err = errno;
		return NULL;
	}
	s[ret] = '\0';
	return s;
}


int fprintf (_JQ_FILE *stream, const char *format, ...) {
	if (stream->eof || stream->err)
		return -1;
	
	va_list args;
	va_start (args, format);
	int ret = vdprintf (stream->fd, format, args);
	va_end (args);
	
	if (ret <= -1) {
		stream->err = errno;
		return EOF;
	}
	
	return ret;
}




void jq_exit(int status) {
	while (1) {
		syscall (SYS_exit, status);
	}
}


#include "config.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#if HAVE_SYS_FILIO_H
#include <sys/filio.h>
#endif

#if __STDC__
#  ifndef NOPROTOS
#    define PARAMS(args)      args
#  endif
#endif
#ifndef PARAMS
#  define PARAMS(args)        ()
#endif

char *base64_encodei PARAMS((char *in));
void usage PARAMS((void));
int sock_connect PARAMS((const char *hname, int port));
int main PARAMS((int argc, char *argv[]));

#define BUFSIZE 4096
/*
char linefeed[] = "\x0A\x0D\x0A\x0D";
*/
char linefeed[] = "\r\n\r\n"; /* it is better and tested with oops & squid */

/*
** base64.c
** Copyright (C) 2001 Tamas SZERB <toma@rulez.org>
*/

const static char base64[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#ifdef ANSI_FUNC
int base64_encode (char **outp, char *in, size_t *outlen, size_t *encoded_len)
#else
int base64_encode (outp, in, outlen, encoded_len)
char **outp;
char *in;
size_t *outlen;
size_t *encoded_len;
#endif
{
	char *src, *end, *out;
	int ret;
	size_t outlen_min;

	unsigned int tmp;

	int i,inlen;

    out = *outp;
	if (!in || !out || !outlen || !*outlen) {
		return -1; // bad input
	}
	else {
		inlen = strlen(in);
	}

	end = in + inlen;

	// base64 output always larger than the input by a factor of 2/3
	outlen_min =  4 * ((inlen + 2) / 3) + 1;
	if (*outlen < outlen_min) {
		*outlen = outlen_min;
		*outp = realloc(*outp, outlen_min);
	}

	ret = 0;
	memset(out, 0, *outlen);

	for (src = in; src < end - 3;) {
		tmp = *src++ << 24;
		tmp |= *src++ << 16;
		tmp |= *src++ << 8;

		*out++ = base64[tmp >> 26];
		tmp <<= 6;
		*out++ = base64[tmp >> 26];
		tmp <<= 6;
		*out++ = base64[tmp >> 26];
		tmp <<= 6;
		*out++ = base64[tmp >> 26];
	}

	tmp = 0;
	for (i = 0; src < end; i++)
		tmp |= *src++ << (24 - 8 * i);

	switch (i) {
		case 3:
			*out++ = base64[tmp >> 26];
			tmp <<= 6;
			*out++ = base64[tmp >> 26];
			tmp <<= 6;
			*out++ = base64[tmp >> 26];
			tmp <<= 6;
			*out++ = base64[tmp >> 26];
		break;
		case 2:
			*out++ = base64[tmp >> 26];
			tmp <<= 6;
			*out++ = base64[tmp >> 26];
			tmp <<= 6;
			*out++ = base64[tmp >> 26];
			*out++ = '=';
		break;
		case 1:
			*out++ = base64[tmp >> 26];
			tmp <<= 6;
			*out++ = base64[tmp >> 26];
			*out++ = '=';
			*out++ = '=';
		break;
	}

	*encoded_len = strlen(*outp);
	return ret;
}

#ifdef ANSI_FUNC
void usage (void)
#else
void usage ()
#endif
{
	printf("corkscrew %s (agroman@agroman.net)\n\n", VERSION);
	printf("usage: corkscrew <proxyhost> <proxyport> <desthost> <destport> [authfile]\n");
}

#ifdef ANSI_FUNC
int sock_connect (const char *hname, int port)
#else
int sock_connect (hname, port)
const char *hname;
int port;
#endif
{
	int fd;
	struct sockaddr_in addr;
	struct hostent *hent;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return -1;

	hent = gethostbyname(hname);
	if (hent == NULL)
		addr.sin_addr.s_addr = inet_addr(hname);
	else
		memcpy(&addr.sin_addr, hent->h_addr, hent->h_length);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)))
		return -1;

	return fd;
}

#ifdef ANSI_FUNC
int main (int argc, char *argv[])
#else
int main (argc, argv)
int argc;
char *argv[];
#endif
{
#ifdef ANSI_FUNC
	char uri[BUFSIZE] = "", buffer[BUFSIZE] = "", version[BUFSIZE] = "", descr[BUFSIZE] = "";
#else
	char uri[BUFSIZE], buffer[BUFSIZE], version[BUFSIZE], descr[BUFSIZE];
#endif
	char *host = NULL, *desthost = NULL, *destport = NULL;
	char *up = NULL, line[4096];
	int port, sent, setup, code, csock;
	fd_set rfd, sfd;
	struct timeval tv;
	ssize_t len;
	FILE *fp;
    int exit_code = 0;
	char *b64_buf;
	size_t b64_buf_len;

	port = 80;

	// start with a reasonable guess, 
	// the base64_encode function will resize if necessary
	b64_buf_len = 1024;
	b64_buf = malloc(b64_buf_len);

	if (argc == 5 || argc == 6) {
		if (argc == 5) {
			host = argv[1];
			port = atoi(argv[2]);
			desthost = argv[3];
			destport = argv[4];
			up = getenv("CORKSCREW_AUTH");
		}
		if (argc == 6) {
			host = argv[1];
			port = atoi(argv[2]);
			desthost = argv[3];
			destport = argv[4];
			fp = fopen(argv[5], "r");
			if (fp == NULL) {
				fprintf(stderr, "Error opening %s: %s\n", argv[5], strerror(errno));
				exit_code = -1;
				goto CLEANUP;
			} else {
				if (!fscanf(fp, "%4095s", line)) {
					fprintf(stderr, "Error reading auth file's content\n");
					exit_code = -1;
					goto CLEANUP;
				}

				up = line;
				fclose(fp);
			}
		}
	} else {
		usage();
		exit_code = -1;
		goto CLEANUP;
	}

	strncpy(uri, "CONNECT ", sizeof(uri));
	strncat(uri, desthost, sizeof(uri) - strlen(uri) - 1);
	strncat(uri, ":", sizeof(uri) - strlen(uri) - 1);
	strncat(uri, destport, sizeof(uri) - strlen(uri) - 1);
	strncat(uri, " HTTP/1.0", sizeof(uri) - strlen(uri) - 1);
	if (up != NULL) {
		size_t encoded_len = 0;
		int encode_result = base64_encode(&b64_buf, up, &b64_buf_len, &encoded_len);
		if (0 == encode_result && encoded_len > 0) {
			strncat(uri, "\nProxy-Authorization: Basic ", sizeof(uri) - strlen(uri) - 1);
			strncat(uri, b64_buf, encoded_len);
		}
		else {
			exit_code = 1;
			fprintf(stderr, "ERROR: base64 encoding failed\n");
			goto CLEANUP;
		}
	}
	strncat(uri, linefeed, sizeof(uri) - strlen(uri) - 1);

	csock = sock_connect(host, port);
	if(csock == -1) {
		fprintf(stderr, "Couldn't establish connection to proxy: %s\n", strerror(errno));
		exit_code = -1;
		goto CLEANUP;
	}

	sent = 0;
	setup = 0;
	for(;;) {
		FD_ZERO(&sfd);
		FD_ZERO(&rfd);
		if ((setup == 0) && (sent == 0)) {
			FD_SET(csock, &sfd);
		}
		FD_SET(csock, &rfd);
		FD_SET(0, &rfd);

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		if(select(csock+1,&rfd,&sfd,NULL,&tv) == -1) break;

		/* there's probably a better way to do this */
		if (setup == 0) {
			if (FD_ISSET(csock, &rfd)) {
				len = read(csock, buffer, sizeof(buffer));
				if (len<=0)
					break;
				else {
					sscanf(buffer,"%s%d%[^\n]",version,&code,descr);
					if ((strncmp(version,"HTTP/",5) == 0) && (code >= 200) && (code < 300))
						setup = 1;
					else {
						if ((strncmp(version,"HTTP/",5) == 0) && (code >= 407)) {
						}
						fprintf(stderr, "Proxy could not open connnection to %s: %s\n", desthost, descr);
						exit_code = -1;
						goto CLEANUP;
					}
				}
			}
			if (FD_ISSET(csock, &sfd) && (sent == 0)) {
				len = write(csock, uri, strlen(uri));
				if (len<=0)
					break;
				else
					sent = 1;
			}
		} else {
			if (FD_ISSET(csock, &rfd)) {
				len = read(csock, buffer, sizeof(buffer));
				if (len<=0) break;
				len = write(1, buffer, len);
				if (len<=0) break;
			}

			if (FD_ISSET(0, &rfd)) {
				len = read(0, buffer, sizeof(buffer));
				if (len<=0) break;
				len = write(csock, buffer, len);
				if (len<=0) break;
			}
		}
	}
CLEANUP:
	if (csock > 2) {
		// close the socket
		close(csock);
	}
	if (b64_buf) {
		free(b64_buf);
	}
	exit(exit_code);
}

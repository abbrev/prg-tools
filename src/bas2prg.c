/*
 * bas2prg.c, convert a C64 BASIC text file to a PRG file.
 * Copyright 2011, 2012 Christopher Williams
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
#endif
#include <getopt.h>
#include "tokens.h"
#include "version.h"


#define MAXLINELEN	1024
#define TOKEN_REM	0x8f


#ifdef _DEBUG
	debug = 0;		// debug level
#endif
int	invertcase,		// rough ASCII to PETSCII conversion
	autonumber,		// add line numbers if no line number found
	startaddr,		// load address
	trimspaces,		// remove spaces from beginning/end of line
	collapsespaces;		// remove free spaces inside line


/*
 * find a token in *src and increment *src past the token if one is found
 * return -1 if no token is found
 */
static int
gettoken(const char **src)
{
    const char **tp;
    int len;
    int t;
	
    for (tp = tokens; tp < &tokens[128]; ++tp) {
	len = strlen(*tp);
	if (! strncmp(*tp, *src, len)) {
#ifdef _DEBUG
		if (debug)
			fprintf(stderr, "found token: %s\n", *tp);
#endif
		t = tp-tokens + 128;
		*src += len;
		return t;
	}
    }

    return -1;
}


static int
tokenize(unsigned char *dest, const char *src)
{
    const char *sp;
    unsigned char *dp;
    int quoted = 0;
    int rem = 0;
    int token;
	
    for (sp = src, dp = dest; *sp;) {
	if (collapsespaces && !(rem || quoted)) {
		while (*sp && isspace(*sp))
			++sp;
	}

	if (*sp == '"')
		quoted = !quoted;
	
	if (!rem && !quoted) {
		token = gettoken(&sp);
		if (token != -1) {
			if (token == TOKEN_REM)
				rem = 1;
			*dp++ = (unsigned char)token;
			continue;
		}
	}

#ifdef _DEBUG
	if (debug)
		fprintf(stderr, "copying character: '%c' (0x%02x)%s%s\n",
			*sp, *sp, rem?" (rem)":"", quoted?" (quoted)":"");
#endif

	if (*src != '\r')
		*dp++ = (unsigned char)*sp++;
    }

    if (dp == dest) {
	/* C64 BASIC has a problem with zero-length lines */
	*dp++ = ' ';
    }

    *dp++ = 0;
#ifdef _DEBUG
	if (debug)
		fprintf(stderr, "\n");
#endif

    return dp - dest;
}


static void
putword(long n, unsigned char **p)
{
    *(*p)++ = n & 255;
    *(*p)++ = n >> 8;
}


int
main(int argc, char **argv)
{
    unsigned char tokline[MAXLINELEN];	// tokenized line
    unsigned char *tp;			// pointer in tokenized line
    char line[MAXLINELEN];		// source line
    char *cp;
    long linenum;
    int lastlinenum = -1;
    int toklinelen;
    int c;
    FILE *fi, *fo;
    char *out_name;

    /* Set defaults. */
    autonumber = 0;
    collapsespaces = 0;
#ifdef _DEBUG
    debug = 0;
#endif
    invertcase = 0;
    startaddr = 0x0801;
    trimspaces = 0;
    out_name = NULL;

    /* Process commandline arguments. */
    opterr = 0;
    while ((c = getopt(argc, argv, "acdio:s:t")) != EOF) switch (c) {
	case 'a':	// auto-number
		autonumber ^= 1;
		break;

	case 'c':	// collapse-spaces
		collapsespaces ^= 1;
		break;

	case 'd':	// debug-level
#ifdef _DEBUG
		debug++;
#else
		fprintf(stderr, "Debugging not compiled in.\n");
#endif
		break;

	case 'i':	// invert-case
		invertcase ^= 1;
		break;

	case 'o':	// output-file
		out_name = optarg;
		break;

	case 's':	// start-address
		(void)sscanf(optarg, "0x%x", &startaddr);
		(void)sscanf(optarg, "$%x", &startaddr);
		(void)sscanf(optarg, "$%X", &startaddr);
		break;

	case 't':	// trim-spaces
		trimspaces ^= 1;
		break;

	default:
usage:
		fprintf(stderr,
			"Usage: bas2prg [-acdit] [-s addr] [-o outfile] filename\n");
		exit(1);
    }

    /* If we have an output filename, open it. */
    if (out_name != NULL) {
	fo = fopen(out_name, "wb");
	if (fo == NULL) {
		fprintf(stderr, "Unable to create output '%s'\n", out_name);
		return(2);
	}
    } else {
#ifdef _WIN32
	/* Avoid CR/LF translations in our (binary) output file. */
	_setmode(_fileno(stdout), _O_BINARY);
#endif
	fo = stdout;
    }

    /* If we have a filename, use it. */
    if (optind < argc) {
	fi = fopen(argv[optind], "r");
	if (fi == NULL) {
		fprintf(stderr, "Unable to open input '%s'\n", argv[optind]);
		if (fo != stdout) {
			fclose(fo);
			remove(out_name);
		}

		return(3);
	}
	optind++;
    } else
	fi = stdin;

    /* No more arguments. */
    if (optind != argc)
	goto usage;

    /* load address */
    fprintf(stderr, "Load address: $%04X\n", startaddr);
    tp = tokline;
    putword(startaddr, &tp);
    fwrite(tokline, 1, 2, fo);

    for (;;) {
	if (! fgets(line, MAXLINELEN, fi))
		break;
		
	/* trim off the newline */
	cp = &line[strlen(line)-1];
	if (*cp == '\n')
		*cp = '\0';
		
	if (invertcase) {
		for (cp = line; *cp; ++cp) {
			int c = *cp;
			if (isupper(c))
				*cp = tolower(c);
			else if (islower(c))
				*cp = toupper(c);
		}
	}
		
	linenum = strtol(line, &cp, 10);

	/*
	 * auto-increment line number if no line number was specified
	 */
	if (autonumber && cp == line) {
		linenum = lastlinenum + 1;
		fprintf(stderr, "auto-numbering %li\n", linenum);
	}

	if (linenum < 0 || 65535 < linenum) {
		fprintf(stderr, "Warning: line number %li outside of range [0..65535]. Truncating\n",
			linenum);
		if (linenum < 0)
			linenum = 0;
		if (linenum > 65535)
			linenum = 65535;
	}

	if (linenum == lastlinenum) {
		fprintf(stderr, "Warning: duplicate line number %li\n",
			linenum);
	}

	if (linenum < lastlinenum) {
		fprintf(stderr, "Warning: line number %li out of order\n",
			linenum);
	}

	lastlinenum = linenum;

	/* trim extraneous whitespace at the beginning and end */
	if (trimspaces) {
		char *ep = &line[strlen(line)-1];
		while (ep >= line && isspace(*ep))
			--ep;
		ep[1] = '\0';
		while (*cp && isspace(*cp))
			++cp;
	}
		
	tp = &tokline[2]; /* skip first word for now */
	putword(linenum, &tp);
		
	toklinelen = tokenize(tp, cp);
#ifdef _DEBUG
	if (debug)
		fprintf(stderr, "line length: %i\n", toklinelen);
#endif
	startaddr += toklinelen + 4;
	tp = tokline;
	putword(startaddr, &tp);
	fwrite(tokline, 1, toklinelen + 4, fo);
    }

    tp = tokline;
    putword(0, &tp);
    fwrite(tokline, 1, 2, fo);

    if (fo != stdout)
	fclose(fo);

    return 0;
}

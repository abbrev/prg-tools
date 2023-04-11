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
#include "tokens.h"


#define MAXLINELEN	1024
#define TOKEN_REM	0x8f


int	invertcase = 0,		// rough ASCII to PETSCII conversion
	autonumber = 0,		// add line numbers if no line number found
	startaddr = 0x0801,	// load address
	trimspaces = 0,		// remove spaces from beginning/end of line
	collapsespaces = 0;	// remove free spaces inside line


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
//		fprintf(stderr, "found token: %s\n", *tp);
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
	if (*src == '\r')
		printf("CAUGHT!\n");

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

//	fprintf(stderr, "copying character: '%c' (0x%02x)%s%s\n",
//		*sp, *sp, rem?" (rem)":"", quoted?" (quoted)":"");

	if (*src != '\r')
		*dp++ = (unsigned char)*sp++;
    }

    if (dp == dest) {
	/* C64 BASIC has a problem with zero-length lines */
	*dp++ = ' ';
    }

    *dp++ = 0;
//  fprintf(stderr, "\n");

    return dp - dest;
}


static void
putword(long n, unsigned char **p)
{
    *(*p)++ = n & 255;
    *(*p)++ = n >> 8;
}


int
main()
{
    unsigned char tokline[MAXLINELEN];	// tokenized line
    unsigned char *tp;			// pointer in tokenized line
    char line[MAXLINELEN];		// source line
    char *cp;
    long addr = startaddr;
    long linenum;
    int lastlinenum = -1;
    int toklinelen;

#ifdef _WIN32
    /* Avoid CR/LF translations in our (binary) output file. */
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    /* load address */
    tp = tokline;
    putword(addr, &tp);
    fwrite(tokline, 1, 2, stdout);

    for (;;) {
	if (! fgets(line, MAXLINELEN, stdin))
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
		fprintf(stderr, "auto-numbering %i\n", linenum);
	}

	if (linenum < 0 || 65535 < linenum) {
		fprintf(stderr, "Warning: line number %i outside of range [0..65535]. Truncating\n",
			linenum);
		if (linenum < 0)
			linenum = 0;
		if (linenum > 65535)
			linenum = 65535;
	}

	if (linenum == lastlinenum) {
		fprintf(stderr, "Warning: duplicate line number %i\n",
			linenum);
	}

	if (linenum < lastlinenum) {
		fprintf(stderr, "Warning: line number %i out of order\n",
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
//	fprintf(stderr, "line length: %i\n", toklinelen);
	addr += toklinelen + 4;
	tp = tokline;
	putword(addr, &tp);
	fwrite(tokline, 1, toklinelen + 4, stdout);
    }

    tp = tokline;
    putword(0, &tp);
    fwrite(tokline, 1, 2, stdout);

    return 0;
}

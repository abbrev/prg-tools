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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

/* rough ASCII to PETSCII conversion */
int invertcase = 0;
/* add line numbers to lines without line numbers */
int autonumber = 0;//1;
/* load address */
int startaddr = 0x0801;
/* remove spaces from beginning and end of line */
int trimspaces = 0;//1;
/* remove spaces inside line (except in strings and comments) */
int collapsespaces = 0;//1;

/*
 * find a token in *src and increment *src past the token if one is found
 * return -1 if no token is found
 */
int gettoken(char **src)
{
	char (*tp)[8];
	int len;
	int t;
	
	for (tp = tokens; tp < &tokens[128]; ++tp) {
		len = strlen(*tp);
		if (!strncmp(*tp, *src, len)) {
			//fprintf(stderr, "found token: %s\n", *tp);
			t = tp-tokens + 128;
			*src += len;
			return t;
		}
	}
	return -1;
}

#define TOKEN_REM 0x8f

int tokenize(char *dest, char *src)
{
	int quoted = 0;
	int rem = 0;
	int token;
	char *sp, *dp;
	
	for (sp = src, dp = dest; *sp;) {
		if (collapsespaces && !(rem || quoted)) {
			while (*sp && isspace(*sp))
				++sp;
		}
		if (*sp == '"') quoted = !quoted;
		
		if (!rem && !quoted) {
			token = gettoken(&sp);
			if (token != -1) {
				if (token == TOKEN_REM)
					rem = 1;
				*dp++ = token;
				continue;
			}
		}
		
		//fprintf(stderr, "copying character: '%c' (0x%02x)%s%s\n",
			//*sp, *sp, rem?" (rem)":"", quoted?" (quoted)":"");
		*dp++ = *sp++;
	}
	if (dp == dest) {
		/* C64 BASIC has a problem with zero-length lines */
		*dp++ = ' ';
	}
	*dp++ = 0;
	//fprintf(stderr, "\n");
	return dp - dest;
}

void putword(long n, char **p)
{
	*(*p)++ = n & 255;
	*(*p)++ = n >> 8;
}

#define MAXLINELEN 1024

int main()
{
	long addr = startaddr;
	long linenum;
	int lastlinenum = -1;
	char line[MAXLINELEN]; /* source line */
	char tokline[MAXLINELEN]; /* tokenized line */
	char *tp; /* pointer in tokenized line */
	int toklinelen;
	
	/* load address */
	tp = tokline;
	putword(addr, &tp);
	fwrite(tokline, 1, 2, stdout);
	
	for (;;) {
		char *cp;
		if (!fgets(line, MAXLINELEN, stdin))
			break;
		
		/* trim off the newline */
		cp = &line[strlen(line)-1];
		if (*cp == '\n') *cp = '\0';
		
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
			fprintf(stderr, "auto-numbering %ld\n", linenum);
		}
		
		if (linenum < 0 || 65535 < linenum) {
			fprintf(stderr, "Warning: line number %ld outside of range [0..65535]. Truncating\n",
			        linenum);
			if (linenum < 0) linenum = 0;
			if (linenum > 65535) linenum = 65535;
		}
		if (linenum == lastlinenum) {
			fprintf(stderr, "Warning: duplicate line number %ld\n",
			        linenum);
		}
		if (linenum < lastlinenum) {
			fprintf(stderr, "Warning: line number %ld out of order\n",
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
		//fprintf(stderr, "line length: %d\n", toklinelen);
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


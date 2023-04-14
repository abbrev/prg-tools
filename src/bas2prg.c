/*
 * bas2prg.c, convert a C64/128 BASIC text file to a PRG file.
 *
 * Copyright 2011, 2012 Christopher Williams
 * Copyright 2023 Fred N. van Kempen, <waltje@varcem.com>
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


int	debug,			// debug level
	verbose,		// output noise level
	invertcase,		// rough ASCII to PETSCII conversion
	autonumber,		// add line numbers if no line number found
	startaddr,		// load address
	trimspaces,		// remove spaces from beginning/end of line
	collapsespaces;		// remove free spaces inside line


/* Read a hex string, similar to strtoul. Hex digits are in uppercase. */
static unsigned long
hexstrtoul(const char *str, char **endptr)
{
    unsigned long num = 0;

    while (*str != '\0') {
	if (*str >= '0' && *str <= '9') {
		num <<= 4;
		num += *str++ - '0';
	} else if (*str >= 'A' && *str <= 'F') {
		num <<= 4;
		num += (*str++ - 'A') + 10;
	} else
		break;
    }

    if (endptr != NULL)
	*endptr = (char *)str;

    return num;
}


/*
 * find a token in *src and increment *src past the token if one is found
 * return -1 if no token is found
 */
static int
gettoken(const char **src, const char **table, int table_size)
{
    const char **tp;
    int len;

    for (tp = tokens; tp < table + table_size; tp++) {
	len = strlen(*tp);
	if (! strncmp(*tp, *src, len)) {
#ifdef _DEBUG
		if (debug)
			fprintf(stderr, "found token: %s\n", *tp);
#endif
		*src += len;

		return tp-tokens;
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
	
    for (sp = src, dp = dest; *sp != '\0'; ) {
	if (collapsespaces && !(rem || quoted)) {
		while (*sp && isspace(*sp))
			sp++;
	}

	if (*sp == '"')
		quoted = !quoted;
	
	if (!rem && !quoted) {
		/* Try the standard tokens. */
		token = gettoken(&sp, tokens, tokens_size);
		if (token != -1) {
			token += TOKEN_BASE;

			if (token == TOKEN_REM)
				rem = 1;

			*dp++ = (unsigned char)token;
			continue;
		}

#ifdef USE_EXTEND
		/* Try the extended CE-prefixed tokens. */
		token = gettoken(&sp, tokens_ce, tokens_ce_size);
		if (token != -1) {
			*dp++ = (unsigned char)PREFIX_1;
			*dp++ = (unsigned char)token;
			continue;
		}

		/* Try the extended FE-prefixed tokens. */
		token = gettoken(&sp, tokens_fe, tokens_fe_size);
		if (token != -1) {
			*dp++ = (unsigned char)PREFIX_2;
			*dp++ = (unsigned char)token;
			continue;
		}
#endif
	}

	if ((*sp == '{') && (*(sp+3) == '}')) {
		/*
		 * Escaped non-printable character.
		 *
		 * For now, we escape such characters simply by
		 * converting them to a hexadecimal number, and
		 * then enclose them between { and } characters.
		 *
		 * This works fine, but maybe we should use the
		 * naming as mentioned in
		 *
		 * https://www.c64-wiki.com/wiki/PETSCII_Codes_in_Listings
		 *
		 * or such?
		 */
		token = (int)hexstrtoul(sp+1, (char **)&sp);
		if (*sp != '}') {
			fprintf(stderr, "Invalid format in escape!\n");
			return 0;
		}
		sp++;

		*dp++ = (unsigned char)token;

		continue;
	}

#ifdef _DEBUG
	if (debug)
		fprintf(stderr, "copying character: '%c' (0x%02x)%s%s\n",
			*sp, *sp, rem?" (rem)":"", quoted?" (quoted)":"");
#endif

	if (*sp != '\r')
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
    char *cp, *ep;
    long linenum;
    int lastlinenum = -1;
    int toklinelen;
    FILE *fi, *fo;
    char *out_name;
    int c;

    /* Set defaults. */
    autonumber = 0;
    collapsespaces = 0;
    debug = verbose = 0;
    invertcase = 0;
    startaddr = 0x0801;		// $1C01 for C128/BASICv7
    trimspaces = 0;
    out_name = NULL;

    /* Process commandline arguments. */
    opterr = 0;
    while ((c = getopt(argc, argv, "acdio:s:tv")) != EOF) switch (c) {
	case 'a':	// auto-number
		autonumber ^= 1;
		break;

	case 'c':	// collapse-spaces
		collapsespaces ^= 1;
		break;

	case 'd':	// debug-level
		debug++;
		break;

	case 'i':	// invert-case
		invertcase ^= 1;
		break;

	case 'o':	// output-file
		out_name = optarg;
		break;

	case 's':	// start-address
		if (*optarg == '0' && *(optarg+1) == 'x')
			optarg += 2;
		else if (*optarg == '$')
			optarg++;
		startaddr = hexstrtoul(optarg, NULL);
		printf("Start address is $%04X\n", startaddr);
		break;

	case 't':	// trim-spaces
		trimspaces ^= 1;
		break;

	case 'v':	// output noise level
		verbose++;
		break;

	default:
usage:
		fprintf(stderr,
			"Usage: bas2prg [-acditv] [-s addr] [-o outfile] filename\n");
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

    /* Show the load address. */
    if (verbose)
	fprintf(stderr, "Load address: $%04X\n", startaddr);

    tp = tokline;
    putword(startaddr, &tp);
    fwrite(tokline, 1, 2, fo);

    for (;;) {
	if (! fgets(line, MAXLINELEN, fi))
		break;

	/* Trim off the newline. */
	cp = &line[strlen(line)-1];
	if (*cp == '\n')
		*cp = '\0';
	cp = &line[strlen(line)-1];
	if (*cp == '\r')
		*cp = '\0';
		
	if (invertcase) {
		for (cp = line; *cp; ++cp) {
			c = *cp;
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

	/* Trim extraneous whitespace at the beginning and end. */
	if (trimspaces) {
		ep = &line[strlen(line)-1];

		while (ep >= line && isspace(*ep))
			ep--;
		ep[1] = '\0';
		while (*cp && isspace(*cp))
			cp++;
	}

	tp = &tokline[2]; /* skip first word for now */
	putword(linenum, &tp);

	toklinelen = tokenize(tp, cp);
#ifdef _DEBUG
	if (debug)
		fprintf(stderr, "line length: %i\n", toklinelen);
#endif
	if (toklinelen == 0)
		break;

	startaddr += toklinelen + 4;
	tp = tokline;
	putword(startaddr, &tp);
	fwrite(tokline, 1, toklinelen + 4, fo);
    }

    tp = tokline;
    putword(0, &tp);
    fwrite(tokline, 1, 2, fo);

    fflush(stdout);

    if (fo != stdout)
	fclose(fo);

    return 0;
}

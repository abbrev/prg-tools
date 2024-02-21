/*
 * tokens.c, array of C64 tokens.
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
#include "tokens.h"


/* Standard CBM tokens, for all versions of BASIC. */
const char *tokens[] = {
    // Standard, block $80
    "END",		// 80
    "FOR",		// 81
    "NEXT",		// 82
    "DATA",		// 83
    "INPUT#",		// 84
    "INPUT",		// 85
    "DIM",		// 86
    "READ",		// 87

    // Standard, block $88
    "LET",		// 88
    "GOTO",		// 89
    "RUN",		// 8a
    "IF",		// 8b
    "RESTORE",		// 8c
    "GOSUB",		// 8d
    "RETURN",		// 8e
    "REM",		// 8f

    // Standard, block $90
    "STOP",		// 90
    "ON",		// 91
    "WAIT",		// 92
    "LOAD",		// 93
    "SAVE",		// 94
    "VERIFY",		// 95
    "DEF",		// 96
    "POKE",		// 97

    // Standard, block $98
    "PRINT#",		// 98
    "PRINT",		// 99
    "CONT",		// 9a
    "LIST",		// 9b
    "CLR",		// 9c
    "CMD",		// 9d
    "SYS",		// 9e
    "OPEN",		// 9f

    // Standard, block $A0
    "CLOSE",		// a0
    "GET",		// a1
    "NEW",		// a2
    "TAB(",		// a3
    "TO",		// a4
    "FN",		// a5
    "SPC(",		// a6
    "THEN",		// a7

    // Standard, block $A8
    "NOT",		// a8
    "STEP",		// a9
    "+",		// aa
    "-",		// ab
    "*",		// ac
    "/",		// ad
    "^",		// ae
    "AND",		// af

    // Standard, block $B0
    "OR",		// b0
    ">",		// b1
    "=",		// b2
    "<",		// b3
    "SGN",		// b4
    "INT",		// b5
    "ABS",		// b6
    "USR",		// b7

    // Standard, block $B8
    "FRE",		// b8
    "POS",		// b9
    "SQR",		// ba
    "RND",		// bb
    "LOG",		// bc
    "EXP",		// bd
    "COS",		// be
    "SIN",		// bf

    // Standard, block $C0
    "TAN",		// c0
    "ATN",		// c1
    "PEEK",		// c2
    "LEN",		// c3
    "STR$",		// c4
    "VAL",		// c5
    "ASC",		// c6
    "CHR$",		// c7

    // Standard, block $C8
    "LEFT$",		// c8
    "RIGHT$",		// c9
    "MID$",		// ca
    "GO",		// cb

#ifdef USE_EXTEND
    // Extended, for V3.5 and V7.0
    "RGR",		// V3.5/7.0
    "RCLR",		// V3.5/7.0
    "RLNUM",		// V3.5; prefix CE for V7.0
    "JOY",		// V3.5/7.0

    // Extended, for V3.5 and V7.0
    "RDOT",
    "DEC",
    "HEX$",
    "ERR$",
    "INSTR",
    "ELSE",
    "RESUME",
    "TRAP",

    // Extended, for V3.5 and V7.0
    "TRON",
    "TROFF",
    "SOUND",
    "VOL",
    "AUTO",
    "PUDEF",
    "GRAPHIC",
    "PAINT",

    // Extended, for V3.5 and V7.0
    "CHAR",
    "BOX",
    "CIRCLE",
    "GSHAPE",
    "SSHAPE",
    "DRAW",
    "LOCATE",
    "COLOR",

    // Extended, for V3.5 and V7.0
    "SCNCLR",
    "SCALE",
    "HELP",
    "DO",
    "LOOP",
    "EXIT",
    "DIRECTORY",
    "DSAVE",

    // Extended, for V3.5 and V7.0
    "DLOAD",
    "HEADER",
    "SCRATCH",
    "COLLECT",
    "COPY",
    "RENAME",
    "BACKUP",
    "DELETE",

    // Extended, for V3.5 and V7.0
    "RENUMBER",
    "xKEY",
    "MONITOR",
    "USING",
    "UNTIL",
    "WHILE",
    "{fe}",		// prefix FE for V7.0
    "{pi}"
#endif
};

#ifdef USE_EXTEND
// Extended keywords, for V3.5 and V7.0, using the $CE prefix.
const char *tokens_ce[] = {
    "{00}",
    "{01}",
    "POT",
    "BUMP",
    "PEN",
    "RSPPOS",
    "RSPRITE",
    "RSPCOLOR",
    "XOR",
    "RWINDOW",
    "POINTER"
};

// Extended keywords, for V7.0, using the $FE prefix.
const char *tokens_fe[] = {
    "${00}",
    "${01}",
    "BANK",
    "FILTER",
    "PLAY",
    "TEMPO",
    "MOVSPR",
    "SPRITE",
    "SPRCOLOR",
    "RREG",
    "ENVELOPE",
    "SLEEP",
    "CATALOG",
    "DOPEN",
    "APPEND",
    "DCLOSE",
    "BSAVE",
    "BLOAD",
    "RECORD",
    "CONCAT",
    "DVERIFY",
    "DCLEAR",
    "SPRSAV",
    "COLLISION",
    "BEGIN",
    "BEND",
    "WINDOW",
    "BOOT",
    "WIDTH",
    "SPRDEF",
    "QUIT",
    "STASH",
    "{20}",
    "FETCH",
    "{22}",
    "SWAP",
    "OFF",
    "FAST",
    "SLOW"

    /* Extensions for V7.1. */
    "CWIND",
    "SSCRN",
    "LSCRN",
    "HIDE",
    "SHOW",
    "SFONT",
    "LFONT",
    "VIEW",
    "FCOPY",
    "ESAVE",
    "SEND",
    "CHECK",
    "ESC",
    "OLD",
    "FIND",
    "DUMP",
    "MERGE"
};
#endif

int	tokens_size = sizeof(tokens) / sizeof(const char *);
#ifdef USE_EXTEND
int	tokens_ce_size = sizeof(tokens_ce) / sizeof(const char *);
int	tokens_fe_size = sizeof(tokens_fe) / sizeof(const char *);
#endif

/*
 * BAS2PRG	Convert C64 BASIC source to encoded PRG file.
 *
 *		Define application version and build info.
 *
 * Version:	@(#)version.h	1.0.3	2023/04/13
 *
 * Author:	Fred N. van Kempen, <waltje@varcem.com>
 *
 *		Copyright 2023 Fred N. van Kempen.
 *
 * MIT License 	Permission is hereby  granted, free  of charge, to  any person
 *		obtaining a copy of this software and associated documentation
 *		files  (the  "Software"), to  deal  in  the  Software  without
 *		restriction, including without  limitation the  rights to use,
 *		copy, modify,  merge, publish, distribute,  sublicense, and/or
 *		sell copies of  the Software, and  to permit persons  to  whom
 *		the Software is  furnished to do so, subject  to the following
 *		condition:
 *
 *		The above copyright notice and this permission notice shall be
 *		included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT  WARRANTY OF  ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS  OR  COPYRIGHT HOLDERS BE  LIABLE FOR  ANY CLAIM, DAMAGES  OR OTHER
 * LIABILITY, WHETHER  IN  AN ACTION OF  CONTRACT, TORT  OR OTHERWISE, ARISING
 * FROM, OUT OF  OR IN  CONNECTION  WITH THE  SOFTWARE  OR THE  USE  OR  OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef VERSION_H
# define VERSION_H


/* Application name. */
#define APP_NAME	"C64-PRG-Tools"
#define APP_TITLE	"C64 BASIC Tokenizer Tools"

/* Version info. */
#define APP_VER_MAJOR	1
#define APP_VER_MINOR	0
#define APP_VER_REV	3
#define APP_VER_PATCH	0


/* Standard C preprocessor macros. */
#define STR_STRING(x)	#x
#define STR(x)		STR_STRING(x)
#define STR_RC(a,e)	a ## , ## e


/* These are used in the application. */
#define APP_VER_NUM	APP_VER_MAJOR.APP_VER_MINOR.APP_VER_REV
#if defined(APP_VER_PATCH) && APP_VER_PATCH > 0
# define APP_VER_NUM_4	APP_VER_MAJOR.APP_VER_MINOR.APP_VER_REV.APP_VER_PATCH
#else
# define APP_VER_NUM_4	APP_VER_MAJOR.APP_VER_MINOR.APP_VER_REV.0
#endif
#define APP_VERSION	STR(APP_VER_NUM)
#define APP_VERSION_4	STR(APP_VER_NUM_4)


#endif	/*VERSION_H*/

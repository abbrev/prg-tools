Commodore 64/128 BASIC/PRG tools
================================

This is a pair of quick-and-dirty programs I wrote originally in 2011 to
convert between a PRG (tokenized) file and a BAS (text) file. Although
originally only handling the BASIC V2 keywords (Commodore 64), support 
for the V3.5 (Plus/4 ?) and V7 (Commodore 128) was added. This can be
turned off by NOT defining the USE_EXTEND macro in the Makefiles.


Usage
-----

Convert a BAS file to a PRG file:

    bas2prg <program.bas >program.prg

or

    bas2prg -o program.prg program.bas

Convert a PRG file to a BAS file:

    prg2bas <program.prg >program.bas

or

    prg2bas -o program.bas program.prg

whichever format feels better.

Each program takes its own commandline options; you can see these by calling
them with the -? argument:

    prg2bas -?

or

    bas2prg -?


How to build
------------

Run the following command:

    make -f unix/Makefile.GCC

or

    make -f win32/Makefile.MinGW

or

    make -f win32/Makefile.VC

depending on your operating system and development tools.


Layout of a BASIC PRG file
--------------------------

* load address (2 bytes)
* tokenized BASIC code (repeated for each line):
  * address of next line (2 bytes)
  * line number (2 bytes)
  * line contents (tokenized)
  * nul byte (1 byte)
* value 0000 (2 bytes) (end of PRG)


Future directions
-----------------

These two programs have scratched my itch, so I don't plan to do anything else
with them:

- It would be nice to expose the internal flags in bas2prg as command-line
  arguments. It's trivial enough to edit the source code and recompile, so I
  haven't put in the effort to do so.

- It would be nice if prg2bas would escape unprintable characters (inside
  strings, for example) as hexadecimal codes inside braces (such as "{d0}").
  This would minimize problems related to differences in character sets
  (PETSCII vs ASCII). It would be doubly nice if bas2prg recognized and
  converted these escaped sequences properly.

That said, done :)

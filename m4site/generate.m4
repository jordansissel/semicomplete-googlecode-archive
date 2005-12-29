ifdef(`BACKPATH',,`define(`BACKPATH', `')')dnl
include(BACKPATH`'macros.m4)dnl
divert(9)dnl
include(INPUTPAGE)dnl
divert(0)dnl
pinclude(layout.m4)dnl

#ifndef CHIP8_H
#define CHIP8_H

#define _DEFAULT_SOURCE // for unistd.h

#define BUFSIZE         4096
#define FONTSET_START   0x00

#include <SDL2/SDL.h>

typedef unsigned char U8;
typedef unsigned short U16;

extern U8 mem[4096];
extern U8 v[16];
extern U16 stack[16];
extern U8 screen[32][64];

extern U16 opcode;

extern U8 dt;
extern U8 st;
extern U16 I;
extern U16 pc;

extern U8 sp;

extern U8 key[16];

extern U8 chip8_fontset[80];

void cls(void);
void ret(void);
void jp(U16);
void jpv0(U8, U16);
void call(U16);
void seb(U8, U8);
void sneb(U8, U8);
void sev(U8, U8);
void ldb(U8, U8);
void add(U8, U8);
void ldv(U8, U8);
void or(U8, U8);
void and(U8, U8);
void xor(U8, U8);
void addc(U8, U8);
void sub(U8, U8);
void shr(U8);
void subn(U8, U8);
void shl(U8);
void snev(U8, U8);
void ldi(U16);
void rnd(U16, U8);
void drw(U8, U8, U8);
void lddtvx(U8);
void ldk(U8);
void ldvxdt(U8);
void ldvxst(U8);
void addi(U8);
void ldspr(U8);
void ldbcd(U8);
void streg(U8);
void readreg(U8);
void skp(U8);
void sknp(U8);
void update_keys(SDL_Event);

#endif
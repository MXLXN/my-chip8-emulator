#include "chip8.h"
#include "screen.h"
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

U8 mem[4096] = {0};
U8 v[16] = {0};
U16 stack[16] = {0};
U8 screen[32][64] = {0};

U16 opcode = 0;

U8 dt = 0;
U8 st = 0;
U16 I = 0;
U16 pc = 0;

U8 sp = 0;

U8 key[16] = {0};

U8 chip8_fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};



void cls()
{
    for (int i = 0; i < 32; ++i)
        for (int j = 0; j < 64; ++j)
            screen[i][j] = 0;
}

void jp(U16 addr)
{
    pc = addr - 512;
}

void jpv0(U8 V_number, U16 addr)
{
    pc = addr + v[V_number] - 512;
}

void call(U16 addr)
{
    stack[sp] = pc + 2;
    sp++;
    pc = addr - 512;
}

void ret()
{
    sp--;
    pc = stack[sp];
}

void seb(U8 V_number, U8 byte)
{
    if (v[V_number] == byte)
        pc += 2;
}

void sneb(U8 V_number, U8 byte)
{
    if (v[V_number] != byte)
        pc += 2;
}

void sev(U8 V_number1, U8 V_number2)
{
    if (v[V_number1] == v[V_number2])
        pc += 2;
}

void ldb(U8 V_number, U8 byte)
{
    v[V_number] = byte;
}

void add(U8 V_number, U8 byte)
{
    v[V_number] += byte;
}

void ldv(U8 V_number1, U8 V_number2)
{
    v[V_number1] = v[V_number2];
}

void or(U8 V_number1, U8 V_number2)
{
    v[V_number1] |= v[V_number2];
}

void and(U8 V_number1, U8 V_number2)
{
    v[V_number1] &= v[V_number2];
}

void xor(U8 V_number1, U8 V_number2)
{
    v[V_number1] ^= v[V_number2];
}

void addc(U8 V_number1, U8 V_number2)
{
    U16 result = v[V_number1] + v[V_number2];
    if (result > 255)
    {
        v[15] = 1;
        v[V_number1] = result & 0xFF;
    }
    else
        v[V_number1] = result;
}

void sub(U8 V_number1, U8 V_number2)
{
    U16 result = v[V_number1] - v[V_number2];
    if (v[V_number1] > v[V_number2])
        v[15] = 1;
    else
        v[15] = 0;

    v[V_number1] = result & 0xFF;
}

void shr(U8 V_number)
{
    U16 result = v[V_number] >> 1;
    if ((v[V_number] & 1) == 1)
        v[15] = 1;
    else
        v[15] = 0;
    v[V_number] = result;
}

void subn(U8 V_number1, U8 V_number2)
{
    U16 result = v[V_number2] - v[V_number1];
    if (v[V_number2] > v[V_number1])
        v[15] = 1;
    else
        v[15] = 0;
    v[V_number1] = result & 0xFF;
}

void shl(U8 V_number)
{
    U16 result = v[V_number] << 1;
    if ((v[V_number] >> 7) == 1)
        v[15] = 1;
    else
        v[15] = 0;
    v[V_number] = result;
}

void snev(U8 V_number1, U8 V_number2)
{
    if (v[V_number1] != v[V_number2])
        pc += 2;
}

void ldi(U16 addr)
{
    I = addr;
}

void rnd(U16 V_number, U8 byte)
{
    v[V_number] = (rand() % 256) & byte;
}

void drw(U8 V_number1, U8 V_number2, U8 n)
{
    U8 x = v[V_number1];
    U8 y = v[V_number2];

    int I_saved = I;
    int i, j, k;
    v[15] = 0;

    while (n--)
    {
        U8 sprite = mem[I_saved];
        for (i = y; i < y + 1; ++i)
        {
            for (j = x, k = 7; j < x + 8 && k >= 0; ++j, --k)
            {
                if (j > 63 || i > 31 || j < 0 || i < 0)
                    continue;
                if (screen[i][j] == 1 && ((sprite >> k) & 1) == 1) 
                    v[15] = 1;
                screen[i][j] ^= (sprite >> k) & 1;
            }
        }
        y++;
        I_saved++;
    }
}

void lddtvx(U8 V_number)
{
    v[V_number] = dt;
}

void ldk(U8 V_number)
{
    if      (key[0x1]) v[V_number] = 0x1;
    else if (key[0x2]) v[V_number] = 0x2;
    else if (key[0x3]) v[V_number] = 0x3;
    else if (key[0x4]) v[V_number] = 0x4;
    else if (key[0x5]) v[V_number] = 0x5;
    else if (key[0x6]) v[V_number] = 0x6;
    else if (key[0x7]) v[V_number] = 0x7;
    else if (key[0x8]) v[V_number] = 0x8;
    else if (key[0x9]) v[V_number] = 0x9;
    else if (key[0xA]) v[V_number] = 0xA;
    else if (key[0xB]) v[V_number] = 0xB;
    else if (key[0xC]) v[V_number] = 0xC;
    else if (key[0xD]) v[V_number] = 0xD;
    else if (key[0xE]) v[V_number] = 0xE;
    else if (key[0xF]) v[V_number] = 0xF;
}

void ldvxdt(U8 V_number)
{
    dt = v[V_number];
}

void ldvxst(U8 V_number)
{
    st = v[V_number];
}

void addi(U8 V_number)
{
    I += v[V_number];
}

void ldspr(U8 V_number)
{
    I = FONTSET_START + ((v[V_number] & 0xF) * 5);
}

void ldbcd(U8 V_number)
{
    mem[I] = v[V_number] / 100;
    mem[I+1] = (v[V_number] % 100) / 10;
    mem[I+2] = v[V_number] % 10;
}

void streg(U8 V_number)
{
    for (int i = 0; i <= V_number; ++i)
        mem[I + i] = v[i];
}

void readreg(U8 V_number)
{
    for (int i = 0; i <= V_number; ++i)
        v[i] = mem[I + i];
}

void skp(U8 V_number)
{
    if (key[v[V_number]])
        pc += 2;
}

void sknp(U8 V_number)
{
    if (!key[v[V_number]])
        pc += 2;
}

void update_keys(SDL_Event event)
{
    int flag = 0;
    if (event.type == SDL_KEYDOWN)
        flag = 1;
    else if (event.type == SDL_KEYUP)
        flag = 0;
    else
        return;

    switch (event.key.keysym.scancode)
    {
        case SDL_SCANCODE_1: key[0x1] = flag; break;
        case SDL_SCANCODE_2: key[0x2] = flag; break;
        case SDL_SCANCODE_3: key[0x3] = flag; break;
        case SDL_SCANCODE_4: key[0xC] = flag; break;

        case SDL_SCANCODE_Q: key[0x4] = flag; break;
        case SDL_SCANCODE_W: key[0x5] = flag; break;
        case SDL_SCANCODE_E: key[0x6] = flag; break;
        case SDL_SCANCODE_R: key[0xD] = flag; break;

        case SDL_SCANCODE_A: key[0x7] = flag; break;
        case SDL_SCANCODE_S: key[0x8] = flag; break;
        case SDL_SCANCODE_D: key[0x9] = flag; break;
        case SDL_SCANCODE_F: key[0xE] = flag; break;

        case SDL_SCANCODE_Z: key[0xA] = flag; break;
        case SDL_SCANCODE_X: key[0x0] = flag; break;
        case SDL_SCANCODE_C: key[0xB] = flag; break;
        case SDL_SCANCODE_V: key[0xF] = flag; break;

        default: break;
    }
}

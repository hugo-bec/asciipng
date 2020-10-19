#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <string.h>


/**
 *
 *  Modification du programme décrit par Robert Jan Schaper: http://www.strudel.org.uk/itoa/
 */
char* itoa(int val, int base){
	static char buf[33] = {[0 ... 31] = 48};
    buf[32] = '\n';
    int i = 32;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];

    //printf("buf15: %c\n", buf[15]);
    return &buf[0];

}


void bitprint(int n){
    printf ("0b%s\n", itoa(n, 2));
}


int main() {

    u_int16_t dividende = 53;
	u_int16_t diviseur = 7;
	u_int16_t quotient = 0;
	u_int16_t reste = 0;

    if (diviseur < dividende) {
    	while (diviseur < dividende) {
    		diviseur <<= 1;
    	}
    	diviseur >>= 1;
    }

	printf("diviseur: \t%s\n", itoa((int)diviseur, 2));
    printf("dividende: \t%s\n", itoa((int)dividende, 2));

    return 0;
}

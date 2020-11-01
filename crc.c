#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define CRC32_PNG 0x04C11DB7    // 100 11000001 00011101 10110111

/*
 *  Inspiration du modèle de curseur de DVSoft dans son article: https://dvsoft.developpez.com/Articles/CRC/
 *  Un grand merci pour son article malgré le fait qu'il est déjà plus de 15 ans.
 */
union Curseur {
    u_int64_t corp;
    struct {
        u_int8_t padding;    //  8 bits vide pour faire "coller" entree avec champ (voir "Data structure alignment")
        u_int8_t entree;
        u_int32_t champ;
        u_int8_t tete;
        u_int8_t padding2;    //  meme raison que padding
    }part;
};

static union Curseur curseur;
static char cur2[8];

/**
 *  Modification du programme décrit par Robert Jan Schaper: http://www.strudel.org.uk/itoa/
 */
void itoa(u_int32_t val, int base, char buf[]) {
    buf[32] = '\0';
    int i = 31;
	for(; val && i ; --i, val /= base){
		buf[i] = "0123456789abcdef"[val % base];
	}
}

/*
 *  nl: new line (1=oui)
 */
void bitprint(int n, int nb_bit, int nl){
	char buf[33] = {[0 ... 31] = 48};
	buf[32] = '\0';
	itoa(n, 2, buf);
    char* pbuf = buf+(32-nb_bit);
    (nl == 1) ? printf ("%s\n", pbuf)
             : printf ("%s", pbuf);
}

/*
 *  affiche les bit du curseur de type Curseur
 */
void bitprint_curseur(union Curseur c){
    bitprint(c.part.tete, 8, 0);
    printf(" ");
    bitprint(c.part.champ, 32, 0);
    printf(" ");
    bitprint(c.part.entree, 8, 1);
    printf("         ");
    bitprint((int)CRC32_PNG, 32, 1);
}


/**
 *  taille: taille du "message + CRC". taille >= 4
 *  polynome: polynome du CRC-32 pour le format png:
 *      ->   0x04C11DB7
 *      <->  x^32+x^26+x^23+x^16+x^22+x^21+x^20+x^8+x^7+x^5+x^4+x^2+x+1
 *      <->  (00000)100 11000001 00011101 10110111.
 *  message: message + CRC.
 */
u_int32_t check_CRC(char* message, size_t taille, int polynome) {

    curseur.corp = 0;
    curseur.part.champ = 0xFFFFFFFF;
    curseur.part.tete = 0;

    u_int32_t bit_de_tete = 1;
	while (polynome / bit_de_tete != 1) {
		bit_de_tete <<= 1;
	}
    printf("taille: %lu, bit_de_tete: ", taille);
    bitprint(bit_de_tete, 32, 1);
    printf("\n");

    for (size_t i=0; i<taille; i++) {
        curseur.part.entree = message[i];
        for (size_t b=0; b<8; b++) {
            curseur.corp <<= 1;
            curseur <<= 1;
            //curseur.part.entree <<= 1;
            bitprint_curseur(curseur);
            //printf("test (curseur.part.champ & bit_de_tete) : ");
            //bitprint((curseur.part.champ & bit_de_tete), 32, 1);
            /*if ((curseur.part.champ & bit_de_tete) == bit_de_tete) {
                //printf("XOR\n");
                curseur.part.champ = curseur.part.champ ^ polynome;
            }*/

            //printf("entree: ");bitprint((int)curseur.part.entree, 32, 1);
            //printf("champ : ");bitprint(curseur.part.champ, 32, 1);
        }
        printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - \n");
    }
    bitprint_curseur(curseur);

    return curseur.part.champ;
}


int main() {

    char message[] = {'a', 'b', 'c', 'd', 'e', 'f', 'X', 'X', 'X', 'X'};

    bitprint((int)CRC32_PNG, 32, 1);
    bitprint((int)CRC32_PNG, 8, 1);
    bitprint((int)CRC32_PNG, 16, 1);
    bitprint((int)CRC32_PNG, 24, 1);
    printf("CRC32_PNG : %x (%d)\n", CRC32_PNG, CRC32_PNG);

    int taille = 6; //strlen(message);
    int reste = check_CRC(message, taille, CRC32_PNG);
    char* restechar = (char*)&reste;

    printf("reste: %x (%d)\n", reste, reste);





/*      TESTS
    curseur_test.Value = 0;
    curseur_test.Part.Tail = 75;
    for (size_t b=0; b<8; b++) {
        curseur_test.Value <<= 1;
        printf("Value : ");bitprint(curseur_test.Value, 32, 1);
        printf("Remainder value : %d\n", curseur_test.Part.Remainder);
    }

    printf("sizeof curseur.corp : %ld, pointeur : %p\n",
            sizeof(curseur.corp), &curseur.corp);
    printf("sizeof curseur.part.entree : %ld, pointeur : %p\n",
            sizeof(curseur.part.entree), &curseur.part.entree);
    printf("sizeof curseur.part.champ : %ld, pointeur : %p\n",
            sizeof(curseur.part.champ), &curseur.part.champ);
    printf("sizeof curseur.part.tete : %ld, pointeur : %p\n",
            sizeof(curseur.part.tete), &curseur.part.tete);
    printf("sizeof curseur : %ld, pointeur : %p\n",
            sizeof(curseur), &curseur_test);
*/
    return 0;
}

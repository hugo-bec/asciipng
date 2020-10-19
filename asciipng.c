#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <string.h>

#include <png.h>


typedef struct chunk chunk;
struct chunk{
	int taille;
	char type[5];
	int crc;
	chunk* suiv;
};


char* imagelocation =  "sicpwaifubg.png";		// /home/alstm/Images/culpourturkisbg.png
int cur=0;
unsigned taille, largeur, hauteur, crc;
char typechunk[5];
unsigned char profondeur_bits, type_color, compression, filtrage, entrelacement;
chunk chunk_1;

unsigned char* buf;
int nbread;


void printintbit(u_int32_t var0){
	u_int32_t b = 1;
	char chainebit[32] = "";
	char charbit;

	for(int i=0; i<32; i++){
		charbit = ((b << i) & var0)? '1' : '0';
		chainebit[31-i] = charbit;
	}
	for(int i=0; i<32; i++){
		printf("%c", chainebit[i]);
	}
	printf("\n");
}


void printhex(unsigned char c){
	printf("%x ", c);
}

u_int32_t swap_endian(u_int32_t n){
	u_int32_t o1, o2, o3, o4;
	o1 = (n & 0x000000ff) << 24;
	o2 = (n & 0x0000ff00) << 8;
	o3 = (n & 0x00ff0000) >> 8;
	o4 = (n & 0xff000000) >> 24;
	return o1 | o2 | o3 | o4;
}


void print_chunk(chunk* c){
	printf("-----%s-----\nTaille: %d\nCRC: %d\n", c->type, c->taille, c->crc);
}

int read_chunks(int fd){
	u_int32_t* pointeur_uint32;
	int seek = 0;
	int nb_idat = 0;

	chunk* chunk_courant = &chunk_1;
	printf("----------80 premier octets du buf en hexa----------\n");
	for(int i=0; i<20; i++){
		for(int h=0; h<4; h++){
			printhex(buf[i*4+h]);
		}
		printf("\n");
	}
	printf("----------------------------------------------------\n");

	while (1) {
		pointeur_uint32 = buf+cur;						cur += 4;
		chunk_courant->taille = swap_endian(*pointeur_uint32);

		for (size_t i=0; i <4; i++) {
			chunk_courant->type[i] = buf[cur+i];
		} chunk_courant->type[4] = '\0';
		cur += 4;

		/*
		if (cur+chunk_courant.taille >= nbread) {
			seek = lseek(fd, cur+seek, SEEK_SET);
			nbread = read(fd, buf, 4096);
			cur = 0;
		}*/

		if(strcmp(chunk_courant->type, "IHDR") == 0){
			pointeur_uint32 = buf+cur;
			largeur = swap_endian(*pointeur_uint32);
			pointeur_uint32++;
			hauteur = swap_endian(*pointeur_uint32);		cur += 4*2;

			profondeur_bits = buf[cur];
			type_color = buf[cur+1];
			compression = buf[cur+2];
			filtrage = buf[cur+3];
			entrelacement = buf[cur+4];		cur+=5;

			printf("---IHDR---\n");

			printf("largeur: %u\nhauteur: %u\n", largeur, hauteur);
			printf("profondeur: %d\n", (int) profondeur_bits);
			printf("typecolor: %d\n", (int) type_color);
			printf("compression: %d\n", (int) compression);
			printf("filtrage: %d\n", (int) filtrage);
			printf("entrelacement: %d\n", (int) entrelacement);

		}
		else if(strcmp(chunk_courant->type, "IEND") == 0){
			printf("---IEND---\n");
			return nb_idat;
		}

		else {
			nb_idat++;
			cur += chunk_courant->taille;
			if (cur >= nbread) {
				seek = lseek(fd, cur+seek, SEEK_SET);
				nbread = read(fd, buf, 4096);
				cur = 0;
			}
		}

		pointeur_uint32 = buf+cur;		cur+=4;
		chunk_courant->crc = swap_endian(*pointeur_uint32);

		print_chunk(chunk_courant);
		chunk_courant->suiv = malloc(sizeof(chunk));
		chunk_courant = chunk_courant->suiv;
	}
	free(buf);
}


void main() {

	u_int16_t dividende = 53;
	u_int16_t diviseur = 7;
	u_int16_t quotient = 0;
	u_int16_t reste = 0;

	while (dividende < diviseur) {
		diviseur << 1;
	}
	diviseur >> 1;
	printf("diviseur: \t%b\n", diviseur);
	printf("dividende: \t%b\n", dividende);

	for (size_t i=0; i< (sizeof(dividende) - sizeof(diviseur)); i++) {

	}

	int ret;

	int fd = open(imagelocation, O_RDONLY);
	if (fd < 0){
		printf("error: fd: %d\n", fd);
		exit(1);
	}

	buf = malloc(4096);
	int nbread = read(fd, buf, 4096);

	char a[5] = "titi";
	char b[5] = "titi";

	if (strcmp(a, b) == 0) {
		printf("titi\n");
	} else {
		printf("pas titi\n");
	}

	/*printf("----------80 premier octets du buf en hexa----------\n");
	for(int i=0; i<20; i++){
		for(int h=0; h<4; h++){
			printhex(buf[i*4+h]);
		}
		printf("\n");
	}
	printf("----------------------------------------------------\n");
	*/

	printf("signature :");
	for(int i=0; i<8; i++){
		printhex(buf[i]);
	} printf("\n");					cur += 8;

	printf("fd: %d\n", fd);
	ret = read_chunks(fd);
	printf("ret: %d\n", ret);
	printf("taille approximative du fichier: %d octets\n", ret*8192);

	/*
	u_int32_t taille2;
	pointeur_uint32 = buf+cur;		cur += 4;
	taille2 = swap_endian(*pointeur_uint32);
	printf("taille: %u\n", taille2);

	char typechunk2[5];
	for (size_t i=0; i <4; i++) {
		typechunk2[i] = buf[cur+i];
	} typechunk2[4] = '\0';
	printf("typechunk mystère: %s\n", typechunk2);
	cur += 4;
	*/



	free(buf);

	exit(0);
}

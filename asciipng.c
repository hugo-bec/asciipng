#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <string.h>

#include <png.h>



int cur=0;
unsigned taille, largeur, hauteur, crc;
char typechunk[5];
unsigned char profondeur_bits, type_color, compression, filtrage, entrelacement;


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


void main() {

	u_int32_t* pointeur_uint32;
	//byte* pointeur_octet;

	char* imagelocation = "/home/alstm/Images/culpourturkisbg.png";		//sicpwaifubg.png

	int fd = open(imagelocation, O_RDONLY);
	if (fd < 0){
		printf("error: fd: %d\n", fd);
		exit(1);
	}

	unsigned char* buf = malloc(4096);
	int nbread = read(fd, buf, 4096);

	int o = 0;
	printf("char0: %x\n", buf[o]);
	printintbit((int)buf[o]);

	printf("----------80 premier octets du buf en hexa----------\n");
	for(int i=0; i<20; i++){
		for(int h=0; h<4; h++){
			printhex(buf[i*4+h]);
		}
		printf("\n");
	}
	printf("----------------------------------------------------\n");


	printf("signature :");
	for(int i=0; i<8; i++){
		printhex(buf[i]);
	}
	printf("\n");
	cur += 8;


	pointeur_uint32 = buf+cur;		cur += sizeof(u_int32_t);
	taille = swap_endian(*pointeur_uint32);
	printf("taille: %u\n", taille);

	for (size_t i=0; i <4; i++) {
		typechunk[i] = buf[cur+i];
	} typechunk[4] = '\0';
	printf("typechunk: %s\n", typechunk);
	cur += 4;


	pointeur_uint32 = buf+cur;
	largeur = swap_endian(*pointeur_uint32);
	pointeur_uint32++;
	hauteur = swap_endian(*pointeur_uint32);		cur += 4*2;
	//largeur = swap_endian((u_int32_t)(*(buf+cur)));		cur+=sizeof(u_int32_t);
   	//hauteur = swap_endian((u_int32_t)(*(buf+cur)));		cur+=sizeof(u_int32_t);
	printf("largeur: %u\nhauteur: %u\n", largeur, hauteur);
	printf("cur: %d\n", cur);


	profondeur_bits = buf[cur];
	type_color = buf[cur+1];
	compression = buf[cur+2];
	filtrage = buf[cur+3];
	entrelacement = buf[cur+4];		cur+=5;

	printf("profondeur: %d\n", (int) profondeur_bits);
	printf("typecolor: %d\n", (int) type_color);
	printf("compression: %d\n", (int) compression);
	printf("filtrage: %d\n", (int) filtrage);
	printf("entrelacement: %d\n", (int) entrelacement);

	pointeur_uint32 = buf+cur;		cur+=4;
	crc = swap_endian(*pointeur_uint32);
	printf("CRC: %u\n", crc);	// e8 f6 0b 97 = 3908438935

	u_int32_t taille2;
	pointeur_uint32 = buf+cur;		cur += sizeof(u_int32_t);
	taille2 = swap_endian(*pointeur_uint32);
	printf("taille: %u\n", taille2);

	char typechunk2[5];
	for (size_t i=0; i <4; i++) {
		typechunk2[i] = buf[cur+i];
	} typechunk2[4] = '\0';
	printf("typechunk mystère: %s\n", typechunk2);
	cur += 4;



	exit(0);
}

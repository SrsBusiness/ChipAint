#include <stdio.h>
#include "chip.h"


int init(){
	I = 0x0;
	stackPointer = 0;
	delayTimer = 0;
	pc = 0x200;
	return 1;
}

int load(char *rom){
	FILE *file = fopen(rom, "r");

	return 1;
}

int draw(){
	for (int j = 0; j < 100; j++){
		printf("\n");
	}
	int i = 0;
	while(i < 2048){
		if (i%64 == 0){
			printf("\n");
		}
		if (display[i] == 1){
			printf("#");
		}
		else{
			printf(" ");
		}
		i++;

	}
}

int run(){

	//fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];
	printf("%x: ", opcode);
	
	//decode opcode
	switch(opcode & 0xF000){
		default:
			printf("Unsupported Opcode\n");
			return 0;
	}
}

int main(int argc, char **argv){
	init();
	load(argv[1]);
	draw();
	run();
	return 1;
}
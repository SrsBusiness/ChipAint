#include <stdio.h>
#include "chip.h"

int init(){
	I = 0x0;
	stackPointer = 0;
	delayTimer = 0;
	pc = 0x200;
	return 1;
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

int main(){
	init();
	run();
	return 1;
}
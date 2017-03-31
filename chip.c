#include <stdio.h>
#include <string.h>
#include "chip.h"


int init(){
	I = 0x0;
	stackPointer = 0;
	delayTimer = 0;
	pc = 0x200;
	memcpy(memory+0x50,fontset,80);
	return 0;
}

int load(char *rom){
	FILE *file = fopen(rom, "r");
	fseek(file, 0L, SEEK_END);
	int size = ftell(file);
	rewind(file);
	fread(memory+0x200, 1, size, file);

	return 0;
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
	return 0;
}

int run(){

	//fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];
	printf("\n%x: ", opcode);
	
	//decode opcode
	switch(opcode & 0xF000){

		//2nnn - Call subroutine at nnn. The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
		case 0x2000:{
			uint16_t address = opcode & 0x0FFF;
			stack[stackPointer] = pc;
			stackPointer++;
			pc = address;
			break;
		}
		//6xkk - Set Vx = kk.  The interpreter puts the value kk into register Vx.
		case 0x6000:{
			int x = (opcode & 0x0F00) >> 8;
			V[x] = opcode & 0x00FF;
			pc += 2;
			break;
		}
		//7xkk - Set Vx = Vx + kk. Adds the value kk to the value of register Vx, then stores the result in Vx. 
		case 0x7000:{
			int x = (opcode & 0x0F00) >> 8;
			V[x] = V[x] + opcode & 0x00FF;
			pc += 2;
			break;
		}
		//Annn - Set I = nnn. The value of register I is set to nnn.
		case 0xA000:{
			I = opcode & 0x0FFF;
			pc += 2;
			break;
		}
		/*
		***********************WARNING NOT YET SUPPORTED******************************
		*Dxyn - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision. 
		*The interpreter reads n bytes from memory, starting at the address stored in I. These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). 
		*Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. 
		*If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen. 
		*See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more information on the Chip-8 screen and sprites.
		*/
		case 0xD000:{
			pc += 2;
			break;
		}
		default:
			printf("Unsupported Opcode\n");
			return 1;
	}
}

int main(int argc, char **argv){
	init();
	load(argv[1]);
	int status;
	do{
		//draw();
		status = run();
	}while(status != 1);
	return 0;
}
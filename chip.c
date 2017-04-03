#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "chip.h"
#include "lcurses.h"

//clock. Higher CLOCK_SPEED = higher T (slower cpu).
#define CLOCK_SPEED 1000

int init(){
	I = 0x0;
	stackPointer = 0;
	delayTimer = 0;
	pc = 0x200;

	//loading fontset into address 0x50
	memcpy(memory+0x50,fontset,80);
	return 0;
}

//load file from argv[1] into address 0x200
int load(char *rom){
	FILE *file = fopen(rom, "r");
	fseek(file, 0L, SEEK_END);
	int size = ftell(file);
	rewind(file);
	fread(memory+0x200, 1, size, file);
    fclose(file);
	return 0;
}

int draw(int row, int col){
    lcurses_save_cursor_position();
	if (drawFlag) {
        for (int r = 0; r < CHIP8_DISPLAY_HEIGHT; r++) {
            lcurses_move_cursor(row + r, col);
            for (int c = 0; c < CHIP8_DISPLAY_WIDTH; c++) {
                int index = r * CHIP8_DISPLAY_WIDTH + c;
                if (display[index] == 1) {
                    lcurses_background_white();
                    printf(" ");
                } else {
                    lcurses_background_black();
                    printf(" ");
                }
            }
        }
		drawFlag = false;
	}
    lcurses_restore_cursor_position();
	return 0;
}

int run(){

	//fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];
	printf("\n%x: ", opcode);
	
	//decode opcode
	switch(opcode & 0xF000){
		case 0x0000:{
			switch(opcode & 0x00FF){
				//00E0 - clear screen 
				case 0x00E0:{
					for(int i = 0; i < 32*64;i++){
						display[i] = 0;
					}
					pc += 2;
					break;

				}
				//00EE - Return from a subroutine. The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
				case 0x00EE:{
					stackPointer--;
					pc = stack[stackPointer];
					pc += 2;
					break;
				}
			}
			break;
		}
		//1nnn - Jump to location nnn. The interpreter sets the program counter to nnn.
		case 0x1000:{
			uint16_t nnn = opcode & 0x0FFF;
			pc = nnn;
			break;
		}
		//2nnn - Call subroutine at nnn. The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
		case 0x2000:{
			uint16_t address = opcode & 0x0FFF;
			stack[stackPointer] = pc;
			stackPointer++;
			pc = address;
			break;
		}
		//3xkk - Skip next instruction if Vx = kk. The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2 (4).
		case 0x3000:{
			int x = (opcode & 0x0F00) >> 8;
			uint8_t kk = opcode & 0x00FF;
			if (V[x] == kk){
				pc += 4;
			}
			else{
				pc += 2;
			}
			break;
		}
		//4xkk - Skip next instruction if Vx != kk. The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2 (4).
		case 0x4000:{
			int x = (opcode & 0x0F00) >> 8;
			int kk = opcode & 0x00FF;
			if (V[x] != kk){
				pc += 4;
			}
			else{
				pc += 2;
			}
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
		
		case 0x8000:{
			switch(opcode & 0x000F){
				//8xy0 - Set Vx = Vy. Stores the value of register Vy in register Vx.
				case 0x0000:{
					int x = (opcode & 0x0F00) >> 8;
					int y = (opcode & 0x00F0) >> 4;
					V[x] = V[y];
					pc += 2;
					break;
				}
				//8xy2 - Set Vx = Vx AND Vy.
				case 0x0002:{
					int x = (opcode & 0x0F00) >> 8;
					int y = (opcode & 0x00F0) >> 4;
					V[x] = V[x] & V[y];
					pc += 2;
					break;
				}
				//8xy4 - Set Vx = Vx + Vy, set VF = carry. 
				//The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
				case 0x0004:{
					int x = (opcode & 0x0F00) >> 8;
					int y = (opcode & 0x00F0) >> 4;
					if(V[x] + V[y] > 255){
						V[0xF] = 1;
					}
					else{
						V[0xF] = 0;
					}
					V[x] = V[x] + V[y];
					pc += 2;
					break;
				}
				//8xy5 - Set Vx = Vx - Vy, set VF = NOT borrow.
				//If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
				case 0x0005:{
					int x = (opcode & 0x0F00) >> 8;
					int y = (opcode & 0x00F0) >> 4;
					if (V[x] > V[y]){
						V[0xF] = 1;
					}
					else{
						V[0xF] = 0;
					}
					V[x] = V[x] - V[y];
					pc += 2;
					break;
				}
			}
			break;
		}
		//9xy0 - Skip next instruction if Vx != Vy. The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
		case 0x9000:{
			int x = (opcode & 0x0F00) >> 8;
			int y = (opcode & 0x00F0) >> 4;
			if (V[x] != V[y]){
				pc += 4;
			}
			else{
				pc += 2;
			}
			break;
		}
		//Annn - Set I = nnn. The value of register I is set to nnn.
		case 0xA000:{
			I = opcode & 0x0FFF;
			pc += 2;
			break;
		}
		//Cxkk - Set Vx = random byte AND kk. 
		//The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx. See instruction 8xy2 for more information on AND.
		case 0xC000:{
			int x = (opcode & 0x0F00) >> 8;
			uint8_t kk = opcode & 0x00FF;
			uint8_t random = rand() % 256;
			V[x] = random & kk;
			pc += 2;
			break;
		}
		/*
		*Dxyn - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision. 
		*The interpreter reads n bytes from memory, starting at the address stored in I. These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). 
		*Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. 
		*If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen. 
		*See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more information on the Chip-8 screen and sprites.
		*/
		case 0xD000:{
			int x = V[(opcode & 0x0F00) >> 8];
			int y = V[(opcode & 0x00F0) >> 4];
			int n = opcode & 0x000F; 

			V[0xF] = 0;

			for (int i = 0; i < n; i++){
				uint8_t drawLine = memory[I + i];
				for (int j = 0; j < 8; j++){
					uint8_t pixel = drawLine & (0x80 >> j);
					if (pixel > 0){
						int totalX = x + j;
						int totalY = y + i;
						int ind = totalY * 64 + totalX;

						//Actual collision checking
						if (display[ind] == 1){
							V[0xF] = 1;
						}
						display[ind] ^= 1;
					}
				}
			}
			drawFlag = true;
			pc += 2;
			break;
		}
		case 0xE000:{
			switch(opcode & 0x00FF){
				//ExA1 - Skip next instruction if key with the value of Vx is not pressed.
				//Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
				case 0x00A1:{
					int x = (opcode & 0x0F00) >> 8;
					if (keys[x] == 1){
						pc += 4;
					}
					else{
						pc += 2;
					}
				}
				case 0x009E:{
					int x = (opcode & 0x0F00) >> 8;
					if (keys[x] == 0){
						pc += 4;
					}
					else{
						pc += 2;
					}	
				}
			}
			break;
		}
		case 0xF000:{
			switch(opcode & 0x00FF){
				//Fx07 - Set Vx = delay timer value. The value of DT is placed into Vx.
				case 0x0007:{
					int x = (opcode & 0x0F00) >> 8;
					V[x] = delayTimer;
					pc += 2;
					break;
				}
				//Fx15 - Set delay timer = Vx. DT is set equal to the value of Vx.
				case 0x0015:{
					int x = (opcode & 0x0F00) >> 8;
					delayTimer = V[x];
					pc += 2;
					break;
				}
				//Fx18 - Set sound timer = Vx. ST is set equal to the value of Vx.
				case 0x0018:{
					int x = (opcode & 0x0F00) >> 8;
					soundTimer = V[x];
					pc += 2;
					break;
				}
				//Fx1E - Set I = I + Vx. The values of I and Vx are added, and the results are stored in I.
				case 0x001E:{
					int x = (opcode & 0x0F00) >> 8;
					I = I + V[x];
					pc += 2;
					break;
				}
				//Fx29 - Set I = location of sprite for digit Vx. 
				//The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
				case 0x0029:{
					int x = (opcode & 0x0F00) >> 8;
					uint8_t character = V[x];
					I = 0x50 + (character * 5);
					pc += 2;
					break;
				}
				//Fx33 - Store BCD representation of Vx in memory locations I, I+1, and I+2. 
				//The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
				case 0x0033:{
					int x = (opcode & 0x0F00) >> 8;
					x = V[x];
					uint8_t hundreds = (x - (x % 100)) / 100;
					x -= 100 * hundreds;
					uint8_t tens = (x - (x % 10)) / 10;
					x -= 10 * tens;
					memory[I] = hundreds;
					memory[I+1] = tens;
					memory[I+2] = x;
					pc += 2;
					break;
				}
				//Fx65 - Read registers V0 through Vx from memory starting at location I. The interpreter reads values from memory starting at location I into registers V0 through Vx.
				case 0x0065:{
					int x = (opcode & 0x0F00) >> 8;
					for (int i = 0; i <= x; i++){
						V[i] = memory[I + i];
					}
					I = I + x;
					pc += 2;
					break;
				}
			}
			break;
		}
		default:
			printf("Unsupported Opcode: %x \n", opcode);
			return 1;
	}
	if (delayTimer > 0){
		delayTimer--;
	}
	if (delayTimer <= 2){
		delayTimer = 0;
	}

	if (soundTimer > 0){
		soundTimer--;
		printf("\a");
	}
	if (soundTimer <= 2){
		soundTimer = 0;
	}


}

int debugDisplay(){
	printf("\n");
	for (int i = 0; i < 32 * 64; i++) {
		printf("%d",display[i]);
		if (i % 64 == 0){
			printf("\n");
		}		
	}
}

int debugMode(){
	printf("Printing Registers");
	for(int i=0; i<0xF;i++){
		printf("\nV[%d] = %d", i, V[i]);
	}
}

int main(int argc, char **argv){
	clock_t tick;
    lcurses_clear_all();
    lcurses_hide_cursor();
	init();
	load(argv[1]);
	int status;
	do{
		tick = clock();
        lcurses_move_cursor(40, 1);
        lcurses_clear_line();
		status = run();
		draw(1, 1);
        while(clock() - tick < CLOCK_SPEED){}
	}while(status != 1);
    lcurses_show_cursor();
    lcurses_reset_attr();
	return 0;
}

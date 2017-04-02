#include <stdio.h>
#include <string.h>
#include "chip.h"
#include "lcurses.h"


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
					printf("Unsupported");
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
			int kk = opcode & 0x00FF;
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
		//Annn - Set I = nnn. The value of register I is set to nnn.
		case 0xA000:{
			I = opcode & 0x0FFF;
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
				int drawLine = memory[I + i];
				for (int j = 0; j < 8; j++){
					int pixel = drawLine & (0x80 >> j);
					if (pixel == 1){
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
		case 0xF000:{
			switch(opcode & 0x00FF){
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
			}
			break;
		}
		default:
			printf("Unsupported Opcode\n");
			return 1;
	}
}

int main(int argc, char **argv){
    lcurses_clear_all();
    lcurses_hide_cursor();
	init();
	load(argv[1]);
	int status;
	do{
        lcurses_move_cursor(40, 1);
        lcurses_clear_line();
		status = run();
		draw(1, 1);
        getchar();
	}while(status != 1);
    lcurses_show_cursor();
    lcurses_reset_attr();
	return 0;
}

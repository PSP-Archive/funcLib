#include "././Code/funcLib.h"
#include "emulator.h"
#include "main.h"
#include <pspgu.h>

unsigned char  emuRegisterV[16];
unsigned short emuRegisterI = 0;
unsigned short emuRegisterPC = 0x0200;
unsigned short emuRegisterSP = 0x0F00;
unsigned char  emuMemory[4096];

unsigned char  emuTimerDelay = 0;
unsigned char  emuTimerSound = 0;
float          emuTimerFrequency = 0.0167f; // 60.0f;

unsigned short emuKeyboard = 0;
unsigned char  emuKeyboardWaiting = 255;

unsigned char  emuDisplay[8][32];
unsigned char  emuDisplayScaleX = 7;
unsigned char  emuDisplayScaleY = 8;

float          emuFrequency = 0.001f; // 1000.0f;		// CPU Frequency in Hz (~1KHz is best)

void emuDebugInfo() {
     char tempInfo[1024];
     int i;
     sprintf(tempInfo, "Register PC: 0x%04X (%i)", emuRegisterPC, emuRegisterPC);
     sprintf(tempInfo, "%s\nRegister SP: 0x%04X (%i)", tempInfo, emuRegisterSP, emuRegisterSP);
     sprintf(tempInfo, "%s\nRegister V : ", tempInfo);
     for(i = 0; i < 8; i++)
          sprintf(tempInfo, "%s0x%02X ", tempInfo, emuRegisterV[i]);
     sprintf(tempInfo, "%s\n             ", tempInfo);
     for(i = 0; i < 8; i++)
          sprintf(tempInfo, "%s0x%02X ", tempInfo, emuRegisterV[i + 8]);
     sprintf(tempInfo, "%s\nRegister I : 0x%04X (%i)", tempInfo, emuRegisterI, emuRegisterI);
     fontDraw2dString(0, 0, emuFont, tempInfo);
}

bool emuKeyboardDown(unsigned char inKey) {
	if(inKey >= 16)
		return false;
	if(((1 << inKey) & emuKeyboard) > 0)
		return true;
	return false;
}

void emuInputButtonDown(unsigned char inKey) {
	unsigned short tempShort = (1 << inKey);
	emuKeyboard |= tempShort;
	if(emuKeyboardWaiting <= 15) {
		emuRegisterV[emuKeyboardWaiting] = inKey;
		emuKeyboardWaiting = 255;
	}
}

void emuInputButtonUp(unsigned char inKey) {
	unsigned short tempShort = (1 << inKey);
	emuKeyboard &= ~tempShort;
}

void emuDisplayClear() {
	memClear(emuDisplay, 256);
}

bool emuDisplayPlot(unsigned int inX, unsigned int inY) {
	unsigned char tempOld;
	unsigned char tempByte = 0;
	unsigned char tempMask = 0;

	while(inX >= 64)
		inX -= 64;
	while(inY >= 32)
		inY -= 32;
	while(inX >= 8)	{
		tempByte++;
		inX -= 8;
	}
	tempMask = (1 << inX);
	tempOld = emuDisplay[tempByte][inY];
	emuDisplay[tempByte][inY] = emuDisplay[tempByte][inY] ^ tempMask;
	return (tempOld != emuDisplay[tempByte][inY]);
}

bool emuDisplayPlotLine(unsigned int inX, unsigned int inY, unsigned char inLine) {
	bool tempOut = false;
	if((inLine & 0x80) != 0)
		if(emuDisplayPlot(inX + 0, inY))
			tempOut = true;
	if((inLine & 0x40) != 0)
		if(emuDisplayPlot(inX + 1, inY))
			tempOut = true;
	if((inLine & 0x20) != 0)
		if(emuDisplayPlot(inX + 2, inY))
			tempOut = true;
	if((inLine & 0x10) != 0)
		if(emuDisplayPlot(inX + 3, inY))
			tempOut = true;
	if((inLine & 0x08) != 0)
		if(emuDisplayPlot(inX + 4, inY))
			tempOut = true;
	if((inLine & 0x04) != 0)
		if(emuDisplayPlot(inX + 5, inY))
			tempOut = true;
	if((inLine & 0x02) != 0)
		if(emuDisplayPlot(inX + 6, inY))
			tempOut = true;
	if((inLine & 0x01) != 0)
		if(emuDisplayPlot(inX + 7, inY))
			tempOut = true;
	return true;
}

void emuDisplayRender() {
     graphicsRenderToVmem(texVmemCache2, 64);
     int i, j, k;
     for(j = 0; j < 32; j++) {
          for(i = 0; i < 8; i++) {
               for(k = 0; k < 8; k++) {
                    if(emuDisplay[i][j] & (1 << k))
                         graphics2dDrawPoint(((i << 3) + k), j, 0xFFFFFFFF);
                    else
                         graphics2dDrawPoint(((i << 3) + k), j, 0xFF000000);
               }
          }
     }
     graphicsRenderToFramebuffer();

     sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
     sceGuTexImage(0, 64, 32, 64, (void*)texVmemCache2);
     sceGuTexFunc(GU_TFX_DECAL, GU_TCC_RGB);
     sceGuTexFilter(GU_LINEAR, GU_LINEAR);
     sceGuTexWrap(GU_CLAMP, GU_CLAMP);
     
     vertTsVs* tempVerts = (vertTsVs*)sceGuGetMemory(sizeof(vertTsVs) << 1);

     tempVerts[0].vertX = 0;
     tempVerts[0].vertY = 0;
     tempVerts[0].vertZ = 0;
     tempVerts[0].vertU = 0;
     tempVerts[0].vertV = 0;

     tempVerts[1].vertX = SCREEN_WIDTH;
     tempVerts[1].vertY = SCREEN_HEIGHT;
     tempVerts[1].vertZ = 0;
     tempVerts[1].vertU = 64;
     tempVerts[1].vertV = 32;
     sceGuDisable(GU_DEPTH_TEST);
     sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, tempVerts);     
     sceGuEnable(GU_DEPTH_TEST);
     
     
	/*unsigned char tempColor;
    int i, j, k;
    for(j = 0; j < 32; j++) {
		for(i = 0; i < 8; i++) {
            for(k = 0; k < 8; k++) {
                tempColor = (emuDisplay[i][j] & (1 << k));
                if(tempColor != 0)
                    emuDisplayRenderPixel((i << 3) + k, (SCREEN_HEIGHT - j) - 1);                 
            }      
		}
    }*/
}

/*void emuDisplayRenderPixel(unsigned int inX, unsigned int inY) {
	inX *= emuDisplayScaleX;
    inY *= emuDisplayScaleY;
    graphics2dDrawRectFilled(inX, inY, emuDisplayScaleX, emuDisplayScaleY, 0xFF7F7F7F);
    graphics2dDrawRect(inX, inY, emuDisplayScaleX, emuDisplayScaleY, 0xFFFFFFFF);
}*/

void emuMemoryClear() {
	memClear(emuMemory, 4096);
}

void emuTimerTick() {
	if(emuTimerDelay != 0)
		emuTimerDelay -= 1;
	if(emuTimerSound != 0) {
		//Beep(784, (1000 / Timer_Frequency));
		emuTimerSound -= 1;
	}
}

void emuExecSingle() {
	unsigned short tempOp = (emuMemory[emuRegisterPC] << 8) + emuMemory[emuRegisterPC + 1];
	if(tempOp == 0x00E0)				// 00E0 - Clear Screen
	{
		emuDisplayClear();
		goto Emu_Exec_Done;
	}
	if(tempOp == 0x00EE)				// 00EE - Returns from a subroutine
	{
		emuRegisterPC = (emuMemory[emuRegisterSP] << 8) + emuMemory[emuRegisterSP + 1];
		emuRegisterSP -= 2;
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF000) == 0x1000)		// 1NNN - Jump to NNN
	{
		emuRegisterPC = (tempOp & 0x0FFF);
		return;
	}
	if((tempOp & 0xF000) == 0x2000)		// 2NNN - Calls subroutine at NNN.
	{
		emuRegisterSP += 2;
		emuMemory[emuRegisterSP + 1] = emuRegisterPC & 0x00FF;
		emuMemory[emuRegisterSP] = ((emuRegisterPC & 0xFF00) >> 8);
		emuRegisterPC = (tempOp & 0x0FFF);
		return;
	}
	if((tempOp & 0xF000) == 0x3000)		// 3XNN - Skip next inst if VX == NN
	{
		if(emuRegisterV[(tempOp & 0x0F00) >> 8] == (tempOp & 0x00FF))
			emuRegisterPC += 2;
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF000) == 0x4000)		// 4XNN - Skip next inst if VX != NN
	{
		if(emuRegisterV[(tempOp & 0x0F00) >> 8] != (tempOp & 0x00FF))
			emuRegisterPC += 2;
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF00F) == 0x5000)		// 5XY0 - Skip next inst if VX == VY
	{
		if(emuRegisterV[(tempOp & 0x0F00) >> 8] == emuRegisterV[(tempOp & 0x00F0) >> 4])
			emuRegisterPC += 2;
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF000) == 0x6000)		// 6XNN - VX = NN
	{
		emuRegisterV[(tempOp & 0x0F00) >> 8] = (tempOp & 0x00FF);
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF000) == 0x7000)		// 7XNN - VX += NN 
	{
		emuRegisterV[(tempOp & 0x0F00) >> 8] += (tempOp & 0x00FF);
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF00F) == 0x8000)		// 8XY0 - VX = VY
	{
		emuRegisterV[(tempOp & 0x0F00) >> 8] = emuRegisterV[(tempOp & 0x00F0) >> 4];
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF00F) == 0x8001)		// 8XY1 - VX = VX | VY
	{
		emuRegisterV[(tempOp & 0x0F00) >> 8] = (emuRegisterV[(tempOp & 0x0F00) >> 8] | emuRegisterV[(tempOp & 0x00F0) >> 4]);
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF00F) == 0x8002)		// 8XY2 - VX = VX & VY
	{
		emuRegisterV[(tempOp & 0x0F00) >> 8] = (emuRegisterV[(tempOp & 0x0F00) >> 8] & emuRegisterV[(tempOp & 0x00F0) >> 4]);
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF00F) == 0x8003)		// 8XY3 - VX = VX ^ VY
	{
		emuRegisterV[(tempOp & 0x0F00) >> 8] = (emuRegisterV[(tempOp & 0x0F00) >> 8] ^ emuRegisterV[(tempOp & 0x00F0) >> 4]);
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF00F) == 0x8004)		// 8XY4 - VX = VX + VY, if carry then VF = 1 else VF = 0
	{
		if(((int)emuRegisterV[(tempOp & 0x0F00) >> 8] + (int)emuRegisterV[(tempOp & 0x00F0) >> 4]) > 0x00FF)
			emuRegisterV[15] = 1;
		else
			emuRegisterV[15] = 0;
		emuRegisterV[(tempOp & 0x0F00) >> 8] = (emuRegisterV[(tempOp & 0x0F00) >> 8] + emuRegisterV[(tempOp & 0x00F0) >> 4]);
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF00F) == 0x8005)		// 8XY5 - VX = VX - VY, if borrow then VF = 0 else VF = 1
	{
		if(emuRegisterV[(tempOp & 0x00F0) >> 4] >= emuRegisterV[(tempOp & 0x0F00) >> 8])
			emuRegisterV[15] = 0;
		else
			emuRegisterV[15] = 1;
		emuRegisterV[(tempOp & 0x0F00) >> 8] = (emuRegisterV[(tempOp & 0x0F00) >> 8] - emuRegisterV[(tempOp & 0x00F0) >> 4]);
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF0FF) == 0x8006)		// 8X06 - VX = VX >> 1, VF = (VX & 0x0001)
	{
		emuRegisterV[15] = (emuRegisterV[(tempOp & 0x0F00) >> 8] & 0x0001);
		emuRegisterV[(tempOp & 0x0F00) >> 8] = emuRegisterV[(tempOp & 0x0F00) >> 8] & 0xFFFE;	// Make sure LSB == 0
		emuRegisterV[(tempOp & 0x0F00) >> 8] = (emuRegisterV[(tempOp & 0x0F00) >> 8] >> 1);
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF00F) == 0x8007)		// 8XY7 - VX = VY - VX, if borrow then VF = 0 else VF = 1
	{
		if(emuRegisterV[(tempOp & 0x0F00) >> 8] >= emuRegisterV[(tempOp & 0x00F0) >> 4])
			emuRegisterV[15] = 0;
		else
			emuRegisterV[15] = 1;
		emuRegisterV[(tempOp & 0x0F00) >> 8] = (emuRegisterV[(tempOp & 0x00F0) >> 4] - emuRegisterV[(tempOp & 0x0F00) >> 8]);
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF0FF) == 0x800E)		// 8X0E - VX = VX << 1, VF = (VX & 0x8000) >> 15
	{
		emuRegisterV[15] = ((emuRegisterV[(tempOp & 0x0F00) >> 8] & 0x8000) >> 15);
		emuRegisterV[(tempOp & 0x0F00) >> 8] = emuRegisterV[(tempOp & 0x0F00) >> 8] & 0x7FFF;	// Make sure MSB == 0
		emuRegisterV[(tempOp & 0x0F00) >> 8] = (emuRegisterV[(tempOp & 0x0F00) >> 8] << 1);
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF00F) == 0x5000)		// 9XY0 - Skip next inst if VX != VY
	{
		if(emuRegisterV[(tempOp & 0x0F00) >> 8] != emuRegisterV[(tempOp & 0x00F0) >> 4])
			emuRegisterPC += 2;
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF000) == 0xA000)		// ANNN - Sets register I to NNN
	{
		emuRegisterI = (tempOp & 0x0FFF);
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF000) == 0xB000)		// BNNN - Jumps to the address NNN plus V0
	{
		emuRegisterPC = (tempOp & 0x0FFF) + emuRegisterV[0];
		return;
	}
	if((tempOp & 0xF000) == 0xC000)		// CXNN - Sets VX to a random number and NN.
	{
		emuRegisterV[(tempOp & 0x0F00) >> 8] = (mathRandi(255) & (tempOp & 0x00FF));
		goto Emu_Exec_Done;
    }
	if((tempOp & 0xF000) == 0xD000)		// DXYN - Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn't happen.
	{
		emuRegisterV[15] = 0;
		int j;
		for(j = 0; j < (tempOp & 0x000F); j++)
		{
			if(emuDisplayPlotLine(emuRegisterV[(tempOp & 0x0F00) >> 8], emuRegisterV[(tempOp & 0x00F0) >> 4] + j, emuMemory[emuRegisterI + j]))
				emuRegisterV[15] = 1;
		}
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF0FF) == 0xE09E)		// EX9E - Skips the next instruction if the key stored in VX is pressed.
	{
		if((emuKeyboard & (1 << emuRegisterV[(tempOp & 0x0F00) >> 8])) != 0)
			emuRegisterPC += 2;
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF0FF) == 0xE0A1)		// EXA1 - Skips the next instruction if the key stored in VX isn't pressed.
	{
		if((emuKeyboard & (1 << emuRegisterV[(tempOp & 0x0F00) >> 8])) == 0)
			emuRegisterPC += 2;
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF0FF) == 0xF007)		// FX07 - Sets VX to the value of the delay timer.
	{
		emuRegisterV[(tempOp & 0x0F00) >> 8] = emuTimerDelay;
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF0FF) == 0xF00A)		// FX0A - A key press is awaited, and then stored in VX.
	{
		emuKeyboardWaiting = ((tempOp & 0x0F00) >> 8);
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF0FF) == 0xF015)		// FX15 - Sets the delay timer to VX.
	{
		emuTimerDelay = emuRegisterV[(tempOp & 0x0F00) >> 8];
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF0FF) == 0xF018)		// FX18 - Sets the sound timer to VX.
	{
		emuTimerSound = emuRegisterV[(tempOp & 0x0F00) >> 8];
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF0FF) == 0xF01E)		// FX1E - Adds VX to I.
	{
		emuRegisterI += emuRegisterV[(tempOp & 0x0F00) >> 8];
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF0FF) == 0xF029)		// FX29 - Sets I to the location of the sprite for the character in VX.
	{
		emuRegisterI = 5 * emuRegisterV[(tempOp & 0x0F00) >> 8];
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF0FF) == 0xF033)		// FX33 - Stores the BCD representation of VX at the addresses I, I plus 1, and I plus 2.
	{
		unsigned char tempVX = emuRegisterV[(tempOp & 0x0F00) >> 8];
		emuMemory[emuRegisterI] = tempVX / 100;
		tempVX -= (emuMemory[emuRegisterI] * 100);
		emuMemory[emuRegisterI + 1] = tempVX / 10;
		tempVX -= (emuMemory[emuRegisterI + 1] * 10);
		emuMemory[emuRegisterI + 2] = tempVX;
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF0FF) == 0xF055)		// FX55 - Stores V0 to VX in emuMemory starting at address I.
	{
		memCopy(&emuMemory[emuRegisterI], emuRegisterV, (((tempOp & 0x0F00) >> 8) + 1));
		goto Emu_Exec_Done;
	}
	if((tempOp & 0xF0FF) == 0xF065)		// FX65 - Fills V0 to VX with values from emuMemory starting at address I.
	{
		memCopy(emuRegisterV, &emuMemory[emuRegisterI], (((tempOp & 0x0F00) >> 8) + 1));
	}
	
Emu_Exec_Done:
	emuRegisterPC += 2;
}

void emuProgramLoad(char* inPath) {
	if(!inPath || (inPath[0] == 0))
		return;
	filesysFile *tempFile = filesysOpen(inPath, FILESYS_FILE_MODE_READ | FILESYS_FILE_MODE_BINARY);
	if(!tempFile)
		return;
	emuReset();
	filesysRead(&emuMemory[0x0200], tempFile->fsfSize, tempFile);
	filesysClose(tempFile);
}

void emuReset() {
    memClear(emuMemory, 4096);
    
	// Create loop program when no prog is loaded
	emuMemory[0x0200] = 0x12;
	emuMemory[0x0201] = 0x00;
	
	// Basic Hex Font HardCoded
	emuMemory[0x0000] = 0xF0;
	emuMemory[0x0001] = 0x90;
	emuMemory[0x0002] = 0x90;
	emuMemory[0x0003] = 0x90;
	emuMemory[0x0004] = 0xF0;
	
	emuMemory[0x0005] = 0x20;
	emuMemory[0x0006] = 0x60;
	emuMemory[0x0007] = 0x20;
	emuMemory[0x0008] = 0x20;
	emuMemory[0x0009] = 0x70;

	emuMemory[0x000A] = 0xF0;
	emuMemory[0x000B] = 0x10;
	emuMemory[0x000C] = 0xF0;
	emuMemory[0x000D] = 0x80;
	emuMemory[0x000E] = 0xF0;

	emuMemory[0x000F] = 0xF0;
	emuMemory[0x0010] = 0x10;
	emuMemory[0x0011] = 0xF0;
	emuMemory[0x0012] = 0x10;
	emuMemory[0x0013] = 0xF0;
	
	emuMemory[0x0014] = 0x90;
	emuMemory[0x0015] = 0x90;
	emuMemory[0x0016] = 0xF0;
	emuMemory[0x0017] = 0x10;
	emuMemory[0x0018] = 0x10;

	emuMemory[0x0019] = 0xF0;
	emuMemory[0x001A] = 0x80;
	emuMemory[0x001B] = 0xF0;
	emuMemory[0x001C] = 0x10;
	emuMemory[0x001D] = 0xF0;

	emuMemory[0x001E] = 0xF0;
	emuMemory[0x001F] = 0x80;
	emuMemory[0x0020] = 0xF0;
	emuMemory[0x0021] = 0x90;
	emuMemory[0x0022] = 0xF0;

	emuMemory[0x0023] = 0xF0;
	emuMemory[0x0024] = 0x10;
	emuMemory[0x0025] = 0x20;
	emuMemory[0x0026] = 0x40;
	emuMemory[0x0027] = 0x40;

	emuMemory[0x0028] = 0xF0;
	emuMemory[0x0029] = 0x90;
	emuMemory[0x002A] = 0xF0;
	emuMemory[0x002B] = 0x90;
	emuMemory[0x002C] = 0xF0;

	emuMemory[0x002D] = 0xF0;
	emuMemory[0x002E] = 0x90;
	emuMemory[0x002F] = 0xF0;
	emuMemory[0x0030] = 0x10;
	emuMemory[0x0031] = 0xF0;

	emuMemory[0x0032] = 0xF0;
	emuMemory[0x0033] = 0x90;
	emuMemory[0x0034] = 0xF0;
	emuMemory[0x0035] = 0x90;
	emuMemory[0x0036] = 0x90;

	emuMemory[0x0037] = 0xE0;
	emuMemory[0x0038] = 0x90;
	emuMemory[0x0039] = 0xE0;
	emuMemory[0x003A] = 0x90;
	emuMemory[0x003B] = 0xE0;

	emuMemory[0x003C] = 0xF0;
	emuMemory[0x003D] = 0x80;
	emuMemory[0x003E] = 0x80;
	emuMemory[0x003F] = 0x80;
	emuMemory[0x0040] = 0xF0;

	emuMemory[0x0041] = 0xE0;
	emuMemory[0x0042] = 0x90;
	emuMemory[0x0043] = 0x90;
	emuMemory[0x0044] = 0x90;
	emuMemory[0x0045] = 0xE0;

	emuMemory[0x0046] = 0xF0;
	emuMemory[0x0047] = 0x80;
	emuMemory[0x0048] = 0xF0;
	emuMemory[0x0049] = 0x80;
	emuMemory[0x004A] = 0xF0;

	emuMemory[0x004B] = 0xF0;
	emuMemory[0x004C] = 0x80;
	emuMemory[0x004D] = 0xF0;
	emuMemory[0x004E] = 0x80;
	emuMemory[0x004F] = 0x80;
	
	memClear(emuDisplay, 256);
	
	emuRegisterI = 0;
    emuRegisterPC = 0x0200;
    emuRegisterSP = 0x0F00;

    emuTimerDelay = 0;
    emuTimerSound = 0;
    emuKeyboard = 0;
    emuKeyboardWaiting = 255;
}

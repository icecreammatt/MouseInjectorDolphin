//==========================================================================
// Mouse Injector for Dolphin
//==========================================================================
// Copyright (C) 2018 Carnivorous
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL CARNIVOROUS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==========================================================================
#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include "main.h"
#include "memory.h"
#include "mouse.h"
#include "./games/game.h"

enum EDITINGCURRENT {EDITINGSENSITIVITY, EDITINGCROSSHAIR};

static uint8_t mousetoggle = 0;
static uint8_t selectedoption = EDITINGSENSITIVITY;
static uint8_t locksettings = 0;
static uint8_t welcomed = 0;

uint8_t sensitivity = 20;
uint8_t crosshair = 3;
uint8_t invertpitch = 0;

int32_t main(void);
static void quit(void);
static void GUI_Init(void);
static void GUI_Welcome(void);
static void GUI_Interact(void);
static void GUI_Update(void);
static void GUI_Clear(void);
static void INI_Load(void);
static void INI_Save(void);

//==========================================================================
// Purpose: run everything like a god damn speed demon
//==========================================================================
int32_t main(void)
{
	GUI_Init();
	if(!MEM_Init()) // close if dolphin was not detected
	{
		printf("\n %s%s\n\n   Dolphin not detected. Closing...", TITLE, LINE);
		Sleep(3000);
		return 0;
	}
	if(!MOUSE_Init()) // close if mouse was not detected
	{
		printf("\n %s%s\n\n   Mouse not detected. Closing...", TITLE, LINE);
		Sleep(3000);
		MEM_Quit();
		return 0;
	}
	INI_Load(); // load settings
	if(!welcomed) // if not welcomed before
		GUI_Welcome(); // show welcome message - wait for user input before continuing
	GUI_Update(); // update screen with options
	atexit(quit); // set function to run when program is closed
	while(1) // loop forever
	{
		GUI_Interact();
		if(mousetoggle)
		{
			if(GAME_Status())
				GAME_Inject();
			else
				MEM_UpdateEmuoffset();
		}
		Sleep(TICKRATE);
	}
	return 0;
}
//==========================================================================
// Purpose: run when quit
//==========================================================================
static void quit(void)
{
	INI_Save();
	MOUSE_Quit();
	MEM_Quit();
}
//==========================================================================
// Purpose: initialize console
//==========================================================================
static void GUI_Init(void)
{
	SetConsoleTitle("Mouse Injector");
	MoveWindow(GetConsoleWindow(), 100, 100, 683, 336, TRUE); // get console window handle and move window to required position
}
//==========================================================================
// Purpose: prints the welcome message
// Changed Globals: welcomed
//==========================================================================
static void GUI_Welcome(void)
{
	printf("\n   %s%s\n\n   Addendum - Please Read before Use\n\n\n", TITLE, LINE);
	printf("    1)  This is a unfinished test, expect issues and crashes\n\n");
	printf("    2)  Made for the NTSC releases of TimeSplitters 2/3 and 007: NightFire\n\n");
	printf("    3)  This will only work with game IDs matching: GTSE4F/G3FE69/GO7E69\n\n");
	printf("    4)  You must use the included Dolphin bundle or it will not work\n\n");
	printf("    5)  Please do not install over different versions of Dolphin\n\n");
	printf("    6)  All sub-systems are unsupported - use arrow keys for sentries/map maker\n\n");
	printf("    7)  Read readme.txt for a quick start guide - thank you and enjoy\n\n");
	printf("\n   Press CTRL+0 to confirm you've read this message...\n%s\n", LINE);
	while(1)
	{
		Sleep(250);
		if(K_CTRL0) // wait for user input before continuing
			break;
	}
	welcomed = 1;
}
//==========================================================================
// Purpose: checks keyboard and will update internal values
// Changed Globals: mousetoggle, selectedoption, invertpitch, sensitivity, crosshair, locksettings
//==========================================================================
static void GUI_Interact(void)
{
	uint8_t updateinterface = 0, updatequick = 0;
	if(K_4) // mouse toggle (4)
	{
		MOUSE_Lock();
		MOUSE_Update();
		mousetoggle = !mousetoggle;
		updateinterface = 1;
	}
	if(K_5 && !locksettings && !updateinterface) // mouse sensitivity (5)
	{
		selectedoption = EDITINGSENSITIVITY;
		updateinterface = 1;
	}
	if(K_6 && !locksettings && !updateinterface) // crosshair sensitivity (6)
	{
		selectedoption = EDITINGCROSSHAIR;
		updateinterface = 1;
	}
	if(K_7 && !locksettings && !updateinterface) // invert pitch toggle (7)
	{
		invertpitch = !invertpitch;
		updateinterface = 1;
	}
	if(K_PLUS && !locksettings && !updateinterface) // numpad plus (+)
	{
		if(selectedoption == EDITINGSENSITIVITY && sensitivity < 100)
			sensitivity++, updateinterface = 1;
		if(selectedoption == EDITINGCROSSHAIR && crosshair < 18)
			crosshair++, updateinterface = 1;
		updatequick = 1;
	}
	if(K_MINUS && !locksettings && !updateinterface) // numpad minus (-)
	{
		if(selectedoption == EDITINGSENSITIVITY && sensitivity > 1)
			sensitivity--, updateinterface = 1;
		if(selectedoption == EDITINGCROSSHAIR && crosshair > 0)
			crosshair--, updateinterface = 1;
		updatequick = 1;
	}
	if(K_CTRL0 && !updateinterface) // hide/show settings (CTRL+0)
	{
		locksettings = !locksettings;
		updateinterface = 1;
	}
	if(updateinterface)
	{
		GUI_Update();
		Sleep(updatequick ? 100 : 200);
	}
}
//==========================================================================
// Purpose: update interface
//==========================================================================
static void GUI_Update(void)
{
	GUI_Clear();
	printf("\n %s%s\n\n   Main Menu - Press [#] to Use Menu\n\n\n", TITLE, LINE); // title
	printf(mousetoggle ? "   [4] - [ON] Mouse Injection" : "   [4] - [OFF] Mouse Injection");
	if(!locksettings)
	{
		printf("\n\n   [5] - Mouse Sensitivity: %d%%", sensitivity * 5);
		printf(selectedoption == EDITINGSENSITIVITY ? " [+/-]\n\n" : "\n\n");
		printf("   [6] - Crosshair Movement: ");
		printf(crosshair ? "%d%%" : "Disabled", crosshair * 100 / 6);
		printf(selectedoption == EDITINGCROSSHAIR ? " [+/-]\n\n" : "\n\n");
		printf(invertpitch ? "   [7] - [ON] Invert Pitch\n\n" : "   [7] - [OFF] Invert Pitch\n\n");
		printf("\n\n\n\n");
	}
	else
		printf("\n\n\n\n\n\n\n\n\n\n\n\n");
	printf(!locksettings ? "\n   [CTRL+0] - Lock Settings" : "\n   [CTRL+0] - Unlock Settings");
	printf("\n\n Note: [+/-] to Change Values\n%s\n", LINE);
}
//==========================================================================
// Purpose: clear screen without using system("cls")
//==========================================================================
static void GUI_Clear(void)
{
	DWORD n; // number of characters written
	DWORD size; // number of visible characters
	COORD coord = {0}; // top left screen position
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE consolehandle = GetStdHandle(STD_OUTPUT_HANDLE); // get a handle to the console
	GetConsoleScreenBufferInfo(consolehandle, &csbi);
	size = csbi.dwSize.X * csbi.dwSize.Y; // find the number of characters to overwrite
	FillConsoleOutputCharacter(consolehandle, TEXT(' '), size, coord, &n); // overwrite the screen buffer with whitespace
	GetConsoleScreenBufferInfo(consolehandle, &csbi);
	FillConsoleOutputAttribute(consolehandle, csbi.wAttributes, size, coord, &n);
	SetConsoleCursorPosition(consolehandle, coord); // reset the cursor to the top left position
}
//==========================================================================
// Purpose: loads settings stored in mouseinjector.ini
// Changes Globals: sensitivity, crosshair, invertpitch, locksettings, welcomed
//==========================================================================
static void INI_Load(void)
{
	FILE *fileptr; // create a file pointer and open mouseinjector.ini from same dir as our program
	if((fileptr = fopen("mouseinjector.ini", "r")) != NULL) // if the INI exists
	{
		char line[128][128]; // char array used for file to write to
		char lines[128]; // maximum lines read size
		uint8_t counter = 0; // used to assign each line to a array
		while(fgets(lines, sizeof(lines), fileptr) != NULL && counter < 11) // read the first 10 lines
		{
			strcpy(line[counter], lines); // read file lines and assign value to line array
			counter++; // add 1 to counter, so the next line can be read
		}
		fclose(fileptr); // close the file stream
		if(counter == 5) // check if mouseinjector.ini length is valid
		{
			sensitivity = ClampInt(atoi(line[0]), 1, 100);
			crosshair = ClampInt(atoi(line[1]), 0, 18);
			invertpitch = !(!atoi(line[2]));
			locksettings = !(!atoi(line[3]));
			welcomed = !(!atoi(line[4]));
		}
		else
		{
			MessageBox(HWND_DESKTOP, "Loading mouseinjector.ini failed!\n\nUnknown values detected, loading default settings.", "Error", MB_ICONERROR | MB_OK); // tell the user loading mouseinjector.ini failed
			INI_Save(); // overwrite mouseinjector.ini with valid settings
		}
	}
	else // if loading file failed
	{
		MessageBox(HWND_DESKTOP, "Loading mouseinjector.ini failed!\n\nAttempting to create mouseinjector.ini file.", "Error", MB_ICONERROR | MB_OK); // tell the user loading mouseinjector.ini failed
		INI_Save(); // create mouseinjector.ini
	}
}
//==========================================================================
// Purpose: saves current settings to mouseinjector.ini
//==========================================================================
static void INI_Save(void)
{
	FILE *fileptr; // create a file pointer and open mouseinjector.ini from same dir as our program
	if((fileptr = fopen("mouseinjector.ini", "w")) != NULL) // if the INI exists
	{
		fprintf(fileptr, "%u\n%u\n%u\n%u\n%u", sensitivity, crosshair, invertpitch, locksettings, welcomed); // write current settings to mouseinjector.ini
		fclose(fileptr); // close the file stream
	}
	else // if saving file failed
		MessageBox(HWND_DESKTOP, "Saving mouseinjector.ini failed!", "Error", MB_ICONERROR | MB_OK); // tell the user saving mouseinjector.ini failed
}
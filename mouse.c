//==========================================================================
// Mouse Injector for Dolphin
//==========================================================================
// Copyright (C) 2019 Carnivorous
// All rights reserved.
//
// Mouse Injector is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, visit http://www.gnu.org/licenses/gpl-2.0.html
//==========================================================================
#include <stdint.h>
#include <windows.h>
#include "mouse.h"
#include "./manymouse/manymouse.h"

int32_t xmouse, ymouse; // holds mouse input data (used for gamedrivers)

static POINT mouselock; // center screen X and Y var for mouse
static ManyMouseEvent event; // hold current mouse event
static unsigned char lockmouseclock = 0; // limit SetCursorPos execution

uint8_t MOUSE_Init(void);
void MOUSE_Quit(void);
void MOUSE_Update(void);

//==========================================================================
// Purpose: initialize manymouse and returns detected devices (0 = not found)
//==========================================================================
uint8_t MOUSE_Init(void)
{
	return ManyMouse_Init();
}
//==========================================================================
// Purpose: safely quit manymouse
//==========================================================================
void MOUSE_Quit(void)
{
	ManyMouse_Quit();
}
//==========================================================================
// Purpose: update cursor lock position
//==========================================================================
void MOUSE_Lock(void)
{
	GetCursorPos(&mouselock);
}
//==========================================================================
// Purpose: update xmouse/ymouse with mouse input
// Changed Globals: xmouse, ymouse, event
//==========================================================================
void MOUSE_Update(void)
{
	if(lockmouseclock % 25 == 0) // don't execute every tick
		SetCursorPos(mouselock.x, mouselock.y); // set mouse position back to center of screen
	xmouse = ymouse = 0; // reset mouse input
	while(ManyMouse_PollEvent(&event))
	{
		if(event.type == MANYMOUSE_EVENT_RELMOTION)
		{
			if(event.item == 0)
				xmouse += event.value;
			else
				ymouse += event.value;
		}
	}
	lockmouseclock--;
}
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
#include <stdint.h>
#include <windows.h>
#include "./manymouse/manymouse.h"

int32_t xmouse, ymouse; // holds mouse input data (used for gamedrivers)

static POINT mouselock; // center screen X and Y var for mouse

uint8_t MOUSE_Init(void);
void MOUSE_Quit(void);
void MOUSE_Update(void);

//==========================================================================
// Purpose: initialize manymouse and returns init
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
// Purpose: lock cursor in place
//==========================================================================
void MOUSE_Lock(void)
{
	GetCursorPos(&mouselock);
}
//==========================================================================
// Purpose: update xmouse/ymouse with mouse input
// Changed Globals: xmouse, ymouse
//==========================================================================
void MOUSE_Update(void)
{
	SetCursorPos(mouselock.x, mouselock.y); // set mouse position back to center of screen
	ManyMouseEvent event; // hold current mouse movement event
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
}
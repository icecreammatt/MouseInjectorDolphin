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
#include "../main.h"
#include "../memory.h"
#include "../mouse.h"
#include "game.h"

// COD2BRO ADDRESSES - OFFSET ADDRESSES BELOW (REQUIRES PLAYERBASE TO USE)
#define COD2BRO_footcamx 0x8048B574 - 0x80483280
#define COD2BRO_footcamy 0x8048B570 - 0x80483280
#define COD2BRO_footfov 0x8048B54C - 0x80483280
#define COD2BRO_vehiclecamx 0x8044B2F4 - 0x80442FE0
#define COD2BRO_vehiclecamy 0x8044B2F0 - 0x80442FE0
#define COD2BRO_vehiclefov 0x8044B2CC - 0x80442FE0
// STATIC ADDRESSES BELOW
#define COD2BRO_playerbase 0x800030D4 // playable character pointer

static uint8_t COD2BRO_Status(void);
static void COD2BRO_DetectInterface(void);
static void COD2BRO_Inject(void);

static const GAMEDRIVER GAMEDRIVER_INTERFACE =
{
	"Call of Duty 2: Big Red One",
	COD2BRO_Status,
	COD2BRO_Inject
};

static uint32_t playerbase = 0;
static uint8_t vehiclemode = 0;

const GAMEDRIVER *GAME_COD2BRO = &GAMEDRIVER_INTERFACE;

//==========================================================================
// Purpose: return 1 if game is detected
//==========================================================================
static uint8_t COD2BRO_Status(void)
{
	return (MEM_ReadInt(0x80000000) == 0x47514345 && MEM_ReadInt(0x80000004) == 0x35320000); // check game header to see if it matches COD2BRO
}
//==========================================================================
// Purpose: detects player pointer and interface type
// Changed Globals: playerbase, vehiclemode
//==========================================================================
static void COD2BRO_DetectInterface(void)
{
	const uint32_t tempplayerbase = ((uint32_t)MEM_ReadInt(COD2BRO_playerbase)) + 0x80000000; // offset playerbase to RAM range
	if(NOTWITHINMEMRANGE(tempplayerbase)) // if in menu or map unloaded, reset playerbase and return
	{
		playerbase = 0;
		return;
	}
	const uint32_t footfov = MEM_ReadInt(tempplayerbase + COD2BRO_footfov);
	const uint32_t vehiclefov = MEM_ReadInt(tempplayerbase + COD2BRO_vehiclefov);
	if(playerbase == tempplayerbase || footfov == vehiclefov) // if same map or map hasn't loaded, return
		return;
	playerbase = tempplayerbase;
	vehiclemode = footfov == 0 && vehiclefov != 0; // if on foot fov doesn't equal default zoom value and tank fov is non-zero, likely map is using vehicle interface
}
//==========================================================================
// Purpose: calculate mouse movement and inject into current game
//==========================================================================
static void COD2BRO_Inject(void)
{
	COD2BRO_DetectInterface();
	if(NOTWITHINMEMRANGE(playerbase))
		return;
	if(xmouse == 0 && ymouse == 0) // if mouse is idle
		return;
	uint32_t fovaddress, camxaddress, camyaddress;
	if(!vehiclemode) // if player is using on foot interface
	{
		fovaddress = playerbase + COD2BRO_footfov;
		camxaddress = playerbase + COD2BRO_footcamx;
		camyaddress = playerbase + COD2BRO_footcamy;
	}
	else
	{
		fovaddress = playerbase + COD2BRO_vehiclefov;
		camxaddress = playerbase + COD2BRO_vehiclecamx;
		camyaddress = playerbase + COD2BRO_vehiclecamy;
	}
	const float fov = MEM_ReadFloat(fovaddress);
	float camx = MEM_ReadFloat(camxaddress);
	float camy = MEM_ReadFloat(camyaddress);
	if(!vehiclemode && fov > 0 && fov <= 1.25f || fov <= 2.f) // if fov is valid
	{
		camx -= (float)xmouse / 10.f * ((float)sensitivity / 40.f) * (fov / 1.f); // normal calculation method for X
		camy += (float)(invertpitch ? -ymouse : ymouse) / 10.f * ((float)sensitivity / 40.f) * (fov / 1.f); // normal calculation method for Y
		if(camx < -360)
			camx += 360;
		else if(camx >= 360)
			camx -= 360;
		MEM_WriteFloat(camxaddress, camx);
		MEM_WriteFloat(camyaddress, camy);
	}
}
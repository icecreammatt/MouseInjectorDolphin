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

#define PI 3.14159265f // 0x40490FDB
#define TAU 6.2831853f // 0x40C90FDB
#define CROSSHAIRY 1.483529925f // 0x3FBDE44F
// MOHEA ADDRESSES - OFFSET ADDRESSES BELOW (REQUIRES PLAYERBASE TO USE)
#define MOHEA_camx 0x80A7E610 - 0x80A7E500
#define MOHEA_camy 0x80A7E608 - 0x80A7E500
#define MOHEA_fov 0x80A7E638 - 0x80A7E500
#define MOHEA_health 0x80A7E8D0 - 0x80A7E500
// STATIC ADDRESSES BELOW
#define MOHEA_playerbase 0x80574308 // playable character pointer

static uint8_t MOHEA_Status(void);
static void MOHEA_Inject(void);

static const GAMEDRIVER GAMEDRIVER_INTERFACE =
{
	"Medal of Honor: European Assault",
	MOHEA_Status,
	MOHEA_Inject
};

const GAMEDRIVER *GAME_MOHEA = &GAMEDRIVER_INTERFACE;

//==========================================================================
// Purpose: return 1 if game is detected
//==========================================================================
static uint8_t MOHEA_Status(void)
{
	return (MEM_ReadInt(0x80000000) == 0x474F4E45 && MEM_ReadInt(0x80000004) == 0x36390000); // check game header to see if it matches MOHEA
}
//==========================================================================
// Purpose: calculate mouse look and inject into current game
//==========================================================================
static void MOHEA_Inject(void)
{
	if(xmouse == 0 && ymouse == 0) // if mouse is idle
		return;
	const uint32_t playerbase = (uint32_t)MEM_ReadInt(MOHEA_playerbase);
	if(NOTWITHINMEMRANGE(playerbase)) // if playerbase is invalid
		return;
	const float fov = MEM_ReadFloat(playerbase + MOHEA_fov);
	const float health = MEM_ReadFloat(playerbase + MOHEA_health);
	float camx = MEM_ReadFloat(playerbase + MOHEA_camx);
	float camy = MEM_ReadFloat(playerbase + MOHEA_camy);
	if(camx >= -TAU && camx <= TAU && camy >= -CROSSHAIRY && camy <= CROSSHAIRY && health > 0)
	{
		camx -= (float)xmouse / 10.f * ((float)sensitivity / 40.f) / (360.f / TAU) / (35.f / fov); // normal calculation method for X
		camy += (float)(!invertpitch ? -ymouse : ymouse) / 10.f * ((float)sensitivity / 40.f) / (90.f / CROSSHAIRY) / (35.f / fov); // normal calculation method for Y
		if(camx <= -TAU)
			camx += TAU;
		else if(camx >= TAU)
			camx -= TAU;
		camy = ClampFloat(camy, -CROSSHAIRY, CROSSHAIRY);
		MEM_WriteFloat(playerbase + MOHEA_camx, camx);
		MEM_WriteFloat(playerbase + MOHEA_camy, camy);
	}
}
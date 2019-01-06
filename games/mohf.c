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
#define CROSSHAIRY 1.450000048f // 0xBFB9999A
// MOHF ADDRESSES - OFFSET ADDRESSES BELOW (REQUIRES PLAYERBASE TO USE)
#define MOHF_camx 0x8061365C - 0x806133C0
#define MOHF_camy 0x80613660 - 0x806133C0
#define MOHF_fov 0x806339CC - 0x806335A0
#define MOHF_sentryx 0x806339BC - 0x806335A0
#define MOHF_sentryy 0x806339C0 - 0x806335A0
#define MOHF_sentryxlimit 0x806339B4 - 0x806335A0
#define MOHF_sentryylimit 0x806339B8 - 0x806335A0
#define MOHF_sentryflag 0x80633C6C - 0x806335A0
// STATIC ADDRESSES BELOW
#define MOHF_playerbase 0x801DDDFC // playable character pointer

static uint8_t MOHF_Status(void);
static void MOHF_Inject(void);

static const GAMEDRIVER GAMEDRIVER_INTERFACE =
{
	"Medal of Honor: Frontline",
	MOHF_Status,
	MOHF_Inject
};

const GAMEDRIVER *GAME_MOHF = &GAMEDRIVER_INTERFACE;

//==========================================================================
// Purpose: return 1 if game is detected
//==========================================================================
static uint8_t MOHF_Status(void)
{
	return (MEM_ReadInt(0x80000000) == 0x474D4645 && MEM_ReadInt(0x80000004) == 0x36390000); // check game header to see if it matches MOHF
}
//==========================================================================
// Purpose: calculate mouse movement and inject into current game
//==========================================================================
static void MOHF_Inject(void)
{
	if(xmouse == 0 && ymouse == 0) // if mouse is idle
		return;
	const uint32_t playerbase = (uint32_t)MEM_ReadInt(MOHF_playerbase);
	if(NOTWITHINMEMRANGE(playerbase)) // if playerbase is invalid
		return;
	const int sentryflag = MEM_ReadInt(playerbase + MOHF_sentryflag);
	const float fov = MEM_ReadFloat(playerbase + MOHF_fov);
	if(sentryflag == 1) // if not using sentry
	{
		float camx = MEM_ReadFloat(playerbase + MOHF_camx);
		float camy = MEM_ReadFloat(playerbase + MOHF_camy);
		if(camx >= -TAU && camx <= TAU && camy >= -CROSSHAIRY && camy <= CROSSHAIRY)
		{
			camx -= (float)xmouse / 10.f * ((float)sensitivity / 40.f) / (360.f / TAU) / (35.f / fov); // normal calculation method for X
			camy += (float)(invertpitch ? -ymouse : ymouse) / 10.f * ((float)sensitivity / 40.f) / (90.f / CROSSHAIRY) / (35.f / fov); // normal calculation method for Y
			if(camx <= -TAU)
				camx += TAU;
			else if(camx >= TAU)
				camx -= TAU;
			camy = ClampFloat(camy, -CROSSHAIRY, CROSSHAIRY);
			MEM_WriteFloat(playerbase + MOHF_camx, camx);
			MEM_WriteFloat(playerbase + MOHF_camy, camy);
		}
	}
	else if(sentryflag == 21)// if using sentry
	{
		float sentryx = MEM_ReadFloat(playerbase + MOHF_sentryx);
		float sentryy = MEM_ReadFloat(playerbase + MOHF_sentryy);
		const float sentryylimit = MEM_ReadFloat(playerbase + MOHF_sentryylimit);
		if(sentryx >= -TAU && sentryx <= TAU && sentryy >= -sentryylimit && sentryy <= sentryylimit)
		{
			sentryx -= (float)xmouse / 10.f * ((float)sensitivity / 40.f) / (360.f / TAU) / (35.f / fov); // normal calculation method for X
			sentryy += (float)(invertpitch ? -ymouse : ymouse) / 10.f * ((float)sensitivity / 40.f) / (90.f / CROSSHAIRY) / (35.f / fov); // normal calculation method for Y
			if(sentryx < -TAU)
				sentryx += TAU;
			else if(sentryx >= TAU)
				sentryx -= TAU;
			sentryy = ClampFloat(sentryy, -sentryylimit, sentryylimit);
			MEM_WriteFloat(playerbase + MOHF_sentryx, sentryx);
			MEM_WriteFloat(playerbase + MOHF_sentryy, sentryy);
		}
	}
}
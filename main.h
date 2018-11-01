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
#define DOLPHINVERSION "Dolphin 5.0-8987"
#define BUILDINFO "(v0.21 - "__DATE__")"
#define LINE "_______________________________________________________________"
#define TICKRATE 1 // run mouse injector at 1000 Hz
// input for interface
#define K_4 GetAsyncKeyState(0x34) // key '4'
#define K_5 GetAsyncKeyState(0x35) // key '5'
#define K_6 GetAsyncKeyState(0x36) // key '6'
#define K_7 GetAsyncKeyState(0x37) // key '7'
#define K_CTRL0 (GetAsyncKeyState(0x11) && GetAsyncKeyState(0x30) || GetAsyncKeyState(0x30) && GetAsyncKeyState(0x11)) // key combo control + '0'
#define K_PLUS (GetAsyncKeyState(0x6B) || GetAsyncKeyState(0xBB)) // key '+'
#define K_MINUS (GetAsyncKeyState(0x6D) || GetAsyncKeyState(0xBD)) // key '-'
#if _MSC_VER && !__INTEL_COMPILER // here because some MSVC versions only support __inline :/
#define inline __inline
#endif

inline float ClampFloat(const float value, const float min, const float max)
{
	const float test = value < min ? min : value;
	return test > max ? max : test;
}

inline int32_t ClampInt(const int32_t value, const int32_t min, const int32_t max)
{
	const int32_t test = value < min ? min : value;
	return test > max ? max : test;
}

extern uint8_t sensitivity;
extern uint8_t crosshair;
extern uint8_t invertpitch;
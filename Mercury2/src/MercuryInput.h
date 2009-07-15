#ifndef MERCURYINPUT_H
#define MERCURYINPUT_H

#include <MessageHandler.h>

const MString INPUTEVENT_MOUSE = "MouseInputEvent";
const MString INPUTEVENT_KEYBOARD = "KeyboardInputEvent";


class MouseInput : public MessageData
{
	public:
		typedef union
		{
			uint8_t data;
			struct {
				unsigned int left: 1;
				unsigned int right: 1;
				unsigned int center: 1;
				unsigned int scrollup: 1;
				unsigned int scrolldown: 1;
			};
		} buttonMask;
		static void ProcessMouseInput(int dx, int dy, bool leftButton, bool rightButton, bool centerButton, bool scrollUpButton, bool scrollDownButton);
		
		MouseInput();
		int32_t dx, dy;
		buttonMask buttons;

	private:
		static buttonMask currentButtonMasks;
};

class KeyboardInput : public MessageData
{
	public:
		static void ProcessKeyInput(uint16_t key, bool isDown, bool repeat);
		static bool IsKeyDown(uint16_t key);
		static bool IsKeyRepeat(uint16_t key);
		
		KeyboardInput();
		
		int16_t key;
		bool isDown;
		bool isRepeat;
	private:
		static uint8_t m_keyStates[512];
};

#endif

/****************************************************************************
 *   Copyright (C) 2009 by Joshua Allen                                     *
 *                                                                          *
 *                                                                          *
 *   All rights reserved.                                                   *
 *                                                                          *
 *   Redistribution and use in source and binary forms, with or without     *
 *   modification, are permitted provided that the following conditions     *
 *   are met:                                                               *
 *     * Redistributions of source code must retain the above copyright     *
 *      notice, this list of conditions and the following disclaimer.       *
 *     * Redistributions in binary form must reproduce the above            *
 *      copyright notice, this list of conditions and the following         *
 *      disclaimer in the documentation and/or other materials provided     *
 *      with the distribution.                                              *
 *     * Neither the name of the Mercury Engine nor the names of its        *
 *      contributors may be used to endorse or promote products derived     *
 *      from this software without specific prior written permission.       *
 *                                                                          *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  *
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   *
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  *
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT       *
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  *
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  *
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT    *
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 ***************************************************************************/

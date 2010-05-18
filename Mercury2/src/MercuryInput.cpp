#include <MercuryInput.h>
#include <MercuryMessageManager.h>

MVRefFloat GlobalMouseX_Set( "Input.CursorX" );
MVRefFloat GlobalMouseY_Set( "Input.CursorY" );
MVRefInt GlobalMouseB_Set( "Input.CursorButtons" );

MouseInput::MouseInput()
	:MessageData(), dx(0), dy(0)
{
	buttons.data = 0;
}

void MouseInput::ProcessMouseInput(int dx, int dy, bool leftButton, bool rightButton, bool centerButton, bool scrollUpButton, bool scrollDownButton, bool bMotionEvent )
{	
	MouseInput* mi = new MouseInput();
	mi->dx = dx;
	mi->dy = dy;
	buttonMask buttons = {0};
	buttons.left = leftButton;
	buttons.right = rightButton;
	buttons.center = centerButton;
	buttons.scrollup = scrollUpButton;
	buttons.scrolldown = scrollDownButton;
	mi->buttons = buttons;
	mi->buttons.motion = bMotionEvent;
	currentButtonMasks = buttons;

	GlobalMouseX_Set.Set( (float)dx );
	GlobalMouseY_Set.Set( (float)dy );
	GlobalMouseB_Set.Set( currentButtonMasks.data );

	POST_MESSAGE( INPUTEVENT_MOUSE, mi, 0 );
}

KeyboardInput::KeyboardInput()
	:key(0), isDown(false)
{
	//init initial keymap
	if (m_keyStates[0] < 1)
	{
		m_keyStates[0] = 1;
		for (uint16_t i = 1; i < 512; ++i)
			m_keyStates[i] = 0;
	}	
}

void KeyboardInput::ProcessKeyInput(uint16_t key, bool isDown, bool repeat)
{
	KeyboardInput* ki = new KeyboardInput();
	ki->isDown = isDown;
	ki->key = key;
	ki->isRepeat = repeat;
	
	//if statements are for old people, bruit force it

	m_keyStates[key] &= ~(1 << 1); //disable
	m_keyStates[key] |= (isDown << 1); //enable if true
	
	m_keyStates[key] &= ~(1 << 2); //disable
	m_keyStates[key] |= (repeat << 2); //enable if true
	
	POST_MESSAGE( INPUTEVENT_KEYBOARD, ki, 0 );
}

bool KeyboardInput::IsKeyDown(uint16_t key)
{
	return (m_keyStates[key] & (1 << 1)) > 0;
}

bool KeyboardInput::IsKeyRepeat(uint16_t key)
{
	return (m_keyStates[key] & (1 << 2)) > 0;
}


uint8_t KeyboardInput::m_keyStates[512];
MouseInput::buttonMask MouseInput::currentButtonMasks;


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

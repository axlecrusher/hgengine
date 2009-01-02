#include <MercuryTimer.h>

#ifndef WIN32
#include <sys/time.h>
#else
#include <windows.h>
#endif

int64_t GetTimeInMicroSeconds()
{
	struct timeval tv;
	gettimeofday( &tv, 0 );
	
	return (int64_t(tv.tv_sec) * 1000000) + tv.tv_usec;
}

MercuryTimer::MercuryTimer()
	:m_lastTouch(0), m_thisTouch(0)
{
	if (m_initTime == 0)
		m_initTime = GetTimeInMicroSeconds();
	
	Touch();
	m_lastTouch = m_thisTouch;
}

uint64_t MercuryTimer::MicrosecondsSinceInit()
{
	return m_thisTouch;
}

float MercuryTimer::Touch()
{
	m_lastTouch = m_thisTouch;
	m_thisTouch = GetTimeInMicroSeconds() - m_initTime;
	return Age();
}

float MercuryTimer::Touch(const MercuryTimer& t)
{
	m_thisTouch = t.m_thisTouch;
	return Age();
}

const MercuryTimer& MercuryTimer::operator=(const MercuryTimer& t)
{
	m_lastTouch = t.m_lastTouch;
	m_thisTouch = t.m_thisTouch;
	return *this;
}

uint64_t MercuryTimer::m_initTime = 0;

/****************************************************************************
 *   Copyright (C) 2008 by Joshua Allen                                     *
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

#ifndef MERCURYMESSAGEMANAGER_H
#define MERCURYMESSAGEMANAGER_H

#include <Callback.h>
#include <MessageHandler.h>
#include <map>
#include <PriorityQueue.h>
#include <MercuryString.h>
#include <MercuryUtil.h>

/* This message system uses absolute integer time values to fire off events.
This ensures accuarate firing times while eliminating floating point error.
Because we use absolute times in the queue we do not need to "count down" the
firing times of all the messages. We only need to pop off the nearest events
from the beginning of the queue." */
class MercuryMessageManager
{
	public:
		void PostMessage(const MString& message, float delay);
		void PumpMessages(const uint64_t& currTime);
		void RegisterForMessage(const MString& message, MessageHandler* ptr);
		
		static MercuryMessageManager& GetInstance();
	private:
		void FireOffMessage(const MString& message);
		
		PriorityQueue<uint64_t, MString> m_messageQueue;
		uint64_t m_currTime; //microseconds
		
		std::map< MString, std::list< MessageHandler* > > m_messageRecipients;
};

static InstanceCounter<MercuryMessageManager> MMcounter("MessageManager");

#define MESSAGEMAN MercuryMessageManager
#define REGISTER_FOR_MESSAGE(x) MESSAGEMAN::GetInstance().RegisterForMessage(#x, this)
#define POST_MESSAGE(x,delay) MESSAGEMAN::GetInstance().PostMessage(#x, delay)

#endif

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
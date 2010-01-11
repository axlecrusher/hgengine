#ifndef MERCURYMESSAGEMANAGER_H
#define MERCURYMESSAGEMANAGER_H


#include <PriorityQueue.h>
#include <Callback.h>
#include <MessageHandler.h>
#include <MercuryHash.h>
#include <list>
#include <MercuryString.h>
#include <MercuryUtil.h>
#include <Mint.h>
#include <MAutoPtr.h>
#include <MSemaphore.h>
#include <MercuryValue.h>

class MessageHolder : public RefBase
{
	public:
		MessageHolder();
		MString message;
		MessageData* data;
		uint64_t when;
		static bool Compare( void * left, void * right );
};

typedef void (MessageHandler::*MessageDelegate)(const MessageData&);

/* This message system uses absolute integer time values to fire off events.
This ensures accuarate firing times while eliminating floating point error.
Because we use absolute times in the queue we do not need to "count down" the
firing times of all the messages. We only need to pop off the nearest events
from the beginning of the queue." */
class MercuryMessageManager
{
	public:
		MercuryMessageManager() : m_messageQueue( MessageHolder::Compare ) { }

		///Dispatch message whenever the message manager gets control again; delay after now.
		void PostMessage(const MString& message, MessageData* data, float delay);

		///Immediately dispatch message
		void BroadcastMessage( const MString & message, const MessageData * data );

		void PumpMessages(const uint64_t& currTime);
		
		void RegisterForMessage(const MString& message, MessageHandler* ptr,  MessageDelegate d = 0 );
		void UnRegisterForMessage(const MString& message, MessageHandler* ptr);

		MValue * GetValue( const MString & sVariableName );

		static MercuryMessageManager& GetInstance();
	private:
		void FireOffMessage( const MessageHolder & message );
		MessageHolder* GetNextMessageFromQueue();
		
		PriorityQueue m_messageQueue;
		uint64_t m_currTime; //microseconds

		struct MessagePair
		{
			MessagePair( MessageHandler * th, MessageDelegate td ) : h(th), d(td) { }
			MessageHandler * h;
			MessageDelegate d;
		};

		MHash< std::list< MessagePair > > m_messageRecipients;
		MHash< MValue > m_allValues;	//Careful - these cannot be deleted.

//		MercuryMutex m_lock;
		MSemaphore m_queueLock;
		MSemaphore m_recipientLock;
};

static InstanceCounter<MercuryMessageManager> MMcounter("MessageManager");

#define MESSAGEMAN MercuryMessageManager::GetInstance()
#define REGISTER_MESSAGE_WITH_DELEGATE(x, d) MESSAGEMAN.RegisterForMessage(x, this, (MessageDelegate)d)
#define REGISTER_FOR_MESSAGE(x) MESSAGEMAN.RegisterForMessage(x, this)
#define UNREGISTER_FOR_MESSAGE(x) MESSAGEMAN.UnRegisterForMessage(x, this)
#define POST_MESSAGE(x,data,delay) MESSAGEMAN.PostMessage(x, data, delay)

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

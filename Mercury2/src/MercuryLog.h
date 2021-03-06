#ifndef MERCURYLOG_H
#define MERCURYLOG_H

#include <list>
#include <MercuryString.h>
#include <MercuryThreads.h>
#include <MercuryUtil.h>
#include <InstanceCounter.h>

class MercuryLog
{
	public:
		MercuryLog();
		~MercuryLog();
		
		static MercuryLog& GetInstance();
		
		void Open(const MString& file);
		void Close();
		
		void Write(const MString& message);

	private:
		static void* ThreadLoop(void* d);
		void CopyQueue();
		void WriteQueue();
		
		FILE * m_file;
		std::list< MString > m_queue;
		std::list< MString > m_outQueue;
		
		MercuryMutex m_mutex;
		
		MercuryThread m_thread;
};

class MercuryProgramLog
{
	public:
		static MercuryProgramLog& GetInstance();
		inline MercuryLog& Log() { return m_log; }
	private:
		MercuryLog m_log;
};

static InstanceCounter<MercuryProgramLog> MLOGcounter("MercuryProgramLog");

#define LOG MercuryProgramLog::GetInstance().Log()

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

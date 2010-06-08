#include <MercuryLog.h>
#include <iostream>

using namespace std;

static bool gIsLogUp = 0;

void* MercuryLog::ThreadLoop(void* d)
{

	MercuryLog* log = static_cast<MercuryLog*>(d);
	log->Open( "log.txt" );
	
	while (true)
	{
		log->CopyQueue();
		log->WriteQueue();
		msleep(100); //10x/sec
	}
	log->Close();
	return 0;
}

MercuryLog::MercuryLog()
	:m_file(NULL)
{
	m_thread.Create(MercuryLog::ThreadLoop, this, true);
	gIsLogUp = 1;
}

MercuryLog::~MercuryLog()
{
	gIsLogUp = 0;
	m_thread.Halt();
	CopyQueue();
	WriteQueue();
}

void MercuryLog::Open(const MString& file)
{
	if( m_file )
		Close();

	m_file = fopen( file.c_str(), "w" );
}

void MercuryLog::Close()
{
	if( m_file )
	{
		fclose( m_file );
		m_file = 0;
	}
}

void MercuryLog::Write(const MString& message)
{
	if( gIsLogUp )
	{
		AutoMutexLock lock(m_mutex);
		m_queue.push_back( message );
	}
	else
	{
		printf( "POST-LOG: %s\n", message.c_str() );
	}
}

void MercuryLog::CopyQueue()
{
	AutoMutexLock lock(m_mutex);
	
	std::list< MString >::iterator i;
	
	for (i = m_queue.begin(); i != m_queue.end(); ++i)
		m_outQueue.push_back( *i );
	
	m_queue.clear();
}

void MercuryLog::WriteQueue()
{
	std::list< MString >::iterator i;
	
	for (i = m_outQueue.begin(); i != m_outQueue.end(); ++i)
	{
		const MString& m = *i;
		fprintf( m_file, "%s\n", m.c_str() );
		printf( "%s\n", m.c_str() );
	}
	
	fflush( m_file );
	
	m_outQueue.clear();
}

MercuryProgramLog& MercuryProgramLog::GetInstance()
{
	static MercuryProgramLog* pl = NULL;
	if (!pl)
	{
		pl = new MercuryProgramLog;
	}
	return *pl;
}

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

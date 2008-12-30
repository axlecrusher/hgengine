#ifndef _MERCURY_FILE_DRIVER_NET
#define _MERCURY_FILE_DRIVER_NET

#include <MercuryFile.h>


///[internal] Net file type (Using HTTP://)
class MercuryFileObjectNet: public MercuryFile
{
public:
	MercuryFileObjectNet() : m_pSocket(0), m_Buffer(0) { }
	virtual ~MercuryFileObjectNet();
	virtual bool Init( const MString & fName, FilePermission p );
	virtual bool Seek( unsigned long position );
	virtual void Close();
	virtual unsigned long Tell();
	virtual unsigned long Length();
	virtual bool Write( void * data, unsigned long length ) { return false; }
	virtual unsigned long Read( void * data, unsigned long length );
	virtual bool Check();
	virtual bool Eof();
private:
	unsigned long m_size;
	unsigned long m_location;

	void * m_pSocket;

	//Used for the file dump buffer as well as the file itself when done
	char * m_Buffer;
};

///[internal] Net file driver
class MercuryFileDriverNet : public MercuryFileDriver
{
public:
	virtual ~MercuryFileDriverNet() {};

	virtual void Init() { }
	virtual MercuryFile * GetFileHandle( const MString & sPath, FilePermission p  );
	virtual void ListDirectory( const MString & sPath, MVector< MString > & output, bool bDirsOnly );
};



#endif

/* 
 * Copyright (c) 2005-2008, Charles Lohr
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *	-	Redistributions of source code must retain the above
 *		copyright notice, this list of conditions and the following disclaimer.
 *	-	Redistributions in binary form must reproduce the above copyright
 *		notice, this list of conditions and the following disclaimer in
 *		the documentation and/or other materials provided with the distribution.
 *	-	Neither the name of the Mercury Engine nor the names of its
 *		contributors may be used to endorse or promote products derived from
 *		this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

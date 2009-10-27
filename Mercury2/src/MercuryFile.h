#ifndef _MERCURY_FILE_H
#define _MERCURY_FILE_H

#include <MAutoPtr.h>
#include <MercuryUtil.h>

//Forward decleration, as there's no real need to have this included unless we _need_ it.
template< typename t >
class MVector;

///File permissions (self-explainatory)
enum FilePermission
{
	MFP_READ_ONLY,
	MFP_WRITE_ONLY,
	MFP_READ_AND_WRITE,
	MFP_APPEND,
};

///Base file object for accessing files
class MercuryFile
{
public:
	MercuryFile();
	virtual ~MercuryFile();
	///[internal] Initialize the file
	virtual bool Init( const MString & sPath, FilePermission p );
	///Close the file
	virtual void Close() = 0;
	///Set the current seek position
	virtual bool Seek( unsigned long position ) = 0;
	///Return the current seek position
	virtual unsigned long Tell() = 0;
	///Return the length of the file (in bytes)
	virtual unsigned long Length() = 0;
	///Writes length bytes, returns true if it wrote successfully
	virtual bool Write( const void * data, unsigned long length ) = 0;
	///Returns the number of bytes read
	virtual unsigned long Read( void * data, unsigned long length ) = 0;
	///Reads one line from the file.
	virtual bool ReadLine( MString & data );
	///Returns non-zero value if there's an error
	virtual bool Check() = 0;
	///Return true if end of file
	virtual bool Eof() = 0;
	///Return the last time the file was modified.
	/** If the time is unknown, 0 is returned. This time has nothing to
	    do with actual changed time.  It is system dependent and should
	    only be used to see if a file has been changed, unless the user
	    wishes to take advantage of something they know about the
	    underlying system. */
	virtual unsigned long GetModTime() { return 0; }

	const MString& GetName() const { return m_sPath; }
protected:
	MString m_sPath;
	FilePermission m_p;
};

///[internal] Base file driver
class MercuryFileDriver : public RefBase
{
public:
	virtual ~MercuryFileDriver() {};

	virtual void Init() { }
	virtual MercuryFile * GetFileHandle( const MString & sPath, FilePermission p  ) = 0;
	virtual void ListDirectory( const MString & sPath, MVector< MString > & output, bool bDirsOnly ) = 0;
};

///The Mercury File Manager -- accessable via FILEMAN.
class MercuryFileManager
{
public:
	MercuryFileManager();
	~MercuryFileManager();
	
	static MercuryFileManager& GetInstance();
	
	///[internal] Initialize the file manager
	void Init();
	///[internal] Check to see if our static-time driver has been initalized

	///Open a file by name and permission type.  This returns a MercuryFile.
	MercuryFile * Open( const MString & sPath, FilePermission p = MFP_READ_ONLY );

	///List the contents of a directory, use DOS-style wildcards here.
	void ListDirectory( const MString & sPath, MVector< MString > & output, bool bDirsOnly=false );

	static bool FileMatch( const MString & file, const MString & regex );
private:
	///[internal] Has the DB been initialized
	MVector< MAutoPtr< MercuryFileDriver> > * m_Drivers;
};


static InstanceCounter<MercuryFileManager> MFMcounter("MercuryFileManager");

#define FILEMAN MercuryFileManager::GetInstance()

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

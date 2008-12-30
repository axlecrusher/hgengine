#ifndef _MERCURY_FILE_DRIVER_ZIPPED_H
#define _MERCURY_FILE_DRIVER_ZIPPED_H

#include <MercuryFile.h>
#include <map>

///[internal] File object that accesses zipped files in the root or Packed/ folder
class MercuryFileObjectZipped: public MercuryFile
{
public:
	virtual ~MercuryFileObjectZipped();
	virtual bool Init( const MString & fName, FilePermission p, const MString & base, unsigned offset, unsigned size, unsigned packedsize, unsigned decompID );
	virtual bool MemFileInit( const MString & sName, const char * pData, unsigned long iSize );
	virtual bool Seek( unsigned long position );
	virtual void Close();
	virtual unsigned long Tell();
	virtual unsigned long Length();
	virtual bool Write( void * data, unsigned long length );
	virtual unsigned long Read( void * data, unsigned long length );
	virtual bool Check();
	virtual bool Eof();
private:
	unsigned long m_size;
	unsigned long m_location;

	bool m_bIsDummy;

	//Used for the file dump buffer as well as the file itself when done
	char * m_Buffer;
};


///[internal] File driver that accesses files in a zip file
class MercuryFileDriverZipped : public MercuryFileDriver
{
public:	
	virtual void Init();
	virtual ~MercuryFileDriverZipped() {}
	virtual MercuryFile * GetFileHandle( const MString & sPath, FilePermission p  );
	virtual void ListDirectory( const MString & sPath, MVector< MString > & output, bool bDirsOnly );
private:
	///An entry in a packed file
	struct ZipFileEntry
	{
		bool m_bFolder;
		MString m_sPackageName;
		unsigned long m_iOffset;
		unsigned long m_iSize;

		unsigned long m_iPackedSize;
		unsigned long m_pkID;
		unsigned long m_pkID2;
	};
	std::map< MString, std::map< MString, ZipFileEntry > > m_mFileFolders;
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


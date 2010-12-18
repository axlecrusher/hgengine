#include <MercuryFileDriverMem.h>

const char MemoryFileName[] = "memory.zip";

/********************FILE DRIVER MEMORY********************/

#if defined ( USE_MEMFILE )
extern unsigned char MemFileData[];
#endif

void MercuryFileDriverMem::Init()
{
	MercuryFileDriver::Init();
#if defined ( USE_MEMFILE )
	m_iFileSize = MemFileData[0] + MemFileData[1] * 0x100 + MemFileData[2] * 0x10000 + MemFileData[3] * 0x1000000;
#endif
}

MercuryFile * MercuryFileDriverMem::GetFileHandle( const MString & sPath, enum FilePermission p )
{
#if defined ( USE_MEMFILE )
	if ( sPath == MemoryFileName && p == MFP_READ_ONLY )
	{
		MercuryFileObjectZipped * ret = new MercuryFileObjectZipped;
		ret->MemFileInit( MemoryFileName, (const char *)(&MemFileData[0]) + 4, m_iFileSize );
		return ret;
	}
	else
#endif
		return NULL;
}

void MercuryFileDriverMem::ListDirectory( const MString & sPath, MVector< MString > & output, bool bDirsOnly )
{
#if defined ( USE_MEMFILE )
	if ( !bDirsOnly && sPath == "*.*" || sPath == "*.zip" || sPath == MemoryFileName )
		output.push_back( MemoryFileName );
#endif
}

/*
 * Copyright (c) 2005-2006, Charles Lohr
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


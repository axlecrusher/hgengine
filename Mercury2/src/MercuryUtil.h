#ifndef MERCURYUTIL_H
#define MERCURYUTIL_H

#include <stdlib.h>
#include <MercuryString.h>
#include <assert.h>

/*#ifndef NULL
#define NULL 0
#endif*/

#define SAFE_DELETE( x ) { if (x) { delete x; } x = NULL; }
#define SAFE_DELETE_ARRAY( x ) { if (x) { delete[] x; } x = NULL; }
#define SAFE_FREE(p)       { if(p) { free(p); p=0; } }

#define TO_ENDIAN( x )

//returns an aligned pointer, mem is the actual (unaligned) pointer for freeing
void* mmemalign(size_t align, size_t size, void*& mem);
bool isAligned(size_t align, const void* mem);

#define ASSERT(x) assert(!x)

#if defined(__GNUC__)
#define M_ALIGN(n) __attribute__((aligned(n)))
#else
#define M_ALIGN(n)
#endif

MString ToUpper(const MString & s);

float StrToFloat(const MString & s);

//This counter is used with singletons to
//ensure proper destruction order of the
//singleton
#include <stdio.h>
template<typename T>
class InstanceCounter
{
	public:
		InstanceCounter(const MString & name)
	:m_name(name)
		{
			if (m_count == 0)
			{
				printf("Creating instance %s\n", m_name.c_str());
				m_instance = &T::GetInstance();
			}
			++m_count;
		}
		~InstanceCounter()
		{
			--m_count;
			if (m_count == 0)
			{
				printf("Destroying instance %s\n", m_name.c_str());
				SAFE_DELETE(m_instance);
			}
		}
	private:
		static unsigned long m_count;
		MString m_name;
		T* m_instance;
};

template<typename T>
		unsigned long InstanceCounter<T>::m_count = 0;

///Open up filename: sFileName and dump it into a new buffer; you must delete the return value when done.
///The return value is -1 if there was an issue, otherwise it is valid.
long FileToString( const MString & sFileName, char * & data );

#endif

/* Copyright (c) 2008, Joshua Allen
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

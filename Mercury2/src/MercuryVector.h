#ifndef _MERCURY_VECTOR_H
#define _MERCURY_VECTOR_H

#include <string.h>
#include <stdlib.h>

///Stripped down and fast vector class
template<typename T>
class MVector
{
	public:
		MVector()
			:m_reserved(0), m_size(0), m_data(0)
		{  }
		~MVector()
		{
			clear();
		}
		MVector(const MVector<T>& v)
			:m_reserved(v.m_reserved), m_size(v.m_size)
		{
			m_data = (T*)malloc(sizeof(T)*m_reserved);
			for (unsigned int i = 0; i < m_size; ++i)
			{
				new( &m_data[i] ) T (v.m_data[i]);
				#if defined( _DEBUG_MEMORY )
				MercuryAFree( &m_data[i] );
				#endif
			}
		}
		void push_back(const T& element);
		inline unsigned int size() const { return m_size; }
		inline bool empty() const { return m_size == 0; }
		void resize(unsigned int newSize);
		void reserve(unsigned int newSize);
		inline T& operator[](unsigned int x) { return m_data[x]; }
		inline const T& operator[](unsigned int x) const { return m_data[x]; }
		inline T& at( unsigned int x ) { return m_data[x]; }
		inline const T& at( unsigned int x ) const { return m_data[x]; }
		const MVector<T>& operator=(const MVector<T>& v);
		void clear();
		void remove( int iPlace );
		void insert( int iPlace, const T & data );
	private:
		unsigned int m_reserved; //available space
		unsigned int m_size; //number of valid (constructed) objects
		T * m_data;
};

#include "MercuryUtil.h"

template<typename T>
void MVector<T>::push_back(const T& element)
{
	if (m_size >= m_reserved)
	{
		if (m_reserved <= 0)
		{
			SAFE_FREE( m_data );
			m_data = (T*)malloc(sizeof(T));
			m_reserved = 1;
		}
		else
		{
			int tsize = nextPow2(m_reserved);
			int size = m_size;
			reserve(tsize);
			m_size = size;
		}
	}

	new( &m_data[m_size] ) T ( element );

	++m_size;
}

template<typename T>
void MVector<T>::resize(unsigned int newSize)
{
	if (newSize == 0)
	{
		clear();
		return;
	}
	//Fully reserve space and initalize constructors
	//Make larger and reserve space
	if (newSize == m_size)
	{
		return;
	}
	else if(newSize > m_size)
	{
		if (newSize > m_reserved)
		{
			m_reserved = newSize;
			m_data = (T*)realloc(m_data, sizeof(T)*m_reserved);
		}

		for (unsigned int i = m_size; i < newSize; ++i)
		{
			new (&m_data[i]) (T);
			#if defined( _DEBUG_MEMORY )
			MercuryAFree( &m_data[i] );
			#endif

		}
	}
	else
	{
		//get smaller
		m_reserved = newSize;

		//destroy data that will be lost
		for (unsigned int i = m_reserved; i < m_size; ++i)
			(&m_data[i])->~T();

		m_data = (T*)realloc(m_data, sizeof(T)*m_reserved);
	}
	m_size = newSize;
}

template<typename T>
void MVector<T>::reserve(unsigned int newSize)
{
	if (newSize == 0)
	{
		clear();
		return;
	}
	if (newSize == m_reserved)
	{
		return;
	}
	else if (newSize > m_reserved)
	{
		m_reserved = newSize;
		m_data = (T*)realloc(m_data, sizeof(T)*m_reserved);
	}
	else
	{
		m_reserved = newSize;

		//destroy data that will be lost
		for (unsigned int i = m_reserved; i < m_size; ++i)
			(&m_data[i])->~T();

		m_data = (T*)realloc(m_data, sizeof(T)*m_reserved);
		m_size = newSize;
	}
}

template<typename T>
void MVector<T>::remove( int iPlace )
{
	m_size--;
	(&m_data[iPlace])->~T();
	for (unsigned long i = iPlace; i < m_size; ++i)
		memcpy( &m_data[i], &m_data[i+1], sizeof(T) );
}

#include <stdio.h>
#include <new>
template<typename T>
void MVector<T>::insert( int iPlace, const T & data )
{
	int i;
	resize( m_size + 1 );

	(&m_data[m_size-1])->~T();
	for( i = m_size - 1; i > iPlace; i-- )
		memcpy( &m_data[i], &m_data[i-1], sizeof (T) );

	new ( &m_data[iPlace] ) T( data );
	#if defined( _DEBUG_MEMORY )
	MercuryAFree( &m_data[iPlace] );
	#endif

}

template<typename T>
void MVector<T>::clear()
{
	for (unsigned int i = 0; i < m_size; ++i)
		(&m_data[i])->~T();

	SAFE_FREE(m_data);
	m_size = m_reserved = 0;
}
template<typename T>
const MVector<T>& MVector<T>::operator=(const MVector<T>& v)
{
	clear();
	m_reserved = v.m_reserved;
	m_size = v.m_size;
	m_data = (T*)malloc(sizeof(T)*m_reserved);
	for (unsigned int i = 0; i < m_size; ++i)
	{
		new( &m_data[i]) T( v.m_data[i] );
#if defined( _DEBUG_MEMORY )
		MercuryAFree( &m_data[i] );
#endif
	}
	return *this;
}


#endif
/* 
 * Copyright (c) 2006-2009 Joshua Allen, Charles Lohr
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

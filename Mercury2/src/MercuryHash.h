#ifndef MERCURYHASH_H
#define MERCURYHASH_H

#include "global.h"
#include <MercuryVector.h>
#include <assert.h>

///Mercury Hash Table for Strings
template<typename T>
class MHash
{
public:
	MHash( ) : m_iSize( GetAPrime( m_iPrimest = 1 ) ) 
	{ 
		m_pTable = new MHashNode<T> * [m_iSize]; ///XXX what? and how does this affect delete?
		memset( m_pTable, 0, m_iSize * sizeof( MHashNode<T>* ) );
		m_iCount = 0;
	}

	MHash( const MHash & rhs ) 
	{
		m_iPrimest = rhs.m_iPrimest;
		m_iCount = rhs.m_iCount;
		m_pTable = new MHashNode<T> * [rhs.m_iSize];
		m_iSize = rhs.m_iSize;
		
		memset( m_pTable, 0, m_iSize * sizeof( MHashNode<T>* ) );

		for( unsigned i = 0; i < m_iSize; ++i )
		{
			MHashNode <T> * TMP = rhs.m_pTable[i];
			while (TMP)
			{
				m_pTable[i] = new MHashNode<T>( m_pTable[i], TMP->Index, TMP->Data );
				TMP = TMP->Next;
			}
		}
	}

	MHash & operator = ( const MHash & rhs ) 
	{
		m_iPrimest = rhs.m_iPrimest;
		m_iCount = rhs.m_iCount;
		m_pTable = new MHashNode<T> * [rhs.m_iSize];
		m_iSize = rhs.m_iSize;
		
		memset( m_pTable, 0, m_iSize * sizeof( MHashNode<T>* ) );

		for( unsigned i = 0; i < m_iSize; ++i )
		{
			MHashNode <T> * TMP = rhs.m_pTable[i];
			while (TMP)
			{
				m_pTable[i] = new MHashNode<T>( m_pTable[i], TMP->Index, TMP->Data );
				TMP = TMP->Next;
			}
		}
		return *this;
	}

	void resize( int iNewSize )
	{
		MHashNode <T> ** pOldTable = m_pTable;
		int iOldSize = m_iSize;

		m_pTable = new MHashNode<T> * [iNewSize];
		m_iSize = iNewSize;
		memset( m_pTable, 0, iNewSize * sizeof( MHashNode<T>* ) );

		for( int i = 0; i < iOldSize; ++i )
		{
			while (pOldTable[i])
			{
				MHashNode <T> * TMP = pOldTable[i];
				pOldTable[i] = TMP->Next;

				int iNewHash = TMP->Index.hash();
				MHashNode <T> * TMP2 =  m_pTable[iNewHash%m_iSize];
				m_pTable[iNewHash%m_iSize] = TMP;
				TMP->Next = TMP2;
			}
		}
		
		//I think a delete[] is OK here because its an allocated array of pointers. No destructors are called
		SAFE_DELETE_ARRAY(pOldTable);
	}

	~MHash()
	{
		for( unsigned i = 0; i < m_iSize; ++i )
			while (m_pTable[i])
			{
				MHashNode <T> * TMP = m_pTable[i];
				m_pTable[i] = TMP->Next;
				SAFE_DELETE( TMP );
			}
		SAFE_DELETE_ARRAY( m_pTable );
	}

	void clear()
	{
		for( unsigned i = 0; i < m_iSize; ++i )
		{
			while (m_pTable[i])
			{
				MHashNode <T> * TMP = m_pTable[i];
				m_pTable[i] = TMP->Next;
				SAFE_DELETE( TMP );
			}
			m_pTable[i] = 0;
		}
		m_iCount = 0;
	}

	///Return reference to data in the index's cell, if no data exists, it is generated
	T & operator[]( const MString & pIndex )
	{
		unsigned int iHash = pIndex.hash();
		MHashNode<T> * m_pTmp = m_pTable[iHash%m_iSize];

		while( m_pTmp )
			if( m_pTmp->Index == pIndex )
				return m_pTmp->Data;
			else
				m_pTmp = m_pTmp->Next;

		insert( pIndex, T() );

		m_pTmp = m_pTable[iHash%m_iSize];

		while( m_pTmp )
			if( m_pTmp->Index == pIndex )
				return m_pTmp->Data;
			else
				m_pTmp = m_pTmp->Next;

		//This should NEVER HAPPEN.
		assert( "FAIL! Hash insertion failed!" );
		return m_pNil;
	}

	///Insert pData into pIndex's cell
	void insert( const MString & pIndex, const T & pData )
	{
		unsigned int iHash = pIndex.hash();
		m_pTable[iHash%m_iSize] = new MHashNode<T>( m_pTable[iHash%m_iSize], pIndex, pData );
		
		if( ++m_iCount > m_iSize + 5 )
			resize( GetAPrime( ++m_iPrimest ) );
	}

	///Get pointer to pIndex's cell, if no data exists, NULL is returned.
	T * get( const MString & pIndex )
	{
		unsigned int iHash = pIndex.hash();
		MHashNode<T>	* m_pTmp = m_pTable[iHash%m_iSize];

		while( m_pTmp )
			if( m_pTmp->Index == pIndex )
				return &m_pTmp->Data;
			else
				m_pTmp = m_pTmp->Next;

		return 0;
	}

	bool remove( const MString & pIndex )
	{
		unsigned int iHash = pIndex.hash();
		MHashNode<T>	* m_pPrev = 0; 
		MHashNode<T>	* m_pTmp = m_pTable[iHash%m_iSize];

		while( m_pTmp )
			if( m_pTmp->Index == pIndex )
			{
				if( m_pPrev )
					m_pPrev->Next = m_pTmp->Next;
				else
					m_pTable[iHash%m_iSize] = m_pTable[iHash%m_iSize]->Next;
				SAFE_DELETE( m_pTmp );
				m_iCount--;
				return true;
			}
			else
			{
				m_pPrev = m_pTmp;
				m_pTmp = m_pTmp->Next;
			}

		return false;
	}

	void VectorIndicies( MVector < MString > & vOut )
	{
		unsigned int i;
		MHashNode<T> * m_pTmp;
		for( i = 0; i < m_iSize; i++ )
		{
			m_pTmp = m_pTable[i];
			while( m_pTmp )
			{
				vOut.push_back( m_pTmp->Index );
				m_pTmp = m_pTmp->Next;
			}
		}
	}

private:
	template<typename TC>
	struct MHashNode
	{
		MHashNode() : Next(0) { }
		MHashNode( MHashNode<TC> * pA, const MString & pIn, const TC & pD ) : Next( pA ), Index( pIn ), Data( pD ) { }

		MHashNode <TC> * Next;
		MString	Index;
		TC		Data;
	};

	MHashNode <T> ** m_pTable;
	T				m_pNil;
	unsigned int	m_iSize;
	unsigned int	m_iPrimest;
	unsigned int	m_iCount;
};

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

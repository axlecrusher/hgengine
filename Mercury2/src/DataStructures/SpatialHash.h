#ifndef SPATIALHASH_H
#define SPATIALHASH_H

//#include <stdint.h>
#include <list>
#include <math.h>

template<typename T>
class SpatialHash
{
	public:
		SpatialHash()
			:m_hashTable(NULL), m_spacing(1.0f)
		{
			m_cellCount = 0;
		}
		
		~SpatialHash()
		{
			DeleteHash();
		}
		
		void Allocate(uint32_t cellCount, float spacing)
		{
			DeleteHash();
			
			m_spacing = spacing;
			
			cellCount = cellCount==0?1:cellCount;
			m_cellCount = cellCount;
			
			uint32_t size = cellCount*cellCount*cellCount;
			m_hashTable = new std::list<T>[size];
		}
		
		void Insert(float x, float y, float z, const T& d)
		{
			float s = 1.0f/m_spacing;
			uint32_t ix = (uint32_t)(abs(x)*s);
			uint32_t iy = (uint32_t)(abs(y)*s);
			uint32_t iz = (uint32_t)(abs(z)*s);
			
			ix = ix % m_cellCount;
			iy = iy % m_cellCount;
			iz = iz % m_cellCount;
			
			//check for and skip duplicate
			std::list<T>& cell = m_hashTable[ Index( ix, iy, iz ) ];
			typename std::list<T>::iterator i = cell.begin();
			for (;i != cell.end(); ++i) if (*i == d) return;
			
			cell.push_back( d );
//			printf("added at %d %d %d\n", ix, iy, iz);
		}
		
		std::list<T> FindByXY(float x, float y)
		{
			float s = 1.0f/m_spacing;
			uint32_t ix = (uint32_t)(abs(x)*s);
			uint32_t iy = (uint32_t)(abs(y)*s);

			ix = ix % m_cellCount;
			iy = iy % m_cellCount;

			std::list<T> r;
			
			for (uint32_t iz = 0; iz < m_cellCount; ++iz)
				CopyIntoList(m_hashTable[Index(ix, iy, iz)], r);
			
			return r;
		}
	
		std::list<T> FindByXZ(float x, float z)
		{
			float s = 1.0f/m_spacing;
			uint32_t ix = (uint32_t)(abs(x)*s);
			uint32_t iz = (uint32_t)(abs(z)*s);

			ix = ix % m_cellCount;
			iz = iz % m_cellCount;

			std::list<T> r;
			
			for (uint32_t iy = 0; iy < m_cellCount; ++iy)
				CopyIntoList(m_hashTable[Index(ix, iy, iz)], r);
			
			return r;
		}
	private:
		inline uint32_t Index(uint32_t x, uint32_t y, uint32_t z)
		{
			return x + (m_cellCount * y) + (m_cellCount * m_cellCount * z);
		}
		
		void DeleteHash()
		{
			if (m_hashTable) delete[] m_hashTable;
			m_spacing = 1;
			m_cellCount = 0;
		}
		
		void CopyIntoList(std::list<T>& in, std::list<T>& r)
		{
			if ( in.empty() ) return;
			typename std::list<T>::iterator i = in.begin();
			for (;i != in.end(); ++i) r.push_back(*i);
		}
		
		std::list<T>* m_hashTable;
		uint32_t m_spacing;
		uint32_t m_cellCount;
};

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

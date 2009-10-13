#ifndef SPATIALHASH_H
#define SPATIALHASH_H

#include <stdint.h>
#include <list>
#include <math.h>

template<typename T>
class SpatialHash
{
	public:
		SpatialHash()
			:m_hashTable(NULL), m_spacing(1.0f)
		{
			m_xSize = m_ySize = m_zSize = 0;
		}
		
		~SpatialHash()
		{
			DeleteHash();
		}
		
		void Allocate(uint32_t xmax, uint32_t ymax, uint32_t zmax, uint32_t spacing)
		{
			DeleteHash();
			
			m_spacing = spacing;
			
			m_xSize = (abs(xmax)/m_spacing) + 1;
			m_ySize = (abs(ymax)/m_spacing) + 1;
			m_zSize = (abs(zmax)/m_spacing) + 1;
			
			uint32_t size = m_xSize*m_ySize*m_zSize;
			m_hashTable = new std::list<T>[size];
		}
		
		void Insert(float x, float y, float z, const T& d)
		{
			unsigned int ix = abs(x) / m_spacing;
			unsigned int iy = abs(y) / m_spacing;
			unsigned int iz = abs(z) / m_spacing;
			
			if (ix >= m_xSize || iy >= m_ySize || iz >= m_zSize)
			{
				printf("%d >= %d || %d >= %d || %d >= %d\n", ix, m_xSize, iy, m_ySize, iz, m_zSize);
				return;
			}
			
			//check for and skip duplicate
			std::list<T>& cell = m_hashTable[ Index( ix, iy, iz ) ];
			typename std::list<T>::iterator i = cell.begin();
			for (;i != cell.end(); ++i) if (*i == d) return;
			
			cell.push_back( d );
//			printf("added at %d %d %d\n", ix, iy, iz);
		}
		
		std::list<T> FindByXY(float x, float y)
		{
			unsigned int ix = abs(x) / m_spacing;
			unsigned int iy = abs(y) / m_spacing;

			std::list<T> r;
			
			if (ix < m_xSize || iy < m_ySize )
			{
				for (uint32_t iz = 0; iz < m_zSize; ++iz)
					CopyIntoList(m_hashTable[Index(ix, iy, iz)], r);
			}
			
			return r;
		}
	
		std::list<T> FindByXZ(float x, float z)
		{
			unsigned int ix = abs(x) / m_spacing;
			unsigned int iz = abs(z) / m_spacing;

			std::list<T> r;
			
			if (ix < m_xSize || iz < m_zSize )
			{
				for (uint32_t iy = 0; iy < m_ySize; ++iy)
					CopyIntoList(m_hashTable[Index(ix, iy, iz)], r);
			}
			
			return r;
		}
	private:
		inline uint32_t Index(uint32_t x, uint32_t y, uint32_t z)
		{
			return x + (m_xSize * y) + (m_xSize * m_ySize * z);
		}
		
		void DeleteHash()
		{
			if (m_hashTable) delete[] m_hashTable;
			m_spacing = 1;
			m_xSize = m_ySize = m_zSize = 0;
		}
		
		void CopyIntoList(std::list<T>& in, std::list<T>& r)
		{
			if ( in.empty() ) return;
			typename std::list<T>::iterator i = in.begin();
			for (;i != in.end(); ++i) r.push_back(*i);
		}
		
		std::list<T>* m_hashTable;
		uint32_t m_spacing;
		uint32_t m_xSize, m_ySize, m_zSize;
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

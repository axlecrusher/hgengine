#ifndef ARRAY3D_h
#define ARRAY3D_h

class Array3dOutOfBounds
{
	public:
		Array3dOutOfBounds(uint16_t xx, uint16_t yy, uint16_t zz, uint16_t mx, uint16_t my, uint16_t mz)
			:X(xx), Y(yy), Z(zz), MaxX(mx), MaxY(my), MaxZ(mz)
		{ }
		uint16_t X,Y,Z;
		uint16_t MaxX,MaxY,MaxZ;
};

template<typename T>
class Array3d
{
	public:
		Array3d(uint16_t x, uint16_t y, uint16_t z)
			:m_array(0), m_x(x), m_y(y), m_z(z)
		{
			m_array = new T[m_x*m_y*m_z];
		}
		
		~Array3d()
		{
			if (m_array!=0) delete[] m_array;
			m_array=0;
			m_x = m_y = m_z = 0;
		}
		
		inline T& Get(uint16_t x, uint16_t y, uint16_t z)
		{
			if (x>=m_x || y>=m_y || z>=m_z) throw Array3dOutOfBounds(x,y,z,m_x,m_y,m_z);
			return m_array[x + (m_x * y) + (m_x * m_y * z)];
		}

	private:
		T* m_array;
		uint16_t m_x,m_y,m_z;
};

#endif


/****************************************************************************
 *   Copyright (C) 2010 by Joshua Allen                                     *
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

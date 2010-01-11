#ifndef MERCURYVBO_H
#define MERCURYVBO_H

#include <MercuryAsset.h>
#include <AlignedBuffer.h>
#include <BoundingBox.h>

#include <MSemaphore.h>

class MercuryVBO : public MercuryAsset
{
	public:
		static const uint16_t STRIDE = 8;
		static const uint16_t VERTEX_OFFSET = 5;
		
		MercuryVBO( const MString & key, bool bInstanced, bool useVertexColor = false );
		virtual ~MercuryVBO();
		
		virtual void Render(const MercuryNode* node);

		void AllocateVertexSpace(unsigned int count);
		void AllocateIndexSpace(unsigned int count);

		static uint32_t BatchCount() { return m_vboBatches; }
		static uint32_t ResetBatchCount() { uint32_t t = m_vboBatches; m_vboBatches = 0; return t; }
		static uint32_t ResetBindCount() { uint32_t t = m_vboBinds; m_vboBinds = 0; return t; }

		const float* GetVertexHandle() const { return m_vertexData.Buffer(); }
		float* GetVertexHandle() { return m_vertexData.Buffer(); }

		const float* GetVertexColorHandle() const { return m_vertexColorData.Buffer(); }
		float* GetVertexColorHandle() { return m_vertexColorData.Buffer(); }

		const short unsigned int* GetIndexHandle() const { return m_indexData.Buffer(); }
		short unsigned int* GetIndexHandle() { return m_indexData.Buffer(); }
	
		inline uint16_t IndiceCount() const { return m_indexData.Length(); }
		

		inline void DirtyVertices() { m_bDirtyVertices = true; }
		inline void DirtyVerexColor() { m_bDirtyVertexColor = true; }
		inline void DirtyIndices() { m_bDirtyIndices = true; }
		
		static void SetLastRendered(void* p);
		inline static const void* GetLastRendered() { return m_lastVBOrendered; }
		inline static void IncrementBatches() { ++m_vboBatches; }

		GENRTTI( MercuryVBO );
	protected:
		virtual bool CheckForNewer() { return false; }
		virtual void Reload() {};

	private:
		virtual void InitVBO();
		static void* m_lastVBOrendered;
	
		unsigned int m_bufferIDs[3];
		bool m_initiated;
	
		bool m_bDirtyIndices;
		bool m_bDirtyVertices;
		bool m_bDirtyVertexColor;
		
		bool m_useVertexColor;

		void UpdateVertices();
		void UpdateVertexColor();
		void UpdateIndices();
	protected:
		AlignedBuffer<float> m_vertexData;
		AlignedBuffer<float> m_vertexColorData;
		AlignedBuffer<uint16_t> m_indexData;

		static uint32_t m_vboBatches;
		static uint32_t m_vboBinds;
};

#endif

/****************************************************************************
 *   Copyright (C) 2008 by Joshua Allen                                     *
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

#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include <MercuryAsset.h>

class RenderBuffer : public MercuryAsset
{
	public:
		enum RBType
		{
			NONE,
			COLOR,
			DEPTH,
			STENCIL,
			TEXTURE
		};
		
		RenderBuffer();
		virtual ~RenderBuffer();
		
		virtual void Init(MercuryNode* node);
		
		virtual void PreRender(const MercuryNode* node);
		virtual void Render(const MercuryNode* node);
		virtual void PostRender(const MercuryNode* node);
		virtual void LoadFromXML(const XMLNode& node);

		static RenderBuffer* Generate();

	private:
		virtual void InitRenderBuffer();
		void AllocateSpace();
		bool NeedResize();
		
		int GLType();
		int GLAttachPoint();
	
		uint32_t m_bufferID;
		uint32_t m_textureID;
		bool m_initiated;
		int m_width, m_height;
		RBType m_type;
		
		bool m_bound;
		
//		static void* m_lastRendered;

	protected:
//		AlignedBuffer<float> m_vertexData;
//		AlignedBuffer<uint16_t> m_indexData;
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

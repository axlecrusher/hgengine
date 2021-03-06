#ifndef MERCURYFBO_H
#define MERCURYFBO_H

#include <MercuryNode.h>
#include <Texture.h>
#include <RawImageData.h>

class MercuryFBO : public MercuryNode
{
	public:
		MercuryFBO();
		virtual ~MercuryFBO();
		
		virtual void Render(const MercuryMatrix& matrix);
		virtual void PostRender(const MercuryMatrix& matrix);
	
		virtual void LoadFromXML(const XMLNode& node);
		
		inline void SetWidth(uint16_t width) { m_width = width; }
		inline void SetHeight(uint16_t height) { m_height = height; }
		inline void SetUseDepth(bool toggle) { m_useDepth = toggle; }
		inline void SetNumTextures(uint8_t x) { m_numTextures = MIN<uint8_t>(x,4); }
		

		GENRTTI(MercuryFBO);

	private:
		void Setup();
		void Clean();
		void GenerateFBO();
		void Bind();
//		void InitFBOBeforeRender();
	
		uint32_t m_fboID, m_depthBufferID;
		bool m_initiated, m_useDepth, m_useScreenSize;
		uint16_t m_width, m_height;
		MAutoPtr< Texture > m_textures[4];
		uint8_t m_numTextures;
		ColorByteType m_cbt[4];
	protected:
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

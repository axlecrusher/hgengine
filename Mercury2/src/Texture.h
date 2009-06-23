#ifndef TEXTURE_H
#define TEXTURE_H

#include <MercuryAsset.h>
#include <RawImageData.h>

class Texture : public MercuryAsset
{
	public:
		Texture();
		virtual ~Texture();
		
		void Clean();
		
		virtual void Init(MercuryNode* node);
			
		virtual void Render(const MercuryNode* node);
		virtual void PostRender(const MercuryNode* node);
			
		virtual void LoadFromXML(const XMLNode& node);
		
		void LoadFromRaw();
		
		inline static uint8_t NumberActiveTextures() { return m_numActiveTextures; }
		inline static uint32_t ReadAndResetBindCount() { uint32_t t = m_textureBinds; m_textureBinds = 0; return t; }
		inline uint32_t TextureID() const { return m_textureID; }
		
		void MakeDynamic(uint16_t width, uint16_t height, const MString& name);

				
		static Texture* Generate();
		static MAutoPtr< Texture > LoadFromFile(const MString& path);
		static MAutoPtr< Texture > LoadDynamicTexture(const MString& name);
		static const std::list< Texture* >& GetActiveTextures() { return m_activeTextures; }
		
		void SetRawData(RawImageData* raw);
	private:
		void LoadImagePath(const MString& path);
		
		void BindTexture();
		void UnbindTexture();
		
		const RawImageData* m_raw;
		uint32_t m_textureID;
		uint32_t m_textureResource;
		
		static bool m_initTextureSuccess;
		static uint8_t m_numActiveTextures;
		static uint32_t m_textureBinds;
		static std::list< Texture* > m_activeTextures;

		
//		MString m_filename;
};

#endif

/***************************************************************************
 *   Copyright (C) 2008 by Joshua Allen   *
 *      *
 *                                                                         *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: *
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. *
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. *
 *     * Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR *
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR *
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, *
 *   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,   *
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    *
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF *
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  *
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    *
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.          *
 ***************************************************************************/

#ifndef MLIGHT_H
#define MLIGHT_H

#include <MercuryNode.h>
#include <FullscreenQuad.h>
#include <Shader.h>

class Light : public MercuryNode
{
	public:
		Light();
		virtual ~Light();
		
//		virtual void Init(MercuryNode* node);
			
		/** PreRender should be called before any real openGL render commands.
		It is used to handles things like frustum culling, and occlusion culling.
		Currently only occlusion culling test is run here.**/
//		virtual void PreRender(const MercuryNode* node);
			
		virtual void Render(const MercuryMatrix& matrix);
//		virtual void PostRender(const MercuryNode* node) {};
			
		///Loads an asset from an XMLAsset representing itself
		virtual void LoadFromXML(const XMLNode& node);
		
		static Light* Generate();
		
		void DifferedRender();
		inline void SetShader( Shader* shader) { m_shader = shader; }
	private:
		void StrTo3Float(const MString& s, float* a);
		void ComputeRadius();
		
		float m_atten[3];
		float m_color[3];
		float m_radius;
		MercuryMatrix m_worldPosition;
		MercuryMatrix m_worldPosition2;
		
		bool m_fullscreen;
//		const MercuryNode* m_parent;
		
		FullscreenQuad m_fullScreenQuad;
		
		MAutoPtr< Shader > m_shader;
		
		BoundingVolume* m_boundingVolume;
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

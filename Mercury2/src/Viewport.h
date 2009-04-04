#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <RenderableNode.h>
#include <MercuryMatrix.h>
#include <MercuryVertex.h>
#include <MercuryPlane.h>

enum PlanePos
{
	PTOP = 0,
	PBOTTOM,
	PLEFT,
	PRIGHT,
	PNEAR,
	PFAR
};

class Frustum
{

	public:
		void SetPerspective( float fov, float aspect, float znear, float zfar );
		const MercuryMatrix& GetMatrix() const { return m_frustum; }
		void ComputeFrustum(float left, float right, float bottom, float top, float zNear, float zFar);
		void LookAt(const MercuryVertex& eye, const MercuryVector& look, const MercuryVector& up);
		bool Clip(const BoundingBox& bb) const;
	private:
		MercuryPlane m_planes[6];
		MercuryMatrix m_frustum;
		
		float m_aspect, m_fov, m_zNear, m_zFar;
		float m_nh, m_nw, m_fh, m_fw;
		
		MercuryVector m_nc, m_fc;
};

extern const Frustum* FRUSTUM;

class Viewport : public RenderableNode
{
	public:
		virtual void Render(const MercuryMatrix& matrix);
		
		virtual void LoadFromXML(const XMLNode& node);
		
		GENRTTI(Viewport);
	private:
		Frustum m_frustum;
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

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

//#include <MercuryNode.h>
#include <MercuryVertex.h>
#include <MercuryMatrix.h>
#include <Frustum.h>
#include <Mint.h>
#include <MercuryPlane.h>

class BoundingVolume
{
	public:
		virtual ~BoundingVolume() {};

		virtual void LoadFromBinary(char* data) = 0;
		virtual void Transform( const MercuryMatrix& m ) = 0;
		virtual void Render() {};
		virtual BoundingVolume* SpawnClone() const = 0;
		
		virtual bool Clip( const MercuryPlane& p ) = 0;
		virtual bool Clip( const Frustum& f ) = 0;
};

class BoundingBox : public BoundingVolume
{
	public:
		BoundingBox() {};
		BoundingBox(const MercuryVertex& center, const MercuryVertex& extend);
		BoundingBox(const BoundingBox& bb);

		virtual ~BoundingBox() {};
		
		virtual void LoadFromBinary(char* data);
		
		inline const MercuryVertex& GetCenter() const { return m_center; }
		inline const MercuryVertex& GetExtend() const { return m_extend; }
		
		virtual void Transform( const MercuryMatrix& m );
		const MercuryVector& Normal(uint8_t i) const { return m_normals[i]; }

		virtual void Render();
		virtual BoundingVolume* SpawnClone() const;
		
		virtual bool Clip( const MercuryPlane& p );
		virtual bool Clip( const Frustum& f );

	private:
		void ComputeNormals();
		
		MercuryVertex m_center;
		MercuryVertex m_extend;
		
		MercuryVector m_normals[3];
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

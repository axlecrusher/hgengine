#ifndef TRANSFORMNODE_H
#define TRANSFORMNODE_H

#include <MercuryNode.h>
#include <MercuryVertex.h>
#include <MercuryMatrix.h>
#include <MQuaternion.h>

//I am not sure if I like the idea of rippling a taint flag down the tree
//if a transform hs changed.  There is probably a better way of doing this.
class TransformNode : public MercuryNode
{
	public:
		TransformNode();
		
		virtual void Update(float dTime);

		void SetScale( const MercuryVertex& scale );
		void SetPosition( const MercuryVertex& position );
		void SetRotation( const MQuaternion& rotation );
		
		inline const MercuryVertex& GetScale() const { return m_scale; }
		inline const MercuryVertex& GetPosition() const { return m_position; }
		inline const MQuaternion& GetRotation() const { return m_rotation; }
		
		inline const MercuryMatrix& GetGlobalMatrix() const { return m_globalMatrix; }
		const MercuryMatrix& GetParentMatrix() const;
		
		void SetTaint(bool taint);

		virtual void ComputeMatrix();
		
		virtual void LoadFromXML(const XMLNode& node);
		
		virtual void OnAdded();

		GENRTTI(TransformNode);
		
	private:
		void RippleTaintDown();
		
		//use of accessor required
		MercuryVertex m_scale;
		MercuryVertex m_position;
		MQuaternion m_rotation;
		
//		MercuryMatrix m_localMatrix;
	protected:

		MercuryMatrix m_globalMatrix;
		
		bool m_tainted;
};

class RotatorNode : public TransformNode
{
	public:
		virtual void Update(float dTime);
		GENRTTI(TransformNode);
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

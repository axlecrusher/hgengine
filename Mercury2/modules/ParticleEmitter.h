#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include <MercuryNode.h>
#include <Callback.h>

class ParticleEmitter;

class ParticleBase
{
	public:
		ParticleBase();
		~ParticleBase();
		
		virtual void Init();
		virtual void Update(float dTime);
		
//		virtual void RecursiveRender();
    
		void Activate();
		void Deactivate();
		void WriteToVBO();
		
		inline bool IsActive() const { return m_age < m_lifespan; }
		
//		GENRTTI( ParticleBase );
	private:
//		CLASS_HELPERS( MercuryNode );

		static const uint8_t STRIDE = 9;

		void WriteAgeToVBO();
		void WriteLifespanToVBO();
		void WriteRand1ToVBO();
		void WriteRand2ToVBO();
		void WriteFloatToVertices(float v, uint8_t vertexIndex, uint8_t offset);

		friend class ParticleEmitter;
//		ParticleBase *m_prev, *m_next;
		float m_age;
		float m_lifespan;
		float m_rand1, m_rand2;
		ParticleEmitter* m_emitter;
//		MercuryNode* m_particleGraph;
		float* m_particleVobData; //pointer to position in VBO
};

class ParticleEmitter : public MercuryNode
{
	public:
		ParticleEmitter();
		~ParticleEmitter();
		
		virtual void Init();
		virtual void Update(float dTime);

		void DeactivateParticle(ParticleBase* p);
		virtual void LoadFromXML(const XMLNode& node);
		

		virtual void PreRender(const MercuryMatrix& matrix);
		virtual void Render(const MercuryMatrix& matrix);

		void SetMaxParticleCount(uint16_t count);
		inline void SetDirtyVBO() { m_dirtyVBO=true; }

		GENRTTI( ParticleEmitter );
	private:
//		void DestroyParticles();
		void ActivateParticle();

//		void FillUnusedParticleList(ParticleBase* p, uint32_t i);
//		void InitNewParticles(ParticleBase* p, uint32_t i, uint16_t vobStep, float* vob);

		CLASS_HELPERS( MercuryNode );
		
		uint32_t m_maxParticles;
		float m_age;
		float m_emitDelay; //seconds
		float m_lifespan; //emitter lifespan
		uint32_t m_particlesEmitted;
		float m_particleMinLife, m_particleMaxLife;
		
		ParticleBase *m_particles;
		
		Callback1R<uint32_t,ParticleBase*>* GenerateParticlesClbk;
		AlignedBuffer<float> m_vertexData;
		
		unsigned int m_bufferID;
		bool m_dirtyVBO;

		std::list< ParticleBase* > m_active, m_inactive;
//		MercuryNode* m_masterParticle;
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

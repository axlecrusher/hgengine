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
		
		virtual void Update(float dTime);
		
		void Activate();
		void Deactivate();
		void WriteToVBO();

		void Init(float time);
		
		inline bool IsActive() const { return m_currentTime < m_lifespan+m_startTime; }
	private:
		void WriteAgeToVBO();
		void WriteLifespanToVBO();
		void WriteRand1ToVBO();
		void WriteRand2ToVBO();
		void WriteFloatToVertices(float v, uint8_t vertexIndex, uint8_t offset);

		friend class ParticleEmitter;
		float m_startTime,m_currentTime;
		float m_lifespan;
		float m_rand1, m_rand2;
		ParticleEmitter* m_emitter;
		float* m_particleDynamicData; //pointer to position in VBO
		uint16_t* m_particleIndexData; //pointer to position in index list
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

		void SetDynamicUpdateRange(void* begin, void* end);
		void SetIndexUpdateRange(void* begin, void* end);

		static uint32_t ResetDrawnCount();

		GENRTTI( ParticleEmitter );
	private:
		void ActivateParticle(float genTime);
		void InitGeometry(float* g);
		void UpdateVBO();

		CLASS_HELPERS( MercuryNode );
		
		uint32_t m_maxParticles;
		float m_age;
		float m_emitDelay; //seconds
		float m_lifespan; //emitter lifespan
		uint32_t m_particlesEmitted;
		float m_particleMinLife, m_particleMaxLife;
		
		ParticleBase *m_particles;
		
		Callback1R<uint32_t,ParticleBase*>* GenerateParticlesClbk;
		AlignedBuffer<float> m_geometryData; //xyzuv, stride 5
		AlignedBuffer<float> m_vertexDynamicData; //1 age, 1 lifespan, 2 random, stide 4
		AlignedBuffer<uint16_t> m_indexData;
		
		unsigned int m_bufferID[3];
		unsigned int m_fLastD;
		unsigned int m_fLastI;
		void *m_dBegin, *m_dEnd; //pointers to area of the m_vertexDynamicData that changed
		void *m_iBegin, *m_iEnd; //pointers to area of the m_indexData that changed

		ShaderAttribute* m_shdrAttrEmitterTime;


		std::list< ParticleBase* > m_active, m_inactive;
		static uint32_t m_particlesDrawn;
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

#include <ParticleEmitter.h>
#include <Shader.h>
#include <GLHeaders.h>

//REGISTER_NODE_TYPE(ParticleBase);
REGISTER_NODE_TYPE(ParticleEmitter);


ParticleBase::ParticleBase()
	:m_nextParticle(NULL), m_age(0), m_lifespan(0)
{
}

ParticleBase::~ParticleBase()
{
//	m_children.clear();
}

void ParticleBase::Init()
{
	base::Init();
	m_age = 0;
//	m_lifespan = (rand()%5000)/1000.f;
//	LOG.Write("init particle");
}

void ParticleBase::Update(float dTime)
{
	base::Update(dTime);

	m_age += dTime;
	if (m_age >= m_lifespan) m_emitter->DeactivateParticle(this);
}

void ParticleBase::RecursiveRender()
{
	ShaderAttribute sa;
	sa.type = ShaderAttribute::TYPE_FLOATV4;
	sa.value.fFloatV4[0] = m_age;
	sa.value.fFloatV4[1] = m_lifespan;
	sa.value.fFloatV4[2] = m_seed1;
	sa.value.fFloatV4[3] = m_seed2;
	Shader::SetAttribute("HG_ParticleTime", sa);
	
	GLCALL( glPushAttrib(GL_ENABLE_BIT|GL_DEPTH_BUFFER_BIT) );
	GLCALL( glDepthMask( false ) );
	GLCALL( glDisable(GL_CULL_FACE) );
	
	base::RecursiveRender();
	
	GLCALL( glPopAttrib() );
}
/*
void ParticleBase::Activate()
{
	LOG.Write("Activate");
	m_emitter->AddChild(this);
}

void ParticleBase::Deactivate()
{
	LOG.Write("Deactivate");
	m_emitter->RemoveChild(this);
	m_emitter->DeactivateParticle(this);
}
*/
ParticleEmitter::ParticleEmitter()
	:m_maxParticles(50), m_age(0), m_emitDelay(0.01), m_lifespan(0),
	m_particlesEmitted(0), m_particleMinLife(0.01), m_particleMaxLife(5),
	m_inactiveParticles(NULL), m_particles(NULL), GenerateParticlesClbk(NULL)
{
	Init();
}


ParticleEmitter::~ParticleEmitter()
{
	DestroyParticles();

	SAFE_DELETE(GenerateParticlesClbk);
//	SAFE_DELETE(m_masterParticle);
}


void ParticleEmitter::Init()
{
	MercuryNode::Init();
	DestroyParticles();
	
	if (GenerateParticlesClbk) m_particles = (*GenerateParticlesClbk)(m_maxParticles);
	m_particles = new ParticleBase[m_maxParticles];
	m_inactiveParticles = m_particles;
	FillUnusedParticleList(m_particles, 0);
}


void ParticleEmitter::DestroyParticles()
{
	for (uint32_t i = 0; (i < m_maxParticles) && m_particles; ++i)
		RemoveChild(m_particles+i);
	SAFE_DELETE_ARRAY(m_particles); //do we need to destroy each element????
}

void ParticleEmitter::Update(float dTime)
{
	m_age += dTime;

	/* create particles until we meet the total number of
	   particles possible in terms of the age of the emitter */
	while (((m_age-(m_particlesEmitted*m_emitDelay)) > m_emitDelay) && (m_emitDelay>0))
	{
//		LOG.Write("Emit");
		++m_particlesEmitted;  //always increment even if the maximum number of particles exist
		ActivateParticle();
	}
}

void ParticleEmitter::ActivateParticle()
{
	if (m_inactiveParticles)
	{
		ParticleBase* p = m_inactiveParticles;
		m_inactiveParticles = p->m_nextParticle;
		p->m_nextParticle = NULL;
		p->Init();
		
		
		p->m_lifespan = m_particleMinLife;
		p->m_lifespan = (rand()%(int(m_particleMaxLife*1000) - int(m_particleMinLife*1000)))/1000.0f;
		p->m_seed1 = rand()%100000;
		p->m_seed2 = rand()%100000;
//		+((rand()%((m_particleMaxLife*1000)-(m_particleMinLife*1000)))/1000.0f);
		
		AddChild(p);
	}
}

void ParticleEmitter::DeactivateParticle(ParticleBase* p)
{
//	LOG.Write("Deactivate");
	RemoveChild(p);
	if (!m_inactiveParticles)
	{
		m_inactiveParticles = p;
	}
	else
	{
		ParticleBase* ip = m_inactiveParticles;
		while (ip->m_nextParticle) ip = ip->m_nextParticle;
		ip->m_nextParticle = p;
	}
}

void ParticleEmitter::FillUnusedParticleList(ParticleBase* p, uint32_t i)
{
	if (p)
	{
		p->m_emitter = this;
		++i;
		if (i<m_maxParticles) p->m_nextParticle = m_particles+i;
		FillUnusedParticleList(p->m_nextParticle, i);
	}
}

void ParticleEmitter::LoadFromXML(const XMLNode& node)
{
	base::LoadFromXML(node);
	
	XMLNode particleXML;
	for (XMLNode n = node.Child(); n.IsValid(); n=n.NextNode())
		if (n.Name() == "particle")
			particleXML = n;
	
	for (uint32_t i = 0; (i < m_maxParticles) && m_particles; ++i)
		m_particles[i].LoadFromXML(particleXML);
}


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

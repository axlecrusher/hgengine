#include "Cu2.h"
#include <MercuryInput.h>
#include <MercuryMessageManager.h>
#include <MercuryWindow.h>
#include <MercuryLog.h>
#include <TextNode.h>

#include <GLHeaders.h>

///////////////////////////////////////COPPER 2 ELEMENT///////////////////////////////////////

Cu2Element::Cu2Element() : TransformNode(),
	m_pFocusNode(0), m_bCanTabStop( true ), m_bWasMouseInThisFrame(0), 
	m_fX(0), m_fY(0), m_fW(100), m_fH(100),
	m_fOrigX(0), m_fOrigY(0), m_fOrigW(100), m_fOrigH(100)
{
}

void Cu2Element::LoadFromXML(const XMLNode& node)
{
	LOAD_FROM_XML( "x", m_fOrigX, StrToFloat );
	LOAD_FROM_XML( "y", m_fOrigY, StrToFloat );
	LOAD_FROM_XML( "width", m_fOrigW, StrToFloat );
	LOAD_FROM_XML( "height", m_fOrigH, StrToFloat );

	TransformNode::LoadFromXML( node );

	SetXY( m_fOrigX, m_fOrigY );
	SetSize( m_fOrigW, m_fOrigH );
}

void Cu2Element::SaveToXMLTag( MString & sXMLStream )
{
	if( GetX() != 0 ) sXMLStream += ssprintf( "x=\"%f\" ", GetX() );
	if( GetY() != 0 ) sXMLStream += ssprintf( "y=\"%f\" ", GetY() );
	if( GetW() != 0 ) sXMLStream += ssprintf( "width=\"%f\" ", GetW() );
	if( GetH() != 0 ) sXMLStream += ssprintf( "height=\"%f\" ", GetH() );

	TransformNode::SaveToXMLTag( sXMLStream );
}

void Cu2Element::ResetAttributes()
{
	SetXY( m_fOrigX, m_fOrigY );
	SetSize( m_fOrigW, m_fOrigH );
}

int Cu2Element::MouseMotion( int x, int y, unsigned char iCurrentButtonMask, unsigned char iLastButtonMask )
{
	if(  IsHidden() )
		return false;

	bool bIsInside = ( x >= 0 && x < m_fW && y >= 0 && y < m_fH );

	if( m_bWasMouseInThisFrame && !bIsInside )
		MouseAction( x, y, MOVE_OUT, 0 );
	else if( !m_bWasMouseInThisFrame && bIsInside )
		MouseAction( x, y, MOVE_IN, 0 );

	m_bWasMouseInThisFrame = bIsInside;

	for( unsigned button = 0; button < 3; button++ )
	{
		unsigned char Mask = 1<<button;
		bool bWasDown = GetBit(iLastButtonMask,Mask);
		bool bIsDown = GetBit(iCurrentButtonMask,Mask);
		if( bWasDown && !bIsDown )
		{
			//XXX: When we release outside - we want to propogate that information, and that can be tricky..
			//So, instead, we choose to propogate that elsewhere...
			if( bIsInside )
				MouseAction( x, y, RELEASE_IN, button );
			else
				PropogateReleaseOut( x, y, button );
		}
		else if( !bWasDown && bIsDown && bIsInside )
		{
			MouseAction( x, y, PRESS_IN, button );
		}
	}

	if( bIsInside )
		for( MercuryNode* send = LastChild(); send; send = PrevChild( send ) )
		{
			Cu2Element * sa = dynamic_cast<Cu2Element*>(send);
			if( sa )
			{
				//Break on the first one that is a hit, that way we don't pass mouse info to buttons below.
				if( sa->MouseMotion( x - int(sa->m_fX), y - int(sa->m_fY), iCurrentButtonMask, iLastButtonMask ) )
					return 2;
			}
		}

	return bIsInside?1:0;
}

void Cu2Element::PropogateReleaseOut( int x, int y, int iWhichButton )
{
	MouseAction( x, y, RELEASE_OUT, iWhichButton );

	for( MercuryNode* send = FirstChild(); send; send = NextChild( send ) )
	{
		Cu2Element * sa = dynamic_cast<Cu2Element*>(send);
		if( sa )
			sa->PropogateReleaseOut( x - int(sa->m_fX), y - int(sa->m_fY), iWhichButton );
	}
}

void Cu2Element::MouseAction( int x, int y, Cu2Action c, int iWhichButton )
{
	if( c == PRESS_IN )
		RaiseFocus();
}

void Cu2Element::AddChild(MercuryNode* n)
{
	if( !m_pFocusNode )
	{
		Cu2Element * sa = dynamic_cast< Cu2Element * >(n);
		if( sa && sa->IsEnableTabStop() && !sa->IsHidden() )
			m_pFocusNode = sa;
	}
	MercuryNode::AddChild( n );
}

void Cu2Element::RemoveChild(MercuryNode* n)
{
	if( m_pFocusNode == n )
	{
		m_pFocusNode->SetEnableTabStop( false );
		UpdateTab();
	}

	MercuryNode::RemoveChild( n );
}

Cu2Element * Cu2Element::NextTab()
{
	if( m_pFocusNode )
	{
		for( MercuryNode* send = NextChild( m_pFocusNode ); send; send = NextChild( send ) )
		{
			Cu2Element * sa = dynamic_cast<Cu2Element*>(send);
			if( sa && sa->IsEnableTabStop() && !sa->IsHidden() )
				return sa;
		}
	}

	for( MercuryNode* send = FirstChild(); send; send = NextChild( send ) )
	{
		Cu2Element * sa = dynamic_cast<Cu2Element*>(send);
		if( sa && sa->IsEnableTabStop() && !sa->IsHidden() )
			return sa;
	}
	return 0;
}

bool Cu2Element::HasFocus()
{
	Cu2Element * sa = dynamic_cast<Cu2Element*>(Parent());

	if( sa && sa->m_pFocusNode == this )
		return true;
	else
		return false;
}

void Cu2Element::RaiseFocus()
{
	Cu2Element * ca = dynamic_cast<Cu2Element*>(Parent());

	if( ca )
	{
		bool bCouldTabStop = IsEnableTabStop( );

		//Remove this node from wherever it is
		ca->RemoveChild( this );

		//This is reset by RemoveChild - so we have to update.
		SetEnableTabStop( bCouldTabStop );

		//Make it so it draws last.
		ca->AddChild( this );

		ca->m_pFocusNode = this;

		ca->RaiseFocus();
	}
}

void Cu2Element::SetHidden( bool bHide )
{
	MercuryNode::SetHidden( bHide );
	Cu2Element * parent = dynamic_cast<Cu2Element *>(Parent());
	if( parent )
		parent->UpdateTab();
}

void Cu2Element::SetEnableTabStop( bool bStop )
{
	m_bCanTabStop = bStop;
	Cu2Element * parent = dynamic_cast<Cu2Element *>(Parent());
	if( parent )
		parent->UpdateTab();
}

void Cu2Element::UpdateTab()
{
	for( MercuryNode* send = m_pFocusNode; send; send = NextChild( send ) )
	{
		Cu2Element * sa = dynamic_cast<Cu2Element*>(send);
		if( sa && sa->IsEnableTabStop() && !sa->IsHidden() )
		{
			m_pFocusNode = sa;
			return;
		}
	}
	for( MercuryNode* send = FirstChild(); send; send = NextChild( send ) )
	{
		Cu2Element * sa = dynamic_cast<Cu2Element*>(send);

		if( sa && sa->IsEnableTabStop() && !sa->IsHidden() )
		{
			m_pFocusNode = sa;
			return;
		}
	}

	m_pFocusNode = 0;
}

void Cu2Element::GetKeypress( int key, bool bDown, bool bRepeat )
{
	if( m_pFocusNode )
		m_pFocusNode->GetKeypress( key, bDown, bRepeat );
}

REGISTER_NODE_TYPE(Cu2Element);




///////////////////////////////////////COPPER 2 ROOT///////////////////////////////////////

MVRefBool Cu2Root::CursorGrabbed( "Input.CursorGrabbed" );

Cu2Root::Cu2Root()
{
	g_pCurrentInstance = this;
	REGISTER_MESSAGE_WITH_DELEGATE( INPUTEVENT_MOUSE, &Cu2Root::HandleMouseInput );
	REGISTER_MESSAGE_WITH_DELEGATE( INPUTEVENT_KEYBOARD, &Cu2Root::HandleKeyboardInput );
}

Cu2Root::~Cu2Root()
{
	g_pCurrentInstance = 0;
}

void Cu2Root::SetHidden( bool bHide )
{
	CursorGrabbed.Set( bHide );
	MercuryNode::SetHidden( bHide );
}

void Cu2Root::LoadFromXML(const XMLNode& node)
{
	Cu2Element::LoadFromXML( node );
	CursorGrabbed.Set( IsHidden() );
}


void Cu2Root::HandleMouseInput(const MessageData& data)
{
	const MouseInput& m( dynamic_cast<const MouseInput&>( data ) );

	//Don't pass on things if we're hidden.
	if( IsHidden() )
	{
		//Don't do anything	
	}
	else
	{
		if( MouseMotion( m.dx, MercuryWindow::GetCurrentWindow()->Height()-m.dy, m.buttons.data, m_iLastButtonMask ) != 2 )
		{
			MESSAGEMAN.BroadcastMessage( "UIMissMouse", &data );
		}
	}

	m_iLastButtonMask = m.buttons.data;
}

void Cu2Root::HandleKeyboardInput(const MessageData& data)
{
	const KeyboardInput& m( dynamic_cast<const KeyboardInput&>( data ) );

	if( m.key == 27 && m.isDown && !m.isRepeat )
	{
		SetHidden( !IsHidden() );
	}

	//Don't pass on things if we're hidden.
	if( IsHidden() ) return;

	GetKeypress( m.key, m.isDown, m.isRepeat );
}

Cu2Root * Cu2Root::g_pCurrentInstance;

REGISTER_NODE_TYPE(Cu2Root);

///////////////////////////////////////COPPER 2 BUTTON///////////////////////////////////////

Cu2Button::Cu2Button() : Cu2Element()
{
	m_pText = (TextNode*)NODEFACTORY.Generate( "TextNode" );
	AddChild( m_pText );
	m_bAutoSize = true;
	m_bDown = false;
}	

void Cu2Button::LoadFromXML(const XMLNode& node)
{
	LOAD_FROM_XML( "clickMessage", m_sMessageToSend, );
	LOAD_FROM_XML( "text", m_sText,  );
	LOAD_FROM_XML( "autoSize",  m_bAutoSize, StrToBool );
	LOAD_FROM_XML( "clickPayload", m_sValueToSend, );
	LOAD_FROM_XML( "associatedValue", m_sAssociatedValue, );
	LOAD_FROM_XML( "associatedValueSet", m_sAssociatedValueSet, );

	LOAD_FROM_XML( "associatedValueX", m_sAssociatedValueX, );
	LOAD_FROM_XML( "associatedValueY", m_sAssociatedValueY, );

	LOAD_FROM_XML( "xRangeMin", m_sxRangeMin, );
	LOAD_FROM_XML( "xRangeMax", m_sxRangeMax, );
	LOAD_FROM_XML( "yRangeMin", m_syRangeMin, );
	LOAD_FROM_XML( "yRangeMax", m_syRangeMax, );

	if( m_pText )
	{
		m_pText->SetAlignment( TextNode::LEFT );
		m_pText->LoadFont( node.Attribute("font") );
		m_pText->SetSize( StrToFloat( node.Attribute("size") ) );
		SetText( m_sText );
		m_pText->SetShiftAbsolute( true );
		m_pText->SetShiftX( 5 );
		m_pText->SetShiftY( 5 );
	}

	Cu2Element::LoadFromXML( node );

	Refresh();
}

void Cu2Button::SaveToXMLTag( MString & sXMLStream )
{
	if( m_sMessageToSend.length() ) sXMLStream += ssprintf( "clickMessage=\"%s\" ", m_sMessageToSend.c_str() );
	if( m_sValueToSend.length() ) sXMLStream += ssprintf( "clickPayload=\"%s\" ", m_sValueToSend.c_str() );
	if( m_bAutoSize ) sXMLStream += ssprintf( "autoSize=\"%d\" ", m_bAutoSize );
	if( m_sAssociatedValue.length() ) sXMLStream += ssprintf( "associatedValue=\"%s\" ", m_sAssociatedValue.c_str() );
	if( m_sAssociatedValueSet.length() ) sXMLStream += ssprintf( "associatedValueSet=\"%s\" ", m_sAssociatedValueSet.c_str() );
	if( m_sAssociatedValueX.length() ) sXMLStream += ssprintf( "associatedValueX=\"%s\" ", m_sAssociatedValueX.c_str() );
	if( m_sAssociatedValueY.length() ) sXMLStream += ssprintf( "associatedValueY=\"%s\" ", m_sAssociatedValueY.c_str() );

	if( m_sxRangeMin.length() ) sXMLStream += ssprintf( "xRangeMin=\"%s\" ",m_sxRangeMin.c_str() );
	if( m_sxRangeMax.length() ) sXMLStream += ssprintf( "xRangeMax=\"%s\" ",m_sxRangeMax.c_str() );
	if( m_syRangeMin.length() ) sXMLStream += ssprintf( "yRangeMin=\"%s\" ",m_syRangeMin.c_str() );
	if( m_syRangeMax.length() ) sXMLStream += ssprintf( "yRangeMax=\"%s\" ",m_syRangeMax.c_str() );

	if( !m_pText )
		m_pText->SaveToXMLTag( sXMLStream );

	Cu2Element::SaveToXMLTag( sXMLStream );
}

void Cu2Button::MouseAction( int x, int y, Cu2Action c, int iWhichButton )
{
	if( c == PRESS_IN )
		m_bDown = true;

	if( c == RELEASE_IN )
	{
		if( m_bDown )
			Click( x, y );
		m_bDown = false;
	}

	if( c == RELEASE_OUT )
	{
		m_bDown = false;
	}

	Cu2Element::MouseAction( x, y, c, iWhichButton );
}

int Cu2Button::MouseMotion( int x, int y, unsigned char iCurrentButtonMask, unsigned char iLastButtonMask )
{
	if( m_sAssociatedValueX.length() && m_bDown && x >= 0 && y >= 0 && x < GetW() && y < GetH() )
	{
		float fxRangeMin = (float)atof( m_sxRangeMin.c_str() );
		float fxRangeMax = m_sxRangeMax.length()?(float)atof( m_sxRangeMax.c_str() ):GetW();

		float fX = ( float(x) / float(GetW()-1) ) * (fxRangeMax - fxRangeMin) + fxRangeMin;
		MESSAGEMAN.GetValue( m_sAssociatedValueX )->SetFloat( fX );
	}

	if( m_sAssociatedValueY.length() && m_bDown && x >= 0 && y >= 0 && x < GetW() && y < GetH() )
	{
		float fyRangeMin = (float)atof( m_syRangeMin.c_str() );
		float fyRangeMax = m_syRangeMax.length()?(float)atof( m_syRangeMax.c_str() ):GetW();

		float fY = ( float(y) / float(GetH()-1) ) * (fyRangeMax - fyRangeMin) + fyRangeMin;
		MESSAGEMAN.GetValue( m_sAssociatedValueY )->SetFloat( fY );
	}

	return Cu2Element::MouseMotion( x, y, iCurrentButtonMask, iLastButtonMask );
}


void Cu2Button::Refresh()
{
	if( !m_pText )
	{
		LOG.Write( "Warning: Cu2Button \"" + GetName() + "\" does not have valid Text box associated." );
		return;
	}

	m_pText->SetText( m_sText );
	m_pText->RenderText();

	if( m_bAutoSize )
	{
		SetSize( m_pText->GetRMaxX() + 8, m_pText->GetRMaxY() + 8 );
	}
}

void Cu2Button::Click( int x, int y )
{
	if( m_sAssociatedValue.length() )
		MESSAGEMAN.GetValue( m_sAssociatedValue )->SetString( m_sAssociatedValueSet );

	if( m_sMessageToSend.length() )
		MESSAGEMAN.BroadcastMessage( m_sMessageToSend, new PointerDataMessage( this ) );
}

void Cu2Button::Render( const MercuryMatrix& m )
{
	glDisable( GL_TEXTURE_2D );
	if( m_bDown )
		glColor3f( 0.3f, 0.3f, 0.3f );
	else
		glColor3f( 0.5f, 0.5f, 0.5f );

	glBegin( GL_QUADS );
	glVertex2f( 1., 1. );
	glVertex2f( GetW()-1, 1 );
	glVertex2f( GetW()-1, GetH()-1);
	glVertex2f( 1., GetH()-1 );
	glEnd();

	glLineWidth( 2 );
	glBegin( GL_LINES );
	if( m_bDown )
		glColor3f( 0.1f, 0.1f, 0.1f );
	else
		glColor3f( 0.7f, 0.7f, 0.7f );
	glVertex2f( 1, 1 );
	glVertex2f( 1, GetH()-1 );
	glVertex2f( 1, GetH()-1 );
	glVertex2f( GetW()-2, GetH()-1 );
	if( !m_bDown )
		glColor3f( 0.1f, 0.1f, 0.1f );
	else
		glColor3f( 0.7f, 0.7f, 0.7f );
	glVertex2f( GetW()-1, GetH()-2 );
	glVertex2f( GetW()-1, 1 );
	glVertex2f( GetW()-1, 1 );
	glVertex2f( 1, 1 );
	glEnd();
	glEnable( GL_TEXTURE_2D );

	glColor3f( 1., 1., 1. );

	TransformNode::Render( m );
}

REGISTER_NODE_TYPE(Cu2Button);

///////////////////////////////////////COPPER 2 LABEL///////////////////////////////////////

Cu2Label::Cu2Label() : Cu2Element()
{
	m_pText = (TextNode*)NODEFACTORY.Generate( "TextNode" );
	AddChild( m_pText );
	m_bAutoSize = true;
	m_bDown = false;
}

Cu2Label::~Cu2Label()
{
	if( m_sAssociatedValue.length() )
		MESSAGEMAN.GetValue( m_sAssociatedValue )->DetachModifyDelegate( (ValueDelegate)&Cu2Label::ChangeValue, this );
}

void Cu2Label::LoadFromXML(const XMLNode& node)
{
	LOAD_FROM_XML( "associatedValue", m_sAssociatedValue, );
	if( m_sAssociatedValue.length() )
	{
		MESSAGEMAN.GetValue( m_sAssociatedValue )->AttachModifyDelegate( (ValueDelegate)&Cu2Label::ChangeValue, this );
		printf( "Associating Value: %s\n", m_sAssociatedValue.c_str() );
	}

	LOAD_FROM_XML( "text", m_sText,  );
	LOAD_FROM_XML( "autoSize",  m_bAutoSize, StrToBool );

	if( m_pText )
	{
		m_pText->SetAlignment( TextNode::LEFT );
		m_pText->LoadFont( node.Attribute("font") );
		m_pText->SetSize( StrToFloat( node.Attribute("size") ) );
		SetText( m_sText );
		m_pText->SetShiftAbsolute( true );
		m_pText->SetShiftX( 5 );
		m_pText->SetShiftY( 5 );
	}

	Cu2Element::LoadFromXML( node );

	Refresh();
}

void Cu2Label::SaveToXMLTag( MString & sXMLStream )
{
	if( m_sAssociatedValue.length() ) sXMLStream += ssprintf( "m_sAssociatedValue=\"%s\" ", m_sAssociatedValue.c_str() );
	if( m_bAutoSize ) sXMLStream += ssprintf( "autoSize=\"%d\" ", m_bAutoSize );

	if( !m_pText )
		m_pText->SaveToXMLTag( sXMLStream );

	Cu2Element::SaveToXMLTag( sXMLStream );
}
void Cu2Label::Refresh()
{
	if( !m_pText )
	{
		LOG.Write( "Warning: Cu2Button \"" + GetName() + "\" does not have valid Text box associated." );
		return;
	}

	m_pText->SetText( m_sText );
	m_pText->RenderText();

	if( m_bAutoSize )
	{
		SetSize( m_pText->GetRMaxX() + 8, m_pText->GetRMaxY() + 8 );
	}
}

void Cu2Label::Render( const MercuryMatrix& m )
{
	TransformNode::Render( m );
}

void Cu2Label::ChangeValue( MValue * v )
{
	m_sText = v->GetString();
	Refresh();
}

REGISTER_NODE_TYPE(Cu2Label);

///////////////////////////////////////COPPER 2 DIALOG///////////////////////////////////////


Cu2Dialog::Cu2Dialog() : Cu2Element(), m_bDragging( false ), m_iClickX( 0 ), m_iClickY( 0 )
{
	m_sTitle = "(not set)";
	m_pTitle = (TextNode*)NODEFACTORY.Generate( "TextNode" );
	AddChild( m_pTitle );
}

void Cu2Dialog::LoadFromXML(const XMLNode& node)
{
	LOAD_FROM_XML( "text", m_sTitle,  );

	if( m_pTitle )
	{
		m_pTitle->SetAlignment( TextNode::LEFT );
		m_pTitle->LoadFont( node.Attribute("font") );
		m_pTitle->SetSize( StrToFloat( node.Attribute("size") ) );
		m_pTitle->SetShiftAbsolute( true );
		m_pTitle->SetShiftX( 3 );
		m_pTitle->SetShiftY( GetH() - 18 );
		SetText( m_sTitle );
	}
	Cu2Element::LoadFromXML( node );
}

void Cu2Dialog::SaveToXMLTag( MString & sXMLStream )
{
	if( !m_pTitle )
		m_pTitle->SaveToXMLTag( sXMLStream );

	Cu2Element::SaveToXMLTag( sXMLStream );
}

void Cu2Dialog::Render( const MercuryMatrix& m )
{
	glDisable( GL_TEXTURE_2D );
	glColor3f( 0.5, 0.5, 0.5 );

	glBegin( GL_QUADS );
	glVertex2f( 1., 1. );
	glVertex2f( GetW()-1, 1 );
	glVertex2f( GetW()-1, GetH()-1);
	glVertex2f( 1., GetH()-1 );
	glEnd();

	glLineWidth( 2 );
	glBegin( GL_LINES );
	glColor3f( 0.7f, 0.7f, 0.7f );
	glVertex2f( 1, 1 );
	glVertex2f( 1, GetH()-1 );
	glVertex2f( 1, GetH()-1 );
	glVertex2f( GetW()-2, GetH()-1 );
	glColor3f( 0.1f, 0.1f, 0.1f );
	glVertex2f( GetW()-1, GetH()-2 );
	glVertex2f( GetW()-1, 1 );
	glVertex2f( GetW()-1, 1 );
	glVertex2f( 1, 1 );
	glEnd();

	if( HasFocus() )
		glColor3f( 0., 0., 1. );
	else
		glColor3f( .3f, .3f, .3f );

	glBegin( GL_QUADS );
	glVertex2f( 2., GetH()-19 );
	glVertex2f( GetW()-2, GetH()-19 );
	glVertex2f( GetW()-2, GetH()-3 );
	glVertex2f( 2., GetH()-3 );
	glEnd();

	glEnable( GL_TEXTURE_2D );

	glColor3f( 1., 1., 1. );

	TransformNode::Render( m );
}

void Cu2Dialog::MouseAction( int x, int y, Cu2Action c, int iWhichButton )
{
	if( y > GetH() - 14 && c == PRESS_IN )
	{
		m_bDragging = true;
		m_iClickX = x;
		m_iClickY = y;
	}

	if( c == RELEASE_IN || c == RELEASE_OUT )
		m_bDragging = false;


	Cu2Element::MouseAction( x, y, c, iWhichButton );
}

int Cu2Dialog::MouseMotion( int x, int y, unsigned char iCurrentButtonMask, unsigned char iLastButtonMask )
{
	if( m_bDragging )
	{
		float ix = GetX() - ( m_iClickX - x );
		float iy = GetY() - ( m_iClickY - y );

		int dx = x - m_iClickX;
		int dy = y - m_iClickY;

		SetXY( ix, iy );
		return Cu2Element::MouseMotion( x - dx, y - dy, iCurrentButtonMask, iLastButtonMask );
	}
	return Cu2Element::MouseMotion( x, y, iCurrentButtonMask, iLastButtonMask );
}

void Cu2Dialog::SetText( const MString & sText )
{
	m_sTitle = sText;
	if( m_pTitle )
	{
		m_pTitle->SetText( m_sTitle );
		m_pTitle->RenderText();
	}
}

void Cu2Dialog::UpdateSize()
{
	if( m_pTitle )
	{
		m_pTitle->SetShiftY( GetH() - 18 );
		m_pTitle->RenderText();
	}
}


REGISTER_NODE_TYPE(Cu2Dialog);


/****************************************************************************
 *   Copyright (C) 2008-2009 by Joshua Allen                                *
 *                              Charles Lohr                                *
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



#include "MainMenu.h"
#include <iostream>
#include "Global.h"
#include "TextureDraw.h"
#include "Graphics.h"
#include "GameLogic.h"
#include "SimpleButton.h"
#include "Utility.h"
#include "GameLogic.h"
#include "Core.h"
#include "Audio.h"
//
namespace Framework
{
	MainMenu::MainMenu()
		:m_Idbg( 0 ),
		m_btnStart( 0 ),
		m_btnExit( 0 )
	{
		//
	}
	//
	MainMenu::~MainMenu()
	{
		Destroy();
	}
	//
	int MainMenu::Show()
	{
		LOGIC->Pause( true );
		GRAPHICS->TurnOnDebugCamera();
		Create();
		//
		return RET_SUCCESS;
	}
	//
	int MainMenu::Hide()
	{
		Destroy();
		GRAPHICS->TurnOffDebugCamera();
		LOGIC->Pause( false );
		//
		return RET_SUCCESS;
	}
	//
	void MainMenu::Create()
	{
		int w, h;
		int sx = GRAPHICS->screen_width, sy = GRAPHICS->screen_height;
		m_Idbg = g_textureDraw->CreateTexture( "Assets\\textures\\TheWitcher2_Triss720.jpg" );
		g_textureDraw->GetDimensions( m_Idbg, w, h );
		g_textureDraw->SetPosition( m_Idbg, (sx - w)*0.5f, (sy - h)*0.5f );
		//
		m_btnStart = new SimpleButton( 1 );
		m_btnStart->Init( "Assets\\textures\\start_normal.png","Assets\\textures\\start_over.png", "Assets\\textures\\start_select.png", 100, 100 );
		m_btnStart->AddCallback( &MainMenu::StartCallback, this );
		w = m_btnStart->Width();
		h = m_btnStart->Height();
		m_btnStart->SetPosition( (sx - w)*0.5f +200.0f, (sy - h)*0.5f - 10.0f );
		//
		m_btnExit = new SimpleButton( 2 );
		m_btnExit->Init( "Assets\\textures\\exit_normal.png","Assets\\textures\\exit_over.png", "Assets\\textures\\exit_select.png", 100, 100 );
		m_btnExit->AddCallback( &MainMenu::ExitCallback, this );
		w = m_btnExit->Width();
		h = m_btnExit->Height();
		m_btnExit->SetPosition( (sx - w)*0.5f +200.0f, (sy - h)*0.5f + 90.0f );
	}
	//
	void MainMenu::Destroy()
	{
		if( m_Idbg )
		{
			g_textureDraw->DeleteTexture( m_Idbg );
			m_Idbg = 0;
		}
		//
		utility::safeDelete( m_btnExit );
		utility::safeDelete( m_btnStart );
	}
	//
	void MainMenu::StartCallback( unsigned int id, int state )
	{
		LOGIC->Pause(false);
		AUDIO->Play("main");
		Hide();
	}
	//
	void MainMenu::ExitCallback( unsigned int id, int state )
	{
		Message m(Mid::Quit);
		CORE->BroadcastMessage(&m);
		std::cout<<"exit clicked\n";
	}
}
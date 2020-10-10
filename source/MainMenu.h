#pragma once
//
#include "Singleton.h"
#include "ReturnCodes.h"
//
namespace Framework
{
	class SimpleButton;
	//
	class MainMenu: public Singleton<MainMenu>
	{
	public:
		MainMenu();
		~MainMenu();
		//
	public:
		int				Show();
		int				Hide();
		//
	private:
		void			Create();
		void			Destroy();
		void			StartCallback( unsigned int id, int state );
		void			ExitCallback( unsigned int id, int state );
		//
	private:
		SimpleButton*	m_btnStart;
		SimpleButton*	m_btnExit;
		unsigned int	m_Idbg;
	};
}
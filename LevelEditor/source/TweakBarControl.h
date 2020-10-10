#pragma once
#include "AntTweakBar.h"
#include "Precompiled.h"

namespace Framework
{

class TweakBarControl
{
private:
	char defines[255];
	
	const char* m_name;
	int m_posX, m_posY;
	int m_sizeX, m_sizeY;
	bool m_visible;
	TwBar * m_TwBar;
	std::string s3 ;

public:
	TweakBarControl(const char* s,int posX,int posY,int sizeX, int sizeY, bool visible);
	~TweakBarControl();
	void Initialize();
	TwBar* GetTWBar(); //temp remove later
	const char * GetName();
	void HideBar();
	void ShowBar();
	bool GetIsVisible(){return m_visible;}
	

};

}

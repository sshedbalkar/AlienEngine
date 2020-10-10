#include "TweakBarControl.h"


namespace Framework
{


TweakBarControl::TweakBarControl(const char* s,int posX,int posY,int sizeX, int sizeY, bool visible):m_name(s),m_posX(posX),m_posY(posY),m_sizeX(sizeX),m_sizeY(sizeY),m_visible(visible)
{
	m_TwBar=TwNewBar( m_name  );
	sprintf_s( defines, " %s label = '%s' refresh = 0.5 position = '%d %d' size='%d %d' alpha=100.0 valueswidth=120 visible = %s", m_name,  m_name, m_posX, m_posY, m_sizeX, m_sizeY, (m_visible)?"true":"false" );
	TwDefine( defines );
}

TweakBarControl::~TweakBarControl()
{

}

void TweakBarControl::Initialize()
{

}

const char* TweakBarControl::GetName()
{
	return m_name;
}

TwBar* TweakBarControl::GetTWBar() //temp remove later
{
	return m_TwBar;
}

void TweakBarControl::HideBar()
{
	sprintf_s( defines, " %s visible = false ", m_name  );
	TwDefine( defines );
	m_visible=false;
}
void TweakBarControl::ShowBar()
{
	sprintf_s( defines, " %s visible = true ", m_name );
	TwDefine( defines );
	m_visible=true;
}


}

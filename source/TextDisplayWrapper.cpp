#include "TextDisplayWrapper.h"


namespace Framework
{

	void DisplayOnScreen::Display()
	{
		int x,y;
		x=15;
		y=110;
		for(unsigned int i=0;i<strings.size();++i)
		{
			if(i==20) break;
			g_textDisplay->AddNewText1Frame(strings[i].c_str(),x,y);
			y+=15;
		}
		strings.clear();

		x=1050;
		y=10;
		for(unsigned int i=0;i<static_strings.size();++i)
		{
			g_textDisplay->AddNewText1Frame(static_strings[i].c_str(),x,y);
			y+=15;
		}
	}

}
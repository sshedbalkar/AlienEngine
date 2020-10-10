#pragma once
#include "Global.h"
#include "TextDisplay.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <deque>

namespace Framework
{

	class DisplayOnScreen
	{
	public:
		DisplayOnScreen(){}
		~DisplayOnScreen(){}

		//one frame prints
		template <typename T>
		void AddToPrint(const T& a)
		{
			using namespace std;
			stringstream ss (stringstream::in | stringstream::out);
			ss<<a;
			std::string st=ss.str();
			strings.push_back(st);
		}

		template <typename T>
		void AddToPrint(const char* c,const T& a)
		{
			using namespace std;
			stringstream ss (stringstream::in | stringstream::out);
			ss<<c<<" : ";
			ss<<a;
			std::string st=ss.str();
			strings.push_back(st);
		}

		//static prints
		template <typename T>
		void AddToPrints(const T& a)
		{
			using namespace std;
			stringstream ss (stringstream::in | stringstream::out);
			ss<<a;
			std::string st=ss.str();
			static_strings.push_back(st);
			if(static_strings.size()==4) static_strings.pop_front();
		}

		template <typename T>
		void AddToPrints(const char* c,const T& a)
		{
			using namespace std;
			stringstream ss (stringstream::in | stringstream::out);
			ss<<c<<" : ";
			ss<<a;
			std::string st=ss.str();
			static_strings.push_back(st);
			if(static_strings.size()==4) static_strings.pop_front();
		}

		void Display();
	private:
		std::vector<std::string> strings;
		std::deque<std::string> static_strings;
	};
}
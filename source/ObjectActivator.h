#pragma once //Makes sure this header is only included once


namespace Framework
{
	
	class ObjectActivator
	{
	public:
		ObjectActivator():m_size(100)
		{
			m_activator[0]=1;
			for(int  i=1;i<m_size;++i)
			{
				m_activator[i]=0;
			}
		
		}
		~ObjectActivator(){};

		void Update(const float &t);
		void UpdateButtons(const float &);
		void UpdateOtherObjects(const float &);
		void UpdateAttachedObjects(const float&);
		void InitializeAttachedObjects();
		int m_activator[100];

	private:
		int m_size;
	};
}
#ifndef INSTANCECOUNTER_H
#define INSTANCECOUNTER_H

//This counter is used with singletons to
//ensure proper destruction order of the
//singleton
template<typename T>
class InstanceCounter
{
	public:
		InstanceCounter(const MString & name)
	:m_name(name)
		{
			if (m_count == 0)
			{
				printf("Creating instance %s\n", m_name.c_str());
				m_instance = &T::GetInstance();
			}
			++m_count;
		}
		~InstanceCounter()
		{
			--m_count;
			if (m_count == 0)
			{
				printf("Destroying instance %s\n", m_name.c_str());
				SAFE_DELETE(m_instance);
			}
		}
	private:
		static unsigned long m_count;
		MString m_name;
		T* m_instance;
};

template<typename T>
		unsigned long InstanceCounter<T>::m_count = 0;

#endif

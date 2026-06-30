#ifndef SINGLETON_H
#define SINGLETON_H

#include <mutex>
#include <memory>
#include <iostream>

template<typename T>
class Singleton
{
protected:
	Singleton<T>() = default;
	Singleton(const Singleton<T>& ) = delete;
	Singleton<T>& operator=(const Singleton<T>& ) = delete;

	static std::shared_ptr<T> m_pInstance;

public:
	static std::shared_ptr<T> Instance()
	{
		static std::once_flag flag;
		std::call_once(flag, []() {
			m_pInstance = std::shared_ptr<T>(new T);
			});
		return m_pInstance;
	}

	void PrintAddress()
	{
	}

	~Singleton<T>()
	{
	}
};

template<typename T>
std::shared_ptr<T> Singleton<T>::m_pInstance = nullptr;

#endif // SINGLETON_H
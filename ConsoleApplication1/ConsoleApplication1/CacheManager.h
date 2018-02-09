#pragma once
#include <map>
#include <memory>

template<typename T>
class CacheManager
{
public:
	static CacheManager<T>& Instance() {
		static CacheManager<T> c;
		return c;
	}
	T* getResource(std::string symbol) {
		std::map<std::string, T*>::iterator it = Cache.find(symbol);
		if (it == Cache.end()) return NULL;
		return it->second;
	}
	void releaseResource(std::string symbol) {
		std::map<std::string, T*>::iterator it = Cache.find(symbol);
		
		if (it == Cache.end()) return;
		
		Cache.erase(it);
	}
	void setResource(std::string symbol, T* ptr)
	{
		Cache.insert(std::make_pair(symbol,ptr));
	}
	T* setOrGetResource(std::string symbol)
	{
		T* tmp;
		if ((tmp = getResource(symbol)) != NULL) return tmp;
		tmp = new T(symbol.data());
		Cache.insert(std::make_pair(symbol, tmp));
		return tmp;
	}
	CacheManager() {};
	~CacheManager() {};
private:
	std::map<std::string, T*> Cache;
};


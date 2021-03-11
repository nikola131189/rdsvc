#pragma once

#include <boost/signals2.hpp>
#include <any>
#include <map>
#include <typeindex>


class EventBus
{
public:
	template<typename T, typename F>
	static boost::signals2::connection  subscribe(const F& f)
	{	
		return inst()._subscribe<T>(f);
	}



	template<typename T>
	static void post(const T& t)
	{
		inst()._post(t);
	}

private:
	template<typename T, typename F>
	boost::signals2::connection _subscribe(const F& f)
	{
		auto ind = std::type_index(typeid(T));
		boost::signals2::signal<void(T)>* sig = nullptr;
		auto it = _data.find(ind);
		if (it == _data.end())
		{
			sig = new boost::signals2::signal<void(T)>;
			_data[ind] = sig;
		}
		else
		{
			sig = (boost::signals2::signal<void(T)>*)it->second;
		}
		return sig->connect(f);
	}



	template<typename T>
	void _post(const T& t)
	{
		if (_data.find(std::type_index(typeid(T))) == _data.end())
			return;
		boost::signals2::signal<void(T)>* sig = (boost::signals2::signal<void(T)> *)_data[std::type_index(typeid(T))];
		sig->operator()(t);
	}


	static EventBus& inst()
	{
		static EventBus _inst;
		return _inst;
	}

private:
	std::map<std::type_index, void *> _data;

};


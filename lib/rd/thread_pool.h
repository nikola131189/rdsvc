#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <list>

namespace utility
{
	class thread_pool
	{
	public:
		thread_pool(size_t thr_count = 1);
		~thread_pool();

		void reset();
		void stop();

		void join();

		template<class _Fx, class... _Args > inline
		void push(_Fx&& _Func, _Args&& ... _Ax);

		template<class _Fx> inline
			void push(_Fx&& _Func);

		void clear();
		size_t size();
	private:
		std::queue<std::function<void()>> tasks;
		bool terminate;
		std::condition_variable cv;
		std::mutex mut;
		void thrProc();
		std::list<std::thread> pool;
	};


	template<class _Fx, class ..._Args>
	inline void thread_pool::push(_Fx&& _Func, _Args&& ..._Ax)
	{
		{
			std::unique_lock<std::mutex> lock(mut);
			tasks.push(std::bind(_STD forward<_Fx>(_Func), _STD forward<_Args>(_Ax)...));
			cv.notify_one();
		}
	}

	template<class _Fx>
	inline void thread_pool::push(_Fx && _Func)
	{
		{
			std::unique_lock<std::mutex> lock(mut);
			tasks.push(_Func);
			cv.notify_one();
		}
	}






	inline thread_pool::thread_pool(size_t thr_count)
		: terminate(0)
	{
		for (size_t i = 0; i < thr_count; i++)
		{
			pool.push_back(std::thread(std::bind(&thread_pool::thrProc, this)));
		}
	}


	inline thread_pool::~thread_pool()
	{
		if(!terminate)
		stop();
	}


	inline void thread_pool::reset()
	{
		terminate = 0;
		for (auto it = pool.begin(); it != pool.end(); it++)
		{
			*it = std::thread(std::bind(&thread_pool::thrProc, this));
		}
	}






	inline void thread_pool::clear()
	{
		{
			std::unique_lock<std::mutex> lock(mut);
			while (!tasks.empty())
			{
				tasks.pop();
			}
			cv.notify_one();
		}
	}

	inline size_t thread_pool::size()
	{
		std::unique_lock<std::mutex> lock(mut);
		return tasks.size();
	}

	inline void thread_pool::stop()
	{
		{
			std::unique_lock<std::mutex> lock(mut);
			terminate = true;
			cv.notify_all();
		}

		for (auto i = pool.begin(); i != pool.end(); i++) {

			i->detach();
			if (i->joinable())
				i->join();
		}
	}


	inline void thread_pool::join()
	{

		{
			std::unique_lock<std::mutex> lock(mut);
			terminate = true;
	
			cv.notify_all();
			
		}
		for (auto i = pool.begin(); i != pool.end(); i++) {
			if (i->joinable())
				i->join();
		}
	
	}


	inline void thread_pool::thrProc()
	{
		while (1)
		{
			std::function<void()> job;
			{
				std::unique_lock<std::mutex> lock(mut);
				if (terminate)
					return;
				if (!tasks.empty()) {
					job = tasks.front();
					tasks.pop();
				}
				else {
					cv.wait(lock);
				}
			}
			if (job)
				job();
		}
	}
}


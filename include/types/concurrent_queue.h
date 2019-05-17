#pragma once

#include <condition_variable>
#include <atomic>
#include <chrono>
#include <mutex>
#include <queue>

namespace types{

template<typename type>
class concurrent_queue{
	std::queue<type> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::atomic_bool m_forceExit;
public:
	concurrent_queue(){
		m_forceExit = false;
	}

	void push(const type &data){
		m_forceExit = false;
		std::unique_lock<std::mutex> lk(m_mutex);
		m_queue.push(data);
		lk.unlock();
		m_cv.notify_one();
	}

	template <class... Args> void emplace (Args&&... args){
		m_forceExit = false;
		std::unique_lock<std::mutex> lk(m_mutex);
		m_queue.emplace(args...);
		lk.unlock();
		m_cv.notify_one();
	}
	
	bool empty()const{
		std::unique_lock<std::mutex> lk(m_mutex);
		return m_queue.empty();
	}

	std::size_t size()const{
		std::unique_lock<std::mutex> lk(m_mutex);
		return m_queue.size();
	}

	bool pop(type &data){
		std::unique_lock<std::mutex> lk(m_mutex);
		if(m_forceExit || m_queue.empty())
			return false;
		data = m_queue.front();
		m_queue.pop();
		return true;
	}

	bool wait_pop(type &data){
		std::unique_lock<std::mutex> lk(m_mutex);
		m_cv.wait(lk, [&]()->bool{
			return !m_queue.empty() || m_forceExit;
		});
		if(m_forceExit || m_queue.empty())
			return false;
		data = m_queue.front();
		m_queue.pop();
		return true;
	}

	bool wait_pop(type &data, std::chrono::nanoseconds time){
		std::unique_lock<std::mutex> lk(m_mutex);
		m_cv.wait_for(lk, time, [&]()->bool{
			return !m_queue.empty() || m_forceExit;
		});
		if(m_forceExit || m_queue.empty())
			return false;
		data = m_queue.front();
		m_queue.pop();
		return true;
	}

	void clear(){ 
		m_forceExit = true;
		std::unique_lock<std::mutex> lk(m_mutex);
		while (!m_queue.empty()){
			m_queue.pop();
		}
		m_cv.notify_one();
	}

	bool exit_requested()const{
		return m_forceExit;
	}
};

}

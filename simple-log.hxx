#ifndef SIMPLE_LOG_H
#define SIMPLE_LOG_H

#define LOG_NAME "output.log"

#include <ctime>
#include <chrono>
#include <queue>
#include <mutex>
#include <thread>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <condition_variable>


    enum msg_type
	{
		DEBUG = 1,
		INFO,
		WARNING,
		CRITICAL,
		ERRORS
	};

namespace tools
{
    template <class T>
    class tsqueue
    {
        public:
            void push(T item) {
                std::unique_lock<std::mutex> lock(m_mutex);
  
                m_queue.push(item);
                m_condition.notify_one();
            }
  
            T pop() {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_condition.wait(lock,
                    [this]() { return !m_queue.empty(); });
  
                T item = m_queue.front();
                m_queue.pop();
  
                return item;
            }

        private:
            std::queue<T> m_queue;
            std::mutex    m_mutex;
            std::condition_variable m_condition;
    };

    class loger_client
    {
        public:
            loger_client(loger_client const&) = delete;
            loger_client& operator= (loger_client const&) = delete;

            loger_client(msg_type type = msg_type::INFO);
            ~loger_client();

            inline static loger_client& log(msg_type type)
            {
                static loger_client  instance(type);
                return instance;
            }

            template<typename T>           
            loger_client& operator<< (const T& data) 
            {
                m_stream.clear();
                m_stream << data << "\n";
                std::cout << m_stream.str();
                return *this;
            }

            void update()
            {
                std::cout << m_task_log.msg_text;
            }
        
        private:
            struct task_log_t
            {
                std::time_t time_now;
                msg_type type;
                std::string msg_text;
            } m_task_log;
        protected:
            std::stringstream m_stream;
    };
    
    loger_client::loger_client(msg_type type)
    {
        m_task_log.type = type;
        m_task_log.time_now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }
    
    loger_client::~loger_client()
    {
    }
    
    __declspec(selectany) loger_client& xout = loger_client::log(INFO); //msvc or extern
} // namespace tools


#endif
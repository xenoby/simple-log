#ifndef SIMPLE_LOG_H
#define SIMPLE_LOG_H


#include <ctime>
#include <chrono>

#include <array>
#include <queue>
#include <string>

#include <mutex>
#include <thread>
#include <condition_variable>

#include <iostream>
#include <sstream>
#include <fstream>

#ifdef _WIN64
#include <windows.h>
#endif

#define LOG_NAME "output.log"

    enum msg_type
	{
        LOG = 1,
		DEBUG,
		INFO,
		WARNING,
		CRITICAL,
		ERRORS,
        NONE = 0
	};

namespace tools
{
    template <class T>
    class tsqueue
    {
        public:
            void push(T &item) {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_queue.push(item);
                lock.unlock(); 
                m_condition.notify_one();
            }

            bool empty() const
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                bool value = m_queue.empty();
                lock.unlock();
                return value;
            }

            bool try_pop(T& popped_value) {
                std::unique_lock<std::mutex> lock(m_mutex);
                
                if(m_queue.empty())
                {
                    lock.unlock();
                    return false;
                }
        
                popped_value = m_queue.front();
                m_queue.pop();
                
                lock.unlock();
                return true;
            }

            /*
            void wait_and_pop(T& popped_value){
                std::unique_lock<std::mutex> lock(m_mutex);
                //while(m_queue.empty())
                //{
                m_condition.wait(lock, [&]() { return !m_queue.empty()});
                //}

                popped_value = m_queue.front();
                m_queue.pop();
            }*/

        private:
            std::queue<T> m_queue;
            std::mutex    m_mutex;
            std::condition_variable m_condition;
    };

    struct task_log_t
    {
        std::time_t time_now;
        msg_type type;
        std::string msg_text;
    };

    tsqueue<task_log_t> queue_tasks;

    class loger_consumer
    {
        public:
            loger_consumer(loger_consumer const&) = delete;
            loger_consumer& operator= (loger_consumer const&) = delete;

            loger_consumer(std::ostream& stream = std::cout);
            ~loger_consumer();

            inline static loger_consumer& log_attach(std::ostream& stream = std::cout)
            {
                static loger_consumer  instance(stream);
                return instance;
            }

#ifdef _WIN64
		private:
		HANDLE hStdOut;
#endif
        private:
            std::thread m_thread;
            std::array<std::string, 7> m_status
            {
                ":",
                "[LOG]:",
                "[DEBUG]:",
                "[INFO]:",
                "[WARNING]:",
                "[CRITICAL]:",
                "[ERROR]:"
            };

        protected:
            void print_message();
            void set_color(msg_type type);
            std::string get_data_time_str(std::time_t time_now);
            std::ostream& m_out_stream;
            std::ofstream m_file;
            
    };
    
    loger_consumer::loger_consumer(std::ostream& stream):
        m_out_stream(stream)
    {
        std::time_t time_now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string fname = get_data_time_str(time_now);
        fname.erase(fname.size() - 9);
        fname.insert(fname.end(), ' ');
        fname+=LOG_NAME;

        m_file.open(fname);
        hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        m_thread = std::thread(&loger_consumer::print_message, this);
    }

    loger_consumer::~loger_consumer()
    {
        m_thread.join();
        m_file.close();
    }
    
    void loger_consumer::print_message()
    {
        while (true)
        {
            std::string str_time;
            task_log_t task = {0};
            
            if(queue_tasks.try_pop(task))
            {
                set_color(task.type);
                str_time = get_data_time_str(task.time_now);
                str_time.insert (str_time.begin(), '[');
                str_time.insert (str_time.end(),   ']');
                
                m_out_stream << std::flush;
                m_out_stream << str_time << m_status[task.type] << task.msg_text << std::endl;
                m_file << str_time << m_status[task.type] << task.msg_text << std::endl;
            } else continue;
            set_color(NONE);
        }
    }

    std::string loger_consumer::get_data_time_str(std::time_t time_now)
    {   
        std::string str_time_buf(20, ' ');
        std::string str_time;

        std::strftime(&str_time_buf[0], str_time_buf.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&time_now));
        str_time = str_time_buf;
        str_time.pop_back();
        return str_time;
    }

#ifdef _WIN64
	void loger_consumer::set_color(msg_type type)
	{
        switch (type)
        {
            case LOG:       SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY); break;
            case DEBUG:     SetConsoleTextAttribute(hStdOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;
		    case INFO:      SetConsoleTextAttribute(hStdOut, FOREGROUND_GREEN | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;
		    case WARNING:   SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;
		    case CRITICAL:  SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN); break;
		    case ERRORS:    SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_INTENSITY); break;
            default:        SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); break;
        }
	}
#elif defined(__linux__)
	void loger_consumer::set_color(msg_type type)
	{
        switch (type)
        {
            case LOG:       std::cout << "\033[ 0m  "; break;
		    case DEBUG:     std::cout << "\033[ 94 ;"; break;
		    case INFO:      std::cout << "\033[ 92 ;"; break;
		    case WARNING:   std::cout << "\033[ 93 ;"; break;
		    case CRITICAL:  std::cout << "\033[ 33 ;"; break;
		    case ERRORS:    std::cout << "\033[ 91 ;"; break;
            default:        std::cout << "\033[ 91 ;"; break;
        }
	}
#endif


    class loger_producer
    {
        public:
            loger_producer(loger_producer const&) = delete;
            loger_producer& operator= (loger_producer const&) = delete;

            loger_producer();
            ~loger_producer();

            inline static loger_producer& log(msg_type type)
            {
                static loger_producer  instance;
                instance.set_type(type);
                return instance;
            }

            template<typename T>           
            loger_producer& operator<< (const T& data) 
            {
                task_log_t task_log;

                m_stream.str(std::string());
                m_stream << data;

                task_log.msg_text.clear();
                task_log.type = m_current_type;
                task_log.time_now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                task_log.msg_text = m_stream.str();
                
                queue_tasks.push(task_log);
                return *this;
            }

        private:   
            msg_type m_current_type;
            void set_type(msg_type type) { m_current_type = type;}
        protected: 
            std::stringstream m_stream;     
    };
    
    loger_producer::loger_producer() 
    {
    }

    loger_producer::~loger_producer()
    {
    }
    

    //__declspec(selectany) 
    loger_producer& xout = loger_producer::log(INFO); //msvc or extern

    
    //__declspec(selectany) 
    loger_consumer& xlog = loger_consumer::log_attach();
    
    
} // namespace tools


#endif

#ifndef DATE_TIME_HPP_201106301147
#define DATE_TIME_HPP_201106301147

#include "interprocess_synch.hpp"
#include "str_algorithm.hpp"
#include <string>
#include <deque>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

namespace sparrow{

	class date_time 
	{
	private:
		int64_t		microsecond_;
		tm					t_;
		
		static ipc_mutex			lock_;
	public:
		date_time(void) : 
		microsecond_(0)
		{memset(&t_, 0, sizeof(tm));};

		date_time(const date_time &rhs) :
		microsecond_(rhs.microsecond_),
		t_(rhs.t_)
		{};

		date_time(const std::string &str) : 
		microsecond_(0)
		{
			using namespace str_algorithm;

			std::deque<string> res;
			split(res, str, is_any_of(" "));
			if(res.size() != 2) return;
			string str_d = res.at(0);
			string str_t = res.at(1);
			res.clear();


			unsigned int   microsecs;
			try{
				split(res, str_d, is_any_of("-"));
				if(res.size() != 3) return;
				t_.tm_year	= lexical_cast<int>(res.at(0)) - 1900;
				t_.tm_mon	= lexical_cast<unsigned short>(res.at(1)) - 1;
				t_.tm_mday  = lexical_cast<unsigned short>(res.at(2));
				res.clear();

				split(res, str_t, is_any_of(":."));
				if(res.size() < 3) return;
				t_.tm_hour	= lexical_cast<unsigned short>(res.at(0));
				t_.tm_min	= lexical_cast<unsigned short>(res.at(1));
				t_.tm_sec	= lexical_cast<unsigned short>(res.at(2));
				if(res.size() == 4){
					microsecs = lexical_cast<unsigned int>(res.at(3));
				}
			}catch(bad_lexical_cast &){
			}

			time_t tt = mktime(&t_);
			microsecond_ = (int64_t)tt * 1000000  + microsecs;
		};

		date_time& operator=(const date_time &rhs){
			if(this == &rhs) return *this;
			
			microsecond_	= rhs.microsecond_;
			t_				= rhs.t_;

			return *this;
		};

		static date_time gettimeofday(void){
			timeval tval;
			::gettimeofday(&tval, NULL);
			int64_t micros = (int64_t)tval.tv_sec * 1000000 + tval.tv_usec;
			date_time dt;
			dt.total_microseconds(micros);
			return dt;
		};

		void total_seconds(const time_t t){
			microsecond_ = ((int64_t)t) * 1000000;
			{
				ipc_mutex::scoped_lock guard(lock_);
				t_			 = *localtime(&t);
			}
			
		};
		time_t total_seconds(void) const {
			return (time_t)(microsecond_ / 1000000);
		};

		void total_milliseconds(const int64_t t){
			microsecond_	= t * 1000;
			time_t tt = (time_t)(t/1000);
			{
				ipc_mutex::scoped_lock guard(lock_);
				t_ = *localtime(&tt);
			}
		};
		int64_t total_milliseconds(void) const {
			return microsecond_ / 1000;
		};

		void total_microseconds(const int64_t t){
			microsecond_	= t;
			time_t tt = (time_t)(microsecond_/1000000);
			{
				ipc_mutex::scoped_lock guard(lock_);
				t_	 = *localtime(&tt);
			}
		};
		int64_t total_microseconds(void){
			return microsecond_;
		};

		int year(void) const {
			return t_.tm_year + 1900;
		};
		unsigned int month(void) const {
			return t_.tm_mon + 1;
		};
		unsigned int day(void) const {
			return t_.tm_mday;
		};
		unsigned short hour(void) const {
			return t_.tm_hour;
		};
		unsigned short minute(void) const {
			return t_.tm_min;
		};
		unsigned short second(void) const {
			return t_.tm_sec;
		};

		unsigned int microsecond(void) const {
			return (unsigned int)(microsecond_%1000000);
		};

		std::string to_string(void){
			char tstr[128] = {0};
			sprintf(tstr, "%d-%02u-%02u %02u:%02u:%02u", year(), month(), day(), hour(), minute(), second());
			return std::string(tstr);
		};

	};
}

#endif



#ifndef LOGGING_HPP_201107152115
#define LOGGING_HPP_201107152115

#include "singleton.hpp"
#include <string>
#include <list>
#include <sstream>
#include <fstream>
#include <stdint.h>
#include "date_time.hpp"

namespace sparrow{
	
	class logging
	{
	public:
		enum{
			LOG_INFO		= 0x01,
			LOG_WARNING		= 0x02,
			LOG_ERROR		= 0x04,
			LOG_DEBUG		= 0x08,
			LOG_USER		= 0x10,
		};

		class log_format;

		struct log_end_type{};

		/**日志文件策略*/
		struct file_strategy{
			file_strategy(void) :
			rf_duration(0), f_max_size(0),
			keep_days(7)
			{};

			file_strategy(const uint32_t &rfduration, 
				const uint32_t &fmaxsize,
				const uint32_t &keepdays = 7
				) :
			rf_duration(rfduration), f_max_size(fmaxsize * 1024),
			keep_days(keepdays)
			{};

			uint32_t rf_duration;  //事实日志文件的时间长度(分钟)
			uint32_t f_max_size;	  //日志文件的最大限制
			uint32_t keep_days;    //日志文件保留天数
		};

	public:
		logging(void);
		~logging(void);

		void log_type(const std::string &type);

		void file_path(const std::string &fp);

		void set_file_strategy(const file_strategy &fsgy){
			fsgy_	= fsgy;
		};

		const file_strategy& get_file_strategy(void) const{
			return fsgy_;
		};

		void enable_cout(const bool b){enable_cout_ = b;};

		void write_log(const int type, const std::string &msg);

	private:
		void init_file(void);
		void file_managed(const date_time &curt);
		void clear_history_log(const date_time &curt);
		void split_file(void);
		std::string cr_logf_path(void);
		std::string get_last_logf_path(void);
		void app_file(const std::string &dstfile, const std::string &srcfile);
	private:
		int				type_;
		mutex			lock_;

		std::string		fpath_;
		std::ofstream	ofs_;
		file_strategy	fsgy_;
		date_time		f_tstamp_, fmanaged_t_;

		bool		enable_cout_;
	};

	typedef singleton<logging>	ins_log;

	class logging::log_format
	{
	public:
		log_format(const int level) : level_(level){};

	public:
		template<typename T>
		log_format& operator<<(const T &data){
			ss_ << data;
			return *this;
		}

		//template<>
		inline log_format& operator<<(const logging::log_end_type &end){
			ss_ << "\n";
			ins_log::instance()->write_log(level_, ss_.str());
			return *this;
		}

	public:
		std::stringstream	ss_;
		int					level_;
	};

	static const logging::log_end_type	log_end = logging::log_end_type();

}

#define pins_log sparrow::ins_log::instance()

#define log_info sparrow::logging::log_format(sparrow::logging::LOG_INFO)
#define log_warning sparrow::logging::log_format(sparrow::logging::LOG_WARNING)
#define log_error sparrow::logging::log_format(sparrow::logging::LOG_ERROR)
#define log_debug sparrow::logging::log_format(sparrow::logging::LOG_DEBUG)
#define log_user sparrow::logging::log_format(sparrow::logging::LOG_USER)
#define log_end sparrow::logging::log_end_type()

#endif


#ifndef CMD_LINE_HPP_201203131606
#define CMD_LINE_HPP_201203131606

#include <string>
#include <map>
#include <sstream>
#include <exception>

using std::string;

namespace sparrow
{
	class cmd_line
	{
	public:
		class user_opt{
		public:
			string name;
			string defval;
			string val;
			string desc;

			cmd_line *pcmdline;

			user_opt(cmd_line *pcl) : pcmdline(pcl){};
			user_opt(const user_opt &rhs) :
			name(rhs.name), defval(rhs.defval), 
			val(rhs.val), desc(rhs.desc),
			pcmdline(rhs.pcmdline)
			{}

			user_opt& operator=(const user_opt &rhs){
				if(this == &rhs) return *this;
				name	= rhs.name;
				defval	= rhs.defval;
				val		= rhs.val;
				desc	= rhs.desc;
				pcmdline = rhs.pcmdline;
			};

			user_opt operator()(const string &_name, const string &_desc){
				name	= _name;
				desc	= _desc;
				pcmdline->add_user_opt(*this);
				return user_opt(pcmdline);
			};

			user_opt operator()(const string &_name, const string &_defval, const string &_desc){
				name	= _name;
				defval	= _defval;
				desc	= _desc;
				pcmdline->add_user_opt(*this);
				return user_opt(pcmdline);
			};
		};

		class no_found_opt : public std::exception
		{
		public:
			no_found_opt(const string &err) : err_(err){};
			virtual ~no_found_opt(void) throw(){};

			const char* what(void) const throw(){
				return err_.c_str();
			};

		public:
			string err_;
		};

	public:
		cmd_line(void) : size_(0){};
		~cmd_line(void);

		user_opt add_opt(void){
			return user_opt(this);
		};
		
		bool parse_cmd_line(int argc, char **argv, string *err = NULL);

		size_t size(void) const {return size_;};

		bool has_opt(const string &name);

		template<typename T>
		T get_opt_as(const string &name)
		{
			T	ret;
			
			_map_opts::iterator iter = opts_.find(name);
			if(opts_.end() == iter) {
				std::stringstream ss;
				ss << "not found opt " << name;
				throw no_found_opt(ss.str());
			}
			if(iter->second->val.length() == 0) {
				std::stringstream ss;
				ss << "opt " << name << " value is null";
				throw no_found_opt(ss.str());
			}
			std::stringstream ss;
			ss << iter->second->val;
			ss >> ret;
			return ret;
		};

	private:
		void add_user_opt(const user_opt &opt);

	private:
		typedef std::map<string, user_opt*>		_map_opts;

	private:
		_map_opts		opts_;
		size_t			size_;


	public:
		template<typename ostream_type>
		friend ostream_type& operator<<(ostream_type &ost, const cmd_line &cmdl){
			cmd_line::_map_opts::const_iterator iter;
			string str = "";
			for(iter = cmdl.opts_.begin(); iter != cmdl.opts_.end(); ++ iter){
				cmd_line::user_opt *popt = iter->second;
				str += "-" + popt->name + " default[" + popt->defval + "] " + popt->desc + "\n";
			}
			ost << str;
			return ost;
		}
	};

}





#endif


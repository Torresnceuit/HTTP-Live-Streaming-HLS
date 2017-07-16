
#ifndef __TFC_BASE_FILE_CONFIG_H__
#define __TFC_BASE_FILE_CONFIG_H__

#include "tfc_base_config.h"

#include <deque>

using namespace std;

namespace tfc { namespace base {

/**
 * 配置类: CConfig的File实现版本<br>
 * 配置中\n表示回车符
 * @author  casper@tencent.com
 * @version  0.4
 * @see #CConfig
 * @since version 0.3
 */
class CFileConfig:public CConfig
{
public:
	CFileConfig();
	/**
	 * no implementation
	 */
	CFileConfig(const CFileConfig&);
	virtual ~CFileConfig();

public:
	/**
	 * 初始化,设置配置文件名&Load
	 * @throw conf_load_error when Load fail
	 * @param filename 配置文件名
	 */
	void Init(const string& filename) throw(conf_load_error);

	/**
	 * 加载配置
	 * @throw conf_load_error when Load fail
	 */
	virtual void Load() throw (conf_load_error);

	/**
	 * 取配置,支持层次结构,以'\'划分,如conf["Main\\ListenPort"]
	 * @throw conf_not_find when Load fail
	 */
	virtual const string& operator [](const string& name) const throw(conf_not_find);
	/**
	 * 取path下所有name-value对
	 */
	virtual const map<string,string>& GetPairs(const string& path) const;
	/**
	 * 取path下所有name-value或single配置
	 */
	virtual const vector<string>& GetDomains(const string& path) const;
	/**
	 * 取path下所有subpath名(只取一层)
	 */
	virtual const vector<string>& GetSubPath(const string& path) const;

    virtual void Load(string& scontent);
protected:
	enum EntryType {
		T_STARTPATH = 0,
		T_STOPPATH = 1,
		T_NULL = 2,
		T_PAIR = 3,
		T_DOMAIN =4,
		T_ERROR = 5
	};

	string start_path(const string& s);
	string stop_path(const string& s);
	void decode_pair(const string& s,string& name,string& value);
	string trim_comment(const string& s); //trim注释和\n符号
	string path(const deque<string>& path);
	string parent_path(const deque<string>& path);
	string sub_path(const deque<string>& path);

	EntryType entry_type(const string& s);
protected:
	string _filename;

	map<string,map<string,string> > _pairs;
	map<string,vector<string> > _paths;
	map<string,vector<string> > _domains;

	map<string,string> _null_map;
	vector<string> _null_vector;
};

}}
#endif //



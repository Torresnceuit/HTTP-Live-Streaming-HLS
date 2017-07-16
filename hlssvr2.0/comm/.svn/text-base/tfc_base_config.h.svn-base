
#ifndef __TFC_BASE_CONFIG_H__
#define __TFC_BASE_CONFIG_H__
/*
	配置接口类

	要求支持层次结构,层次以\分割,如conf["Main\\ListenPort"]取Main路径下的ListenPort配置值
	大小写敏感,
	a.name-value型配置:[]访问单个配置值,如果不存在,抛出异常conf_not_find
			GetPairs返回某个path下所有的name-value对,不考虑顺序
			相同路径下存在同样的配置名,取后一个

	b.domain型配置:GetDomains返回某个path下所有的domain配置项,与顺序相关

	c.GetSubPath返回某个path下所有的subpath名
	Create by casper@tencent.com ,2004-11-15
*/

#include <string>
#include <map>
#include <vector>
#include <stdexcept>

using namespace std;

namespace tfc { namespace base {

struct conf_load_error: public runtime_error{ conf_load_error(const string& s):runtime_error(s){}};
struct conf_not_find: public runtime_error{ conf_not_find(const string& s):runtime_error(s){}};

/**
 * 配置类接口<br>
 * @author  casper@tencent.com
 * @version  0.4
 * @see #CFileConfig
 * @see #CDbConfig
 * @since version 0.3
 */
class CConfig
{
public:
	CConfig(){}
	CConfig(const CConfig&);
	virtual ~CConfig(){}

public:
	/**
	 * 加载配置
	 * @throw conf_load_error when Load fail
	 */
	virtual void Load()=0;

	/**
	 * 取配置,要求支持层次结构,以'\'划分,如conf["Main\\ListenPort"]
	 * @throw conf_not_find when Load fail
	 */
	virtual const string& operator [](const string& name) const = 0;

	/**
	 * 取path下所有name-value对
	 */
	virtual const map<string,string>& GetPairs(const string& path) const = 0;
	/**
	 * 取path下所有name-value或single配置
	 */
	virtual const vector<string>& GetDomains(const string& path) const = 0;
	/**
	 * 取path下所有subpath名(只取一层)
	 */
	virtual const vector<string>& GetSubPath(const string& path) const = 0;

    virtual void Load(string& scontent) = 0; 
};

}}
#endif //


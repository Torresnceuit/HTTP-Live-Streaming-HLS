
#include "tfc_base_config_file.h"
#include "tfc_base_comm.h"
#include <sstream>
#include <fstream>

using namespace std;
using namespace tfc::base;

CFileConfig::CFileConfig(){}
CFileConfig::~CFileConfig(){}
inline string CFileConfig::start_path(const string& s)
{
	return string(s.c_str(),1,s.length()-2);
}

inline string CFileConfig::stop_path(const string& s)
{
	return string(s.c_str(),2,s.length()-3);
}

inline void CFileConfig::decode_pair(const string& s,string& name,string& value)
{
	bool find=false;
	char *p=const_cast<char *>(s.c_str());
	char *p1 = p;

	while(*p1 != 0) {
		if(*p1 == '=' || *p1 == '\t' || *p1 == ' ') {
			find = true;
			break;
		}
		p1++;
	}
	if(!find || p1==p) {
		throw conf_load_error(string("CFileConfig::Load:can not find name: ")+s);
	}
	name = string(p,0,p1-p);
	p += s.length(); find = false;
	while(*p1 != 0) {
		if(*p1 != '=' && *p1 != '\t' && *p1 != ' ') {
			find = true;
			break;
		}
		p1++;
	}
	value = string(p1,0,p-p1);
}

CFileConfig::EntryType CFileConfig::entry_type(const string& s)
{
	if(s.length() == 0) return T_NULL;
	if(s.c_str()[0] == '<') {
		if(s.c_str()[s.length()-1] == '>') {
			if(s.c_str()[1] == '/') return T_STOPPATH;
			else return T_STARTPATH;
		} else {
			return T_ERROR;
		}
	} else {
		if(s.find(" ") == string::npos && s.find("\t") == string::npos && s.find("=") == string::npos) {
			return T_DOMAIN;
		}
		else return T_PAIR;
	}
	return T_ERROR;
}

// #开头的为注释,同时trim \n
inline string CFileConfig::trim_comment(const string& s)
{
	if(s.c_str()[0] == '#') return "";
	size_t pos = s.find("\\n");
	if(pos == string::npos) return s;
	string trimn;
	size_t idex = 0;
	while(pos!=string::npos) {
		if(pos > idex) {
			trimn += s.substr(idex,pos-idex);
		}
		trimn += "\n";
		idex = pos+2;
		pos = s.find("\\n",idex);
	}
	trimn += s.substr(idex,s.length()-idex);
	return trimn;
}

// 得到当前路径,调用之前判断是否为空
inline string CFileConfig::path(const deque<string>& path)
{
	deque<string>::const_iterator it;
	string s;
	if(path.size() == 0) {
		throw conf_load_error("CFileConfig::Load:must not into here");
	}
	for(it=path.begin();it!=path.end();it++) {
		s += "\\";
		s += (*it);
	}
	return string(s.c_str(),1,s.length()-1);
}

// 得到parent路径，调用前判断是否为空，可能返回""
inline string CFileConfig::parent_path(const deque<string>& path)
{
	deque<string>::const_iterator it;
	string s("");
	unsigned i=0;
	if(path.size() == 0) {
		throw conf_load_error("CFileConfig::Load:must not into here");
	}
	if(path.size() == 1) {
		return s;
	}
	for(it=path.begin();it!=path.end();it++) {
		if(i+1==path.size()) break;
		s += "\\";
		s += (*it);
		i++;
	}
	return string(s.c_str(),1,s.length()-1);
}

// 得到sub路径，调用前判断是否为空
inline string CFileConfig::sub_path(const deque<string>& path)
{
	deque<string>::reverse_iterator it;
	string s;
	if(path.size() == 0) {
		throw conf_load_error("CFileConfig::Load:must not into here");
	}

	return (*it);
}

void CFileConfig::Init(const string& filename)
	throw (conf_load_error)
{
	_filename = filename;
	Load();
}

void CFileConfig::Load()
	throw (conf_load_error)
{
	if(_filename == "")  throw conf_load_error("CFileConfig::Load: filename is null");

	string sline;
	string sreal;
	ifstream in(_filename.c_str());
	if(!in) 
		throw conf_load_error(string("CFileConfig::Load: can not open ")+_filename);

	deque<string> path_stack; 
	_pairs.clear();
	_paths.clear();
	_domains.clear();

	string name,value;
	deque<string>::reverse_iterator it;

	while(getline(in, sline)) {
		sreal = trim_comment(trim_right(trim_left(sline," \t")," \t\r\n"));
		switch(entry_type(sreal)) {
		case T_STARTPATH:
			path_stack.push_back(start_path(sreal));
//			_pairs[path(path_stack)] = _null_map;
//			_domains[path(path_stack)] = _null_vector;
//			if(_paths.find(parent_path(path_stack)) == _paths.end())[_paths[parent_path(path_stack)]] = _null_vector);
			_paths[parent_path(path_stack)].push_back(start_path(sreal));			
			break;
		case T_STOPPATH:
			it = path_stack.rbegin();
			if(it==path_stack.rend() || (*it) != stop_path(sreal)) {
				throw conf_load_error(string("CFileConfig::Load: path not match: ")+stop_path(sreal));
			}
			path_stack.pop_back();
			break;
		case T_NULL:
			break;
		case T_PAIR: // 同时也作为domain
			if(path_stack.size() == 0) {
				throw conf_load_error("CFileConfig::Load: path is null");
			}
			decode_pair(sreal,name,value);

			_pairs[path(path_stack)][name] = value;
			_domains[path(path_stack)].push_back(sreal);
			break;
		case T_DOMAIN:
			if(path_stack.size() == 0) {
				throw conf_load_error("CFileConfig::Load: path is null");
			}
			_domains[path(path_stack)].push_back(sreal);
			break;
		default:
			throw conf_load_error(string("CFileConfig::Load: invalid:")+sreal);
		}		
	}
	//文件解析完要判断path_stack是否为空
	if(path_stack.size() != 0) {
		throw conf_load_error("CFileConfig::Load: path havenot over");
	}

/* 
        size_t i=0;
        cout << "pairs:" << _pairs.size() << endl;
        map<string,map<string,string> >::iterator it_pair;
        map<string,string>::iterator it_map_str;
        map<string,vector<string> >::iterator it_path_domain;

        for(it_pair=_pairs.begin();it_pair!=_pairs.end();it_pair++) {
                cout << i++ << " " << (*it_pair).first << endl;
                for(it_map_str=(*it_pair).second.begin();it_map_str!=(*it_pair).second.end();it_map_str++) {
                        cout << " " << i++ << " " << (*it_map_str).first << "-" << (*it_map_str).second << endl;
                }
        }

        cout << "domains:" << _domains.size() << endl;
        i=0;
        for(it_path_domain=_domains.begin();it_path_domain!=_domains.end();it_path_domain++) {
                cout << i++ << " " << (*it_path_domain).first << endl;
                for(size_t j=0;j<(*it_path_domain).second.size();j++) {
                        cout << " " << i++ << " " << (*it_path_domain).first << "-" << (*it_path_domain).second[j] << endl;
                }
        }

        cout << "paths:" << _paths.size() << endl;
        for(it_path_domain=_paths.begin();it_path_domain!=_paths.end();it_path_domain++) {
                cout << i++ << " " << (*it_path_domain).first << endl;
                for(size_t j=0;j<(*it_path_domain).second.size();j++) {
                        cout << " " << i++ << " " << (*it_path_domain).first << "-" << (*it_path_domain).second[j] << endl;
                }
        }
*/
}

const string& CFileConfig::operator [](const string& name) const throw(conf_not_find)
{
	if(name.length()==0) {
		throw conf_not_find("CFileConfig::[] is not find");
	}

	char *p0=const_cast<char *>(name.c_str());
	char *p1=p0+name.length()-1;
	char *p=p1;
	while(p > p0) {
		if(*p=='\\') break;
		p--;
	}
	if(p == p0 || p == p1) throw conf_not_find(string("CFileConfig::[] path is null:")+name);
	string path(p0,0,p-p0);
	string realname(p+1,0,p1-p);

	map<string,map<string,string> >::const_iterator it = _pairs.find(path);
	if(it == _pairs.end()) throw conf_not_find(string("CFileConfig::[")+name+"] is not find");
	map<string,string>::const_iterator it2 = (*it).second.find(realname);
	if(it2==(*it).second.end()) throw conf_not_find(string("CFileConfig::[")+name+"] is not find");
	return (*it2).second;

}

const map<string,string>& CFileConfig::GetPairs(const string& path) const
{
	map<string,map<string,string> >::const_iterator it = _pairs.find(path);
	if(it == _pairs.end()) return _null_map;
	return (*it).second;
}

const vector<string>& CFileConfig::GetDomains(const string& path) const
{
	map<string,vector<string> >::const_iterator it = _domains.find(path);
	if(it == _domains.end()) return _null_vector;
	return (*it).second;
}

const vector<string>& CFileConfig::GetSubPath(const string& path) const
{
	map<string,vector<string> >::const_iterator it = _paths.find(path);
	if(it == _paths.end()) return _null_vector;
	return (*it).second;	
}

void tfc::base::CFileConfig::Load( string& scontent )
{
    if(scontent == "")  throw conf_load_error("CFileConfig::Load: content is null");

    string sline;
    string sreal;
    stringstream in(scontent.c_str());
    if(!in) 
        throw conf_load_error(string("CFileConfig::Load: can not open ")+_filename);

    deque<string> path_stack; 
    _pairs.clear();
    _paths.clear();
    _domains.clear();

    string name,value;
    deque<string>::reverse_iterator it;

    while(getline(in, sline)) {
        sreal = trim_comment(trim_right(trim_left(sline," \t")," \t\r\n"));
        switch(entry_type(sreal)) {
        case T_STARTPATH:
            path_stack.push_back(start_path(sreal));
            //			_pairs[path(path_stack)] = _null_map;
            //			_domains[path(path_stack)] = _null_vector;
            //			if(_paths.find(parent_path(path_stack)) == _paths.end())[_paths[parent_path(path_stack)]] = _null_vector);
            _paths[parent_path(path_stack)].push_back(start_path(sreal));			
            break;
        case T_STOPPATH:
            it = path_stack.rbegin();
            if(it==path_stack.rend() || (*it) != stop_path(sreal)) {
                throw conf_load_error(string("CFileConfig::Load: path not match: ")+stop_path(sreal));
            }
            path_stack.pop_back();
            break;
        case T_NULL:
            break;
        case T_PAIR: // 同时也作为domain
            if(path_stack.size() == 0) {
                throw conf_load_error("CFileConfig::Load: path is null");
            }
            decode_pair(sreal,name,value);

            _pairs[path(path_stack)][name] = value;
            _domains[path(path_stack)].push_back(sreal);
            break;
        case T_DOMAIN:
            if(path_stack.size() == 0) {
                throw conf_load_error("CFileConfig::Load: path is null");
            }
            _domains[path(path_stack)].push_back(sreal);
            break;
        default:
            throw conf_load_error(string("CFileConfig::Load: invalid:")+sreal);
        }		
    }
    //文件解析完要判断path_stack是否为空
    if(path_stack.size() != 0) {
        throw conf_load_error("CFileConfig::Load: path havenot over");
    }
}

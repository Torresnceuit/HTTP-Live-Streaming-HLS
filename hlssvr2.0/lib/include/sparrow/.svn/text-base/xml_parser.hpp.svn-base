#ifndef XML_PARSER_HPP_201208061710
#define XML_PARSER_HPP_201208061710

#include "tinyxml/tinyxml.h"
#include "str_algorithm.hpp"
#include <deque>
#include <string>
#include <sstream>

using std::string;
using std::stringstream;

namespace sparrow
{
	class xmlparser;

	class xml_error
	{
	public:
		xml_error(const string &err) : 
		errmsg_(err)
		{};
		virtual ~xml_error(void)
		{};

		const string message(void) const 
		{
			return errmsg_;
		};

	private:
		string errmsg_;
	};

	class xmltree
	{
	public:
		typedef std::deque<string>	path_type;

	public:
		xmltree(void) :
		isroot_(false),
		pxml_elem_(NULL)
		{
		};

		bool is_invalid(void) const 
		{
			if(NULL == pxml_elem_) return true;
			return false;
		};

		const char* name(void)
		{
			if(NULL == pxml_elem_) return NULL;
			return pxml_elem_->Value();
		};
	
		template<typename T>
		T get(const string &path)
		{
			if(NULL == pxml_elem_) return T();
			path_type	xpath;
			parse_path(xpath, path);
			return get<T>(xpath);
		};

		xmltree get_tree(const string &path)
		{
			if(NULL == pxml_elem_) return xmltree();
			path_type xpath;
			parse_path(xpath, path);

			if(isroot_ && xpath.size() > 0){
				string nodename = xpath.front();
				xpath.pop_front();
				if(nodename.compare(pxml_elem_->Value()) != 0){
					stringstream ss;
					ss << "xml path error. path:" << path << " value="  << pxml_elem_->Value();
					throw xml_error(ss.str());
				}
			}
			TiXmlElement *pelem = get_element(pxml_elem_, xpath);
			if(NULL == pelem)
			{
				stringstream ss;
				ss << "can't find child. path:" << path;
				throw xml_error(ss.str());
			}

			xmltree xmlt;
			xmlt.pxml_elem_ = pelem;
			return xmlt;
		}

		xmltree get_first_child(void)
		{
			if(NULL == pxml_elem_) return xmltree();
			xmltree xtree;
			xtree.pxml_elem_ =  pxml_elem_->FirstChildElement();
			return xtree;
		}
		xmltree get_first_child(const string &name)
		{
			if(NULL == pxml_elem_) return xmltree();
			xmltree xtree;
			xtree.pxml_elem_ =  pxml_elem_->FirstChildElement(name.c_str());
			return xtree;
		}

		xmltree next(void)
		{
			xmltree xtree;
			if(isroot_ && NULL == pxml_elem_) return xtree;
			TiXmlElement *pelem = pxml_elem_->NextSiblingElement();
			xtree.pxml_elem_ = pelem;
			return xtree;
		};

	private:
		template<typename T>
		T get(path_type &xpath)
		{
			using namespace str_algorithm;
			T ret = T();
			
			const char *pval = NULL;
			if(xpath.size() == 0)
			{
				pval = pxml_elem_->GetText();
			}else
			{
				string lastnode = xpath.back();
				string attrname;
				if(lastnode.find("@") == 0)
				{
					xpath.pop_back();
					attrname = lastnode.substr(1);
				}

				if(isroot_ && xpath.size() > 0)
				{
					string nodename = xpath.front();
					xpath.pop_front();
					if(nodename.compare(pxml_elem_->Value()) != 0){
						stringstream ss;
						ss << "xml path error.  value="  << pxml_elem_->Value();
						throw xml_error(ss.str());
					}
				}

				TiXmlElement *pelem = get_element(pxml_elem_, xpath);
				if(NULL == pelem){
					throw xml_error("xml path error");
				}
				if(attrname.length() > 0)
				{
					pval = pelem->Attribute(attrname.c_str());
				}else{
					pval = pelem->GetText();
				}
			}

			try{
				ret = lexical_cast<T>(string(pval));
			}catch(bad_lexical_cast&){
				stringstream ss;
				ss << "type convert error. data:" << string(pval);
				throw xml_error(ss.str());
			}

			return ret;
		};

		TiXmlElement* get_element(TiXmlElement *pelem, const path_type &xpath)
		{
			if(NULL == pelem) return NULL;
			TiXmlElement *pret = pelem;
			for(size_t i = 0; i < xpath.size(); ++ i)
			{
				const string &nodename = xpath.at(i);
				pret = pret->FirstChildElement(nodename.c_str());
				if(NULL == pret) break;
			}

			return pret;
		};

		void parse_path(path_type &xpath, const string &path)
		{
			using namespace str_algorithm;
			split(xpath, path, is_any_of("."));
		};

	private:
		bool			isroot_;
		TiXmlElement	*pxml_elem_;

		friend class xmlparser;
	};

	class xmlparser
	{
	public:
		
		void load_file(const string &fpath)
		{
			xml_doc_.Clear();
			if(!xml_doc_.LoadFile(fpath.c_str()))
			{
				stringstream ss;
				ss << "load xml file error. " << xml_doc_.ErrorDesc();
				throw xml_error(ss.str());
			}
			
		};

		void parse_xml(const string &xmlstr)
		{
			xml_doc_.Clear();
			xml_doc_.Parse(xmlstr.c_str());
			if(xml_doc_.Error()){
				stringstream ss;
				ss << "parse xml string error. " << xml_doc_.ErrorDesc();
				throw xml_error(ss.str());
			}
		};

		xmltree root(void){
			xmltree xtree;
			xtree.isroot_ = true;
			xtree.pxml_elem_ = xml_doc_.RootElement();
			return xtree;
		};

	private:
		TiXmlDocument	xml_doc_;
	};
}

#endif

#ifndef SSMAIN_HPP_201205091744
#define SSMAIN_HPP_201205091744

#include <string>
#include "sparrow/str_algorithm.hpp"

using namespace sparrow::str_algorithm;

using std::string;

template<typename OP>
void show_info(void *mem, size_t size, OP &op){
	typedef typename OP::d_type T;
	size_t count = size / sizeof(T);
	T *p = NULL;
	for(size_t i = 0; i < count; ++ i){
		p = (T*)((char*)mem + sizeof(T) * i);
		if(!op(p)) break;
	}
};

template<typename tar_t>
tar_t hex_strtonum(const string &str){
	tar_t ret = 0;
	size_t ts = sizeof(tar_t);
	string tmp = str;
	to_upper(tmp);
	string::size_type start = tmp.find("0X");
	if(string::npos != start){
		tmp = tmp.substr(2);
	}
	size_t s_len = tmp.length();
	if(s_len % 2 != 0) {
		tmp = "0" + tmp;
		++ s_len;
	}
	ts = ts < (s_len / 2) ? ts : (s_len / 2);
	for(size_t i = 0; i < ts; ++ i){
		if(s_len == 0) break;
		char h = tmp[i * 2];
		char l = tmp[i * 2 + 1];
		for(int j = 0; j < 2; ++ j){
			char *p = NULL;
			if(0 == j) p = &l;
			if(1 == j) p = &h;
			if(*p >= '0' && *p <= '9'){
				*p = *p - '0';
			}else if(*p >= 'A' && *p <= 'F'){
				*p = *p - 'A' + 10;
			}else{
				*p = 0;
			}
		}
		unsigned char b8 = (h << 4) | l;
		ret |= (((tar_t)b8) << ((ts - 1 - i) * 8));
	}
	return ret;
};

void* shm_open(key_t shmk, int &shmid, size_t &shmsize);


#endif


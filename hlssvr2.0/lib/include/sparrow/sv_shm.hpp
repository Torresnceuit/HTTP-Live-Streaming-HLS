#ifndef SV_SHM_HPP_201203071709
#define SV_SHM_HPP_201203071709

#include <string>
#include <sys/ipc.h>

using std::string;

namespace sparrow
{
	class sv_shm
	{
	public:
		sv_shm(void) : 
	    mem_(NULL), memsize_(0),
		shm_key_(0), shm_id_(0)
		{};


		int create(const string &name, std::size_t len);

		int open(const string &name, const bool readonly = false);

		int open(const key_t shmkey, const bool readonly = false);

		int open_or_create(const string &name, std::size_t len);

		const string& shm_name(void) const{return name_;};
		const key_t& shm_key(void) const {return shm_key_;};

		void* mem_ptr(void);
		std::size_t mem_size(void) const;

		void close(void);

		const string& errmsg(void) const {return errmsg_;}

		static key_t name_to_key(const string &name, string &err);
		static int remove(const string &name);

	private:
		string			name_;
		void			*mem_;
		std::size_t		memsize_;
		key_t			shm_key_;
		int				shm_id_;
		string			errmsg_;
	};
}




#endif

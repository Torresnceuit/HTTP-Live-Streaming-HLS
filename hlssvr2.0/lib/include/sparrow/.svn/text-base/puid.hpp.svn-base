#ifndef PUID_HPP_201112071718
#define PUID_HPP_201112071718

#include "singleton.hpp"
#include "thread_synch.hpp"
#include <stdint.h>

//进程内唯一id生成器
namespace sparrow{

	template<typename mutex_type>
	class puid
	{
	public:
		puid(void) : id_(0){};

		uint64_t get_id64(void){
			typename mutex_type::scoped_lock guard(lock_);
			return id_++;
		};
		uint32_t get_id32(void){
			typename mutex_type::scoped_lock guard(lock_);
			return (uint32_t)((id_++) & 0xffffffff);
		};
		
	private:
		uint64_t	id_;
		mutex_type	lock_;
	};
	typedef puid<null_mutex>	sth_puid;
	typedef puid<spin_mutex>	mth_puid;

	typedef singleton<sth_puid>	ins_sthpuid;
	typedef singleton<mth_puid>	ins_mthpuid;
}

#define pins_sthpuid sparrow::ins_sthpuid::instance()
#define pins_mthpuid sparrow::ins_mthpuid::instance()

#endif



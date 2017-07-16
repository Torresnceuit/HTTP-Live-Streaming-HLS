#include "tfc_net_conn_map.hpp"
#include <sys/time.h>
#include <time.h>
 void xprintf(const char *szFormat, ...){};

#ifdef _DEBUG_
#define SAY printf
#else
#define SAY xprintf
#endif

ConnMap::ConnMap(int maxconn)
{
	_max_conn = maxconn;
	if (_max_conn <= 0)
	{
		_max_conn = 10000;
	}
}
ConnMap::~ConnMap()
{}

int ConnMap::add(unsigned queue_flow, stconn* pstconn)
{
	std::map<unsigned, stconn>::iterator it = _flow2stconn.find(queue_flow);

	int ret = 0;
	if (it == _flow2stconn.end())
	{	
		_flow2stconn[queue_flow] = *pstconn;
		ret =  0;
	}
	else
	{
		ret =  -1;
	}
	SAY("ConnMap::add(queue_flow=%d) return %d\n",queue_flow,ret);
	return ret;
}

stconn* ConnMap::get_conn_frm_flow(unsigned queue_flow)
{
	std::map<unsigned, stconn>::iterator it = _flow2stconn.find(queue_flow);
	if (it == _flow2stconn.end())
	{
		return NULL;
	}
	else
	{
		return &(it->second);
	}
}	

int ConnMap::delconn(unsigned queue_flow)
{
	int ret = 0;
	std::map<unsigned, stconn>::iterator it = _flow2stconn.find(queue_flow);
	if (it == _flow2stconn.end())
	{
		ret = -1;
	}
	
	_flow2stconn.erase(it);
	SAY("ConnMap::delconn(queue_flow=%d) return %d\n",queue_flow,ret);
	return ret;
}	

int ConnMap::GetExpire(std::vector<unsigned> &vec_key)
{
	time_t tNow = time(0);
	static time_t tLastCheck = time(0);

	if (tNow - tLastCheck < 180)
	{
		return 0;
	}
	tLastCheck = tNow;

	if (_flow2stconn.size() < (unsigned)_max_conn)
	{
		return 0;
	}

	for (std::map<unsigned, stconn>::iterator it  = _flow2stconn.begin();
		it != _flow2stconn.end(); it++)
	{
		if (tNow - it->second.lastavtive >= 180)
		{
			SAY("ConnMap::GetExpire push flow(%d)\n",it->first);
			vec_key.push_back(it->first);
		}
	}
	return 1;
}

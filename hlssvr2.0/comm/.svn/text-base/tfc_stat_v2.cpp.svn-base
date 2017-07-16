
#include <sys/time.h>
#include "tfc_stat_v2.h"

using namespace tfc::stat;

CStat::CStat()
{
	_stat_gap = 60;
	_delay_up = 100;
	last_print_time = time(NULL);
	v_head_items.clear();
	v_body_row_items.clear();
	v_body_col_items.clear();
	v_tail_items.clear();
	Reset();
}

//初始化
int CStat::Open(TLogPara &stLogPara, unsigned stat_gap)
{
	_stat_gap = stat_gap;
	return m_StatFile.open(stLogPara.log_level_, stLogPara.log_type_, 
		stLogPara.path_, stLogPara.name_prefix_, 
		stLogPara.max_file_size_, stLogPara.max_file_no_);	
}

//设置日志级别
void CStat::Set_Level(int level)
{
	m_StatFile.log_level(level);

}

//设置打印间隔时间
void CStat::Set_Print_GapTime(unsigned gap_time)
{
	_stat_gap = gap_time;
}

//设置延时阈值
void CStat::Set_Delay_Up(unsigned delay_up)
{
	_delay_up = delay_up;
}

//重置所有统计值
void CStat::ClearAll()
{
	_count_map.clear();
	_errno_count_map.clear();
	_begin_time_map.clear();
}

//重置统计值
void CStat::Reset()
{
	_count_map.clear();
	_errno_count_map.clear();
}

//加头记录名
void CStat::Add_Report_Head_Item(string item_name)	
{
	v_head_items.push_back(item_name);
}

//加身体行记录名
void CStat::Add_Report_Body_Row_Item(string item_name)	
{
	v_body_row_items.push_back(item_name);
}

//加身体列记录名
void CStat::Add_Report_Body_Col_Item(string item_name)	
{
	v_body_col_items.push_back(item_name);
}

//加单独统计的记录名
void CStat::Add_Report_Tail_Item(string item_name)
{
	v_tail_items.push_back(item_name);
}

//设置统计值
void CStat::Set_Stat(string item_name, unsigned val)
{
	map<string , unsigned>::iterator it;

	if((it = _count_map.find(item_name)) == _count_map.end())
	{
		_count_map.insert(pair<string , unsigned>(item_name, 0));
	}

	it = _count_map.find(item_name);
	it->second = val;
}

//增加统计
void CStat::Inc_Stat(string item_name, unsigned val)
{
	map<string , unsigned>::iterator it;

	if((it = _count_map.find(item_name)) == _count_map.end())
	{
		_count_map.insert(pair<string , unsigned>(item_name, 0));
	}

	it = _count_map.find(item_name);
	it->second += val;
}

//增加身体部分统计
void CStat::Inc_Stat(	string row_name,
						string col_name,
						unsigned val)
{
	string item_name = row_name + col_name;

	map<string , unsigned>::iterator it;

	if((it = _count_map.find(item_name)) == _count_map.end())
	{
		_count_map.insert(pair<string , unsigned>(item_name, 0));
	}

	it = _count_map.find(item_name);
	it->second += val;
}

//错误码统计
void CStat::Inc_Errno_Stat(int error_no, unsigned val)
{
	map<int , unsigned>::iterator it;

	if((it = _errno_count_map.find(error_no)) == _errno_count_map.end())
	{
		_errno_count_map.insert(pair<int , unsigned>(error_no, 0));
	}

	it = _errno_count_map.find(error_no);
	it->second += val;
}

//获取统计值, 如果不存在, 返回 -1
int CStat::Get_Stat(string item_name)
{
	map<string ,unsigned>::iterator it;
	if((it = _count_map.find(item_name)) == _count_map.end())
	{
		return -1;
	}
	else
	{
		return it->second;
	}
}

//获取统计值, 如果不存在, 返回 0
unsigned CStat::Get_Stat_Value(string item_name)
{
	map<string ,unsigned>::iterator it;
	if((it = _count_map.find(item_name)) == _count_map.end())
	{
		return 0;
	}
	else
	{
		return it->second;
	}
}

//获取body 的统计值
unsigned CStat::Get_Stat_Value(string row_name, string col_name)
{
	string item_name = row_name + col_name;
	map<string ,unsigned>::iterator it;
	if((it = _count_map.find(item_name)) == _count_map.end())
	{
		return 0;
	}
	else
	{
		return it->second;
	}
}

//对一类动作进行计时统计，Print 将得到平均值
int CStat::TimeStat_Begin(unsigned seq)				//seq 号要唯一
{
	map<unsigned, struct timeval>::iterator it;
	if((it = _begin_time_map.find(seq)) == _begin_time_map.end())
	{
		struct timeval t;
		gettimeofday(&t, NULL);
		_begin_time_map.insert(pair<unsigned, struct timeval>(seq, t));
		return 0;
	}
	else
	{
		return -1;
	}
}

//动作结束, 放回时间ms
int CStat::TimeStat_End(string row_name, unsigned seq)	//seq 号要唯一
{
	int ret = TimeStat_End2(row_name, seq);
	if(ret < 0)
	{
		return ret;
	}
	else
	{
		return ret / 1000;
	}
}

//动作结束, 放回时间us
int CStat::TimeStat_End2(string row_name, unsigned seq)	//seq 号要唯一
{
	map<unsigned, struct timeval>::iterator it;
	if((it = _begin_time_map.find(seq)) == _begin_time_map.end())
	{
		return -1;
	}

	struct timeval start = (struct timeval)it->second;

	struct timeval t;
	gettimeofday(&t, NULL);
	unsigned spend_us = 1000000 * (t.tv_sec - start.tv_sec) + (t.tv_usec - start.tv_usec);	//6位有效数字，微秒
	unsigned spend = spend_us / 1000;	//3位有效数字，毫秒
	
	//统计延时总和， 最大 40亿微秒
	Inc_Stat(row_name + STAT_DELAY_TOTAL_TIME, spend);

	unsigned max = Get_Stat_Value(row_name + STAT_DELAY_MAX);

	if(spend > max)
	{
		Set_Stat(row_name + STAT_DELAY_MAX, spend);
	}

	//统计大于500 毫秒延时的数量
	//if(spend > _delay_up * 1000)
	if(spend > _delay_up)
	{
		Inc_Stat(row_name + STAT_DELAY_UP);
	}

	Inc_Stat(row_name + STAT_DELAY_END_COUNT);	//统计回复

	//删除序列号 map
	_begin_time_map.erase(it);
	return (int)spend_us;
}

//获取一类动作的平均延时(ms), 最大延时(ms), 超过阈值的延时数量
int CStat::TimeStat_Get(	string row_name,
						unsigned &tcount,
						unsigned &avg_de,
						unsigned &max_de,
						unsigned &up_de)
{
	tcount = 0;
	avg_de = 0;
	max_de = 0;
	up_de = 0;

	map<string, unsigned>::iterator it;
	if((it = _count_map.find(row_name + STAT_DELAY_END_COUNT)) == _count_map.end())
	{
		return -1;
	}

	tcount = it->second;

	it = _count_map.find(row_name + STAT_DELAY_TOTAL_TIME);
	if(it != _count_map.end())
	{
		avg_de = it->second / tcount;
	}

	it = _count_map.find(row_name + STAT_DELAY_MAX);
	if(it != _count_map.end())
	{
		max_de = it->second;
	}
	
	it = _count_map.find(row_name + STAT_DELAY_UP);
	if(it != _count_map.end())
	{
		up_de = it->second;
	}

	return 0;
}

//增加一个时间统计值，delexxie 2009-03-30, ms
void CStat::TimeStat_Inc(string row_name, unsigned ms_val)
{
	unsigned spend = ms_val;

	//统计延时总和， 最大 40亿微秒
	Inc_Stat(row_name + STAT_DELAY_TOTAL_TIME, spend);

	unsigned max = Get_Stat_Value(row_name + STAT_DELAY_MAX);

	if(spend > max)
	{
		Set_Stat(row_name + STAT_DELAY_MAX, spend);
	}

	//统计大于500 毫秒延时的数量
	//if(spend > _delay_up * 1000)
	if(spend > _delay_up)
	{
		Inc_Stat(row_name + STAT_DELAY_UP);
	}

	Inc_Stat(row_name + STAT_DELAY_END_COUNT);	//统计回复
}

//取消未完成的时间统计，delexxie 2009-03-30
void CStat::TimeStat_Cancel(unsigned seq)
{
	map<unsigned, struct timeval>::iterator it;
	if((it = _begin_time_map.find(seq)) == _begin_time_map.end())
	{
		return;
	}

	//删除序列号 map
	_begin_time_map.erase(it);		
}


//清除超时的 时间统计
void CStat::Clear_Time_Out(time_t now)
{
	if(now == 0)
	{
		now = time(NULL);
	}

	struct timeval _start_time;
	map<unsigned,  struct timeval>::iterator it = _begin_time_map.begin();
	while (it != _begin_time_map.end())
	{
		_start_time = it->second;
		if (( now - _start_time.tv_sec) > 120) //120 秒钟超时
		{
			_begin_time_map.erase(it++);
		}
		else
		{
			it++;
		}
	}	
}


//=================================================================================
//打印头部
void CStat::Print_Header()
{
	unsigned i;
	unsigned size = 0; unsigned value = 0;
	string name;
    char line[1024]  = {0};
    char line1[1024] = {0};
    char line2[1024] = {0};
	unsigned len, len1, len2;

	time_t now = time(NULL);
	unsigned time_gap = now - last_print_time;
	if(time_gap == 0)
	{
		return;
	}

	//打印头标题
	m_StatFile.log_p_no_time(LOG_NORMAL, "Statistic in %ds,  CTime: %s", time_gap, ctime(&now));
    m_StatFile.log_p_no_time(LOG_NORMAL, "-------------------------------------------------------------------\n");
	
	len = snprintf(line, 1023, "%23s|", "");
	len1 = snprintf(line1, 1023, "%-23s|", "total:");
	len2 = snprintf(line2, 1023, "%-23s|", "count /1s:");
	size = v_head_items.size();

	for(i=0; i<size; i++)
	{
		name = v_head_items[i];
		value = Get_Stat_Value(name);

		len += snprintf(line + len,  1023 - len, "%8s |", name.c_str());
		len1 += snprintf(line1 + len1, 1023 - len, "%8u |", value);
		len2 += snprintf(line2 + len2, 1023 -len, "%8u |", value  / time_gap);
	}
	
	m_StatFile.log_p_no_time(LOG_NORMAL, "%s\n", line);
	m_StatFile.log_p_no_time(LOG_NORMAL, "%s\n", line1);
	m_StatFile.log_p_no_time(LOG_NORMAL, "%s\n", line2);
}

//打印身体
void CStat::Print_Body()
{
	unsigned i, j;
	unsigned row_size = 0; unsigned col_size = 0;
	unsigned value, line_len, row_total_count;
	string name, col_name;
    char line[1024] = {0};
	unsigned tcount, avg, max, up;

	//每个命令的统计
	row_size = v_body_row_items.size();
	col_size = v_body_col_items.size();

	if(row_size == 0)
	{
		return;
	}

	//body表头
	m_StatFile.log_p_no_time(LOG_NORMAL, "------------------------------------------------------------------------------------\n");	
	
	line[0] = 0;
	line_len = snprintf(line, 1023, "%-23s", "");
	for(j=0; j<col_size; j++)
	{
		line_len += snprintf(line + line_len, 1023 - line_len, "|%8s ", v_body_col_items[j].c_str());
	}
	
	line_len += snprintf(line + line_len, 1023 - line_len, "|%8s |%8s |%8s | >%0.3fs |\n", 
										"tcount",
										"avg_de_s",
										"de_max_s",
										_delay_up/1000.0);
	
	m_StatFile.log_p_no_time(LOG_NORMAL, "%s", line);

	//body 单项
	for(i=0; i<row_size; i++)
	{
		row_total_count = 0;	//一行里，所有的自定义值相加
		name = v_body_row_items[i];
		line_len = snprintf(line,  1023, "%-23s", ((string)name + ":").c_str());

		for(j=0; j<col_size; j++)
		{
			col_name = v_body_col_items[j];
			value = Get_Stat_Value(name, col_name);
			row_total_count += value;
			line_len += snprintf(line + line_len, 1023 - line_len, "|%8d ", value);
		}

		TimeStat_Get(name, tcount, avg, max, up);
		if(row_total_count == 0 && tcount == 0)
		{
			continue;
		}

		line_len += snprintf(line + line_len, 1023 - line_len, "|%8u |%8.3f |%8.3f |%8d |\n", 
									tcount,
									avg/ 1000.0,
									max/ 1000.0,
									up);

		m_StatFile.log_p_no_time(LOG_NORMAL, "%s", line);
	}
}

//打印单项的
void CStat::Print_Tail()
{
	unsigned i = 0;
	unsigned size = 0;
	string name;

	//tail 的统计
	size = v_tail_items.size();
	if(size > 0)
	{
		m_StatFile.log_p_no_time(LOG_NORMAL, "-------------------------------------------------------------------------------\n");
		for(i=0; i<size; i++)
		{
			name = v_tail_items[i].c_str();
			m_StatFile.log_p_no_time(LOG_NORMAL, "%-23s|%8d\n", 
												((string)name + ":").c_str(),
												Get_Stat_Value(name));
		}
	}
}

//用户自定义的
void CStat::Print_Other()
{
}

//打印错误码
void CStat::Print_Error()
{
	if(_errno_count_map.size() > 0)
	{
		m_StatFile.log_p_no_time(LOG_NORMAL, "----------------------------------\n");
		//错误码的统计
		map<int, unsigned>::iterator it = _errno_count_map.begin();
		while (it != _errno_count_map.end())
		{
			m_StatFile.log_p_no_time(LOG_NORMAL, "ret: %-18d|%8u |\n", 
											(int)it->first,
											(unsigned)it->second);
			it++;
		}
	}
}

//打印
void CStat::Print()
{
	Print_Header();
	Print_Body();
	Print_Tail();
	Print_Other();
	Print_Error();

	m_StatFile.log_p_no_time(LOG_NORMAL,"\n");
	m_StatFile.log_p_no_time(LOG_NORMAL,"\n");
	return;
}


//检查和打印统计信息
void CStat::Check_And_Print(time_t now)
{
	if(now == 0)
	{
		now = time(NULL);
	}

	if( (unsigned)(now - last_print_time) >=  _stat_gap)
	{
		Print();
		Reset();
		Clear_Time_Out(now);
		last_print_time = now;
	}
}

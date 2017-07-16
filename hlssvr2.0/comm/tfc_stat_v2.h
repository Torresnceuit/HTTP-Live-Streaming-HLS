/* 
	delexxie
	Tencent.Com
	2008-4-21

说明：
	Head 的项用户需自己定义
	Body 的项用来延时统计, 已经由 计数，end_count, avg_de_s, de_max_s, de_up
	Tail 的项主要是单项计数统计
	Error 的项是统计错误码及个数
	Print_Other() 由用户自定义输出

报表例子:

Statistic in 60s,  CTime: Sat Apr 19 20:25:03 2008
------------------------------------------------------
			 |    recv |   reply | timeout |    fail |
total:       |   57489 |   57392 |       0 |       2 |
count /1s:   |     958 |     956 |       0 |       0 |

------------------------------------------------------------------------------
                             |   calls |  failed |avg_de_s |de_max_s | deup 1s |
UpdateFileByMinidx:          |    5374 |       0 |   0.157 |   1.876 |      21 |
DownloadFileByMinidx:        |   52115 |       2 |   0.081 |   1.919 |      25 |

------------------------------------------------------------------------------
mq_mcd_2_ccd(bytes)			 |     230
mq_ccd_2_mcd(bytes)			 |     240

ret: -11005                  |       1
ret: -11006                  |       3
*/

#ifndef _TFC_STAT_V2_H_
#define _TFC_STAT_V2_H_

#include "tfc_object.h"
#include "sys/time.h"
#include <string>
#include <map>
#include <vector>
using namespace std;

#define STAT_DELAY_END_COUNT	"tcount"		//记录Time_End 调用次数
#define	STAT_DELAY_TOTAL_TIME	"de_total_ms"	//总计的微秒数
#define	STAT_DELAY_MAX			"de_max_s"
#define	STAT_DELAY_UP			"de_up"


namespace tfc{ namespace stat{

class CStat
{
public:	
	CStat();
	virtual ~CStat(){}

	//初始化=================================================================
	int		Open(TLogPara &stLogPara, unsigned stat_gap);	//初始化
	void	Set_Level(int level);					//设置日志级别
	void	Set_Print_GapTime(unsigned gap_time);	//设置打印时间间隔
	void	Set_Delay_Up(unsigned delay_up);		//设置最大延时阈值， 毫秒


	//初始化报表项=================================================================
	//BODY 已有延时类的统计，其他统计需要添加
	void Add_Report_Head_Item(string item_name);		//加头记录名
	void Add_Report_Body_Row_Item(string item_name);	//加身体行记录名
	void Add_Report_Body_Col_Item(string item_name);	//加身体列记录名
	void Add_Report_Tail_Item(string item_name);		//加单独统计的记录名

	
	//=================================================================
	//统计数量
	void Set_Stat(string item_name, unsigned val);		//设置统计值
	void Inc_Stat(	string item_name, unsigned val = 1);
	void Inc_Stat(	string row_name, string col_name, unsigned val = 1);
	void Inc_Errno_Stat(int error_no, unsigned val = 1);//错误码统计

	
	//获取统计数量值
	int		 Get_Stat(string item_name);			//获取统计值, 如果不存在, 返回 -1
	unsigned Get_Stat_Value(string item_name);		//获取统计值, 如果不存在, 返回 0
	unsigned Get_Stat_Value(string row_name, string col_name);


	//=================================================================
	//对一类动作进行计时统计, 单位 ms
	int TimeStat_Begin(unsigned seq);					//seq 号要唯一， -1 已存在
	int TimeStat_End(string row_name, unsigned seq);	//seq 号要唯一，返回时间ms，-1 不存在
	int TimeStat_End2(string row_name, unsigned seq);	//seq 号要唯一，返回时间us，-1 不存在
	int	TimeStat_Get(	string row_name,	//行名称
						unsigned &count,	//数量
						unsigned &avg_de,	//平均延时(ms)
						unsigned &max_de,	//最大延时(ms)
						unsigned &up_de);	//超过阈值的延时数量
	
	void TimeStat_Inc(string row_name, unsigned ms_val);	//增加一个时间统计值，delexxie 2009-03-30
	void TimeStat_Cancel(unsigned seq);			//取消未完成的时间统计，delexxie 2009-03-30
	

	//打印=================================================================
	virtual void Print();				//用默认的打印方式

	//分项打印
	virtual void Print_Header();		//打印头部
	virtual void Print_Body();			//打印身体
	virtual void Print_Tail();			//打印单项的
	virtual void Print_Other();			//自定义的打印
	virtual void Print_Error();			//打印错误码
	
	virtual void Check_And_Print(time_t now = 0);		//检查如果到打印时间则打印
	virtual void ClearAll();			//清空所有的统计
	virtual void Reset();				//重置 _count_map， _errno_count_map， 不重重置_begin_time_map
	

public:
	vector<string>		v_head_items;		//头标题名
	vector<string>		v_body_row_items;	//行名
	vector<string>		v_body_col_items;	//列名
	vector<string>		v_tail_items;		//单项名

	TFCDebugLog			m_StatFile;
	time_t				last_print_time;	// 最近的一次打印时间



protected:
	void	Clear_Time_Out(time_t now = 0);

protected:
	map<string, unsigned>	_count_map;				//计数池
	map<unsigned, struct timeval> _begin_time_map;	//时间统计池
	map<int, unsigned>		_errno_count_map;		//错误码计数池
	
	unsigned _delay_up;				//时间阈值, 如果超时它, 该项统计超过值计数加1
	unsigned _stat_gap;				//统计间隔时间秒, 触发Print
};

}}

#endif


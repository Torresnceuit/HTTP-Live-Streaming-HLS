/* 
	delexxie
	Tencent.Com
	2008-4-21

˵����
	Head �����û����Լ�����
	Body ����������ʱͳ��, �Ѿ��� ������end_count, avg_de_s, de_max_s, de_up
	Tail ������Ҫ�ǵ������ͳ��
	Error ������ͳ�ƴ����뼰����
	Print_Other() ���û��Զ������

��������:

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

#define STAT_DELAY_END_COUNT	"tcount"		//��¼Time_End ���ô���
#define	STAT_DELAY_TOTAL_TIME	"de_total_ms"	//�ܼƵ�΢����
#define	STAT_DELAY_MAX			"de_max_s"
#define	STAT_DELAY_UP			"de_up"


namespace tfc{ namespace stat{

class CStat
{
public:	
	CStat();
	virtual ~CStat(){}

	//��ʼ��=================================================================
	int		Open(TLogPara &stLogPara, unsigned stat_gap);	//��ʼ��
	void	Set_Level(int level);					//������־����
	void	Set_Print_GapTime(unsigned gap_time);	//���ô�ӡʱ����
	void	Set_Delay_Up(unsigned delay_up);		//���������ʱ��ֵ�� ����


	//��ʼ��������=================================================================
	//BODY ������ʱ���ͳ�ƣ�����ͳ����Ҫ���
	void Add_Report_Head_Item(string item_name);		//��ͷ��¼��
	void Add_Report_Body_Row_Item(string item_name);	//�������м�¼��
	void Add_Report_Body_Col_Item(string item_name);	//�������м�¼��
	void Add_Report_Tail_Item(string item_name);		//�ӵ���ͳ�Ƶļ�¼��

	
	//=================================================================
	//ͳ������
	void Set_Stat(string item_name, unsigned val);		//����ͳ��ֵ
	void Inc_Stat(	string item_name, unsigned val = 1);
	void Inc_Stat(	string row_name, string col_name, unsigned val = 1);
	void Inc_Errno_Stat(int error_no, unsigned val = 1);//������ͳ��

	
	//��ȡͳ������ֵ
	int		 Get_Stat(string item_name);			//��ȡͳ��ֵ, ���������, ���� -1
	unsigned Get_Stat_Value(string item_name);		//��ȡͳ��ֵ, ���������, ���� 0
	unsigned Get_Stat_Value(string row_name, string col_name);


	//=================================================================
	//��һ�ද�����м�ʱͳ��, ��λ ms
	int TimeStat_Begin(unsigned seq);					//seq ��ҪΨһ�� -1 �Ѵ���
	int TimeStat_End(string row_name, unsigned seq);	//seq ��ҪΨһ������ʱ��ms��-1 ������
	int TimeStat_End2(string row_name, unsigned seq);	//seq ��ҪΨһ������ʱ��us��-1 ������
	int	TimeStat_Get(	string row_name,	//������
						unsigned &count,	//����
						unsigned &avg_de,	//ƽ����ʱ(ms)
						unsigned &max_de,	//�����ʱ(ms)
						unsigned &up_de);	//������ֵ����ʱ����
	
	void TimeStat_Inc(string row_name, unsigned ms_val);	//����һ��ʱ��ͳ��ֵ��delexxie 2009-03-30
	void TimeStat_Cancel(unsigned seq);			//ȡ��δ��ɵ�ʱ��ͳ�ƣ�delexxie 2009-03-30
	

	//��ӡ=================================================================
	virtual void Print();				//��Ĭ�ϵĴ�ӡ��ʽ

	//�����ӡ
	virtual void Print_Header();		//��ӡͷ��
	virtual void Print_Body();			//��ӡ����
	virtual void Print_Tail();			//��ӡ�����
	virtual void Print_Other();			//�Զ���Ĵ�ӡ
	virtual void Print_Error();			//��ӡ������
	
	virtual void Check_And_Print(time_t now = 0);		//����������ӡʱ�����ӡ
	virtual void ClearAll();			//������е�ͳ��
	virtual void Reset();				//���� _count_map�� _errno_count_map�� ��������_begin_time_map
	

public:
	vector<string>		v_head_items;		//ͷ������
	vector<string>		v_body_row_items;	//����
	vector<string>		v_body_col_items;	//����
	vector<string>		v_tail_items;		//������

	TFCDebugLog			m_StatFile;
	time_t				last_print_time;	// �����һ�δ�ӡʱ��



protected:
	void	Clear_Time_Out(time_t now = 0);

protected:
	map<string, unsigned>	_count_map;				//������
	map<unsigned, struct timeval> _begin_time_map;	//ʱ��ͳ�Ƴ�
	map<int, unsigned>		_errno_count_map;		//�����������
	
	unsigned _delay_up;				//ʱ����ֵ, �����ʱ��, ����ͳ�Ƴ���ֵ������1
	unsigned _stat_gap;				//ͳ�Ƽ��ʱ����, ����Print
};

}}

#endif


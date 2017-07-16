#include "ssmain.hpp"

#include "muxsvr.h"
#include "muxsvr_mcd_cfg.h"
#include "voice_channel.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>

#include "sparrow/cmd_line.hpp"
#include "sparrow/byte_buffer.hpp"
#include "sparrow/inet_addr.hpp"
#include "sparrow/date_time.hpp"

#include <iostream>
#include <sstream>
#include <deque>
#include <errno.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using std::cout;
using std::endl;
using std::deque;
using std::stringstream;

using sparrow::buffers::wrapper_buffer;
using sparrow::inet_address;
using sparrow::cmd_line;
using sparrow::date_time;

using namespace muxsvr;

///////////////////////////////////////////////////////////

////////////////////////////////////////
void* shm_open(key_t shmk, int &shmid, size_t &shmsize)
{

    shmid	= shmget(shmk, 0, 0);
    if(0 > shmid){
        cout << "shmget error: " << errno << endl;
        return NULL;
    }

    void *mem = shmat(shmid, NULL, SHM_RDONLY);
    if(~(long)mem == 0) {
        cout << "shmat error: " << errno << endl;
        return NULL;
    }

    shmid_ds shm_stat;

    if(shmctl(shmid, IPC_STAT, &shm_stat) != 0) {
        cout << "shmctl error: " << errno << endl;
        return NULL;
    }
    shmsize = (size_t)shm_stat.shm_segsz;
    return mem;
}

string t2s(const time_t t,const string& format="%Y-%m-%d %H:%M:%S")
{
    struct tm curr;
    curr = *localtime(&t);
    char sTmp[1024];
    strftime(sTmp,sizeof(sTmp),format.c_str(),&curr);
    return string(sTmp);
}

namespace muxsvr
{
	class dis_voicechannel
	{
	public:
		typedef SVoiceChannel d_type;
	public:
		dis_voicechannel(uint32_t uin = 0, uint32_t roomid = 0) : total(0), m_uiUin(uin), m_uiRoomId(roomid) {};

		bool operator()(SVoiceChannel *p)
		{
			bool bFlag = false;

			if (0 == m_uiUin)
			{
				if (0 == m_uiRoomId)
				{
					if ( p->getUin()!=0)
					{
						bFlag = true;
					}
				}
				else
				{
					if ( p->getRoomId()==m_uiRoomId)
					{
						bFlag = true;
					}
				}
			}
			else
			{
				if (0 == m_uiRoomId)
				{
					if ( p->getUin()==m_uiUin)
					{
						bFlag = true;
					}
				}
				else
				{
					if ( p->getUin()==m_uiUin && p->getRoomId()==m_uiRoomId)
					{
						bFlag = true;
					}
				}
			}


			if(true == bFlag)
			{
				cout << "--------------------------------\n"
					<< p->to_string("") << "\n"
					;
				++total;
			}
			return true;
		};

		uint32_t total;
		uint32_t m_uiUin;
		uint32_t m_uiRoomId;
	};

	class dis_video
	{
	public:
		typedef SVideoIdInfo d_type;
	public:
		dis_video(uint32_t id) : videoid(id), total(0){};
		bool operator()(SVideoIdInfo *p){
			if(p->uiVideoId == 0) return true;
			if(0 == videoid || p->uiVideoId == videoid)
			{
				cout << "-------------------------" << endl;
				cout << "uiVideoId:    \t" << p->uiVideoId << endl;
				cout << "uiChannelId:  \t" << p->uiChannelId << endl;
				cout << "uiUin:        \t" << p->uiUin << endl;
				cout << "uiRoomId:     \t" << p->uiRoomId << endl;
				cout << "uiMainRoomId: \t" << p->uiMainRoomId << endl;
				cout << "ucRoomType:   \t" << (unsigned int)p->ucRoomType << endl;
				cout << "uiTouchTime:  \t" << t2s(p->uiTouchTime).c_str() << endl;

				unsigned int uiFlowNum = 0;
				for (unsigned int i=0; i<MAX_CDN_FLOW_NUM; i++)
				{
					if (p->oCdnFlowList[i].uiFlow != 0)
					{
						struct in_addr addr;
						addr.s_addr = p->oCdnFlowList[i].uiIp;
						const char* ip_str = inet_ntoa(addr);

						cout << "{" << endl;
						cout << "\t" <<"index:     \t" << i << endl;
						cout << "\t" <<"uiFlow:    \t" << p->oCdnFlowList[i].uiFlow << endl;
						cout << "\t" <<"ip:        \t" << ip_str << endl;
						cout << "\t" <<"usPort:    \t" << ntohs(p->oCdnFlowList[i].usPort) << endl; 
						cout << "\t" <<"uiSrcType: \t" << p->oCdnFlowList[i].uiSrcType << endl;
						cout << "\t" <<"sendHead:  \t" << (int)p->oCdnFlowList[i].unionFlag.ucIsHaveMediaInfo << endl;
						cout << "\t" <<"uiSendTime:\t " << t2s(p->oCdnFlowList[i].uiSendTime).c_str() << endl;
						cout << "}" << endl;

						uiFlowNum++;
					}
				}

				cout << "uiFlowNum: " << uiFlowNum << endl;
				cout << endl << endl;

				++total;
			}
			return true;
		};

		uint32_t videoid;
		uint32_t total;
	};

	class dis_room
	{
	public:
		typedef SRoom2VideoId d_type;
	public:
		dis_room(uint32_t id)
			:uiRoomId(id), total(0){};

		bool operator()(SRoom2VideoId *p)
		{
			if(p->uiRoomId == 0) return true;
			if(0 == uiRoomId || p->uiRoomId == uiRoomId)
			{
				cout << "-------------------------" << endl;
				cout << "uiRoomId: " << p->uiRoomId << endl;
				cout << "uiVideoUin: " << p->uiVideoUin << endl;
				cout << "uiVideoId: " << p->uiVideoId << endl;
				cout << "uiTouchTime: " <<t2s(p->uiTouchTime) << endl;
				cout << endl;

				++total;
			}
			return true;
		};

		uint32_t uiRoomId;
		uint32_t total;
	};

	class dis_cdnflow2videoid
	{
	public:
		typedef SFlow2VideoIdInfo d_type;
	public:
		dis_cdnflow2videoid(uint32_t id, uint32_t srcid) : flowid(id), srcid(srcid), total(0){};
		bool operator()(SFlow2VideoIdInfo *p){
			if(p->uiFlow == 0) return true;
			if(0 == flowid || p->uiFlow == flowid)
			{
				cout << "-------------------------" << endl;
				cout << "uiSrc: " << srcid << endl;
				cout << "uiFlow: " << p->uiFlow << endl;
				cout << "uiVideoId: " << p->uiVideoId << endl;

				cout << endl;

				++total;
			}
			return true;
		};

		uint32_t flowid;
		uint32_t srcid;
		uint32_t total;
	};

	class CMonitor
	{
	public:
		void show_voicechannel(cmd_line &cmdl)
		{
			key_t		shmk = 0x20131005;
			uint32_t    uin = 0;
			uint32_t    roomid = 0;

			if(cmdl.has_opt("shmk")){
				string str = cmdl.get_opt_as<string>("shmk");
				shmk = hex_strtonum<key_t>(str);
			}

			if(cmdl.has_opt("uin")){
				uin = cmdl.get_opt_as<uint32_t>("uin");
			}

			if(cmdl.has_opt("roomid")){
				roomid = cmdl.get_opt_as<uint32_t>("roomid");
			}

			char cbuf[64];
			sprintf(cbuf, "0x%02x", shmk);
			cout << "shmk = " << string(cbuf) << " uin=" << uin << " roomid=" << roomid << endl; 

			int shmid = 0;
			size_t shmsize = 0;

			void *mem = shm_open(shmk, shmid, shmsize);
			if(NULL == mem) return;

			CCfgMng oCfg;
			oCfg.LoadCfg("../../etc/muxsvr_mcd.conf");

			unsigned int uiHeadSize = 5*sizeof(unsigned int) + sizeof(CDataChannelMgr::CVoiceChannelAllocMgr::CacheIndex)*oCfg.m_uiVoiceChannelHashLen*oCfg.m_uiVoiceChannelHashTime;
			void *pDataMem = (void *)((char *)mem+uiHeadSize);
			unsigned int uiDataSize = shmsize - uiHeadSize;

			dis_voicechannel op(uin, roomid);
			show_info(pDataMem, uiDataSize, op);
			cout << "\ntotal: " << op.total << endl;

			shmdt(mem);
		}

		void show_voicechannel_free(cmd_line &cmdl)
		{
			key_t		shmk = 0x20131005;
			uint32_t    channelid = 0;

			if(cmdl.has_opt("shmk")){
				string str = cmdl.get_opt_as<string>("shmk");
				shmk = hex_strtonum<key_t>(str);
			}

			char cbuf[64];
			sprintf(cbuf, "0x%02x", shmk);
			cout << "shmk = " << string(cbuf) << endl; 

			int shmid = 0;
			size_t shmsize = 0;

			void *mem = shm_open(shmk, shmid, shmsize);
			if(NULL == mem) return;

			CCfgMng oCfg;
			oCfg.LoadCfg("../../etc/muxsvr_mcd.conf");

			unsigned int uiCfgNodeNum = oCfg.m_uiVoiceChannelHashLen*oCfg.m_uiVoiceChannelHashTime;
			unsigned int uiIndexSize = sizeof(CDataChannelMgr::CVoiceChannelAllocMgr::CacheIndex)*uiCfgNodeNum;

			const unsigned int *puiNodeNum = (const unsigned int *)((char *)mem);
			const unsigned int *puiFreeListHead = (const unsigned int *)((char *)mem+sizeof(unsigned int));
			const unsigned int *puiFreeListTail = (const unsigned int *)((char *)mem+sizeof(unsigned int)+sizeof(unsigned int));
            const unsigned int *puiUsedListHead = (const unsigned int *)((char *)mem+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int));
            const unsigned int *puiUsedListTail = (const unsigned int *)((char *)mem+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int));
			const CDataChannelMgr::CVoiceChannelAllocMgr::CacheIndex *pIndex = (const CDataChannelMgr::CVoiceChannelAllocMgr::CacheIndex *)((char *)mem+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int));
			const SVoiceChannel *pData = (const SVoiceChannel *)((char *)mem+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int)+sizeof(unsigned int)+uiIndexSize);

			cout << "uiNodeNum: " << *puiNodeNum << endl;
			cout << "uiFreeListHead: " << *puiFreeListHead << endl;
			cout << "uiFreeListTail: " << *puiFreeListTail << endl;
            cout << "uiUsedListHead: " << *puiUsedListHead << endl;
            cout << "uiUsedListTail: " << *puiUsedListTail << endl;

			cout << "free list: " << endl;

			unsigned int uiFreeIndex = *puiFreeListHead;

			for (unsigned int i=0; i<uiCfgNodeNum; i++)
			{
				if (i>=*puiNodeNum)
				{
					break;
				}

				if (uiFreeIndex == INVALID_VOICECHANNEL_ID)
				{
					break;
				}

				printf("%4d ", uiFreeIndex);

				if (i!=0 &&i%16==15)
				{
					cout << endl;
				}

				uiFreeIndex = pIndex[uiFreeIndex].uiNext;
			}

            cout << endl;
            cout << "============= used list: " << endl;

            unsigned int uiUsedIndex = *puiUsedListHead;

            for (unsigned int i=0; i<uiCfgNodeNum; i++)
            {
                if (i>=*puiNodeNum)
                {
                    break;
                }

                if (uiUsedIndex == INVALID_VOICECHANNEL_ID)
                {
                    break;
                }

                printf("%4d ", uiUsedIndex);

                if (i!=0 &&i%16==15)
                {
                    cout << endl;
                }

                uiUsedIndex = pIndex[uiUsedIndex].uiNext;
            }

			cout << endl;

			shmdt(mem);
		}

		void show_video(cmd_line &cmdl)
		{
			key_t		shmk = 0x20131001;
			uint32_t	videoid = 0;

			if(cmdl.has_opt("shmk")){
				string str = cmdl.get_opt_as<string>("shmk");
				shmk = hex_strtonum<key_t>(str);
			}
			if(cmdl.has_opt("videoid")){
				videoid = cmdl.get_opt_as<uint32_t>("videoid");
			}

			char cbuf[64];
			sprintf(cbuf, "0x%02x", shmk);
			cout << "shmk = " << string(cbuf) << " videoid = " << videoid << endl; 

			int shmid = 0;
			size_t shmsize = 0;

			void *mem = shm_open(shmk, shmid, shmsize);
			if(NULL == mem) return;

			dis_video op(videoid);
			show_info(mem, shmsize, op);
			cout << "\ntotal: " << op.total << endl;

			shmdt(mem);
		}

		void show_room(cmd_line &cmdl)
		{
			key_t		shmk		= 0x20131007;
			uint32_t	uiRoomId	= 0;

			if(cmdl.has_opt("shmk")){
				string str	= cmdl.get_opt_as<string>("shmk");
				shmk = hex_strtonum<key_t>(str);
			}
			if(cmdl.has_opt("roomid")){
				uiRoomId	= cmdl.get_opt_as<uint32_t>("roomid");
			}

			char cbuf[64];
			sprintf(cbuf, "0x%02x", shmk);
			cout << "shmk = " << string(cbuf) << " roomid = " << uiRoomId << endl; 

			int shmid		= 0;
			size_t shmsize	= 0;

			void *mem = shm_open(shmk, shmid, shmsize);
			if(NULL == mem) return;

			dis_room op(uiRoomId);
			show_info(mem, shmsize, op);
			cout << "\ntotal: " << op.total << endl;

			shmdt(mem);
		}

		void show_cdnflow2videoid(cmd_line &cmdl)
		{
			key_t shmk80	= 0x20131002;
			key_t shmk8000	= 0x20131003;
			key_t shmk443	= 0x20131004;

			uint32_t cdnflowid = 0;
			uint32_t srcport = 0;

			if(cmdl.has_opt("shmk80")){
				string str = cmdl.get_opt_as<string>("shmk80");
				shmk80 = hex_strtonum<key_t>(str);
			}

			if(cmdl.has_opt("shmk8000")){
				string str = cmdl.get_opt_as<string>("shmk8000");
				shmk8000 = hex_strtonum<key_t>(str);
			}

			if(cmdl.has_opt("shmk443")){
				string str = cmdl.get_opt_as<string>("shmk443");
				shmk443 = hex_strtonum<key_t>(str);
			}

			if(cmdl.has_opt("cdnflowid")){
				cdnflowid = cmdl.get_opt_as<uint32_t>("cdnflowid");
			}

			if(cmdl.has_opt("srcport")){
				srcport = cmdl.get_opt_as<uint32_t>("srcport");
			}

			char cbuf[64];
			sprintf(cbuf, "0x%02x", shmk80);
			cout << "shmk80 = " << string(cbuf) << endl;
			sprintf(cbuf, "0x%02x", shmk8000);
			cout << "shmk8000 = " << string(cbuf) << endl; 
			sprintf(cbuf, "0x%02x", shmk443);
			cout << "shmk443 = " << string(cbuf) << endl; 

			cout << "cdnflowid = " << cdnflowid << endl; 
			cout << "srcport = " << srcport << endl; 

			int shmid80 = 0; 
			int shmid8000 = 0;
			int shmid443 = 0;
			size_t shmsize80 = 0;
			size_t shmsize8000 = 0;
			size_t shmsize443 = 0;

			void *mem80 = shm_open(shmk80, shmid80, shmsize80);
			void *mem8000 = shm_open(shmk8000, shmid8000, shmsize8000);
			void *mem443 = shm_open(shmk443, shmid443, shmsize443);
			if(NULL == mem80) return;
			if(NULL == mem8000) return;
			if(NULL == mem443) return;

			uint32_t total80 = 0;
			uint32_t total8000 = 0;
			uint32_t total443 = 0;

			if (srcport == 80)
			{
				dis_cdnflow2videoid op80(cdnflowid,80);
				uint32_t total_before_80 = op80.total;
				show_info(mem80, shmsize80, op80);
				uint32_t total_after_80 = op80.total;
				total80 = total_after_80 - total_before_80;

				cout << endl;
				cout << "total80: " << total80 << endl;
				cout << "total: " << total80 << endl;
			}
			else if (srcport == 8000)
			{
				dis_cdnflow2videoid op8000(cdnflowid,8000);
				uint32_t total_before_8000 = op8000.total;
				show_info(mem8000, shmsize8000, op8000);
				uint32_t total_after_8000 = op8000.total;
				total8000 = total_after_8000 - total_before_8000;

				cout << endl;
				cout << "total8000: " << total8000 << endl;
				cout << "total: " << total8000 << endl;
			}
			else if (srcport == 443)
			{
				dis_cdnflow2videoid op443(cdnflowid,443);
				uint32_t total_before_443 = op443.total;
				show_info(mem443, shmsize443, op443);
				uint32_t total_after_443 = op443.total;
				total443 = total_after_443 - total_before_443;

				cout << endl;
				cout << "total443: " << total443 << endl;
				cout << "total: " << total443 << endl;
			}
			else
			{
				dis_cdnflow2videoid op80(cdnflowid,80);
				uint32_t total_before_80 = op80.total;
				show_info(mem80, shmsize80, op80);
				uint32_t total_after_80 = op80.total;
				total80 = total_after_80 - total_before_80;

				dis_cdnflow2videoid op8000(cdnflowid,8000);
				uint32_t total_before_8000 = op8000.total;
				show_info(mem8000, shmsize8000, op8000);
				uint32_t total_after_8000 = op8000.total;
				total8000 = total_after_8000 - total_before_8000;

				dis_cdnflow2videoid op443(cdnflowid,443);
				uint32_t total_before_443 = op443.total;
				show_info(mem443, shmsize443, op443);
				uint32_t total_after_443 = op443.total;
				total443 = total_after_443 - total_before_443;

				cout << endl;
				cout << "total80: " << total80 << endl;
				cout << "total8000: " << total8000 << endl;
				cout << "total443: " << total443 << endl;
				cout << "total: " << total80 + total8000 + total_after_443 << endl;
			}

			shmdt(mem80);
			shmdt(mem8000);
			shmdt(mem443);
		}
	};
}

int main(int argc, char **argv)
{
	sparrow::cmd_line cmdl;
    CMonitor oMonitor;

	cmdl.add_opt()
		("h", "show all options")
        ("channel", "show voice channel info")
        ("channel_free", "show free voice channel info")
        ("video", "video")
		("videoid","videoid")
		("room","room")
		("roomid","roomid")
        ("cdnflow2videoid", "show cdnflow2videoid info, [-cdnflowid][-srcport]")
        ("cdnflowid", "cdnflowid")
        ("srcport", "srcport,80, 8000, 443")
		("all", "all buffer")
		;

    if (argc <= 1)
    {
        cout << cmdl << endl;
        return 0;
    }

	string errm;
	if(!cmdl.parse_cmd_line(argc, argv, &errm))
    {
		cout << "parse command line error: " << errm << endl;
		return -1;
	}

	if(cmdl.has_opt("h"))
    {
		cout << cmdl << endl;
		return 0;
	}

    if(cmdl.has_opt("channel"))
    {
        oMonitor.show_voicechannel(cmdl);
    }

    if(cmdl.has_opt("channel_free"))
    {
        oMonitor.show_voicechannel_free(cmdl);
    }

    if(cmdl.has_opt("video")){
        oMonitor.show_video(cmdl);
    }

	if(cmdl.has_opt("room"))
	{
		oMonitor.show_room(cmdl);
	}

    if(cmdl.has_opt("cdnflow2videoid"))
    {
        oMonitor.show_cdnflow2videoid(cmdl);
    }

	return 0;
}

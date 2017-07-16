#include "264tots.h"
#include "crc.h"

H264ToTs::H264ToTs()
{
	m_uiH264BuffLen = 0;
	m_uiTsBuffLen	= 0;

	/*memset(m_ucH264Buff, 0, sizeof(m_ucH264Buff));
	memset(m_ucTsBuff, 0, sizeof(m_ucTsBuff));*/

	FOutVideoTs = NULL;

	//video
	Videopts				= 0;
	Videodts				= 0;

	VideoFrameCount			= 0; 
	Frame_Time				= 0;

	memset(TSbuf,0,TS_PACKET_SIZE);		
	WritePacketNum			= 0;

	memset(&continuity_counter,0,sizeof(continuity_counter));

	b_sps_pps_init = false;
}

H264ToTs::~H264ToTs()
{
	if(FOutVideoTs)
		fclose(FOutVideoTs);
}

int H264ToTs::open_ts(const char *pTsOut, unsigned int uiFps)
{
	if(0 == uiFps)
	{
		return -1;
	}

	if(uiFps > 30)
	{
		uiFps = 25;
	}

	m_uiFps = uiFps;

	if(NULL != pTsOut)
	{
		FOutVideoTs = fopen(pTsOut,"wb");
	}

	return 0;
}

//video
int H264ToTs::H2642PES(unsigned char *ucTsFrameBuf, unsigned int uiTsFrameLen)
{
    Ts_Adaptation_field  ts_adaptation_field_Head ; 
    Ts_Adaptation_field  ts_adaptation_field_Tail ; 

    ts_video_pes.packet_start_code_prefix = 0x000001;
    ts_video_pes.stream_id = TS_H264_STREAM_ID;								//E0~EF表示是视频的,C0~DF是音频,H264-- E0
    ts_video_pes.PES_packet_length = 0 ;										//一帧数据的长度 不包含PES包头,8是自适应的长度,填0 可以自动查找
    ts_video_pes.Pes_Packet_Length_Beyond = uiTsFrameLen;

    if (uiTsFrameLen > 0xFFFF)                                                  //长度就2字节
    {
        ts_video_pes.PES_packet_length = 0x00;
        ts_video_pes.Pes_Packet_Length_Beyond = uiTsFrameLen;
    }

    ts_video_pes.marker_bit				= 0x02;								//固定值
    ts_video_pes.PES_scrambling_control	= 0x00;                             //不加扰
    ts_video_pes.PES_priority				= 0x00;								//无优先级
    ts_video_pes.data_alignment_indicator	= 0x00;
    ts_video_pes.copyright					= 0x00;								//无版权
    ts_video_pes.original_or_copy			= 0x00;
    ts_video_pes.PTS_DTS_flags				= 0x03;								//pts 和 dts都有
    ts_video_pes.ESCR_flag					= 0x00;
    ts_video_pes.ES_rate_flag				= 0x00;
    ts_video_pes.DSM_trick_mode_flag		= 0x00;
    ts_video_pes.additional_copy_info_flag = 0x00;
    ts_video_pes.PES_CRC_flag				= 0x00;
    ts_video_pes.PES_extension_flag		= 0x00;
    ts_video_pes.PES_header_data_length	= 0x0a;								//包括了PTS和DTS所占的字节数

    //清 0 
    ts_video_pes.tsptsdts.pts_32_30		= 0;
    ts_video_pes.tsptsdts.pts_29_15		= 0;
    ts_video_pes.tsptsdts.pts_14_0			= 0;
    ts_video_pes.tsptsdts.dts_32_30		= 0;
    ts_video_pes.tsptsdts.dts_29_15		= 0;
    ts_video_pes.tsptsdts.dts_14_0			= 0;

    ts_video_pes.tsptsdts.reserved_1 = 0x0003;                                 //填写pts信息

    // Videopts大于30bit，使用最高三位 
    if(Videopts > 0x7FFFFFFF)
    {
        ts_video_pes.tsptsdts.pts_32_30 = (Videopts >> 30) & 0x07;                 
        ts_video_pes.tsptsdts.marker_bit1 = 0x01;
    }
    else 
    {
        ts_video_pes.tsptsdts.marker_bit1 = 0;
    }

    // Videopts大于15bit，使用更多的位来存储
    if(Videopts > 0x7FFF)
    {
        ts_video_pes.tsptsdts.pts_29_15 = (Videopts >> 15) & 0x007FFF ;
        ts_video_pes.tsptsdts.marker_bit2 = 0x01;
    }
    else
    {
        ts_video_pes.tsptsdts.marker_bit2 = 0;
    }

    //使用最后15位
    ts_video_pes.tsptsdts.pts_14_0 = Videopts & 0x007FFF;
    ts_video_pes.tsptsdts.marker_bit3 = 0x01;


    ts_video_pes.tsptsdts.reserved_2 = 0x0001;                                 //填写 dts信息
    // Videopts大于30bit，使用最高三位 
    if(Videodts > 0x7FFFFFFF)
    {
        ts_video_pes.tsptsdts.dts_32_30 = (Videodts >> 30) & 0x07;                 
        ts_video_pes.tsptsdts.marker_bit4 = 0x01;
    }
    else 
    {
        ts_video_pes.tsptsdts.marker_bit4 = 0;
    }
    // Videopts大于15bit，使用更多的位来存储
    if(Videodts > 0x7FFF)
    {
        ts_video_pes.tsptsdts.dts_29_15 = (Videodts >> 15) & 0x007FFF ;
        ts_video_pes.tsptsdts.marker_bit5 = 0x01;
    }
    else
    {
        ts_video_pes.tsptsdts.marker_bit5 = 0;
    }
    //使用最后15位
    ts_video_pes.tsptsdts.dts_14_0 = Videodts & 0x007FFF;
    ts_video_pes.tsptsdts.marker_bit6 = 0x01;

    //memcpy(ts_video_pes.Es, ucTsFrameBuf, uiTsFrameLen);
    ts_video_pes.setFrameBuff(ucTsFrameBuf, uiTsFrameLen);

    WriteAdaptive_flags_Head(&ts_adaptation_field_Head); //填写自适应段标志帧头
    WriteAdaptive_flags_Tail(&ts_adaptation_field_Tail); //填写自适应段标志帧尾

    PES2TS(&ts_video_pes,TS_H264_PID,&ts_adaptation_field_Head,&ts_adaptation_field_Tail); 

    return 1;
}

//一个或多个naul都可以
int H264ToTs::processNalu(unsigned char *ucBuff, unsigned int uiLen, unsigned int &uiOutTsLen, unsigned int uiTime, unsigned int uiVideoId)
{
    if(NULL == ucBuff || 0 == uiLen)
    {
        return -1;
    }

    m_nal_t.clear();

    if(ucBuff[0] == 0x00 && ucBuff[1] == 0x00 && ucBuff[2] == 0x00 && ucBuff[3] == 0x01)
    {
        m_nal_t.uiPreFix = 4;
    }
    else if(ucBuff[0] == 0x00 && ucBuff[1] == 0x00 && ucBuff[2] == 0x01)
    {
        m_nal_t.uiPreFix = 3;
    }
    else
    {
        //cout << "invalid nalu ..." << endl;
        return -2;
    }

    int ret = 0;

    //有头
    ret = m_nal_t.setBuff(ucBuff, uiLen);
    if(ret <0)
    {
        //cout << "processNalu set buffer error.." << endl;
        return -3;
    }

    unsigned int uiPreFix = m_nal_t.uiPreFix;

    m_nal_t.forbidden_bit		= m_nal_t.buf[uiPreFix] & 0x80;					//1 bit  设置nal 头
    m_nal_t.nal_reference_idc	= m_nal_t.buf[uiPreFix] & 0x60;					//2 bit
    m_nal_t.nal_unit_type		= m_nal_t.buf[uiPreFix] & 0x1f;					//5 bit

    VideoFrameCount++;

    Videopts += (1000 / m_uiFps) * 90;											//  1000/fps * 90hz
    Videodts = Videopts;

    if(m_uiH264BuffLen + m_nal_t.len < MAX_TS_BUFF)
    {
        memcpy(m_ucH264Buff + m_uiH264BuffLen, m_nal_t.buf, m_nal_t.len);
        m_uiH264BuffLen += m_nal_t.len;
    }

    //cout << "m_nal_t.len: " << m_nal_t.len << endl;

    if(m_nal_t.len >= 188)
    {
        H2642PES(m_ucH264Buff, m_uiH264BuffLen);

        //cout << "m_uiBuffLen: " << m_uiBuffLen << endl;

        //长度清0
        m_uiH264BuffLen = 0;
    }
    else
    {
        uiOutTsLen = m_uiTsBuffLen;

        return 0;
    }

    //是否写入文件
    if(NULL != FOutVideoTs)
    {
        fwrite(m_ucTsBuff,1,m_uiTsBuffLen,FOutVideoTs);
    }

    uiOutTsLen = m_uiTsBuffLen;

    m_uiTsSumLen += m_uiTsBuffLen;

    ret = m_oSm3u8Info.writeM3u8(m_ucTsBuff, m_uiTsBuffLen, uiTime, uiVideoId);

    if(ret < 0)
    {
        SUBMCD_DEBUG_P(LOG_ERROR,"CProtol::processNalu writeM3u8 error! tsBufLen: %u, time: %u\n", m_uiTsBuffLen, uiTime);

        return -1;
    }

    SUBMCD_DEBUG_P(LOG_TRACE,"CProtol::processNalu writeM3u8 ok! tsBufLen: %u, time: %u\n", m_uiTsBuffLen, uiTime);

    return 0;
}

//ts
int H264ToTs::CreatePAT()
{
    TsPat ts_pat;
    TsPacketHeader ts_header;

    static unsigned char PatBuf[TS_PACKET_SIZE];
    memset(PatBuf,0xFF,TS_PACKET_SIZE);

    unsigned char *pointer_pat	= NULL;
    unsigned char *pat			= NULL;
    pointer_pat = pat = PatBuf;

    unsigned long long PAT_CRC = 0xFFFFFFFF;

    CreateTsHeader(&ts_header,TS_PAT_PID,0x01,0x01);  //PID = 0x00,有效荷载单元起始指示符_play_init = 0x01, ada_field_C,0x01,仅有有效负载 ；
    TsHeader2buffer(&ts_header,PatBuf);

    pointer_pat[4]					= 0;                     //自适应段的长度为0
    pointer_pat						+= 5;
    ts_pat.table_id					= 0x00;
    ts_pat.section_syntax_indicator = 0x01;
    ts_pat.zero						= 0x00;
    ts_pat.reserved_1				= 0x03;                  //设置为11
    ts_pat.section_length			= 0x0d;                  //pat结构体长度 16个字节减去上面的3个字节
    ts_pat.transport_stream_id		= 0x01;
    ts_pat.reserved_2				= 0x03;                  //设置为11；
    ts_pat.version_number			= 0x00;
    ts_pat.current_next_indicator	= 0x01;					 //当前的pat 有效
    ts_pat.section_number			= 0x00;
    ts_pat.last_section_number		= 0x00;
    ts_pat.program_number			= 0x01;
    ts_pat.reserved_3				= 0x07;                  //设置为111；
    ts_pat.program_map_PID			= TS_PMT_PID;            //PMT的PID
    ts_pat.CRC_32 = PAT_CRC;                        

    pointer_pat[0] = ts_pat.table_id;
    pointer_pat[1] = ts_pat.section_syntax_indicator << 7 | ts_pat.zero  << 6 | ts_pat.reserved_1 << 4 | ((ts_pat.section_length >> 8) & 0x0F);
    pointer_pat[2] = ts_pat.section_length & 0x00FF;
    pointer_pat[3] = ts_pat.transport_stream_id >> 8;
    pointer_pat[4] = ts_pat.transport_stream_id & 0x00FF;
    pointer_pat[5] = ts_pat.reserved_2 << 6 | ts_pat.version_number << 1 | ts_pat.current_next_indicator;
    pointer_pat[6] = ts_pat.section_number;
    pointer_pat[7] = ts_pat.last_section_number;
    pointer_pat[8] = ts_pat.program_number>>8;
    pointer_pat[9] = ts_pat.program_number & 0x00FF;
    pointer_pat[10]= ts_pat.reserved_3 << 5 | ((ts_pat.program_map_PID >> 8) & 0x0F);
    pointer_pat[11]= ts_pat.program_map_PID & 0x00FF;
    pointer_pat += 12;

    PAT_CRC = Zwg_ntohl(calc_crc32(pat + 5, pointer_pat - pat - 5));
    memcpy(pointer_pat, (unsigned char *)&PAT_CRC, 4);

    if(m_uiTsBuffLen + 188 < 2*TS_MAX_OUT_BUFF)						//pat 写入缓存
    {
        memcpy(m_ucTsBuff + m_uiTsBuffLen, PatBuf, 188);
        m_uiTsBuffLen += 188;
    }

    return 1;
}

int H264ToTs::CreatePMT()
{    
    TsPmt ts_pmt;
    TsPacketHeader ts_header;

    static unsigned char PmtBuf[TS_PACKET_SIZE];
    memset(PmtBuf,0xFF,TS_PACKET_SIZE);                         //填成0xFF

    unsigned char *pointer_pmt	= NULL;
    unsigned char *pmt			= NULL;
    pointer_pmt = pmt = PmtBuf; 

    unsigned long long PMT_CRC = 0xFFFFFFFF; 

    CreateTsHeader(&ts_header,TS_PMT_PID,0x01,0x01);				//PID = 0x00,有效荷载单元起始指示符_play_init = 0x01, ada_field_C,0x01,仅有有效负载；
    TsHeader2buffer(&ts_header,PmtBuf);

    pointer_pmt[4]					= 0;                            //自适应段的长度为0
    pointer_pmt						+= 5;
    ts_pmt.table_id					= 0x02;                         //0x02 for pmt table
    ts_pmt.section_syntax_indicator = 0x01;
    ts_pmt.zero						= 0x00;
    ts_pmt.reserved_1				= 0x03;
    ts_pmt.section_length			= 0x17;                         //PMT结构体长度 16 + 5 + 5个字节减去上面的3个字节
    ts_pmt.program_number			= 01;                           //一个节目
    ts_pmt.reserved_2				= 0x03;
    ts_pmt.version_number			= 0x00;
    ts_pmt.current_next_indicator	= 0x01;							//当前的PMT有效
    ts_pmt.section_number			= 0x00;
    ts_pmt.last_section_number		= 0x00;
    ts_pmt.reserved_3				= 0x07;
    ts_pmt.PCR_PID					= TS_H264_PID ;					//视频PID                                   
    ts_pmt.reserved_4				= 0x0F;
    ts_pmt.program_info_length		= 0x00;                         //后面无 节目信息描述

    ts_pmt.stream_type_video		= PMT_STREAM_TYPE_VIDEO;        //视频的类型
    ts_pmt.reserved_5_video			= 0x07;
    ts_pmt.elementary_PID_video		= TS_H264_PID;                  //视频的PID
    ts_pmt.reserved_6_video= 0x0F;
    ts_pmt.ES_info_length_video = 0x00;								//视频无跟随的相关信息

    //ts_pmt.stream_type_audio = PMT_STREAM_TYPE_AUDIO;				//音频类型
    //ts_pmt.reserved_5_audio = 0x07;
    //ts_pmt.elementary_PID_audio = TS_MP3_PID;						//音频PID 
    //ts_pmt.reserved_6_audio = 0x0F;
    //ts_pmt.ES_info_length_audio = 0x00;							//音频无跟随的相关信息

    ts_pmt.CRC_32 = PMT_CRC; 

    pointer_pmt[0] = ts_pmt.table_id;
    pointer_pmt[1] = ts_pmt.section_syntax_indicator << 7 | ts_pmt.zero  << 6 | ts_pmt.reserved_1 << 4 | ((ts_pmt.section_length >> 8) & 0x0F);
    pointer_pmt[2] = ts_pmt.section_length & 0x00FF;
    pointer_pmt[3] = ts_pmt.program_number >> 8;
    pointer_pmt[4] = ts_pmt.program_number & 0x00FF;
    pointer_pmt[5] = ts_pmt.reserved_2 << 6 | ts_pmt.version_number << 1 | ts_pmt.current_next_indicator;
    pointer_pmt[6] = ts_pmt.section_number;
    pointer_pmt[7] = ts_pmt.last_section_number;
    pointer_pmt[8] = ts_pmt.reserved_3 << 5  | ((ts_pmt.PCR_PID >> 8) & 0x1F);
    pointer_pmt[9] = ts_pmt.PCR_PID & 0x0FF;
    pointer_pmt[10]= ts_pmt.reserved_4 << 4 | ((ts_pmt.program_info_length >> 8) & 0x0F);
    pointer_pmt[11]= ts_pmt.program_info_length & 0xFF;

    pointer_pmt[12]= ts_pmt.stream_type_video;                               //视频流的stream_type
    pointer_pmt[13]= ts_pmt.reserved_5_video << 5 | ((ts_pmt.elementary_PID_video >> 8 ) & 0x1F);
    pointer_pmt[14]= ts_pmt.elementary_PID_video & 0x00FF;
    pointer_pmt[15]= ts_pmt.reserved_6_video<< 4 | ((ts_pmt.ES_info_length_video >> 8) & 0x0F);
    pointer_pmt[16]= ts_pmt.ES_info_length_video & 0x0FF;

    //pointer_pmt[17]= ts_pmt.stream_type_audio;                               //音频流的stream_type
    //pointer_pmt[18]= ts_pmt.reserved_5_audio<< 5 | ((ts_pmt.elementary_PID_audio >> 8 ) & 0x1F);
    //pointer_pmt[19]= ts_pmt.elementary_PID_audio & 0x00FF;
    //pointer_pmt[20]= ts_pmt.reserved_6_audio << 4 | ((ts_pmt.ES_info_length_audio >> 8) & 0x0F);
    //pointer_pmt[21]= ts_pmt.ES_info_length_audio & 0x0FF;
    //pointer_pmt += 22;
    pointer_pmt += 17;

    int len = 0;

    len  = pointer_pmt - pmt - 8 + 4;
    len = len > 0xffff ? 0:len;
    *(pmt + 6) = 0xb0 | (len >> 8);
    *(pmt + 7) = len;

    PMT_CRC = Zwg_ntohl(calc_crc32(pmt + 5, pointer_pmt - pmt - 5));
    memcpy(pointer_pmt, (unsigned char  *)&PMT_CRC, 4);                                       

    if(m_uiTsBuffLen + 188 < 2*TS_MAX_OUT_BUFF)						//pat写入buf
    {
        memcpy(m_ucTsBuff + m_uiTsBuffLen, PmtBuf, 188);
        m_uiTsBuffLen += 188;
    }

    return 1;
}

TsPacketHeader* H264ToTs::CreateTsHeader(TsPacketHeader * ts_header,unsigned int PID,unsigned char play_init,unsigned char ada_field_C)
{
    ts_header->sync_byte	= TS_SYNC_BYTE;
    ts_header->tras_error	= 0x00;
    ts_header->play_init	= play_init;
    ts_header->tras_prio	= 0x00;
    ts_header->PID			= PID;
    ts_header->tras_scramb	= 0x00;
    ts_header->ada_field_C	= ada_field_C;

    if (PID == TS_PAT_PID)             //0x00 for PAT
    {
        ts_header->conti_cter = (continuity_counter.continuity_counter_pat++ %16);
    }
    else if (PID == TS_PMT_PID)        //0XFFF for pmt
    {
        ts_header->conti_cter = (continuity_counter.continuity_counter_pmt++ %16);
    }
    else if (PID == TS_H264_PID )      //0x100 for h264
    {
        ts_header->conti_cter = (continuity_counter.continuity_counter_video++ %16);
    }
    else if (PID == TS_MP3_PID)        //0x101 for mp3
    {
        ts_header->conti_cter = (continuity_counter.continuity_counter_audio++ %16);
    }
    else
    {
        return NULL;
    }

    return ts_header;
}

int H264ToTs::TsHeader2buffer(TsPacketHeader *ts_header, unsigned char *ucBuffer)
{
    ucBuffer[0]=ts_header->sync_byte;
    ucBuffer[1]=ts_header->tras_error<<7|ts_header->play_init<<6|ts_header->tras_prio<<5|((ts_header->PID>>8)&0x1f); //pid 高5位
    ucBuffer[2]=(ts_header->PID&0x00ff);//剩下八位
    ucBuffer[3]=ts_header->tras_scramb<<6|ts_header->ada_field_C<<4|ts_header->conti_cter;

    return 1;
}

int H264ToTs::WriteAdaptive_flags_Head(Ts_Adaptation_field  *ts_adaptation_field)
{
    //填写自适应段
    ts_adaptation_field->discontinuty_indicator = 0;
    ts_adaptation_field->random_access_indicator = 0;
    ts_adaptation_field->elementary_stream_priority_indicator = 0;
    ts_adaptation_field->PCR_flag = 1;											//只用这一个
    ts_adaptation_field->OPCR_flag = 0;
    ts_adaptation_field->splicing_point_flag = 0;
    ts_adaptation_field->transport_private_data_flag = 0;
    ts_adaptation_field->adaptation_field_extension_flag = 0;

    //需要自己算
    ts_adaptation_field->pcr  = Videopts * 300;
    ts_adaptation_field->adaptation_field_length = 7;                          //只用了PCR_FLAG,占用7位,参考iso13818-1.pdf page 40/174 Table 2-6 Transport Stream adaptation field

    ts_adaptation_field->opcr = 0;
    ts_adaptation_field->splice_countdown = 0;
    ts_adaptation_field->private_data_len = 0;
    return 1;
}

int H264ToTs::WriteAdaptive_flags_Tail(Ts_Adaptation_field  *ts_adaptation_field)
{
    ts_adaptation_field->discontinuty_indicator     = 0;
    ts_adaptation_field->random_access_indicator    = 0;
    ts_adaptation_field->elementary_stream_priority_indicator = 0;
    ts_adaptation_field->PCR_flag = 0;                                      
    ts_adaptation_field->OPCR_flag = 0;
    ts_adaptation_field->splicing_point_flag = 0;
    ts_adaptation_field->transport_private_data_flag = 0;
    ts_adaptation_field->adaptation_field_extension_flag = 0;

    ts_adaptation_field->pcr  = 0;
    ts_adaptation_field->adaptation_field_length = 1;                       //flag全0,只用了1位

    ts_adaptation_field->opcr = 0;
    ts_adaptation_field->splice_countdown = 0;
    ts_adaptation_field->private_data_len = 0;                    
    return 1;
}

int H264ToTs::CreateAdaptive_Ts(Ts_Adaptation_field * ts_adaptation_field,unsigned char * pointer_ts,unsigned int AdaptiveLength)
{
    unsigned int CurrentAdaptiveLength = 1;                                 //当前已经用的自适应段长度  
    unsigned char Adaptiveflags = 0;                                        //自适应段的标志

    //填写自适应字段
    if (ts_adaptation_field->adaptation_field_length > 0)
    {
        pointer_ts += 1;                                                    //8个flag占用1字节
        CurrentAdaptiveLength += 1;

        if (ts_adaptation_field->discontinuty_indicator)
        {
            Adaptiveflags |= 0x80;
        }
        if (ts_adaptation_field->random_access_indicator)
        {
            Adaptiveflags |= 0x40;
        }
        if (ts_adaptation_field->elementary_stream_priority_indicator)
        {
            Adaptiveflags |= 0x20;
        }
        if (ts_adaptation_field->PCR_flag)
        {
            unsigned long long pcr_base;
            unsigned int pcr_ext;

            pcr_base = (ts_adaptation_field->pcr / 300);
            pcr_ext = (ts_adaptation_field->pcr % 300);

            Adaptiveflags |= 0x10;

            pointer_ts[0] = (pcr_base >> 25) & 0xff;
            pointer_ts[1] = (pcr_base >> 17) & 0xff;
            pointer_ts[2] = (pcr_base >> 9) & 0xff;
            pointer_ts[3] = (pcr_base >> 1) & 0xff;
            pointer_ts[4] = pcr_base << 7 | pcr_ext >> 8 | 0x7e;
            pointer_ts[5] = (pcr_ext) & 0xff;
            pointer_ts += 6;

            CurrentAdaptiveLength += 6;
        }
        if (ts_adaptation_field->OPCR_flag)
        {
            unsigned long long opcr_base;
            unsigned int opcr_ext;

            opcr_base = (ts_adaptation_field->opcr / 300);
            opcr_ext = (ts_adaptation_field->opcr % 300);

            Adaptiveflags |= 0x08;

            pointer_ts[0] = (opcr_base >> 25) & 0xff;
            pointer_ts[1] = (opcr_base >> 17) & 0xff;
            pointer_ts[2] = (opcr_base >> 9) & 0xff;
            pointer_ts[3] = (opcr_base >> 1) & 0xff;
            pointer_ts[4] = ((opcr_base << 7) & 0x80) | ((opcr_ext >> 8) & 0x01);
            pointer_ts[5] = (opcr_ext) & 0xff;
            pointer_ts += 6;
            CurrentAdaptiveLength += 6;
        }
        if (ts_adaptation_field->splicing_point_flag)
        {
            pointer_ts[0] = ts_adaptation_field->splice_countdown;

            Adaptiveflags |= 0x04;

            pointer_ts += 1;
            CurrentAdaptiveLength += 1;
        }
        if (ts_adaptation_field->private_data_len > 0)
        {
            Adaptiveflags |= 0x02;
            if ((unsigned int)(1+ ts_adaptation_field->private_data_len) > AdaptiveLength - CurrentAdaptiveLength)
            {
                return -1;
            }
            else
            {
                pointer_ts[0] = ts_adaptation_field->private_data_len;
                pointer_ts += 1;
                memcpy(pointer_ts, ts_adaptation_field->private_data, ts_adaptation_field->private_data_len);
                pointer_ts += ts_adaptation_field->private_data_len;

                CurrentAdaptiveLength += (1 + ts_adaptation_field->private_data_len) ;
            }
        }
        if (ts_adaptation_field->adaptation_field_extension_flag)
        {
            Adaptiveflags |= 0x01;
            pointer_ts[1] = 1;
            pointer_ts[2] = 0;
            CurrentAdaptiveLength += 2;
        }

        TSbuf[5] = Adaptiveflags;                        //将标志放入内存
    }
    return 1;
}

int H264ToTs::PES2TS(TsPes * ts_pes,unsigned int Video_Audio_PID ,Ts_Adaptation_field * ts_adaptation_field_Head ,Ts_Adaptation_field * ts_adaptation_field_Tail)
{
    TsPacketHeader ts_header;

    unsigned char *pointer_ts	= NULL;			//一个188包的指针
    unsigned char *NeafBuf		= NULL;		//分片包总负载的指针

    unsigned int AdaptiveLength			= 0;	//要填写0XFF的长度
    unsigned int NeafPacketCount		= 0;	//分片包的个数
    unsigned int FirstPacketLoadLength	= 0;		//分片包的第一个包的负载长度
    unsigned int NeafPacketCount_Now	= 0;            //当前分片包计数          
    //unsigned int i = 0;															//测试包正确用的循环参数可去掉

    FirstPacketLoadLength = 188 -4 - 1 - ts_adaptation_field_Head->adaptation_field_length - 19; //pcr有效7个字节, 计算分片包的第一个包的负载长度
    NeafPacketCount += 1;														//第一个分片包
    NeafPacketCount += (ts_pes->Pes_Packet_Length_Beyond - FirstPacketLoadLength) / 184;     
    NeafPacketCount += 1;														//最后一个分片包

    AdaptiveLength = 188 - 4 - 1 - ((ts_pes->Pes_Packet_Length_Beyond - FirstPacketLoadLength) % 184);  //要填写0XFF的长度

    if ((WritePacketNum % 40) == 0)                                                         //每40个包打一个 pat一个pmt
    {
        CreatePAT();                                                                        //创建PAT
        CreatePMT();                                                                        //创建PMT
    }
    //开始处理第一个包,分片包的个数最少也会是两个
    memset(TSbuf,0,TS_PACKET_SIZE);   
    pointer_ts = TSbuf;

    CreateTsHeader(&ts_header,Video_Audio_PID,0x01,0x03);							//PID = TS_H264_PID,有效荷载单元起始指示符
    //play_init = 0x01
    //ada_field_C,0x03,含有调整字段和有效负载
    TsHeader2buffer(&ts_header,TSbuf);

    pointer_ts += 4;																//ts 4 字节头
    pointer_ts[0] = ts_adaptation_field_Head->adaptation_field_length;              //自适应区adaptation_field_length 1字节
    pointer_ts += 1;

    CreateAdaptive_Ts(ts_adaptation_field_Head,pointer_ts,(188 - 4 - 1 - 19));      //自适应字段
    pointer_ts += ts_adaptation_field_Head->adaptation_field_length;                //自适应区长度=flag + flag=1时,该flag的数据

    pointer_ts[0] = (ts_pes->packet_start_code_prefix >> 16) & 0xFF;				//填充pes头部，19字节
    pointer_ts[1] = (ts_pes->packet_start_code_prefix >> 8) & 0xFF; 
    pointer_ts[2] = ts_pes->packet_start_code_prefix & 0xFF;
    pointer_ts[3] = ts_pes->stream_id;
    pointer_ts[4] = (ts_pes->PES_packet_length >> 8) & 0xFF;
    pointer_ts[5] = ts_pes->PES_packet_length & 0xFF;

    pointer_ts[6] = ts_pes->marker_bit << 6 
        | ts_pes->PES_scrambling_control << 4 
        | ts_pes->PES_priority << 3 
        | ts_pes->data_alignment_indicator << 2 
        | ts_pes->copyright << 1 |ts_pes->original_or_copy;

    pointer_ts[7] = ts_pes->PTS_DTS_flags << 6 
        |ts_pes->ESCR_flag << 5 
        | ts_pes->ES_rate_flag << 4 
        | ts_pes->DSM_trick_mode_flag << 3 
        | ts_pes->additional_copy_info_flag << 2 
        | ts_pes->PES_CRC_flag << 1 
        | ts_pes->PES_extension_flag;

    pointer_ts[8] = ts_pes->PES_header_data_length;

    if (ts_pes->stream_id == TS_H264_STREAM_ID)
    {
        pointer_ts[9] = (((0x3 << 4) | ((Videopts>> 29) & 0x0E) | 0x01) & 0xff);
        pointer_ts[10]= (((((Videopts >> 14) & 0xfffe) | 0x01) >> 8) & 0xff);
        pointer_ts[11]= ((((Videopts >> 14) & 0xfffe) | 0x01) & 0xff);
        pointer_ts[12]= (((((Videopts << 1) & 0xfffe) | 0x01) >> 8) & 0xff);
        pointer_ts[13]= ((((Videopts << 1) & 0xfffe) | 0x01) & 0xff);


        pointer_ts[14]= (((0x1<< 4) | ((Videodts >> 29) & 0x0E) | 0x01) & 0xff);
        pointer_ts[15]= (((((Videodts >> 14) & 0xfffe) | 0x01) >> 8) & 0xff);
        pointer_ts[16]= ((((Videodts >> 14) & 0xfffe) | 0x01) & 0xff);
        pointer_ts[17]= ((((Videodts << 1) & 0xfffe) | 0x01) >> 8) & 0xff;
        pointer_ts[18]= (((Videodts << 1) & 0xfffe) | 0x01) & 0xff;
    }
    else
    {
        //printf("ts_pes->stream_id  error 0x%x \n",ts_pes->stream_id);
        //return getchar();
    }

    pointer_ts += 19;

    NeafBuf = ts_pes->Es;
    memcpy(pointer_ts,NeafBuf,FirstPacketLoadLength);  
    NeafBuf += FirstPacketLoadLength;

    NeafPacketCount_Now++;

    ts_pes->Pes_Packet_Length_Beyond -= FirstPacketLoadLength;

    if(m_uiTsBuffLen + 188 < 2*TS_MAX_OUT_BUFF)
    {
        memcpy(m_ucTsBuff + m_uiTsBuffLen, TSbuf, 188);							//ts写入buff
        m_uiTsBuffLen += 188;
    }

    WritePacketNum++;															

    while(ts_pes->Pes_Packet_Length_Beyond)
    {
        if ((WritePacketNum % 40) == 0)											//每40个包打一个pat pmt
        {
            CreatePAT();                                                     
            CreatePMT();                                                     
        }

        if(ts_pes->Pes_Packet_Length_Beyond >=184)
        {																			 //处理中间包
            memset(TSbuf,0,TS_PACKET_SIZE);    
            pointer_ts = TSbuf;

            CreateTsHeader(&ts_header,Video_Audio_PID,0x00,0x01);				//PID = TS_H264_PID,不是有效荷载单元起始指示符
            //play_init = 0x00
            //ada_field_C,0x01,仅有有效负载；
            TsHeader2buffer(&ts_header,TSbuf);

            pointer_ts += 4;

            memcpy(pointer_ts,NeafBuf,184); 
            NeafBuf += 184;

            ts_pes->Pes_Packet_Length_Beyond -= 184;

            if(m_uiTsBuffLen + 188 < 2*TS_MAX_OUT_BUFF)
            {
                memcpy(m_ucTsBuff + m_uiTsBuffLen, TSbuf, 188);											//ts写入buff
                m_uiTsBuffLen += 188;
            }
        }
        else
        {
            if(ts_pes->Pes_Packet_Length_Beyond == 183||ts_pes->Pes_Packet_Length_Beyond == 182)
            {
                if ((WritePacketNum % 40) == 0)                                                         //每40个包打pat pmt
                {
                    CreatePAT();                                                                        //创建PAT
                    CreatePMT();                                                                        //创建PMT
                }

                memset(TSbuf,0,TS_PACKET_SIZE);  
                pointer_ts = TSbuf;

                CreateTsHeader(&ts_header,Video_Audio_PID,0x00,0x03);									//PID = TS_H264_PID,非有效playload单元起始指示符
                //play_init = 0x00
                //ada_field_C,0x03,含有调整字段和有效负载
                TsHeader2buffer(&ts_header,TSbuf);

                pointer_ts		+= 4;
                pointer_ts[0]	= 1;
                pointer_ts[1]	= 0x00 ;
                pointer_ts		+= 2;

                memcpy(pointer_ts,NeafBuf,182);

                NeafBuf += 182;
                ts_pes->Pes_Packet_Length_Beyond -= 182;

                if(m_uiTsBuffLen + 188 < 2*TS_MAX_OUT_BUFF)													
                {
                    memcpy(m_ucTsBuff + m_uiTsBuffLen, TSbuf, 188);											//ts写入buff
                    m_uiTsBuffLen += 188;
                }
            }
            else
            {
                if ((WritePacketNum % 40) == 0)                                                         //每40个包打一个 pat,一个pmt
                {
                    CreatePAT();                                                                        //创建PAT
                    CreatePMT();                                                                        //创建PMT
                }

                memset(TSbuf,0,TS_PACKET_SIZE);  
                pointer_ts = TSbuf;
                CreateTsHeader(&ts_header,Video_Audio_PID,0x00,0x03);									//PID = TS_H264_PID,非有效荷载单元起始指示符
                //play_init = 0x00
                //ada_field_C,0x03,含有调整字段和有效负载；
                TsHeader2buffer(&ts_header,TSbuf);
                pointer_ts		+= 4;

                pointer_ts[0]	= 184 - ts_pes->Pes_Packet_Length_Beyond - 1 ;
                pointer_ts[1]	= 0x00 ;
                pointer_ts		+= 2;

                memset(pointer_ts,0xFF,(184-ts_pes->Pes_Packet_Length_Beyond-2));
                pointer_ts += (184-ts_pes->Pes_Packet_Length_Beyond-2);

                memcpy(pointer_ts, NeafBuf, ts_pes->Pes_Packet_Length_Beyond);
                ts_pes->Pes_Packet_Length_Beyond = 0;													//最后一个小于184的包

                if(m_uiTsBuffLen + 188 < 2*TS_MAX_OUT_BUFF)													
                {
                    memcpy(m_ucTsBuff + m_uiTsBuffLen, TSbuf, 188);										//ts写入buff
                    m_uiTsBuffLen += 188;
                }                       
            }
        }

        WritePacketNum++;  
    }

    return 1;
}



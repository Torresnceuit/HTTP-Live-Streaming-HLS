CC=g++
CXX=g++

QTLIBS=./lib
PROTOBUF=./protobuf
PROTOS=./protos
JSON=./json
INC=-I./frame -I./comm  -I./pub -I$(QTLIBS) -I. -I$(PROTOS) -I$(PROTOBUF)/include -I$(JSON)

LIB=/lib/libc.so.6 /lib/libuuid.so.1 ./lib/attr_api.a ./lib/libcomm_oi.a -lz -lpthread $(PROTOBUF)/lib/libprotobuf.a

CFLAGS= -Wall -g 
TARGET=muxsvr_mcd_run.so

OBJ+=CFrameBuffer.o\
    info_channel.o\
    voice_channel.o\
    protol_process.o\
    muxsvr_mcd_cfg.o\
    muxsvr_mcd_proc.o \
    flv/flv_parser.o \
    flv/flv_bytestream.o \
    http.o \
    ./json/json_reader.o \
    ./json/json_value.o \
    ./json/json_writer.o \
    ./protos/muxsvr.pb.o\
    ./protos/LiveProgramMgr.pb.o \
    ./protos/game.pb.o \
    ./pub/buffer.o\
    ./pub/gFunc.o\
    ./pub/submcd_log.o\
    ./frame/fifo_loop_buffer.o\
    ./frame/msg_process.o\
    ./frame/mx_thread.o\
    ./frame/mx_event.o\
    ./frame/timer_base.o\
    ./frame/timer_assist.o\
    ./frame/timer_service.o\
    ./frame/oi_tea.o\
    ./comm/tfc_cache_proc.o \
    ./comm/tfc_net_ipc_mq.o \
    ./comm/tfc_net_epoll_flow.o \
    ./comm/tfc_net_conn_map.o \
    ./comm/tfc_debug_log.o \
    ./comm/tfc_ipc_sv.o \
    ./comm/tfc_error.o \
    ./comm/tfc_stat_v2.o \
    ./comm/tfc_thread_sync.o \
    ./comm/tfc_base_config_file.o \
    ./hls/hls_buf.o\
    ./hls/hls_cache.o\
    ./hls/hls_stream.o\
    ./hls/hls_bitread.o\
    ./hls/hls_mpegts.o\
    ./hls/hls_utils.o\
    ./hls/hls_core.o

all: $(TARGET) hlscheck

$(TARGET):$(OBJ)
	$(CXX) $(CFLAGS) -g -o $@ $^ $(LIB) -shared
	install $(TARGET) ./running/bin/
	md5sum $(TARGET)
    
hlscheck: hls_check.o hls/hls_cache.o muxsvr_mcd_cfg.o ./pub/gFunc.o comm/tfc_base_config_file.o
	$(CXX) $(CFLAGS) -g -o $@ $^ $(LIB)

%.o: %.cpp
	$(CXX)  $(CFLAGS) $(INC) -c -o $@ $<
%.o: %.cc
	$(CXX)  $(CFLAGS) $(INC) -c -o $@ $<
%.o: %.c
	$(CXX)  $(CFLAGS) $(INC) -c -o $@ $<

install:
	scp -i /home/tunx/OpenSSH $(TARGET) root@10.40.18.41:/home/3.0/muxsvr/bin

clean:
	rm -f *.o
	rm -f *.so
	rm -f flv/*.o
	rm -f hls/*.o
	rm -f protos/*.o
	rm -f $(TARGET)

allclean:
	rm -rf comm/*.o
	rm -rf complete/*.o
	rm -rf complete/*.so
	rm -rf frame/*.o
	rm -f flv/*.o
	rm -f protos/*.o
	rm -f json/*.o
	rm -f *.o
	rm -f *.so
	rm -f $(TARGET)

love: clean $(TARGET) hlscheck



#build bal_agent

cc = g++ 
#flags = -W -g -march=i486
flags = -W -g

SPARROW_HOME=../../lib/include
PROTOBUF=../../protobuf
PROTOS=../../protos

INC= -I$(SPARROW_HOME) -I. -I../.. -I../../pub -I../../comm -I../../frame -I$(PROTOS) -I$(PROTOBUF)/include
LIBS=../../lib/lib/libsparrow.a $(PROTOBUF)/lib/libprotobuf.a -lz -lpthread

TARGET=showshm

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp)) \
		../../muxsvr_mcd_cfg.o \
		../../pub/gFunc.o \
		../../frame/fifo_loop_buffer.o\
		../../frame/msg_process.o\
		../../frame/mx_thread.o\
		../../frame/mx_event.o\
		../../frame/timer_base.o\
		../../frame/timer_assist.o\
		../../frame/timer_service.o\
		../../comm/tfc_cache_proc.o \
		../../comm/tfc_net_ipc_mq.o \
		../../comm/tfc_net_epoll_flow.o \
		../../comm/tfc_net_conn_map.o \
		../../comm/tfc_debug_log.o \
		../../comm/tfc_ipc_sv.o \
		../../comm/tfc_error.o \
		../../comm/tfc_stat_v2.o \
		../../comm/tfc_thread_sync.o \
		../../comm/tfc_base_config_file.o

%.o: %.cpp
	$(cc) $(flags) $(INC)  -c $< -o $@ 

%.o: %.cc
	$(cc) $(flags) $(INC)  -c $< -o $@ 

%.o: %.c
	$(cc) $(flags) $(INC)  -c $< -o $@ 

$(TARGET): $(OBJS)
	$(cc) -o $(TARGET) $(OBJS) $(LIBS)
	cp $(TARGET) ../../running/tools/op

clean:
	rm -f ../../muxsvr_mcd_cfg.o
	rm -f $(PROTOS)/namesvr.pb.o
	rm -f *.o
	rm -f showshm
	

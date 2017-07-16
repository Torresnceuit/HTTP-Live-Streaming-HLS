CC=g++
CXX=g++

QTLIBS=../lib

INC= -I../frame -I../comm  -I../pub -I$(QTLIBS)
LIB=/lib/libc.so.6 /lib/libuuid.so.1 $(QTLIBS)/attr_api.a $(QTLIBS)/libcomm_oi.a -lz -lpthread
CFLAGS= -Wall -g 
TARGET=muxsvr_submcd_log_run.so

OBJ+=muxsvr_submcd_log_proc.o \
    muxsvr_submcd_log_cfg.o\
    ../pub/buffer.o\
    ../pub/gFunc.o\
    ../pub/submcd_log.o\
    ../frame/oi_tea.o\
    ../comm/tfc_cache_proc.o \
    ../comm/tfc_net_ipc_mq.o \
    ../comm/tfc_net_epoll_flow.o \
    ../comm/tfc_net_conn_map.o \
    ../comm/tfc_debug_log.o \
    ../comm/tfc_ipc_sv.o \
    ../comm/tfc_error.o \
    ../comm/tfc_stat_v2.o \
    ../comm/tfc_thread_sync.o \
    ../comm/tfc_base_config_file.o 
	

$(TARGET):$(OBJ)
	$(CXX) $(CFLAGS) -o $@ $^ $(LIB) -shared
	install $(TARGET) ../running/bin/

%.o: %.cpp
	$(CXX)  $(CFLAGS) $(INC) -c -o $@ $< 	 
%.o: %.c
	$(CXX)  $(CFLAGS) $(INC) -c -o $@ $<
clean:
	rm -f *.o
	rm -f *.so
	rm -f $(TARGET)

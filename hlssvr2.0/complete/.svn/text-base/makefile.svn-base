INC += -I../ -I../comm/
LIB	+= -ldl -lpthread
BIN	:= cs_udp_complete.so cs_tcp_complete.so ss_udp_complete.so http_complete.so ss_tcp_complete.so
all: $(BIN)
cs_udp_complete.so: cs_udp_complete.o
	g++ -Wall -o $@ $(INC) $^ -shared
	install cs_udp_complete.so ../running/bin
cs_tcp_complete.so: cs_tcp_complete.o http_head.o http_req.o http_rsp.o http_func.o
	g++ -Wall -o $@ $(INC) $^ -shared
	install cs_tcp_complete.so ../running/bin
ss_udp_complete.so: ss_udp_complete.o
	g++ -Wall -o $@ $(INC) $^ -shared
	install ss_udp_complete.so ../running/bin
ss_tcp_complete.so: ss_tcp_complete.o http_head.o http_req.o http_rsp.o http_func.o
	g++ -Wall -o $@ $(INC) $^ -shared
	install ss_tcp_complete.so ../running/bin
http_complete.so: http_complete.o http_head.o http_req.o http_rsp.o http_func.o
	g++ -Wall -o $@ $(INC) $^ -shared
	install http_complete.so ../running/bin
%.o: %.cpp
	g++ -g -Wall $(INC) -c -o $@ $<
clean: 
	rm -rfv *.o *.out *~ core* $(BIN)

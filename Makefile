# default rule
all: udpreplay udpplay

clean:
	rm -f *.o udpreplay udpplay

pcap.o: pcap.c pcap.h ip.h tcp.h udp.h

udpreplay.o: udpreplay.c pcap.h

udpreplay: udpreplay.o pcap.o

udpplay: LDFLAGS += -lrt
udpplay: udpplay.c


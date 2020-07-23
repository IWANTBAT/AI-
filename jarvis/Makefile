LDFLAGS=-ljsoncpp -lcurl -lcrypto


jarvis:jarvis.cc
	g++ -o $@ $^ $(LDFLAGS) -std=c++11

.PHONY:clean
	clean:
	rm -f Jarvis

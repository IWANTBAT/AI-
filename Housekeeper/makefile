LDFLAGS=-ljsoncpp -lcurl -lcrypto
kepper:kepper.cpp
	g++ $^ -o  $@ $(LDFLAGS) -std=c++11
.PHONY:clean
clean:
	rm -f kepper

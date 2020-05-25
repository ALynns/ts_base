curPath=$(shell pwd)
clientPath = ../client
commonPath = ../common

object =  client

head = $(commonPath)/*.h $(clientPath)/*.h 
cppFile = $(commonPath)/*.cpp $(clientPath)/*.cpp 

.PHONY : all clean
all : $(object)

client : 
	g++ -o $(object) -Wall -std=c++11 $(head) $(cppFile)

gdbDebug : 
	g++ -g -rdynamic -Wall -std=c++11 $(head) $(cppFile)

clean :
	-rm -rf $(object) *.o *.out *.gch
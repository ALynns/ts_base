curPath=$(shell pwd)
serverPath = ../server
commonPath = ../common
mysqlLib = `mysql_config --cflags --libs`

object =  server

head = $(commonPath)/*.h $(serverPath)/*.h 
cppFile = $(commonPath)/*.cpp $(serverPath)/*.cpp 

.PHONY : all clean
all : $(object)

server : 
	g++ -o $(object) -Wall -std=c++11 $(head) $(cppFile) $(mysqlLib)

gdbDebug : 
	g++ -g -rdynamic -Wall -std=c++11 $(head) $(cppFile) $(mysqlLib)

clean :
	-rm -rf $(object) *.o *.out *.gch *.log *.xls
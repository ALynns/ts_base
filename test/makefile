curPath=$(shell pwd)
commonPath = ../common

object =  test

head = $(commonPath)/*.h 
cppFile = $(commonPath)/*.cpp $(curPath)/*.cpp 

.PHONY : all clean
all : $(object)

test : 
	g++ -o $(object) -Wall -std=c++11 $(head) $(cppFile)

clean :
	-rm -rf $(object) *.o *.out *.gch *.log
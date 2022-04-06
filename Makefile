#AR 库文件维护程序的名称。默认值为ar
#AS 汇编程序的名称，默认值为as
#CPP C预编译器的名称，默认值为$(CC) -E
#CXX C++编译器的名称，默认值为g++
#FC   FORTRAN编译器的名称，默认值为f77
#ARFLAGS   库文件维护的程序选项，无默认值
#ASFLAGS   汇编程序的选项，无默认值
#CFLAGS     C编译器的选项，无默认值
#CPPFLAGS C预编译的选项，无默认值
#CXXFLAGS C++编译器的选项，无默认值
#FFLAGS      FORTRAN编译器的选项，无默认值

CC ?= gcc
CXX ?= g++

SOURCES += $(wildcard $(CURDIR)/src/*.cpp)

INC_CLUDE = -I$(CURDIR)/inc

objects =  $(patsubst %.cpp, %.o, $(SOURCES))

server : $(objects)
	$(CXX) -o  $@ -g $(objects)

$(objects) : %.o:%.cpp
	$(CXX) -c -g  $< -o $@ $(INC_CLUDE)

clean :
	rm *.o server

BUILD_MODE = debug
PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

CXXFLAGS= -std=c++11 -Wall 

SRCS = $(wildcard *.cpp)

OBJS = $(patsubst %.cpp,%.o,$(SRCS))

ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g -DDEBUG
else ifeq ($(BUILD_MODE),run)
	CFLAGS += -O2
else
	$(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

ifeq ($(TARGET),pthread_test)
	CFLAGS += -pthread -DPTHREAD_TEST
else ifeq ($(TARGET),mpi_test)
	CFLAGS += -DMPI_TEST
endif

ifeq ($(TARGET),)

.PHONY: all pthread_test mpi_test
all: pthread_test

pthread_test:
	@rm -rf *.o
	$(MAKE) -C . TARGET="pthread_test" build
mpi_test:
	@rm -rf *.o
	$(MAKE) -C . TARGET="mpi_test" build
cscope:
	find -name "*.cpp" -or -name "*.h" > cscope.files
	cscope -qb
else

build:$(TARGET)


$(TARGET):$(OBJS)
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(CPPFLAGE) -o $@ $^


%.o:	$(PROJECT_ROOT)%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

endif

clean:
	rm -fr *.o
	rm -rf pthread_test mpi_test
	rm -rf core.*
	rm -rf cscope.* cscope.*.*

CXX = g++
RM = rm -f
CXXFLAGS = --std=c++11
CXXFLAGS += `pkg-config --libs --cflags opencv`

GLOBAL = global.hpp
TARGET = tester
SRCS = $(TARGET).cpp imgDatabase.cpp dict.cpp
OBJS = $(subst .cpp,.o,$(SRCS))

VPATH = ./Util

.PHONY: all clean clean-obj clean-exec

all: $(TARGET)
clean: clean-obj clean-exec

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp %.hpp $(GLOBAL)
	$(CXX) $(CXXFLAGS) -c $<

clean-obj:
	$(RM) *.o

clean-exec:
	$(RM) $(TARGET)

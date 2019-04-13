CXX = g++
ARGS = -O2
LIBS = -lvai-c -lhardcloud -lMPF_VAI
TARGET = md5
SRCS = main.cpp
SRCS_FILES = $(foreach F, $(SRCS), $(F))

all:
	$(CXX) $(ARGS) $(LIBS) $(SRCS_FILES) -o $(TARGET)

clean:
	rm -rf $(TARGET)

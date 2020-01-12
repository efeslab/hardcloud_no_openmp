CXX = g++
ARGS = -O2 -g
LIBS = -L/usr/local/lib64 -lopae-c -lhardcloud -lMPF
TARGET = md5
SRCS = main.cpp
SRCS_FILES = $(foreach F, $(SRCS), $(F))

all:
	$(CXX) $(ARGS) $(LIBS) $(SRCS_FILES) -o $(TARGET)

clean:
	rm -rf $(TARGET)

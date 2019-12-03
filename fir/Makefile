CXX = g++
ARGS = -O2 -g
LIBS = -lvai-c-ase -lhardcloud -lMPF_VAI
TARGET = fir
SRCS = main.cpp
SRCS_FILES = $(foreach F, $(SRCS), $(F))

all:
	$(CXX) $(ARGS) $(LIBS) $(SRCS_FILES) -o $(TARGET)

clean:
	rm -rf $(TARGET)

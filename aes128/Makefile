CXX = g++
ARGS = -O2
LIBS = -L/usr/local/lib64 -lvai-c-ase -lhardcloud -lMPF_VAI
TARGET = aes128
SRCS = main.cpp
SRCS_FILES = $(foreach F, $(SRCS), $(F))

all:
	$(CXX) $(ARGS) $(LIBS) $(SRCS_FILES) -o $(TARGET)

clean:
	rm -rf $(TARGET)

CXX = g++
ARGS = -O2
LIBS = -lopae-c -lhardcloud -lMPF
TARGET = reed_solomon_decoder
SRCS = main.cpp
SRCS_FILES = $(foreach F, $(SRCS), $(F))

all:
	$(CXX) $(ARGS) $(LIBS) $(SRCS_FILES) -o $(TARGET)

clean:
	rm -rf $(TARGET)

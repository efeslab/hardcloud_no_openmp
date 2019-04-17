CXX = g++
ARGS = -O2 -g
LIBS = -lvai-c -lhardcloud -lMPF_VAI -lpng
TARGET = gaussian
SRCS = main.cpp image.cpp
SRCS_FILES = $(foreach F, $(SRCS), $(F))

all:
	$(CXX) $(ARGS) $(LIBS) $(SRCS_FILES) -o $(TARGET)

clean:
	rm -rf $(TARGET)

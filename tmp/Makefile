CCLIB=-lsdsl -ldivsufsort -ldivsufsort64 -Wno-comment 

LIB_DIR = ${HOME}/lib
INC_DIR = ${HOME}/include
MY_CXX_FLAGS= -std=c++14 

MY_CXX_OPT_FLAGS= -O3 -m64 
CXX=g++

CXX_FLAGS=$(MY_CXX_FLAGS) $(MY_CXX_OPT_FLAGS) -I$(INC_DIR) -L$(LIB_DIR) $(LFLAGS) $(DEFINES)

all: naive msa cmsa


naive: naive.cpp
	$(CXX) $^ $(CCLIB) -o $@ $(CXX_FLAGS)

msa: msa.cpp
	$(CXX) $^ $(CCLIB) -o $@ $(CXX_FLAGS)

cmsa: cmsa.cpp
	$(CXX) $^ $(CCLIB) -o $@ $(CXX_FLAGS)

test: naive msa 
	./naive < exemplo-fig1-paper.in
	./msa < exemplo-fig1-paper.in

run: cmsa
	./cmsa < exemplo-fig1-paper.in

clean:
	rm -f msa naive

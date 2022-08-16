
all: naive msa


naive: naive.cpp
	g++ naive.cpp -o naive

msa: msa.cpp
	g++ msa.cpp -o msa 

test: naive msa
	./naive < exemplo-fig1-paper.in
	./msa < exemplo-fig1-paper.in



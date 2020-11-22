
CXX = g++
CXXFLAGS = -O3 -pipe -Wall -Wextra -Wpedantic
LIBS = -lgmpxx

ZetaStack.exe: ZetaStack.o Zetacompiler.o Execute.o
	$(CXX) -o $@ $^

ZetaStack.o: ZetaStack.cpp Zetacompiler.hpp Execute.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

Zetacompiler.o: Zetacompiler.cpp Zetacompiler.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

Execute.o: Execute.cpp Execute.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)


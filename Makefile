CXX = g++
CXXFLAGS = -O3 -pipe -Wall -Wextra -Wpedantic
LIBS = -lgmpxx

ZetaStack.exe: src/ZetaStack.o src/Zetacompiler.o src/Execute.o
	$(CXX) -o $@ $^

src/ZetaStack.o: src/ZetaStack.cpp src/Zetacompiler.hpp src/Execute.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Zetacompiler.o: src/Zetacompiler.cpp src/Zetacompiler.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Execute.o: src/Execute.cpp src/Execute.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)




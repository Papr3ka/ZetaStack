
CXX = g++
CXXFLAGS = -O3 -g -pipe -Wall -Wextra -Wpedantic
LIBS = -lgmpxx

ZetaStack.exe: src/ZetaStack.o src/Zetacompiler.o src/Execute.o src/Preprocessor.o src/variable.o src/Cache.o
	$(CXX) -o $@ $^ $(CXXFLAGS)

src/ZetaStack.o: src/ZetaStack.cpp src/Zetacompiler.hpp src/Execute.hpp src/variable.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/variable.o: src/variable.cpp src src/variable.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Zetacompiler.o: src/Zetacompiler.cpp src/Zetacompiler.hpp src/variable.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Preprocessor.o: src/Preprocessor.cpp src/Preprocessor.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Execute.o: src/Execute.cpp src/Execute.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Cache.o: src/Cache.cpp src/Cache.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)


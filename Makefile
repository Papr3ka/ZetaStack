CXX = g++
CXXFLAGS = -O3 -pipe -Wall -Wextra -Wpedantic -Wdouble-promotion -pthread -march=native -finline-functions -ftree-vectorize -frename-registers -fno-rtti
LDFLAGS = -flto
LD = g++

ZetaStack.exe: src/ZetaStack.o src/Zetacompiler.o src/Execute.o src/Preprocessor.o src/Variable.o src/Cache.o src/Entropy.o src/Function.o src/Status.o src/Analyzer.o src/BuiltIn.o
	$(LD) -o $@ $^ $(CXXFLAGS) -Wl,$(LDFLAGS)

src/ZetaStack.o: src/ZetaStack.cpp src/Zetacompiler.hpp src/Preprocessor.hpp src/Execute.hpp src/Function.hpp src/Cache.hpp src/Status.hpp src/Token.hpp src/Analyzer.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Variable.o: src/Variable.cpp src src/Variable.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Zetacompiler.o: src/Zetacompiler.cpp src/Zetacompiler.hpp src/Token.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Preprocessor.o: src/Preprocessor.cpp src/Preprocessor.hpp src/Token.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Execute.o: src/Execute.cpp src/Execute.hpp src/Token.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Cache.o: src/Cache.cpp src/Cache.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Entropy.o: src/Entropy.cpp src/Entropy.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Function.o: src/Function.cpp src/Function.hpp src/Token.hpp src/BuiltIn.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Status.o: src/Status.cpp src/Status.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/Analyzer.o: src/Analyzer.cpp src/Analyzer.hpp src/Token.hpp 
	$(CXX) -c -o $@ $< $(CXXFLAGS)

src/BuiltIn.o: src/BuiltIn.cpp src/BuiltIn.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)
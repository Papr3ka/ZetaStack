# Compiler settings - Can be customized.
CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -Wpedantic -pthread -frename-registers -march=native -flto
LDFLAGS = 
LD = g++

# g++ release flags
# -O3 -Wall -Wextra -Wpedantic -pthread -frename-registers -march=native -flto

# g++ devel/debug flags
# -O0 -g3 -Wall -Wextra -Wpedantic -pthread

APPNAME = ZetaStack.exe

all: $(APPNAME)

$(APPNAME): Analyzer.o Builtin.o Cache.o Entropy.o Execute.o Function.o Main.o Preprocessor.o Status.o Variable.o Zetacompiler.o ZetaStack.o
	$(LD) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

Analyzer.o: src/Analyzer.cpp src/Analyzer.hpp src/ZetaStack.hpp src/Function.hpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

Builtin.o: src/Builtin.cpp src/Builtin.hpp src/ZetaStack.hpp src/Execute.hpp src/Status.hpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

Cache.o: src/Cache.cpp src/Cache.hpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

Entropy.o: src/Entropy.cpp src/Entropy.hpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

Execute.o: src/Execute.cpp src/Execute.hpp src/ZetaStack.hpp src/Cache.hpp src/Function.hpp src/Variable.hpp src/Zetacompiler.hpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

Function.o: src/Function.cpp src/Function.hpp src/ZetaStack.hpp src/Builtin.hpp src/Execute.hpp src/Zetacompiler.hpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

Main.o: src/Main.cpp src/ZetaStack.hpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

Preprocessor.o: src/Preprocessor.cpp src/Preprocessor.hpp src/ZetaStack.hpp src/Zetacompiler.hpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

Status.o: src/Status.cpp src/Status.hpp src/ZetaStack.hpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

Variable.o: src/Variable.cpp src/Variable.hpp src/ZetaStack.hpp src/Builtin.hpp src/Entropy.hpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

Zetacompiler.o: src/Zetacompiler.cpp src/Zetacompiler.hpp src/ZetaStack.hpp src/Function.hpp src/Variable.hpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

ZetaStack.o: src/ZetaStack.cpp src/ZetaStack.hpp src/Analyzer.hpp src/Cache.hpp src/Execute.hpp src/Function.hpp src/Preprocessor.hpp src/Status.hpp src/Zetacompiler.hpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

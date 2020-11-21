@echo off
cd src
g++ Zetacompiler.cpp -c -O3 -pipe -ftime-report -lgmpxx -Wpedantic -Wextra -Wall -o Zetacompiler.o
g++ Execute.cpp -c -O3 -pipe -ftime-report -lgmpxx -Wpedantic -Wextra -Wall -o Execute.o
g++ ZetaStack.cpp -c -O3 -pipe -ftime-report -lgmpxx -Wpedantic -Wextra -Wall -o ZetaStack.o
g++ Execute.o ZetaStack.o Zetacompiler.o -ftime-report -lgmpxx -o ZetaStack.exe
ZetaStack.exe
#ifndef ENTROPY_HPP
#define ENTROPY_HPP

#include<chrono>
#include<thread>

long int urandom(long seed);

long int getrandnum(long seed);

bool getrandbit(long seed);

long int getrandl(long seed);

unsigned long long int getrandull(long seed);

#endif
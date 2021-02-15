/* Random Number Generator
 * 
 * Copyright (c) 2020-2021 Benjamin Yao.
 * 
 * This file is part of ZetaStack.
 * 
 * ZetaStack is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * ZetaStack is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include<bitset>
#include<chrono>
#include<cstdint>
#include<cstdlib>
#include<iostream>
#include<type_traits>

#include "Entropy.hpp"

uint32_t urandom(uint32_t seed){
    uint32_t wait = 1;
    uint32_t x = 2;

    /*
    The CIA encourages code obfuscation. They make it more complicated than necessary.
    —Terry A. Davis
    */

    uint32_t tepoch = std::chrono::duration_cast<std::chrono::milliseconds>
                        (std::chrono::system_clock::now().time_since_epoch()).count() >> 4;

    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;

    start = std::chrono::steady_clock::now();

    // Not obfuscated just hard to read
    for(uint32_t cnt = 0;
        cnt < (tepoch % 8191+65521) * seed %131071;
        ++cnt){

        wait *= x*x-x;
        x -= x;

    }
    end = std::chrono::steady_clock::now();
    return (std::chrono::duration<uint32_t, std::nano>(end - start).count() % 
           (tepoch % 8191+65521-seed)* seed %131071+wait) % 1021;
}

struct prngstate{
    uint64_t s1, s2;
};

static prngstate pstate;


void initseed(void){
    unsigned long int hang = 0;
    uint32_t temp = pstate.s1; // just take 32 bits

    pstate.s1 = ((uint64_t)getrandnum(pstate.s2) << 32) + (uint64_t)getrandnum(pstate.s1);
    pstate.s2 = ((uint64_t)getrandnum(temp) << 32) + (uint64_t)getrandnum(pstate.s2);

    while(!pstate.s1 && !pstate.s2 && 512 > hang){
        temp = pstate.s1;
        pstate.s1 = ((uint64_t)getrandnum(pstate.s2) << 32) + (uint64_t)getrandnum(pstate.s1);
        pstate.s2 = ((uint64_t)getrandnum(temp) << 32) + (uint64_t)getrandnum(pstate.s2);
        ++hang;
    }

    // The chance for this to happen is really small
    if(512 <= hang || !pstate.s2 || !pstate.s1){
        pstate = {0x58fd6fb33c9effc3,
                  0xfcb8a6412c8a1401};
    }

    psuedo_random_next();

    return;
}

uint64_t psuedo_random_next(void){
    uint64_t t = pstate.s1;
    pstate.s1 = pstate.s2;
    t = pstate.s2;
    t ^= t << 23;
    t ^= t >> 17;
    t ^= pstate.s2 ^ (pstate.s2 >> 26);
    //if(pstate.s1 == 0) pstate.s1 |= 0xBF58476D1CE4E5B9;
    pstate.s2 = t;
    return pstate.s1+pstate.s2;
}

uint32_t getrandnum(uint32_t seed){
    uint32_t t = urandom(seed);
    t ^= t << 23;
    t ^= t >> 17;
    t ^= seed ^ (seed >> 26);
    return urandom(t^seed)-t*seed;
}

bool getrandbit(uint32_t seed){
    return (getrandnum(seed) ^ seed) % 2;
}

uint32_t getrandl(uint32_t seed){
    uint32_t output = 0;
    for(uint32_t bitpos = 0; bitpos < 32; bitpos++){
        if(getrandbit(seed)){
            output |= 1 << bitpos;
        }
    }
    return (uint32_t)(output+seed) ^ seed;
}

uint64_t getrandull(long seed){
    uint64_t output = 0;
    for(uint64_t bitpos = 0; bitpos < 64; bitpos++){
        if(getrandbit(seed)){
            output |= 1 << bitpos;
        }
    }
    return output;
}


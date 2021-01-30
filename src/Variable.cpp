/* Interface for creating, modifying and reading variables
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

#include<algorithm>
#include<atomic>
#include<cctype>
#include<cstdint>
#include<cstdlib>
#include<string>
#include<thread>
#include<unordered_map>
#include<vector>

#include "BuiltIn.hpp"
#include "Entropy.hpp"
#include "Variable.hpp"

namespace var{

    std::unordered_map<std::string, std::string> variabletable;

    std::string mostrecentiden;
    std::string mostrecentvar;

    std::atomic<unsigned long int> bufferindex(0);
    std::atomic<unsigned long int> buffermax(1024);

    bool runbuffer = true;

    uint32_t prevr = rand() % 769 + 47; // for random nums

    static std::vector<uint32_t> randbuffer;

    static inline uint32_t getrand(void){
        uint32_t retval;
        if(!randbuffer.empty()){
            retval = randbuffer.front();
            randbuffer.erase(randbuffer.begin());
            return retval%1000;
        }else{
            prevr = getrandnum(prevr) ^ 127; // Good luck trying to reverse engineer it
            return prevr%1000;
        }
    }

    unsigned long int getrandbufsize(void){
        return randbuffer.size();
    }

    void setbuffermax(unsigned long int setval){
        bufferindex = 0;
        buffermax = setval;
        randbuffer.clear();
        std::vector<uint32_t>().swap(randbuffer);
        return;
    }

    unsigned long int getbuffermax(void){
        return buffermax;
    }

    void clearbuffer(void){
        randbuffer.clear();
        std::vector<uint32_t>().swap(randbuffer);
        return;
    }

    void joinbuffer(void){
        runbuffer = false;
        randbuffer.clear();
        std::vector<uint32_t>().swap(randbuffer);
        return;
    }

    bool changable(std::string iden){
        if(specialIden.find(iden) == specialIden.end()){
            return true;
        }else{
            return false;
        }
    }

    bool exists(std::string iden){
        if(variabletable.find(iden) == variabletable.end()){
            return false;
        }else{
            return true;
        }
    }

    static inline bool validname(const std::string& str){
        for(char x: str){
            if(ispunct(x) && x != '_'){
                return false;
            }
        }
        return true;
    }

    // Add variable to vector
    void update(std::string iden, const std::string& val){
        if(iden.empty() || !validname(iden)){
            throw std::string("Invalid variable name");
        }
        if(safe_mode && !exists(iden) && variabletable.size() > maxobj){
            std::string error = "Unable to assign variable";
            throw error;
        }
        mostrecentvar = val;
        mostrecentiden = iden;
        variabletable[iden] = val;
        return;

    }

    // Return var value if identifier is found else return NULL
    std::string search(std::string iden, bool nothrow){
        if(variabletable.find(iden) == variabletable.end()){
            if(iden == "rand") return std::to_string(getrand());
            if(specialIden.find(iden) != specialIden.end()){
                return specialIden[iden];
            }
            if(nothrow){
                return "NULL";
            }else{
                std::string error = "Undefined Variable: \"";
                error.append(iden).append("\"");
                throw error;				
            }
        }else{
            return variabletable[iden];
        }
        
    }

    std::string	mostrecent(void){
        if(mostrecentiden == "rand"){
            return std::to_string(getrand());
        }
        return mostrecentvar;
    }

    // Wrapper for amount of variables
    unsigned long int count(void){
        return variabletable.size();
    }

    // Delete variable, return 1 if not present 0 if success
    // 2 if cannot be deleted
    int delvar(std::string variden){
        if(specialIden.find(variden) != specialIden.end()){
            return 2;
        }
        if(variabletable.find(variden) == variabletable.end()){
            return 1;
        }else{
            variabletable.erase(variabletable.find(variden));
            return 0;
        }
    }

    void delAll(void){
        variabletable.clear();
        std::unordered_map<std::string, std::string>().swap(variabletable);
        return;
    }

    // Wrapper for all names
    std::vector<std::string> globals(void){
        std::vector<std::string> output;
        output.reserve(variabletable.size());
        for(std::unordered_map<std::string, std::string>::iterator
            itx = variabletable.begin();
            itx != variabletable.end();
            ++itx){

            output.emplace_back(itx->first);
        }
        return output;
    }

    std::vector<std::string> specials(void){
        std::vector<std::string> output;
        output.reserve(specialIden.size());
        for(std::unordered_map<std::string, std::string>::iterator
            itx = specialIden.begin();
            itx != specialIden.end();
            ++itx){

            output.emplace_back(itx->first);
        }
        return output;
    }

    // Target to thread
    void buffer(bool run){
        if(!run){
            return;
        }
        while(runbuffer && run){
            if(randbuffer.size() < buffermax){
                while(randbuffer.size() < buffermax){
                    prevr = getrandnum(prevr) ^ 127;
                    randbuffer.emplace_back(prevr);
                }
            }else if(randbuffer.size() >= buffermax){
                std::this_thread::sleep_for(std::chrono::milliseconds(125));
                if(!randbuffer.empty()){
                    randbuffer.erase(randbuffer.begin());
                    std::this_thread::sleep_for(std::chrono::milliseconds(125));
                    std::vector<uint32_t>(randbuffer).swap(randbuffer);
                }
                bufferindex++;
            }else{
                std::this_thread::sleep_for(std::chrono::milliseconds(125));
                std::vector<uint32_t>(randbuffer).swap(randbuffer);
            }
        }
        return;
    }
}

// Like substr but for vectors
void subVec(std::vector<std::string>& vec, unsigned long int start, unsigned long intend){
    std::vector<std::string> output;
    if((long int)intend - (long int)start >= 0){
        output.reserve(intend - start);
    }
    for(;start < intend; start++){
        output.emplace_back(vec[start]);
    }
    vec = output;
    return;
}
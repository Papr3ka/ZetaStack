#include<algorithm>
#include<atomic>
#include<cctype>
#include<cstdlib>
#include<string>
#include<thread>
#include<vector>
#include<unordered_map>

#include "BuiltIn.hpp"
#include "Entropy.hpp"
#include "Variable.hpp"

namespace var{

    std::unordered_map<std::string, std::string> variabletable;

    std::string mostrecentiden;
    std::string mostrecentvar;

    std::atomic<unsigned long int> buffermax(4096);
    std::atomic<unsigned long int> bufferindex(0);

    bool runbuffer = true;

    long prevr = rand() % 769 + 47; // for random nums

    static std::vector<long int> randbuffer;

    static inline long getrand(void){
        long int retval;
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
        std::vector<long int>().swap(randbuffer);
        return;
    }

    unsigned long int getbuffermax(void){
        return buffermax;
    }

    void clearbuffer(void){
        randbuffer.clear();
        std::vector<long int>().swap(randbuffer);
        return;
    }

    void joinbuffer(void){
        runbuffer = false;
        randbuffer.clear();
        std::vector<long int>().swap(randbuffer);
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

    // Add variable to vector
    void update(std::string iden, std::string val){
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

    // Wrapper for all names
    std::vector<std::string> globals(void){
        std::vector<std::string> output;
        output.reserve(variabletable.size());
        for(std::unordered_map<std::string, std::string>::iterator
            itx = variabletable.begin();
            itx != variabletable.end();
            itx++){

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
            itx++){

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
                    randbuffer.shrink_to_fit();
                }
                bufferindex++;
            }else{
                std::this_thread::sleep_for(std::chrono::milliseconds(125));
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
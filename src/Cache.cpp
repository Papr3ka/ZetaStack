#include<algorithm>
#include<iostream>
#include<string>
#include<unordered_map>

#include "Cache.hpp"

// Cannot use hash table because of the metadata and how ordered the cache needs to be
namespace cch {

    // 128 entries max
    unsigned long int maxlen = 128; // How big the cache can be before discarding

    bool cache_enable = true;

    std::unordered_map<std::string, std::string> anscache;

    void setmaxlen(unsigned long int x){
        maxlen = x;
        while(anscache.size() >= maxlen &&
              anscache.size() > 0){
            anscache.erase(anscache.begin());
        }
        return;
    }

    void addentry(std::string key, std::string ans){
        if(do_cache){
            if(anscache.size() >= maxlen &&
               anscache.size() > 0){
                anscache.erase(anscache.begin());
            }
            anscache[key] = ans;
            return;
        }else{
            return;
        }

    }

    std::string searchentry(std::string key){
        if(do_cache){
            if(anscache.find(key) != anscache.end()){
                return anscache[key];
            }else{
                return "";
            }
        }else{
           return ""; 
        }
    }

    std::vector<ansctable> getentries(void){
        std::vector<ansctable> retvec;
        retvec.reserve(anscache.size());
        for(std::unordered_map<std::string, std::string>::iterator
            ita = anscache.begin();
            ita != anscache.end();
            ita++){
            retvec.emplace_back(ansctable(ita->first, ita->second));
        }
        return retvec;
    }

    void clearcache(void){
        anscache.clear();
        std::unordered_map<std::string, std::string>().swap(anscache);
        return;
    }




}
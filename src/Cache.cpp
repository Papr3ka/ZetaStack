/* Answer cache wrapper for hashtable.
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
#include<iostream>
#include<string>
#include<unordered_map>

#include "Cache.hpp"

// Cannot use hash table because of the metadata and how ordered the cache needs to be
namespace cch {

    // 128 entries max
    static unsigned long int maxlen = 128; // How big the cache can be before discarding

    std::unordered_map<std::string, std::string> anscache;


    void setmaxlen(unsigned long int x){
        maxlen = x;
        while(anscache.size() >= maxlen &&
              anscache.size() > 0){

            // Remove entries
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

    // Searches for entry
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

    // Returns all entries
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

    // Clear and free memory
    void clearcache(void){
        anscache.clear();
        std::unordered_map<std::string, std::string>().swap(anscache);
        return;
    }

}
/* Syntax analysis and token order Checking.
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

#include<string>
#include<vector>

#include "Analyzer.hpp"
#include "Function.hpp"
#include "ZetaStack.hpp"

// Check if there is an unexpected token
// true = Pass, false = Fail
bool checkrpn(const std::vector<token>& checktokens){

    // Must be Signed int
    signed long long int simulatestack = 0;
    for(unsigned long int index = 0; index < checktokens.size(); ++index){
        switch(checktokens[index].type){
            case tok::hold:
            case tok::str:
            case tok::var:
            case tok::num:
                ++simulatestack;
                break;
            case tok::op:
                if(checktokens[index].data == "NEG"){
                    if(simulatestack - 1 < 0) return false;
                    break;
                }else if(checktokens[index].data == "POS"){
                    if(simulatestack - 1 < 0) return false;
                    break;
                }else if(checktokens[index].data == "NOT"){
                    if(simulatestack - 1 < 0) return false;
                    break;
                }else{
                    if(simulatestack - 2 < 0) return false;
                    simulatestack--;
                }
                break;
            case tok::func:
                simulatestack -= (signed long long int)checktokens[index].reserved;
                if(returns(checktokens[index].data, checktokens[index].reserved)) ++simulatestack;
                break;
            case 6:
                break;
            case tok::asn:
                --simulatestack;
                --simulatestack;
                if(simulatestack < 0) return false;
                break;
            default:
                ++index;
                break;
        }
        if(simulatestack < 0){
            return false;
        }
    }
    return true;
}

// Counts if there is a quote mismatch
// Only counts single quotes
bool squotecount(const std::string& str){
    bool instring = false;
    bool indstr = false;
    for(unsigned long int x=0;x < str.size();x++){
        if(str[x] == '"'){
            if(x > 0 && str[x-1] != '\\'){
                indstr = !indstr;
            }else if(x == 0){
                indstr = !indstr;
            }
        }		
        if(str[x] == '\'' && !indstr){
            if(x > 0 && str[x-1] != '\\'){
                instring = !instring;
            }else if(x == 0){
                instring = !instring;
            }
        }
    }
    return instring;
}


// Only counts double quotes
bool quotecount(const std::string& str){
    bool instring = false;
    bool indstr = false;
    for(unsigned long int x=0;x < str.size();x++){
        if(str[x] == '\'' && !indstr){
            if(x > 0 && str[x-1] != '\\'){
                indstr = !indstr;
            }else if(x == 0){
                indstr = !indstr;
            }
        }			
        if(str[x] == '"' && !indstr){
            if(x > 0 && str[x-1] != '\\'){
                instring = !instring;
            }else if(x == 0){
                instring = !instring;
            }
        }
    }
    return instring;
}

// Error Checking
int checkleftBrac(const std::string& str){
    int count = 0;
    bool instring = false;
    for(unsigned long int idx = 0; idx < str.size(); idx++){
        if(str[idx] == '"'){
            if(idx > 0 && str[idx-1] != '\\'){
                instring = !instring;
            }else if(idx == 0){
                instring = !instring;
            }
        }
        if(str[idx] == '(' && !instring) count++;
    }
    return count;
}

int checkrightBrac(const std::string& str){
    int count = 0;
    bool instring = false;
    for(unsigned long int idx = 0; idx < str.size(); idx++){
        if(str[idx] == '"'){
            if(idx > 0 && str[idx-1] != '\\'){
                instring = !instring;
            }else if(idx == 0){
                instring = !instring;
            }
        }
        if(str[idx] == ')') count++;
    }
    return count;
}


bool bracketOrder(const std::vector<token>& tokens){
    long long int layer = 0;
    for(const token& tk: tokens){
        if(tk.type == tok::lbrac) ++layer;
        if(tk.type == tok::rbrac) --layer;
        if(layer < 0) return false;
    }
    return true;
}


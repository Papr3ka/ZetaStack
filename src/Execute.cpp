/* Evaluate an array of tokens 
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
#include<array>
#include<cmath>
#include<cstdlib>
#include<iomanip>
#include<iostream>
#include<limits>
#include<sstream>
#include<string>
#include<vector>
#include<unordered_map>

#include "Cache.hpp"
#include "Execute.hpp"
#include "Function.hpp"
#include "Status.hpp"
#include "Variable.hpp"
#include "ZetaStack.hpp"
#include "Zetacompiler.hpp"

namespace xmath {

    long long int recursionCount = 0;

    inline std::string to_string_hprec(double x){
        std::ostringstream doublestring;
        doublestring << std::setprecision(16) << x;
        return doublestring.str();
    }

    void resetsstreamsettings(void){
        std::stringstream strstream(std::stringstream::in | std::stringstream::out);
        strstream << std::setprecision(6);
        return;
    }

    inline std::string to_string_hprec(long long x){
        std::ostringstream llstring;
        llstring << std::setprecision(std::numeric_limits<long long>::digits10) << x;
        return llstring.str();
    }

    inline std::string to_string_hprec(bool x){
        if(x){
            return "1";
        }else{
            return "0";
        }
    }

    inline bool string_isnum(std::string str){
        if(str.size() <= 1){
            if(isdigit(str[0]) || str[0] == '.'){
                return true;
            }else{
                return false;
            }
        }else{ // eg -. would be considerd as -0
            if(isdigit(str[0]) || 
               isdigit(str[1]) || 
                 str[0] == '.' || 
                 str[1] == '.'){
                return true;
            }else{
                return false;
            }
        }
    }

    // Placeholder functions for Arbitrary math
    inline void add(std::vector<token>& evalstack){
        if(evalstack.back().type == tok::num){
            double y = std::stod(evalstack.back().data);
            evalstack.pop_back();
            double x = std::stod(evalstack.back().data);
            evalstack.back() = token(to_string_hprec(x+y), tok::num);
        }else{
            std::string y = evalstack.back().data;
            evalstack.pop_back();
            std::string x = evalstack.back().data;
            evalstack.back() = token(x.append(y), tok::str);
        }
        return;
    }

    inline void sub(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x - y), tok::num);
        return;
    }

    inline void mul(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x * y), tok::num);
        return;
    }

    inline void divide(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x / y), tok::num);
        return;
    }

    inline void mod(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(fmod(x, y)), tok::num);
        return;
    }

    inline void power(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(pow(x, y)), tok::num);
        return;
    }

    inline void bitexclusiveOr(std::vector<token>& evalstack){
        long long y = std::stoll(evalstack.back().data);
        evalstack.pop_back();
        long long x = std::stoll(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x ^ y), tok::num);
        return;
    }

    inline void shl(std::vector<token>& evalstack){
        long long y = std::stoll(evalstack.back().data);
        evalstack.pop_back();
        long long x = std::stoll(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x << y), tok::num);
        return;
    }

    inline void shr(std::vector<token>& evalstack){
        long long y = std::stoll(evalstack.back().data);
        evalstack.pop_back();
        long long x = std::stoll(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x >> y), tok::num);
        return;
    }

    inline void eql(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x == y), tok::num);
        return;
    }

    inline void nql(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x != y), tok::num);
        return;
    }

    inline void gql(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x >= y), tok::num);
        return;
    }

    inline void lql(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x <= y), tok::num);
        return;
    }

    inline void grt(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x > y), tok::num);
        return;
    }

    inline void lst(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x < y), tok::num);
        return;
    }

    inline void neg(std::vector<token>& evalstack){
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(-x), tok::num);
        return;
    }

    inline void bitwor(std::vector<token>& evalstack){
        long long y = std::stoll(evalstack.back().data);
        evalstack.pop_back();
        long long x = std::stoll(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x | y), tok::num);
        return;
    }

    inline void bitwand(std::vector<token>& evalstack){
        long long y = std::stoll(evalstack.back().data);
        evalstack.pop_back();
        long long x = std::stoll(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x & y), tok::num);
        return;
    }

    inline void land(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x && y), tok::num);
        return;
    }

    inline void lor(std::vector<token>& evalstack){
        double y = std::stod(evalstack.back().data);
        evalstack.pop_back();
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(x || y), tok::num);
        return;
    }

    inline void lnot(std::vector<token>& evalstack){
        double x = std::stod(evalstack.back().data);
        evalstack.back() = token(to_string_hprec(!x), tok::num);
        return;
    }

    inline void asn(std::vector<token>& evalstack){
        token val = evalstack.back();
        evalstack.pop_back();

        if(val.type == 9){
            std::string iden = evalstack.back().data;
            evalstack.pop_back();
            if(var::changable(iden)){
                var::update(iden, val.data);
                return;
            }else{
                std::string error = "Variable \"";
                error.append(iden).append("\" is read-only");
                throw error;
            }   
        }else{
            std::string iden = evalstack.back().data;
            evalstack.pop_back();
            if(var::changable(iden)){
                var::update(iden, to_string_hprec(std::stod(val.data)));
                return;
            }else{
                std::string error = "Variable \"";
                error.append(iden).append("\" is read-only");
                throw error;
            }
        }

    }

    inline void addasn(std::vector<token>& evalstack){
        token val = evalstack.back();
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(!var::changable(iden)){
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
        if(val.type == 9){
            var::update(iden,
                var::search(iden).append(val.data)
            );
        }else{
            var::update(iden, to_string_hprec(
                std::stod(var::search(iden)) + std::stod(val.data)
            ));
            return;  
        }
        
    }

    inline void subasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                std::stod(var::search(iden)) - std::stod(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void mulasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                std::stod(var::search(iden)) * std::stod(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void divasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                std::stod(var::search(iden)) / std::stod(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void modasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                fmod(std::stod(var::search(iden)), std::stod(val))
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void exclusiveOrasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                std::stoll(var::search(iden)) ^ std::stoll(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void powasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                pow(std::stod(var::search(iden)), std::stod(val))
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void shlasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                 std::stoll(var::search(iden)) << std::stoll(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void shrasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                 std::stoll(var::search(iden)) >> std::stoll(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void bitexclusiveOrasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();        
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                 std::stoll(var::search(iden)) ^ std::stoll(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }


    inline void bitorasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();        
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                 std::stoll(var::search(iden)) | std::stoll(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void bitandasn(std::vector<token>& evalstack){
        std::string val = evalstack.back().data;
        evalstack.pop_back();        
        std::string iden = evalstack.back().data;
        evalstack.pop_back();
        if(var::changable(iden)){
            var::update(iden, to_string_hprec(
                 std::stoll(var::search(iden)) & std::stoll(val)
            ));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    std::unordered_map<std::string, void(*)(std::vector<token>&)> ExecOperators = {
        {"ADD", add},
        {"SUB", sub},
        {"MUL", mul},
        {"DIV", divide},
        {"MOD", mod},
        {"POW", power},
        {"XOR", bitexclusiveOr},
        {"SHL", shl},
        {"SHR", shr},
        {"EQL", eql},
        {"NQL", nql},
        {"GQL", gql},
        {"LQL", lql},
        {"GRT", grt},
        {"LST", lst},
        {"NEG", neg},
        {"POS", [](std::vector<token>&)->void{return;}},
        {"BITOR", bitwor},
        {"BITAND", bitwand},
        {"BITXOR", bitexclusiveOr},
        {"LAND", land},
        {"LOR", lor},
        {"NOT", lnot},
        {"ASN", asn},
        {"SUBASN", subasn},
        {"ADDASN", addasn},
        {"MULASN", mulasn},
        {"DIVASN", divasn},
        {"MODASN", modasn},
        {"POWASN", powasn},
        {"SHLASN", shlasn},
        {"SHRASN", shrasn},
        {"BITXORASN", exclusiveOrasn},
        {"BITORASN", bitorasn},
        {"BITANDASN", bitandasn}

    };

    std::unordered_map<std::string, const token> assignment_remap = {
        {"ADDASN", token("ADD", 1)},
        {"SUBASN", token("SUB", 1)},
        {"MULASN", token("MUL", 1)},
        {"DIVASN", token("DIV", 1)},
        {"MODASN", token("MOD", 1)},
        {"POWASN", token("POW", 1)},
        {"XORASN", token("XOR", 1)},
        {"SHLASN", token("SHL", 1)},
        {"SHRASN", token("SHR", 1)},
        {"BITXORASN", token("XOR", 1)},
        {"BITORASN", token("OR", 1)},
        {"BITANDASN", token("AND", 1)}
    };

    // true == success, false == fail
    inline bool remap_asn(token& x){
        if(assignment_remap.find(x.data) == assignment_remap.end()){
            return false;
        }else{
            x = assignment_remap[x.data];
        }
        return true;
    }

    token apply(default_modifier mod, token val){
        if(mod.asn_type.data == "ASN") return mod.rvalue;
        if(mod.asn_type.data.empty()) return val;
        if(!remap_asn(mod.asn_type)) return token();
        std::vector<token> evalstack = {
            val,
            mod.rvalue
        };
        ExecOperators[mod.asn_type.data](evalstack);
        return evalstack.front();
    }


    std::vector<token> simplify(std::vector<token> tokens, std::vector<token> targets, std::string freserved, unsigned long int reservecount){
        if(recursionCount > maxRecurse){
            std::vector<token>().swap(tokens);
            std::string error = "Maximum recursion depth exceeded (limit=";
            error.append(std::to_string(maxRecurse)).append(")");
            recursionCount = 0;
            throw error;
        }
        ++recursionCount;

        bool dumpfunction = false;

        std::vector<token> evalstack;
        evalstack.reserve(reservecount);
        const token first = tokens.front();

        unsigned long int index = 0;
        unsigned long int stopsize = tokens.size();

        long int argtracker = 0;
        long int farg_max;

        /*// Accepted token types

        X - NO
        - - YES

        - 0 - NUM
        - 1 - OPERATOR
        X 2 - LEFT BRACKET
        X 3 - RIGHT BRACKET
        - 4 - FUNCTION 
        - 5 - VARIABLE - Tested with targets
        - 6 - R FUNC
        X 7 - Separator
        - 9 - String
        - 10 - Assign

        -   1000 - Special NULL (VAR)
        */

        std::vector<token> argpack; // To be used in functions
        std::vector<token> dtemp;

        goto forceRun; // At least run through loop once no matter the condition
        while(index < stopsize){
            forceRun:
            if(!inturrupt_exit_flag) throw std::string("");

            switch(tokens[index].type){

                // Accept NUM && String
                case tok::var:
                    for(token test: targets){
                        if(test.data == tokens[index].data){
                            token nubr(test.data, 1000);
                            evalstack.emplace_back(nubr);
                            ++index;
                            break;
                        }
                    }
                    break;
                case tok::str:
                case tok::num:
                    evalstack.emplace_back(tokens[index]);
                    ++index;
                    break;

                case tok::op:
                    if(tokens[index].data == "POS"){
                        ++index;
                        continue;
                    }else if(tokens[index].data == "NOT"){
                        if(evalstack.back().type == 1000 ||
                           evalstack.back().type == tok::op ||
                           evalstack.back().type == tok::func){
                            evalstack.emplace_back(tokens[index]);
                            ++index;
                            continue;
                        }
                        lnot(evalstack);
                        ++index;
                        continue;
                    }
                    if(evalstack.end()[-2].type == 1000 ||
                        evalstack.back().type == 1000 ||
                        evalstack.end()[-2].type == tok::op ||
                        evalstack.back().type == tok::op ||
                        evalstack.end()[-2].type == tok::func ||
                        evalstack.back().type == tok::func){
                        if(evalstack.back().type == 1000 ||
                            evalstack.back().type == tok::op ||
                            evalstack.back().type == tok::func){
                            token tk(evalstack.back().data,1000);
                        }else{
                            token tk(evalstack.end()[-2].data,1000);
                        }
                        evalstack.emplace_back(tokens[index]);
                        ++index;
                        continue;
                    }
                    ExecOperators[tokens[index].data](evalstack);
                    ++index;


                break;

            // Standard Function
            // Can handle Variadic Functions
            // Function Overloading is supported
            case tok::func:{
                std::string funcname = tokens[index].data;
                farg_max = tokens[index].reserved;
                if(!(fexists(funcname, farg_max) || funcname == freserved || f_isspecial(funcname, farg_max) || f_iscorefunc(funcname, farg_max))){
                    std::string error = "No matching function call to \"";
                    error.append(funcname).append("\"");
                    throw error;
                }

                while(!evalstack.empty() && argtracker < farg_max){
                    if(evalstack.back().type == 1000 ||
                       evalstack.back().type == tok::op ||
                        evalstack.back().type == tok::func) dumpfunction = true;
                    if(evalstack.back().type == tok::num){
                        argpack.emplace_back(evalstack.back());
                        evalstack.pop_back();
                        argtracker++;
                    }else{
                        break;
                    }
                }

                // Check if recursive function with 0 arg - freserved
                if(dumpfunction || (funcname == freserved && farg_max == 0) || f_iscorefunc(funcname, farg_max)){
                    dumpfunction = false;
                    evalstack.emplace_back(tokens[index]);
                    ++index;
                    continue;
                }
        
                std::reverse(argpack.begin(),argpack.end());

                token tk;
                tk.type = tok::num;
                if(f_isspecial(funcname, farg_max)){
                    try{
                        tk.data = to_string_hprec(callspecial(argpack, funcname));
                    }catch(const int& e){
                        // Error
                    }
                }else{
                    try{
                        tk.data = calculate(comp::retfillallvars(call(argpack, funcname)), false);
                    }catch(const std::string& err){
                        evalstack.clear();
                        tokens.clear();
                        argpack.clear();
                        std::vector<token>().swap(evalstack);
                        std::vector<token>().swap(tokens);
                        std::vector<token>().swap(argpack);
                        throw err;
                    }               
                }
                
                evalstack.emplace_back(tk);
                
                std::vector<token>().swap(argpack);
                argtracker = 0;
                ++index;
                }break;
            // SEP is ignored (Should not be here)
            case tok::sep:{
                ++index;
            }break;

            default:{

                // Always increment index
                ++index;

                }break;
            }
        }

        // Once functions finishes, recursionCount is reset
        recursionCount = 0;
        for(unsigned long int replaceidx = 0; replaceidx < evalstack.size(); ++replaceidx){
            if(evalstack[replaceidx].type == 1000){
                evalstack[replaceidx].type = tok::var;
            }
        }

        tokens.clear();
        targets.clear();
        evalstack.clear();

        std::vector<token>().swap(tokens);
        std::vector<token>().swap(targets);
        std::vector<token>().swap(evalstack);

        return evalstack;
    }


    std::string calculate(std::vector<token> tokens, bool showprogress, unsigned long int reservecount){
        if(recursionCount > maxRecurse){
            tokens.clear();
            std::vector<token>().swap(tokens);
            std::string error = "Maximum recursion depth exceeded (limit=";
            error.append(std::to_string(maxRecurse)).append(")");
            recursionCount = 0;
            throw error;
        }
        ++recursionCount;

        std::vector<token> evalstack;
        evalstack.reserve(reservecount);
        const token first = tokens.front();

        unsigned long int index = 0;
        unsigned long int stopsize = tokens.size();

        long int argtracker = 0;
        long int farg_max;

        bool varout = true;

        /*// Accepted token types
        - 0 - NUM
        - 1 - OPERATOR
        X 2 - LEFT BRACKET
        X 3 - RIGHT BRACKET
        - 4 - FUNCTION
        - 5 - VARIABLE
        - 6 - R FUNC
        X 7 - Separator
        - 9 - String
        - 10 - Assign
        */

        std::vector<token> argpack; // To be used in functions
        std::vector<token> dtemp;

        goto forceRun; // At least run through loop once no matter the condition
        while(index < stopsize){
            forceRun:

            if(showprogress) bar::setcount(index);
            if(!inturrupt_exit_flag) throw std::string("");

            switch(tokens[index].type){

                // Accept NUM && String, VARIABLE should not be at this stage, will cause error
                case tok::hold:
                case tok::str:
                case tok::var: // Variable accepted anyways
                case tok::num:
                    evalstack.emplace_back(tokens[index]);
                    ++index;
                    break;

                case tok::op:
                case tok::asn:
                    ExecOperators[tokens[index].data](evalstack);
                    ++index;
                    break;

                // Standard Function
                // Function Overloading is supported
                case tok::func:{ 
                    std::string funcname = tokens[index].data;
                    farg_max = tokens[index].reserved;

                    if(!(fexists(funcname, farg_max) ||
                     f_isspecial(funcname, farg_max) ||
                     f_iscorefunc(funcname, farg_max))){

                        std::string error = "No matching function call to \"";
                        error.append(funcname).append("\"");
                        throw error;
                    }

                    while(!evalstack.empty() && argtracker < farg_max){
                        if(evalstack.back().type == tok::num ||
                           evalstack.back().type == tok::str ||
                           evalstack.back().type == tok::hold){
                            argpack.emplace_back(evalstack.back());
                            evalstack.pop_back();
                            ++argtracker;
                        }else{
                            break;
                        }
                    }

                    std::reverse(argpack.begin(),argpack.end());
                    token tk;
                    tk.type = tok::num;
                    if(f_isspecial(funcname, farg_max)){
                        try{
                            tk.data = to_string_hprec(callspecial(argpack, funcname));
                        }catch(const int){
                            // Error
                        }
                    }else if(f_iscorefunc(funcname, farg_max)){
                        varout = false;
                        tk = callcore(argpack, funcname);   
                    
                    }else{
                        try{
                            tk.data = calculate(comp::retfillallvars(call(argpack, funcname)), false);
                        }catch(const std::string& err){
                            evalstack.clear();
                            tokens.clear();
                            argpack.clear();
                            std::vector<token>().swap(evalstack);
                            std::vector<token>().swap(tokens);
                            std::vector<token>().swap(argpack);
                            throw err;
                        }
                    }
                    
                    if(tk.type != tok::tvoid) evalstack.emplace_back(tk);

                    std::vector<token>().swap(argpack);
                    argtracker = 0;
                    ++index;
                    }break;

                // SEP is ignored (Should not be here)
                case tok::sep:{
                    ++index;
                }break;
                default:{

                    // Always increment index
                    ++index;

                    }break;
            }
            if(evalstack.back().type == tok::tvoid && evalstack.size() > 0){
                evalstack.pop_back();
            }
        }

        // Once functions finishes, recursionCount is reset
        recursionCount = 0;

        if(evalstack.size() <= 0 && varout){
            // If empty, return the most recently changed variable
            // Only assigning operators take everything
            return var::mostrecent();

        }

        // Success
        std::string strout;
        strout.append(evalstack.front().data);
        for(unsigned long int sdx = 1; sdx < evalstack.size(); sdx++){
            if(evalstack[sdx].type == tok::num ||
               evalstack[sdx].type == tok::str){
                strout.append(evalstack[sdx].data);
                if(sdx + 1 < evalstack.size()) strout.append("\n");
            }
        }

        tokens.clear();
        evalstack.clear();

        std::vector<token>().swap(tokens);
        std::vector<token>().swap(evalstack);

        return strout;
    }

    std::unordered_map<std::string, default_modifier> func_lvalue_deduction(std::vector<token> tokens){
        if(recursionCount > maxRecurse){
            tokens.clear();
            std::vector<token>().swap(tokens);
            std::string error = "Maximum recursion depth exceeded (limit=";
            error.append(std::to_string(maxRecurse)).append(")");
            recursionCount = 0;
            throw error;
        }
        ++recursionCount;

        std::vector<token> evalstack;
        if(!tokens.empty() && tokens.front().type == tok::func){
            tokens.erase(tokens.begin());
        }
        if(!tokens.empty() && tokens.front().type == tok::lbrac){
            tokens.erase(tokens.begin());
        }
        if(!tokens.empty() && tokens.back().type == tok::rbrac){
            tokens.pop_back();
        }

        if(tokens.empty()) return std::unordered_map<std::string, default_modifier>();

        std::unordered_map<std::string, default_modifier> lvalue_map;

        unsigned long int index = 0;
        unsigned long int stopsize = tokens.size();

        long int argtracker = 0;
        long int farg_max;

        /*// Accepted token types
        - 0 - NUM
        - 1 - OPERATOR
        X 2 - LEFT BRACKET
        X 3 - RIGHT BRACKET
        - 4 - FUNCTION
        - 5 - VARIABLE
        - 6 - R FUNC
        X 7 - Separator
        - 9 - String
        - 10 - Assign
        */

        std::vector<token> argpack; // To be used in functions
        std::vector<token> dtemp;

        goto forceRun; // At least run through loop once no matter the condition
        while(index < stopsize){
            forceRun:

            if(!inturrupt_exit_flag) throw std::string("");

            switch(tokens[index].type){

                // Accept NUM && String, VARIABLE should not be at this stage, will cause error
                case tok::hold:
                case tok::str:
                case tok::var: // Variable accepted anyways
                case tok::num:
                    evalstack.emplace_back(tokens[index]);
                    ++index;
                    break;

                case tok::op:
                    ExecOperators[tokens[index].data](evalstack);
                    ++index;
                    break;

                // Standard Function
                // Function Overloading is supported
                case tok::func:{ 
                    std::string funcname = tokens[index].data;
                    farg_max = tokens[index].reserved;
                    if(!(fexists(funcname, farg_max) ||
                     f_isspecial(funcname, farg_max) ||
                     f_iscorefunc(funcname, farg_max))){

                        std::string error = "No matching function call to \"";
                        error.append(funcname).append("\"");
                        throw error;
                    }

                    while(!evalstack.empty() && argtracker < farg_max){
                        if(evalstack.back().type == tok::num ||
                           evalstack.back().type == tok::str ||
                           evalstack.back().type == tok::hold){
                            argpack.emplace_back(evalstack.back());
                            evalstack.pop_back();
                            argtracker++;
                        }else{
                            break;
                        }
                    }

                    std::reverse(argpack.begin(),argpack.end());
                    token tk;
                    tk.type = 0;
                    if(f_isspecial(funcname, farg_max)){
                        try{
                            tk.data = to_string_hprec(callspecial(argpack, funcname));
                        }catch(const int){
                            // Error
                        }
                    }else if(f_iscorefunc(funcname, farg_max)){
                        tk = callcore(argpack, funcname);   
                    
                    }else{
                        try{
                            tk.data = calculate(comp::retfillallvars(call(argpack, funcname)), false);
                        }catch(const std::string& err){
                            evalstack.clear();
                            tokens.clear();
                            argpack.clear();
                            std::vector<token>().swap(evalstack);
                            std::vector<token>().swap(tokens);
                            std::vector<token>().swap(argpack);
                            throw err;
                        }
                    }
                    
                    if(tk.type != tok::tvoid) evalstack.emplace_back(tk);

                    std::vector<token>().swap(argpack);
                    argtracker = 0;
                    ++index;
                    }break;

                // SEP is ignored
                case 7:{
                    if(evalstack.back().type == tok::var){
                        lvalue_map[evalstack.back().data] = default_modifier();
                    }
                    evalstack.clear(); // clear
                    ++index;
                }break;
                default:{

                    // Always increment index
                    ++index;

                    }break;
                case tok::asn:
                    token rvalue = evalstack.back();
                    evalstack.pop_back();
                    token lvalue = evalstack.back();
                    evalstack.pop_back();
                    lvalue_map[lvalue.data] = default_modifier(tokens[index], rvalue);
                    ++index;

            }

            if(evalstack.back().type == tok::tvoid && evalstack.size() > 0){
                evalstack.pop_back();
            }
        }

        // Once functions finishes, recursionCount is reset
        recursionCount = 0;
        
        evalstack.clear();
        tokens.clear();
        argpack.clear();
        std::vector<token>().swap(evalstack);
        std::vector<token>().swap(tokens);
        std::vector<token>().swap(argpack);

        return lvalue_map;
    }



}

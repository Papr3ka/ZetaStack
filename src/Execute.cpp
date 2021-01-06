#include<array>
#include<cmath>
#include<cstdlib>
#include<iomanip>
#include<iostream>
#include<limits>
#include<sstream>
#include<string>
#include<vector>

#include "Cache.hpp"
#include "Execute.hpp"
#include "Function.hpp"
#include "Status.hpp"
#include "Token.hpp"
#include "Variable.hpp"
#include "Zetacompiler.hpp"

namespace xmath {

    long long int maxRecurse = 1280; // Dont touch this unless your platform has bigger stack size
    long long int recursionCount = 0;

    void setmaxrecurse(long long maxR){
        maxRecurse = maxR;
        return;
    }

    long long getmaxrecurse(void){
        return maxRecurse;
    }


    inline std::string to_string_hprec(double x){
        std::ostringstream doublestring;
        doublestring << std::setprecision(std::numeric_limits<double>::digits10) << x;
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

    inline std::string addstr(std::string x, std::string y){
    x.append(y);
        return x;
    }


    // Placeholder functions for Arbitrary math
    inline double add(double x, double y){
        return x+y;
    }

    inline double sub(double x, double y){
        return x-y;
    }

    inline double mul(double x, double y){
        return x*y;
    }

    inline double divide(double x, double y){
        return x/y;
    }

    inline double mod(double x, double y){
        return fmod(x,y);
    }

    inline double power(double x, double y){
        return pow(x, y);
    }

    inline long long bitexclusiveOr(long long x, long long y){
        return x ^ y;
    }

    inline long long shl(long long x, long long y){
        return x << y;
    }

    inline long long shr(long long x, long long y){
        return x >> y;
    }

    inline double eql(double x, double y){
        if(x == y) return 1;
        return 0;
    }

    inline double nql(double x, double y){
        if(x != y) return 1;
        return 0;
    }

    inline double gql(double x, double y){
        if(x >= y) return 1;
        return 0;
    }

    inline double lql(double x, double y){
        if(x <= y) return 1;
        return 0;
    }

    inline double grt(double x, double y){
        if(x > y) return 1;
        return 0;
    }

    inline double lst(double x, double y){
        if(x < y) return 1;
        return 0;
    }

    inline double neg(double x){
        return x*-1;
    }

    inline long long bitwor(long long x, long long y){
        return x | y;
    }

    inline long long bitwand(long long x, long long y){
        return x & y;
    }

    inline double land(double x, double y){
        return x && y;
    }

    inline double lor(double x, double y){
        return x || y;
    }

    inline double lnot(double x){
        if(x == 0) return 1;
        return 0;
    }

    inline double sum(std::vector<double> addends){
        double ans = 0;
        for(double add: addends){
            ans += add;
        }
        return ans;
    }

    inline double prod(std::vector<double> multiplicands){
        double ans = 1;
        for(double factor: multiplicands){
            ans *= factor;
        }
        return ans;
    }

    inline long long factorial(long long x){
        long long ans=1;
        for(;x>0;x--){
            ans *= x;
        }
        return ans;
    }

    /*// Assign OPS
    0 ASN     =
    1 ADDASN  +=
    2 SUBASN  -=
    3 MULASN  *=
    4 DIVASN  /=
    5 MODASN  %=
    6 XORASN  ^=
    7 POWASN  **=
    8 SHLASN  <<=
    9 SHRASN  >>=
    */

    inline void sasn(std::string iden, std::string val){
        if(var::changable(iden)){
            var::update(iden, val);
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void asn(std::string iden, double val){
        if(var::changable(iden)){
            var::update(iden,to_string_hprec(val));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void addasn(std::string iden, double val){
        if(var::changable(iden)){
            var::update(iden,to_string_hprec(add(std::stod(var::search(iden)), val)));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void subasn(std::string iden, double val){
        if(var::changable(iden)){
            var::update(iden,to_string_hprec(sub(std::stod(var::search(iden)), val)));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void mulasn(std::string iden, double val){
        if(var::changable(iden)){
            var::update(iden,to_string_hprec(mul(std::stod(var::search(iden)), val)));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void divasn(std::string iden, double val){
        if(var::changable(iden)){
            var::update(iden,to_string_hprec(divide(std::stod(var::search(iden)), val)));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void modasn(std::string iden, double val){
        if(var::changable(iden)){
            var::update(iden,to_string_hprec(mod(std::stod(var::search(iden)), val)));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void exclusiveOrasn(std::string iden, long long val){
        if(var::changable(iden)){
            var::update(iden,to_string_hprec(bitexclusiveOr(std::stoll(var::search(iden)), val)));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void powasn(std::string iden, double val){
        if(var::changable(iden)){
            var::update(iden,to_string_hprec(pow(std::stod(var::search(iden)), val)));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void shlasn(std::string iden, double val){
        if(var::changable(iden)){
            var::update(iden,to_string_hprec(shl(std::stod(var::search(iden)), val)));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void shrasn(std::string iden, double val){
        if(var::changable(iden)){
            var::update(iden,to_string_hprec(shr(std::stod(var::search(iden)), val)));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void bitexclusiveOrasn(std::string iden, double val){
        if(var::changable(iden)){
            var::update(iden,to_string_hprec(bitexclusiveOr(std::stoll(var::search(iden)), val)));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void bitorasn(std::string iden, double val){
        if(var::changable(iden)){
            var::update(iden,to_string_hprec(bitwor(std::stoll(var::search(iden)), val)));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    inline void bitandasn(std::string iden, double val){
        if(var::changable(iden)){
            var::update(iden,to_string_hprec(bitwand(std::stoll(var::search(iden)), val)));
            return;
        }else{
            std::string error = "Variable \"";
            error.append(iden).append("\" is read-only");
            throw error;
        }
    }

    std::vector<token> simplify(std::vector<token> tokens, std::vector<token> targets, std::string freserved, unsigned long int reservecount){
        if(recursionCount > maxRecurse){
            std::vector<token>().swap(tokens);
            std::string error = "Maximum recursion depth exceeded (limit=";
            error.append(std::to_string(maxRecurse)).append(")");
            recursionCount = 0;
            throw error;
        }
        recursionCount++;

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

        -0 - NUM
        -1 - OPERATOR
        X2 - LEFT BRACKET
        X3 - RIGHT BRACKET
        -4 - FUNCTION 
        -5 - VARIABLE - Tested with targets
        -6 - R FUNC
        X7 - Separator
        -   9 - String
        -   10 - Assign

        -   1000 - Special NULL (VAR)
        */

        std::vector<token> argpack; // To be used in functions
        std::vector<token> dtemp;

        std::vector<token> operatorsstack;

        goto forceRun; // At least run through loop once no matter the condition
        while(index < stopsize){
            forceRun:
            if(!inturrupt_exit_flag) throw std::string("");

            switch(tokens[index].type){

                // Accept NUM && String
                
                case 5:
                    for(token test: targets){
                        if(test.data == tokens[index].data){
                            token nubr(test.data, 1000);
                            evalstack.emplace_back(nubr);
                            index++;
                            break;
                        }
                    }
                    break;
                case 9:
                case 0:
                    evalstack.emplace_back(tokens[index]);
                    index++;
                    break;

                case 1:
                    if(tokens[index].data == "ADD"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        if(evalstack.back().type == 0){
                            double y = std::stod(evalstack.back().data);
                            evalstack.pop_back();
                            double x = std::stod(evalstack.back().data);
                            token tk(to_string_hprec(add(x, y)), 0);
                            evalstack.back() = tk;
                        }else{
                            std::string y = evalstack.back().data;
                            evalstack.pop_back();
                            std::string x = evalstack.back().data;
                            token tk(addstr(x, y), 9);
                            evalstack.back() = tk;
                        }
                        index++;
                    }else if(tokens[index].data == "SUB"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(sub(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "MUL"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(mul(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "DIV"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(divide(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "MOD"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(mod(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "POW"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(power(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "BITXOR"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        long long y = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        long long x = std::stoll(evalstack.back().data);
                        token tk(to_string_hprec(bitexclusiveOr(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "BITOR"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        long long y = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        long long x = std::stoll(evalstack.back().data);
                        token tk(to_string_hprec(bitwor(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "BITAND"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        long long y = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        long long x = std::stoll(evalstack.back().data);
                        token tk(to_string_hprec(bitwand(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "LAND"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(land(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "LOR"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(lor(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "SHL"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        long long y = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        long long x = std::stoll(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(shl(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "SHR"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        long long y = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        long long x = std::stoll(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(shr(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

            // comparisons
                    }else if(tokens[index].data == "EQL"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(eql(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "NQL"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(nql(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "GQL"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(gql(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "LQL"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(lql(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "GRT"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(grt(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "LST"){
                        if(evalstack.end()[-2].type == 1000 ||
                           evalstack.back().type == 1000 ||
                           evalstack.end()[-2].type == 1 ||
                           evalstack.back().type == 1 ||
                           evalstack.end()[-2].type == 4 ||
                           evalstack.back().type == 4){
                            if(evalstack.back().type == 1000 ||
                               evalstack.back().type == 1 ||
                               evalstack.back().type == 4){
                                token tk(evalstack.back().data,1000);
                            }else{
                                token tk(evalstack.end()[-2].data,1000);
                            }
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(lst(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "NEG"){
                        if(evalstack.back().type == 1000 ||
                           evalstack.back().type == 1 ||
                           evalstack.back().type == 4){
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(neg(x)), 0);
                        evalstack.back() = tk;
                        index++;

                    // POS - no effect on NUM, ignored
                    }else if(tokens[index].data == "POS"){
                        
                        index++;

                    }else if(tokens[index].data == "NOT"){
                        if(evalstack.back().type == 1000 ||
                           evalstack.back().type == 1 ||
                           evalstack.back().type == 4){
                            evalstack.emplace_back(tokens[index]);
                            index++;
                            continue;
                        }
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(lnot(x)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else {
                        //goto lblend; // Error
                    }
                break;

            // Standard Function
            // Can handle Variadic Functions
            // Function Overloading is supported
            case 4:{
                std::string funcname = tokens[index].data;
                farg_max = tokens[index].reserved;
                if(!(fexists(funcname, farg_max) || funcname == freserved || f_isspecial(funcname, farg_max) || f_iscorefunc(funcname, farg_max))){
                    std::string error = "No matching function call to \"";
                    error.append(funcname).append("\"");
                    throw error;
                }

                while(!evalstack.empty() && argtracker < farg_max){
                    if(evalstack.back().type == 1000 ||
                       evalstack.back().type == 1 ||
                        evalstack.back().type == 4) dumpfunction = true;
                    if(evalstack.back().type == 0){
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
                    index++;
                    continue;
                }
        
                std::reverse(argpack.begin(),argpack.end());

                token tk;
                tk.type = 0;
                if(f_isspecial(funcname, farg_max)){
                    try{
                        tk.data = to_string_hprec(callspecial(argpack, funcname));
                    }catch(const int& e){
                        // Error
                    }
                }else{
                    tk.data = calculate(comp::fillallvars(call(argpack, funcname)), false);
                }
                
                evalstack.emplace_back(tk);
                
                std::vector<token>().swap(argpack);
                argtracker = 0;
                index++;
                }break;

            case 6:{ // Right Function && single arg
                if(tokens[index].data == "FACT"){
                    if(evalstack.back().type == 1000) goto lblend;
                    double x = std::stod(evalstack.back().data);
                    token tk;
                    tk.data = to_string_hprec(factorial((long long)x));
                    tk.type = 0;
                    evalstack.back() = tk;
                    index++;

                    }
                }break;

            // SEP is ignored (Should not be here)
            case 7:{
                index++;
            }break;

            default:{

                // Always increment index
                index++;

                }break;
            }
        }

        lblend:

        // Once functions finishes, recursionCount is reset
        recursionCount = 0;
        for(unsigned long int replaceidx = 0; replaceidx < evalstack.size(); replaceidx++){
            if(evalstack[replaceidx].type == 1000){
                evalstack[replaceidx].type = 5;
            }
        }
        for(token op: operatorsstack){
            evalstack.emplace_back(op);
        }
        operatorsstack.clear();
        tokens.clear();
        targets.clear();

        std::vector<token>().swap(operatorsstack);
        std::vector<token>().swap(tokens);
        std::vector<token>().swap(targets);

        return evalstack;
    }

    std::string calculate(std::vector<token> tokens, bool showprogress, unsigned long int reservecount){
        if(recursionCount > maxRecurse){
            std::vector<token>().swap(tokens);
            std::string error = "Maximum recursion depth exceeded (limit=";
            error.append(std::to_string(maxRecurse)).append(")");
            recursionCount = 0;
            throw error;
        }
        recursionCount++;

        std::vector<token> evalstack;
        evalstack.reserve(reservecount);
        const token first = tokens.front();

        unsigned long int index = 0;
        unsigned long int stopsize = tokens.size();

        long int argtracker = 0;
        long int farg_max;

        bool varout = true;

        /*// Accepted token types
        -0 - NUM
        -1 - OPERATOR
        X2 - LEFT BRACKET
        X3 - RIGHT BRACKET
        -4 - FUNCTION
        -5 - VARIABLE
        -6 - R FUNC
        X7 - Separator
        -   9 - String
        -   10 - Assign
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
                case 9:
                case 5: // Variable accepted anyways
                case 0:
                    evalstack.emplace_back(tokens[index]);
                    index++;
                    break;

                case 1:
                    if(tokens[index].data == "ADD"){
                        if(evalstack.back().type == 0){
                            double y = std::stod(evalstack.back().data);
                            evalstack.pop_back();
                            double x = std::stod(evalstack.back().data);
                            token tk(to_string_hprec(add(x, y)), 0);
                            evalstack.back() = tk;
                        }else{
                            std::string y = evalstack.back().data;
                            evalstack.pop_back();
                            std::string x = evalstack.back().data;
                            token tk(addstr(x, y), 9);
                            evalstack.back() = tk;
                        }
                        index++;

                    }else if(tokens[index].data == "SUB"){
                    
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(sub(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "MUL"){
                    
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(mul(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "DIV"){
                    
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(divide(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "MOD"){
                    
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(mod(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "POW"){
                    
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(power(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "BITXOR"){
                    
                        long long y = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        long long x = std::stoll(evalstack.back().data);
                        token tk(to_string_hprec(bitexclusiveOr(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "BITOR"){
                    
                        long long y = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        long long x = std::stoll(evalstack.back().data);
                        token tk(to_string_hprec(bitwor(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "BITAND"){
                    
                        long long y = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        long long x = std::stoll(evalstack.back().data);
                        token tk(to_string_hprec(bitwand(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "LAND"){
                    
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(land(x, y)), 0);
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "LOR"){
                    
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(lor(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "SHL"){
                    
                        long long y = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        long long x = std::stoll(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(shl(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "SHR"){
                    
                        long long y = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        long long x = std::stoll(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(shr(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

            // comparisons
                    }else if(tokens[index].data == "EQL"){
                    
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(eql(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "NQL"){
                    
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(nql(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "GQL"){
                    
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(gql(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "LQL"){
                    
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(lql(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "GRT"){
                    
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(grt(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "LST"){
                    
                        double y = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        double x = std::stod(evalstack.back().data);
                        token tk;
                        tk.data = to_string_hprec(lst(x, y));
                        tk.type = 0;
                        evalstack.back() = tk;
                        index++;

                    }else if(tokens[index].data == "NEG"){
                    
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(neg(x)), 0);
                        evalstack.back() = tk;
                        index++;

                    // POS - no effect on NUM, ignored
                    }else if(tokens[index].data == "POS"){
                    
                        index++;

                    }else if(tokens[index].data == "NOT"){
                        double x = std::stod(evalstack.back().data);
                        token tk(to_string_hprec(lnot(x)), 0);
                        evalstack.back() = tk;
                        index++;

                    // POS - no effect on NUM, ignored
                    }else{
                        goto lblend; // Error
                    }
                break;

            // Standard Function
            // Can handle Variadic Functions
            // Function Overloading is supported
            case 4:{ 
                std::string funcname = tokens[index].data;
                farg_max = tokens[index].reserved;
                if(!(fexists(funcname, farg_max) || f_isspecial(funcname, farg_max) || f_iscorefunc(funcname, farg_max))){
                    std::string error = "No matching function call to \"";
                    error.append(funcname).append("\"");
                    throw error;
                }

                while(!evalstack.empty() && argtracker < farg_max){
                    if(evalstack.back().type == 0 || evalstack.back().type == 9){
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
                    }catch(const int& e){
                        // Error
                    }
                }else if(f_iscorefunc(funcname, farg_max)){
                    varout = false;
                    tk = callcore(argpack, funcname);   
                
                }else{
                    tk.data = calculate(comp::fillallvars(call(argpack, funcname)), false);
                }
                
                evalstack.emplace_back(tk);

                std::vector<token>().swap(argpack);
                argtracker = 0;
                index++;
                }break;

            case 6:{ // Right Function && single arg
                if(tokens[index].data == "FACT"){
                    double x = std::stod(evalstack.back().data);
                    token tk;
                    tk.data = to_string_hprec(factorial((long long)x));
                    tk.type = 0;
                    evalstack.back() = tk;
                    index++;

                    }
                }break;

            // SEP is ignored (Should not be here)
            case 7:{
                index++;
            }break;

            case 10:{
                    if(tokens[index].data == "ASN"){

                        if(evalstack.back().type == 9){

                            std::string strval = evalstack.back().data;
                            evalstack.pop_back();
                            std::string idenx = evalstack.back().data;
                            evalstack.pop_back();
                            sasn(idenx, strval);
                            index++;

                        }else{

                            double x = std::stod(evalstack.back().data);
                            evalstack.pop_back();
                            std::string idenx = evalstack.back().data;
                            evalstack.pop_back();
                            asn(idenx, x);
                            index++;

                        }
                    }else if(tokens[index].data == "ADDASN"){
                    
                        double x = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        std::string idenx = evalstack.back().data;
                        evalstack.pop_back();
                        addasn(idenx, x);
                        index++;

                    }else if(tokens[index].data == "SUBASN"){
                    
                        double x = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        std::string idenx = evalstack.back().data;
                        evalstack.pop_back();
                        subasn(idenx, x);
                        index++;

                    }else if(tokens[index].data == "MULASN"){
                    
                        double x = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        std::string idenx = evalstack.back().data;
                        evalstack.pop_back();
                        mulasn(idenx, x);
                        index++;

                    }else if(tokens[index].data == "DIVASN"){
                    
                        double x = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        std::string idenx = evalstack.back().data;
                        evalstack.pop_back();
                        divasn(idenx, x);
                        index++;

                    }else if(tokens[index].data == "MODASN"){
                    
                        double x = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        std::string idenx = evalstack.back().data;
                        evalstack.pop_back();
                        modasn(idenx, x);
                        index++;

                    }else if(tokens[index].data == "BITXORASN"){
                    
                        long long x = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        std::string idenx = evalstack.back().data;
                        evalstack.pop_back();
                        bitexclusiveOrasn(idenx, x);
                        index++;

                    }else if(tokens[index].data == "BITORASN"){
                    
                        long long x = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        std::string idenx = evalstack.back().data;
                        evalstack.pop_back();
                        bitorasn(idenx, x);
                        index++;

                    }else if(tokens[index].data == "BITANDASN"){
                    
                        long long x = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        std::string idenx = evalstack.back().data;
                        evalstack.pop_back();
                        bitandasn(idenx, x);
                        index++;

                    }else if(tokens[index].data == "POWASN"){
                    
                        double x = std::stod(evalstack.back().data);
                        evalstack.pop_back();
                        std::string idenx = evalstack.back().data;
                        evalstack.pop_back();
                        powasn(idenx, x);
                        index++;

                    }else if(tokens[index].data == "SHLASN"){
                    
                        long long x = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        std::string idenx = evalstack.back().data;
                        evalstack.pop_back();
                        shlasn(idenx, x);
                        index++;

                    }else if(tokens[index].data == "SHRASN"){
                    
                        long long x = std::stoll(evalstack.back().data);
                        evalstack.pop_back();
                        std::string idenx = evalstack.back().data;
                        evalstack.pop_back();
                        shrasn(idenx, x);
                        index++;

                    }else{
                        goto lblend;
                    }
                }break;
            default:{

                // Always increment index
                index++;

                }break;
            }
            if(evalstack.back().type == -1){
                evalstack.pop_back();
            }
        }

        lblend:

        // Once functions finishes, recursionCount is reset
        recursionCount = 0;

        if(evalstack.size() <= 0 && varout){
            // If empty, return the most recently changed variable
            // Only assigning operators take everything
            return var::mostrecent();

        }/*else if(evalstack.size() != 1){

            // For a successful calculation, the size of stack must be 1
            std::string error = "Unexpected Token";
            throw error;

        }else{*/

            // Success
            std::string strout;
            strout.append(evalstack.front().data);
            for(unsigned long int sdx = 1; sdx < evalstack.size(); sdx++){
                strout.append(evalstack[sdx].data);
                if(sdx + 1 < evalstack.size()) strout.append("\n");
            }

            tokens.clear();
            evalstack.clear();

            std::vector<token>().swap(tokens);
            std::vector<token>().swap(evalstack);

            return strout;
        //}
    }
}

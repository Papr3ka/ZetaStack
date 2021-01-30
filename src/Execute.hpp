/* Header for Execute.cpp
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

#ifndef EXECUTE_HPP
#define EXECUTE_HPP

#include<cmath>
#include<cstdlib>
#include<string>
#include<unordered_map>
#include<vector>

#include "Function.hpp" // struct default_modifier
#include "ZetaStack.hpp"

inline double fast_stofloat(const std::wstring& valtofloat){

    // Check of empty
    if (valtofloat.empty()){
        return 0;
    }

    unsigned long int index = 0;

    // true = pos, false = neg
    bool sign = true;

    if (valtofloat.front() == L'-'){

        ++index;
        sign = false;

    }else if(valtofloat.front() == L'+'){

        ++index;
    }

    bool has_f = false;
    bool has_exp = false;

    // Integer and Fractional parts
    double ipart = 0.0;
    double fpart = 0.0;

    while(index < valtofloat.size()){
        if (valtofloat[index] >= L'0' &&
            valtofloat[index] <= L'9'){

            ipart = ipart * 10 + (valtofloat[index] - L'0');

        }else if (valtofloat[index] == L'.'){

            has_f = true;
            ++index;
            break;

        }else if (valtofloat[index] == L'e'){

            has_exp = true;
            ++index;
            break;

        }else{

            if(sign) return ipart;
            return -ipart;

        }
        ++index;
    }

    if (has_f){
        double frac_exp = 0.1;

        while (index < valtofloat.size()){

            if (valtofloat[index] >= L'0' &&
                valtofloat[index] <= L'9'){

                fpart += frac_exp * (valtofloat[index] - L'0');
                frac_exp *= 0.1;

            }else if (valtofloat[index] == L'e'){

                has_exp = true;
                ++index;
                break;

            }else{

                if(sign) return (ipart + fpart);
                return -(ipart + fpart);

            }
        ++index;
        }
    }

    double epart = 1.0;
    if(index < valtofloat.size() &&
       has_exp){

        long int exp_sign = 1;

        if(valtofloat[index] == L'-'){

            exp_sign = -1;
            ++index;

        }else if (valtofloat[index] == L'+'){
            ++index;
        }

        long int e = 0;

        while(index < valtofloat.size() &&
            valtofloat[index] >= L'0' &&
            valtofloat[index] <= L'9'){

            e = e * 10 + valtofloat[index] - L'0';
            ++index;
        }

        long int n = exp_sign * e;
        double epart = 1.0;
        double r = 10.0;
        if(n < 0){
            n = -n;
            r = 0.1;
        }

        while(n){
            if (n & 1){
                epart *= r;
            }
            r *= r;
            n >>= 1;
        }
    }
    

    if(sign) return (ipart + fpart) * epart;
    return -((ipart + fpart) * epart);

}

inline double fast_stofloat(const std::string& valtofloat){

    // Check of empty
    if (valtofloat.empty()){
        return 0;
    }

    unsigned long int index = 0;

    // true = pos, false = neg
    bool sign = true;

    if (valtofloat.front() == '-'){

        ++index;
        sign = false;

    }else if(valtofloat.front() == '+'){

        ++index;
    }

    bool has_f = false;
    bool has_exp = false;

    // Integer and Fractional parts
    double ipart = 0.0;
    double fpart = 0.0;

    while(index < valtofloat.size()){
        if (valtofloat[index] >= '0' &&
            valtofloat[index] <= '9'){

            ipart = ipart * 10 + (valtofloat[index] - '0');

        }else if (valtofloat[index] == '.'){

            has_f = true;
            ++index;
            break;

        }else if (valtofloat[index] == 'e'){

            has_exp = true;
            ++index;
            break;

        }else{

            if(sign) return ipart;
            return -ipart;

        }
        ++index;
    }

    if (has_f){
        double frac_exp = 0.1;

        while (index < valtofloat.size()){

            if (valtofloat[index] >= '0' &&
                valtofloat[index] <= '9'){

                fpart += frac_exp * (valtofloat[index] - '0');
                frac_exp *= 0.1;

            }else if (valtofloat[index] == 'e'){

                has_exp = true;
                ++index;
                break;

            }else{

                if(sign) return (ipart + fpart);
                return -(ipart + fpart);

            }
        ++index;
        }
    }

    double epart = 1.0;
    if(index < valtofloat.size() &&
       has_exp){

        long int exp_sign = 1;

        if(valtofloat[index] == '-'){

            exp_sign = -1;
            ++index;

        }else if (valtofloat[index] == '+'){
            ++index;
        }

        long int e = 0;

        while(index < valtofloat.size() &&
            valtofloat[index] >= '0' &&
            valtofloat[index] <= '9'){

            e = e * 10 + valtofloat[index] - '0';
            ++index;
        }

        long int n = exp_sign * e;
        double epart = 1.0;
        double r = 10.0;
        if(n < 0){
            n = -n;
            r = 0.1;
        }

        while(n){
            if (n & 1){
                epart *= r;
            }
            r *= r;
            n >>= 1;
        }
    }
    

    if(sign) return (ipart + fpart) * epart;
    return -((ipart + fpart) * epart);

}

// Function Prototypes

namespace xmath {

	token apply(default_modifier mod, token val);

	void resetsstreamsettings(void);

	bool string_isnum(std::string str);

	std::vector<token> simplify(std::vector<token> tokens, std::vector<token> targets, std::string reserved="", unsigned long int reservecount=16);

	std::string calculate(std::vector<token> tokens, bool showprogress, unsigned long int reservecount=16);

	std::unordered_map<std::string, default_modifier> func_lvalue_deduction(std::vector<token> tokens);
}

#endif
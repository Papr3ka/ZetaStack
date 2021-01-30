/* Built-in core functions and Variables. 
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

#include<atomic>
#include<cmath>
#include<cstdlib>
#include<iostream>
#include<limits>
#include<string>
#include<thread>
#include<unordered_map>
#include<vector>

#include "BuiltIn.hpp"
#include "Execute.hpp"
#include "Status.hpp"
#include "ZetaStack.hpp"

// Variables
std::unordered_map<std::string, std::string> specialIden = {
    {"rand", ""},

    {"pi",
     "3.141592653589793238462643383279"},

    {"euler",
     "2.718281828459045235360287471352"},

    {"phi",
     "1.618033988749894848204586834365"},
};
    
// Functions

/* How to Add function

1. create function that takes std::vector<token> and returns double
2. Add prototype to BuiltIn.hpp
3. in Function.cpp in function initbuiltin add

    buitlinfuncs[i] = { a, "b", c, {"d" ...}};
    i++;

a = function pointer (*)()
b = alias
c = arguments (-1 if unspecified)
d = argument identifiers

4. In BuiltIn.hpp make sure TOTAL_BUILTIN_FUNCS is set to the number of builtin functions


Prefix meaning

b_  - builtin
bc_ - builtin core function

*/
// Base use functions
inline long long base_factorial(long long x){
    x = std::abs(x);
    long long ans=1;
    for(;x>0;x--){
        ans *= x;
    }
    return ans;
}

constexpr double rad_deg = 57.295779513082320876798154814105;
constexpr double rad_grad = 63.661977236758134307553505349005;


// Trigonomic
double b_sin(std::vector<token> x){
    switch(angle_unit){
        case 0:
            return sin(fast_stofloat(x.front().data));
        case 1:
            return sin(fast_stofloat(x.front().data)/rad_grad);
        case 2:
            return sin(fast_stofloat(x.front().data)/rad_deg);
        default:
            return sin(fast_stofloat(x.front().data));
    }
}

double b_cos(std::vector<token> x){
    switch(angle_unit){
        case 0:
            return cos(fast_stofloat(x.front().data));
        case 1:
            return cos(fast_stofloat(x.front().data)/rad_grad);
        case 2:
            return cos(fast_stofloat(x.front().data)/rad_deg);
        default:
            return cos(fast_stofloat(x.front().data));
    }
}

double b_tan(std::vector<token> x){
    switch(angle_unit){
        case 0:
            return tan(fast_stofloat(x.front().data));
        case 1:
            return tan(fast_stofloat(x.front().data)/rad_grad);
        case 2:
            return tan(fast_stofloat(x.front().data)/rad_deg);
        default:
            return tan(fast_stofloat(x.front().data));
    }
}

double b_asin(std::vector<token> x){
    switch(angle_unit){
        case 0:
            return asin(fast_stofloat(x.front().data));
        case 1:
            return asin(fast_stofloat(x.front().data))*rad_grad;
        case 2:
            return asin(fast_stofloat(x.front().data))*rad_deg;
        default:
            return asin(fast_stofloat(x.front().data));
    }
}

double b_acos(std::vector<token> x){
    switch(angle_unit){
        case 0:
            return acos(fast_stofloat(x.front().data));
        case 1:
            return acos(fast_stofloat(x.front().data))*rad_grad;
        case 2:
            return acos(fast_stofloat(x.front().data))*rad_deg;
        default:
            return acos(fast_stofloat(x.front().data));
    }
}

double b_atan(std::vector<token> x){
    switch(angle_unit){
        case 0:
            return atan(fast_stofloat(x.front().data));
        case 1:
            return atan(fast_stofloat(x.front().data))*rad_grad;
        case 2:
            return atan(fast_stofloat(x.front().data))*rad_deg;
        default:
            return atan(fast_stofloat(x.front().data));
    }
}

// Hyperbolic
double b_sinh(std::vector<token> x){
    return sinh(fast_stofloat(x.front().data));
}

double b_cosh(std::vector<token> x){
    return cosh(fast_stofloat(x.front().data));
}

double b_tanh(std::vector<token> x){
    return tanh(fast_stofloat(x.front().data));
}

double b_asinh(std::vector<token> x){
    return asinh(fast_stofloat(x.front().data));
}

double b_acosh(std::vector<token> x){
    return acosh(fast_stofloat(x.front().data));
}

double b_atanh(std::vector<token> x){
    return atanh(fast_stofloat(x.front().data));
}


////////////////////////////////

double b_exp(std::vector<token> x){
    return exp(fast_stofloat(x.front().data));
}

////////////////////////////////

double b_ln(std::vector<token> x){
    return log(fast_stofloat(x.front().data));
}

double b_log10(std::vector<token> x){
    return log10(fast_stofloat(x.front().data));
}

// [base, x]
double b_log(std::vector<token> x){
    return log(fast_stofloat(x.back().data))/log(fast_stofloat(x.front().data));
}

////////////////////////////////

double b_sqrt(std::vector<token> x){
    return sqrt(fast_stofloat(x.front().data));
}

double b_cbrt(std::vector<token> x){
    return cbrt(fast_stofloat(x.front().data));
}

double b_root(std::vector<token> x){
    return pow(fast_stofloat(x.back().data), 1/fast_stofloat(x.front().data));
}

////////////////////////////////

double b_sum(std::vector<token> x){
    double ans = 0;
    for(token add: x){
        ans += fast_stofloat(add.data);
    }
    return ans;
}

double b_prod(std::vector<token> x){
    double ans = 1;
    for(token fac: x){
        ans *= fast_stofloat(fac.data);
    }
    return ans;
}

double b_avg(std::vector<token> x){
    double ans = 0;
    for(token add: x){
        ans += fast_stofloat(add.data);
    }
    ans /= (double)x.size();
    return ans;
}

////////////////////////////////

double b_floor(std::vector<token> x){
    return floor(fast_stofloat(x.front().data));
}

double b_ceil(std::vector<token> x){
    return ceil(fast_stofloat(x.front().data));
}

double b_round(std::vector<token> x){
    return round(fast_stofloat(x.front().data));
}

double b_abs(std::vector<token> x){
    return abs(fast_stofloat(x.front().data));
}

////////////////////////////////

double b_min(std::vector<token> x){
    if(x.size() == 1) return(fast_stofloat(x.front().data));
    double amin = 0;
    double cur;
    for(token tmin: x){
        cur = fast_stofloat(tmin.data);
        if(cur < amin) amin = cur;
    }
    return amin;
}

double b_max(std::vector<token> x){
    if(x.size() == 1) return(fast_stofloat(x.front().data));
    double amax = 0;
    double cur;
    for(token tmax: x){
        cur = fast_stofloat(tmax.data);
        if(cur > amax) amax = cur;
    }
    return amax;
}

////////////////////////////////

#ifndef ZETA_FAST_F_
constexpr unsigned long int max_zeta_iter = 2560000;
#else
constexpr unsigned long int max_zeta_iter = 128000;
#endif

std::unordered_map<double, double> zetaTable = {
    {0, -0.5},
    {1, HUGE_VAL}, // INF
    {2, 1.644934066848226436472415166646},
    {3, 1.202056903159594285399738161511},
    {4, 1.082323233711138191516003696541},
    {5, 1.036927755143369926331365486457},
    {6, 1.017343061984449139714517929790},
    {7, 1.008349277381922826839797549849},
    {8, 1.004077356197944339378685238508},
    {9, 1.002008392826082214417852769232},
    {10,1.000994575127818085337145958900},
    {11,1.000494188604119464558702282526},
    {12,1.000246086553308048298637998047},
    {13,1.000122713347578489146751836526},
    {14,1.000061248135058704829258545105},
    {15,1.000030588236307020493551728510},
    {16,1.000015282259408651871732571487},
    {17,1.000007637197637899762273600293},
    {18,1.000003817293264999839856461644},
    {19,1.000001908212716553938925656957},
    {20,1.000000953962033872796113152038}
};


void b_zeta_worker(std::atomic<double>* ans,
                   const double s,
                   unsigned long int start,
                   unsigned long int inc,
                   unsigned long int end){

    double fans = 0;

    for(; start < end;
         start += inc){

        // Will never be division by 0
        fans += 1/(pow(start, s));

    }

    *ans = *ans + fans;
    return;

}

double b_zeta(std::vector<token> x){
    const double s = fast_stofloat(x.front().data);
    if(floor(s) == s){
        if(zetaTable.find(s) != zetaTable.end()) return zetaTable[s];
    }
    if(s < 1.0 && floor(s/2) == s/2){
        return 0;
    }
    #if __cplusplus >= 201703L
    if(s <= 1){
         return std::riemann_zeta(s);
    }
    //#else
        //#pragma message "values for zeta function x < 1 are erroneous. Compile with C++17 or later"
        // Program still works perfectly fine with C++11 but values x < 1 are erroneous
    #endif
    std::atomic<double> *ans = new std::atomic<double>;
    *ans = 0;
    std::vector<std::thread> workerpool;
    workerpool.reserve(CPU_COUNT);
    for(unsigned long int inc = 0; inc < CPU_COUNT; inc++){
        workerpool.emplace_back(std::thread(b_zeta_worker, ans, s, inc + 1, CPU_COUNT, max_zeta_iter));
    }
    for(std::thread& th: workerpool){
        th.join();
    }
    double ans_final = *ans;
    delete ans;
    return ans_final;
}

double b_gamma(std::vector<token> x){
    return tgamma(fast_stofloat(x.front().data));
}

////////////////////////////////

double b_erf(std::vector<token> x){
    return erf(fast_stofloat(x.front().data));
}

double b_factorial(std::vector<token> x){
    return base_factorial(fast_stofloat(x.front().data));
}

// CORE Function ////////////////////////////////

token bc_input(std::vector<token> x){
    bar::setstate(false);
    bar::printblank();
	if(x.size() >= 1){
		std::cout << x.front().data;
        std::cout.flush();
	}
	std::string sInput;
	std::getline(std::cin, sInput);
    if(std::cin.fail()){
        std::cin.clear();
        std::string error = "Inputstream Fail";
        throw error;
    }
    bar::setstate(do_bar);
	return token(sInput, 9);
}

// echo
token bc_echo(std::vector<token> x){
    bar::setstate(false);
    bar::printblank();
	for(token tdisp: x){
		std::cout << tdisp.data;
        std::cout.flush();
	}
    bar::setstate(do_bar);
	return token();
}

token bc_abort(std::vector<token> x){
    bar::setstate(false);
    bar::printblank();
    if(x.size() == 1) exit(std::stoul(x.front().data));
    exit(0);
}

token bc_numcast(std::vector<token> x){
    return token(x.front().data, tok::num);
}

token bc_strcast(std::vector<token> x){
    return token(x.front().data, tok::str);
}

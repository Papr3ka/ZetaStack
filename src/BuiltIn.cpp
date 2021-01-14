#include<atomic>
#include<cmath>
#include<string>
#include<vector>
#include<cstdlib>
#include<iostream>
#include<limits>
#include<unordered_map>
#include<thread>

#include "BuiltIn.hpp"
#include "Token.hpp"
#include "Status.hpp"
// Variables
std::unordered_map<std::string, std::string> specialIden = {
    {"rand", ""},

    {"pi",
     "3.141592653589793"},

    {"euler",
     "2.718281828459045"},

    {"phi",
     "1.618033988749894"},
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

constexpr double rad_deg = 57.2957795130823;
constexpr double rad_grad = 63.6619772367581;


// Trigonomic
double b_sin(std::vector<token> x){
    switch(angle_unit){
        case 0:
            return sin(std::stod(x.front().data));
        case 1:
            return sin(std::stod(x.front().data)/rad_grad);
        case 2:
            return sin(std::stod(x.front().data)/rad_deg);
        default:
            return sin(std::stod(x.front().data));
    }
}

double b_cos(std::vector<token> x){
    switch(angle_unit){
        case 0:
            return cos(std::stod(x.front().data));
        case 1:
            return cos(std::stod(x.front().data)/rad_grad);
        case 2:
            return cos(std::stod(x.front().data)/rad_deg);
        default:
            return cos(std::stod(x.front().data));
    }
}

double b_tan(std::vector<token> x){
    switch(angle_unit){
        case 0:
            return tan(std::stod(x.front().data));
        case 1:
            return tan(std::stod(x.front().data)/rad_grad);
        case 2:
            return tan(std::stod(x.front().data)/rad_deg);
        default:
            return tan(std::stod(x.front().data));
    }
}

double b_asin(std::vector<token> x){
    switch(angle_unit){
        case 0:
            return asin(std::stod(x.front().data));
        case 1:
            return asin(std::stod(x.front().data))*rad_grad;
        case 2:
            return asin(std::stod(x.front().data))*rad_deg;
        default:
            return asin(std::stod(x.front().data));
    }
}

double b_acos(std::vector<token> x){
    switch(angle_unit){
        case 0:
            return acos(std::stod(x.front().data));
        case 1:
            return acos(std::stod(x.front().data))*rad_grad;
        case 2:
            return acos(std::stod(x.front().data))*rad_deg;
        default:
            return acos(std::stod(x.front().data));
    }
}

double b_atan(std::vector<token> x){
    switch(angle_unit){
        case 0:
            return atan(std::stod(x.front().data));
        case 1:
            return atan(std::stod(x.front().data))*rad_grad;
        case 2:
            return atan(std::stod(x.front().data))*rad_deg;
        default:
            return atan(std::stod(x.front().data));
    }
}

// Hyperbolic
double b_sinh(std::vector<token> x){
    return sinh(std::stod(x.front().data));
}

double b_cosh(std::vector<token> x){
    return cosh(std::stod(x.front().data));
}

double b_tanh(std::vector<token> x){
    return tanh(std::stod(x.front().data));
}

double b_asinh(std::vector<token> x){
    return asinh(std::stod(x.front().data));
}

double b_acosh(std::vector<token> x){
    return acosh(std::stod(x.front().data));
}

double b_atanh(std::vector<token> x){
    return atanh(std::stod(x.front().data));
}


////////////////////////////////

double b_exp(std::vector<token> x){
    return exp(std::stod(x.front().data));
}

////////////////////////////////

double b_ln(std::vector<token> x){
    return log(std::stod(x.front().data));
}

double b_log10(std::vector<token> x){
    return log10(std::stod(x.front().data));
}

// [base, x]
double b_log(std::vector<token> x){
    return log(std::stod(x.back().data))/log(std::stod(x.front().data));
}

////////////////////////////////

double b_sqrt(std::vector<token> x){
    return sqrt(std::stod(x.front().data));
}

double b_cbrt(std::vector<token> x){
    return cbrt(std::stod(x.front().data));
}

double b_root(std::vector<token> x){
    return pow(std::stod(x.back().data), 1/std::stod(x.front().data));
}

////////////////////////////////

double b_sum(std::vector<token> x){
    double ans = 0;
    for(token add: x){
        ans += std::stod(add.data);
    }
    return ans;
}

double b_prod(std::vector<token> x){
    double ans = 1;
    for(token fac: x){
        ans *= std::stod(fac.data);
    }
    return ans;
}

double b_avg(std::vector<token> x){
    double ans = 0;
    for(token add: x){
        ans += std::stod(add.data);
    }
    ans /= (double)x.size();
    return ans;
}

////////////////////////////////

double b_floor(std::vector<token> x){
    return floor(std::stod(x.front().data));
}

double b_ceil(std::vector<token> x){
    return ceil(std::stod(x.front().data));
}

double b_round(std::vector<token> x){
    return round(std::stod(x.front().data));
}

////////////////////////////////

double b_min(std::vector<token> x){
    if(x.size() == 1) return(std::stod(x.front().data));
    double amin = 0;
    double cur;
    for(token tmin: x){
        cur = std::stod(tmin.data);
        if(cur < amin) amin = cur;
    }
    return amin;
}

double b_max(std::vector<token> x){
    if(x.size() == 1) return(std::stod(x.front().data));
    double amax = 0;
    double cur;
    for(token tmax: x){
        cur = std::stod(tmax.data);
        if(cur > amax) amax = cur;
    }
    return amax;
}

////////////////////////////////

#ifndef ZETA_FAST_F_
constexpr unsigned long int max_zeta_iter = 25600000;
#else
constexpr unsigned long int max_zeta_iter = 128000;
#endif

std::unordered_map<double, double> zetaTable = {
    {0, -0.5},
    {1, HUGE_VAL}, // INF 
    {2, 1.644934066848226},
    {3, 1.202056903159594},
    {4, 1.082323233711138},
    {5, 1.036927755143369},
    {6, 1.017343061984449},
    {7, 1.008349277381922},
    {8, 1.004077356197944},
    {9, 1.002008392826082},
    {10,1.000994575127818},
    {12,1.000246086553308},
    {14,1.000061248135058},
    {16,1.000015282259408},
    {18,1.000003817293264},
    {20,1.000000953962033}
};


void b_zeta_worker(std::atomic<double>* ans,
                   const double s,
                   unsigned long int start,
                   unsigned long int inc,
                   unsigned long int end){

    double fans = 0;

    for(; start < end;
         start += inc){

        fans += 1/(pow(start, s));

    }

    *ans = *ans + fans;
    return;

}

double b_zeta(std::vector<token> x){
    const double s = std::stod(x.front().data);
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
    return tgamma(std::stod(x.front().data));
}

////////////////////////////////

double b_erf(std::vector<token> x){
    return erf(std::stod(x.front().data));
}

double b_factorial(std::vector<token> x){
    return base_factorial(std::stod(x.front().data));
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
    return token(x.front().data, 0);
}

token bc_strcast(std::vector<token> x){
    return token(x.front().data, 9);
}

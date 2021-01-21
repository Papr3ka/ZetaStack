#ifndef TOKEN_HPP
#define TOKEN_HPP

#include<string>

// Main data type to be processed

/*
typedef struct{
    std::string data;
    int type;
}token;
*/

extern bool inturrupt_exit_flag;

/*
    -1 - NULL token
    0 - NUM
    1 - OPERATOR
    2 - LEFT BRACKET
    3 - RIGHT BRACKET
    4 - FUNCTION
    5 - VARIABLE
    6 - R FUNC
    7 - Separator / Func End
    8
    9 - String
    10 - assign operator
*/	

class token{
    private:
    protected:
    public:

        std::string data;
        long int type;
        long int reserved;
        long int special;

        // overloaded constructors for optional parameters
        inline token():
        data(""), type(-1), reserved(-1), special(-1)
        {}
        
        inline token(const std::string& data_,
                     long int type_):
        data(data_), type(type_), reserved(-1), special(-1)
        {}

        inline token(const std::string& data_,
                     long int type_,
                     const long int reserved_):
        data(data_), type(type_), reserved(reserved_), special(-1)
        {}

        inline token(const std::string& data_,
                     long int type_,
                     const long int reserved_,
                     long int special_):
        data(data_), type(type_), reserved(reserved_), special(special_)
        {}

        inline ~token(){
            data.clear();
            std::string().swap(data);
        }

        // comparisons
        inline friend bool operator != (token a, token b){
            if(a.data != b.data) return false;
            if(a.type != b.type) return false;
            return true;
        }

        inline friend bool operator == (token a, token b){
            if(a.data == b.data) return false;
            if(a.type == b.type) return false;
            return true;
        }

        inline friend bool operator == (std::string a, token b){
            if(a == b.data) return true;
            return false;
        }

        // Required for <string> to compare token with const char []
        inline friend bool operator == (token x, const char mval[]){
            if(x.data.c_str() == mval) return true;
            return false;
        }

        inline std::string pass_unary(void){
            if(reserved == -1) return data;
            return data.substr(reserved + 1, data.size()-1);
        }
};

#endif

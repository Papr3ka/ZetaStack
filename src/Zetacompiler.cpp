/* Assembling an array of tokens into reverse polish notation
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
#include<cctype>
#include<cstdlib>
#include<string>
#include<unordered_map>
#include<vector>

#include "Function.hpp"
#include "Variable.hpp"
#include "ZetaStack.hpp"
#include "Zetacompiler.hpp"

namespace comp {

    // std::vector<std::string> assign{
    // 	"ASN",
    // 	"ADDASN",
    // 	"SUBASN",
    // 	"MULASN",
    // 	"DIVASN",
    // 	"MODASN",
    // 	"XORASN",
    // 	"POWASN",
    // 	"SHLASN",
    // 	"SHRASN"
    // };

    std::vector<std::string> operators{
        "ADD",
        "SUB",
        "MUL",
        "DIV",
        "MOD",
        "XOR",
        "POW",
        "SHL",
        "SHR",
        "FLOORDIV",
        "OR",
        "NOT",
        "AND",
        "EQL",
        "NQL",
        "GQL",
        "LQL",
        "GRT",
        "LST"
    };

    bool string_isnum(std::string str){
        if(str.size() <= 1){
            if(isdigit(str[0]) ||
               str[0] == '.'){
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

    int ttype(std::string var){
        if(var == "SEP"){
            return tok::sep;
        }else if(var == "L_BRAC"){
            return tok::lbrac;
        }else if(var == "R_BRAC"){
            return tok::rbrac;
        }else if(string_isnum(var)){
            return tok::num;
        }else if(isalpha(var[0])){
            bool isop = false;
            for(unsigned long int inx=0; inx < operators.size(); inx++){
                if(var == operators[inx]){
                    isop = true;
                }
            }
            if(isop){
                return tok::op;
            // }else if(var == "FACT"){
            //     return 6;
            }else if(var.back() == '('){
                    return tok::func;
            }
        }
        return tok::var;
    }
        /*
            0 - NUM
            1 - OPERATOR
            2 - LEFT BRACKET
            3 - RIGHT BRACKET
            4 - FUNCTION
            5 - VARIABLE
            6 - R FUNC
            7 - Separator
            10 - Assigning operator
        */

    token getop(unsigned long idx){
        token tk(operators[idx], tok::op);
        return tk;
    }

    bool hasvar(std::vector<token> tokens){
        for(unsigned long int index = 0; index < tokens.size(); ++index){
            if(tokens[index].type == 5){
                return true;
            }
        }
        return false;
    }	

    inline unsigned int precedence(std::string op){
        if(op.back() == '('){

            return 1500;

        }else if(op == "NEG" || op == "POS"){

            return 1400;

        }else if(op == "NOT"){

            return 1300;

        }else if(op == "POW"){

            return 1200;

        }else if(op == "MUL" ||
                 op == "DIV" ||
                 op == "MOD" ||
                 op == "FLOORDIV"){

            return 1100;

        }else if(op == "ADD" ||
                 op == "SUB"){

            return 1000;

        }else if(op == "SHL" ||
                 op == "SHR"){

            return 900;

        }else if(op == "GQL" ||
                 op == "LQL" ||
                 op == "GRT" ||
                 op == "LST"){

            return 800;

        }else if(op == "EQL" ||
                 op == "NQL"){

            return 700;

        }else if(op == "BITAND"){

            return 600;

        }else if(op == "BITXOR"){

            return 500;

        }else if(op == "BITOR"){

            return 400;

        }else if(op == "LAND"){

            return 300;

        }else if(op == "LOR"){

            return 200;

        }else if(op.substr(op.size()-3,op.size()) == "ASN"){

            return 100;

        }else{

            return 0; // ERROR
            
        }
    }

    // true = LR, false = RL
    inline bool associativity(std::string op){
        if(op == "MUL" ||
           op == "DIV" || 
           op == "MOD" || 
           op == "FLOORDIV" ||
           op == "ADD" ||
           op == "SUB" ||
           op == "SHL" ||
           op == "SHR" ||
           op == "AND" ||
           op == "XOR" ||
           op == "OR"  ||
           op == "EQL" ||
           op == "NQL" ||
           op == "GQL" ||
           op == "LQL" ||
           op == "GRT" ||
           op == "LST"){
            return true;
        }else{
            return false;
        }
    }

    comp_metadata getcompdata(std::vector<token> vecdata){
        comp_metadata output{0,0,0,0};
        for(token tmp: vecdata){
            switch(tmp.type){
                case tok::hold:
                case tok::num:
                case tok::str:
                    output.nums++;
                    break;
                case tok::asn:
                case 6:
                case tok::op:
                    output.operators++;
                    break;
                case tok::lbrac:
                case tok::rbrac:
                    output.bracs++;
                    break;
                case tok::func:
                    output.functions++;
                    break;
            }
        }
        return output;
    }

    std::vector<token> shuntingYard(std::vector<token> tokens, comp_metadata meta){
        // Main vectors
        std::vector<token> operatorStack;
        std::vector<token> outputQueue;
        std::vector<token> functionStack;

        // Vectors for function use
        std::vector<unsigned long int> lockstack{0};
        std::vector<signed long int> argumentcounter;

        //Reserve vectors so you dont have to keep resizing (increase speed and efficiency)
        operatorStack.reserve(meta.operators+meta.bracs);
        outputQueue.reserve(meta.nums+meta.operators+meta.functions);
        functionStack.reserve(meta.functions);

        lockstack.reserve(meta.functions);
        argumentcounter.reserve(meta.functions);

        // function use
        long int layer = 0;
        long int infunction = 0;

        // Token to be read
        unsigned long int readindex = 0;
        unsigned long int readsize = tokens.size();
        
        while(readindex < readsize){
            /*
            0 - NUM
            1 - OPERATOR
            2 - LEFT BRACKET
            3 - RIGHT BRACKET
            4 - FUNCTION
            5 - VARIABLE
            6 - R FUNC
            7 - SEP
            
            9 - String
            10 - Assignment operator
            */
            switch(tokens[readindex].type){
                case tok::hold:
                case tok::num: // NUM
                    outputQueue.emplace_back(tokens[readindex]);
                    ++readindex;
                    break;

                case tok::op: // OPERATOR
                case tok::asn: // Assignment operator
                    while(!operatorStack.empty()){
                        if(((precedence(operatorStack.back().data) > precedence(tokens[readindex].data) ||
                        ((precedence(operatorStack.back().data) == precedence(tokens[readindex].data)) &&
                        associativity(tokens[readindex].data)))) &&
                        operatorStack.back().data != "L_BRAC"){							
                            outputQueue.emplace_back(operatorStack.back());
                            operatorStack.pop_back();
                        }else{
                            break;
                        }
                    }
                    operatorStack.emplace_back(tokens[readindex]);
                    ++readindex;
                    break;

                case tok::lbrac:
                    ++layer;
                    operatorStack.emplace_back(tokens[readindex]);
                    ++readindex;
                    break;

                case tok::rbrac:
                    while(!operatorStack.empty() &&
                         (operatorStack.back().data != "L_BRAC")){

                        outputQueue.emplace_back(operatorStack.back());
                        operatorStack.pop_back();
                    }
                    if(!operatorStack.empty() &&
                       operatorStack.back().data == "L_BRAC"){

                        operatorStack.pop_back();
                    }
                    ++readindex;
                    --layer;
                    if(layer == infunction && !functionStack.empty()){
                        functionStack.back().reserved = argumentcounter.back();							
                        outputQueue.emplace_back(functionStack.back());
                        functionStack.pop_back();
                        lockstack.pop_back();
                        argumentcounter.pop_back();
                        --infunction;
                    }		
                    break;

                case tok::func:
                    ++infunction;
                    layer = infunction;
                    functionStack.emplace_back(tokens[readindex]);
                    argumentcounter.emplace_back(tokens[readindex].reserved);
                    ++readindex;
                    lockstack.emplace_back(operatorStack.size() + 1);
                    break;

                case tok::var: // Variable
                    outputQueue.emplace_back(tokens[readindex]);
                    ++readindex;
                    break;

                case 6: // Left Function
                    outputQueue.emplace_back(tokens[readindex]);
                    ++readindex;
                    break;

                case tok::sep: // SEP - Dump all
                    if(!argumentcounter.empty()) ++argumentcounter.back();
                    ++readindex;

                    while(!operatorStack.empty() &&
                          !lockstack.empty() &&
                          operatorStack.size() > lockstack.back()){

                        if(operatorStack.back().data == "L_BRAC" ||
                           operatorStack.back().data == "R_BRAC"){
                            operatorStack.pop_back();
                        }
                        if(operatorStack.back().type == tok::func){
                            functionStack.emplace_back(operatorStack.back());
                            operatorStack.pop_back();
                        }else if(operatorStack.size() >= 1){
                            outputQueue.emplace_back(operatorStack.back());
                            operatorStack.pop_back();
                        }
                    }
                    break;
                default:
                    outputQueue.emplace_back(tokens[readindex]);
                    ++readindex;
                    break;
            }
        }
        while(!operatorStack.empty()){
            if(operatorStack.back().data == "L_BRAC" ||
               operatorStack.back().data == "R_BRAC"){

                operatorStack.pop_back();
            }
            if(!operatorStack.empty()){
                outputQueue.emplace_back(operatorStack.back());
                operatorStack.pop_back();
            }

        }
        for(unsigned long int store=0;store < argumentcounter.size(); store++){
            functionStack[store].reserved = argumentcounter[store];
        }
        if(!functionStack.empty()){
            std::copy(functionStack.begin(), functionStack.end(),outputQueue.end());
        }

        // Cleanup

        tokens.clear();
        operatorStack.clear();
        functionStack.clear();
        lockstack.clear();
        argumentcounter.clear();

        std::vector<token>().swap(tokens);
        std::vector<token>().swap(operatorStack);
        std::vector<token>().swap(functionStack);
        std::vector<unsigned long int>().swap(lockstack);
        std::vector<signed long int>().swap(argumentcounter);

        return outputQueue;
    }

    // tokens = compiled list of tokens, to be used at last step after recurselink
    void fillallvars(std::vector<token>& tokens){
        for(unsigned long int index=0;index < tokens.size(); ++index){
            if(tokens[index].type == 5){
                if(index+1 < tokens.size()){
                    if(tokens[index+1].type == 10){
                        continue;
                    }
                }
                tokens[index] = token(var::search(tokens[index].data, false), 0);
            }
        }
    }

    std::vector<token> retfillallvars(std::vector<token> tokens){
        for(unsigned long int index=0;index < tokens.size(); ++index){
            if(tokens[index].type == 5){
                if(index+1 < tokens.size()){
                    if(tokens[index+1].type == 10){
                        continue;
                    }
                }
                tokens[index] = token(var::search(tokens[index].data, false), 0);
            }
        }
        return tokens;
    }

    std::vector<token> retfillspecificvars(std::vector<token> tokens, std::unordered_map<std::string, token> tofill){
        for(unsigned long int index=0;index < tokens.size(); ++index){
            if(tokens[index].type == 5){
                if(index+1 < tokens.size()){
                    if(tokens[index+1].type == 10){
                        continue;
                    }
                }
                if(tofill.find(tokens[index].data) != tofill.end()){
                    tokens[index] = tofill[tokens[index].data];
                }
            }
        }
        return tokens;
    }

    splittedvec<token> splitvectorPoint(std::vector<token> vect, unsigned long int point){
        splittedvec<token> outvec;
        outvec.first = std::vector<token>(vect.begin(), vect.begin()+point);
        if(point == vect.size()) return outvec;
        outvec.second = std::vector<token>(vect.begin()+point, vect.end());
        return outvec;
    }

    std::unordered_map<std::string, std::string> demangle_map = {
        {"ADD", "+"},
        {"SUB", "-"},
        {"MUL", "*"},
        {"DIV", "/"},
        {"MOD", "%"},
        {"POW", "**"},
        {"XOR", "^"},
        {"SHL", "<<"},
        {"SHR", ">>"},
        {"EQL", "=="},
        {"NQL", "!="},
        {"GQL", ">="},
        {"LQL", "<="},
        {"GRT", ">"},
        {"LST", "<"},
        {"NEG", "-"},
        {"POS", "+"},
        {"BITOR", "|"},
        {"BITAND", "&"},
        {"BITXOR", "^"},
        {"LAND", "&&"},
        {"LOR", "||"},
        {"NOT", "!"}

    };

    std::string unmangle(token tk){
        if(tk.type == tok::lbrac) return "(";
        if(tk.type == tok::rbrac) return ")";
        if(tk.type == tok::sep) return ",";
        bool is_asn = (tk.type == tok::asn ? true : false);
        if(is_asn){
            if(!tk.data.empty()) tk.data.pop_back();
            if(!tk.data.empty()) tk.data.pop_back();
            if(!tk.data.empty()) tk.data.pop_back();
            if(tk.data.empty()) return "=";
        }
        std::string temp;
        if(demangle_map.find(tk.data) != demangle_map.end()){
            temp = demangle_map[tk.data];
            if(is_asn) temp.append("=");
            return temp;
        }
        // Error
        return "";


    }
}
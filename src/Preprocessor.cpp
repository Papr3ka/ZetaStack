/* Tokenizing and preprocessing string inputs
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
#include<iostream>
#include<string>
#include<vector>

#include "Preprocessor.hpp"
#include "ZetaStack.hpp"
#include "Zetacompiler.hpp"

std::vector<std::string> split(std::string str, std::string split){
    size_t start = 0, end, splitLen = split.size();
    std::string token;
    std::vector<std::string> output;
    while((end = str.find(split, start)) != std::string::npos){
        token = str.substr(start, end - start);
        start = end + splitLen;
        output.emplace_back(token);
    }
    output.emplace_back(str.substr(start));
    str.clear();
    std::string().swap(str);
    return output;
}

namespace comp {

    // std::vector<std::string> ops {
    //     "<<=",
    //     ">>=",
    //     "**=",
    //     "+=",
    //     "-=",
    //     "*=",
    //     "/=",
    //     "^=",
    //     "%=",
    //     "|=",
    //     "&=",
    //     "==",
    //     "!=",
    //     ">=",
    //     "<=",
    //     ">",
    //     "<",
    //     "!",
    //     "="
    // };


    // Lexical Analysis
    std::vector<std::string> lex(std::string lexInput){

        std::vector<std::string> returnedTokens;
        std::string dualchar;
        std::string trichar;
        unsigned long int loopcount = 0;
        unsigned long int index = 0;
        unsigned long int countindex;

        bool hexmode = false;

        while(index < lexInput.size()){
            if(lexInput[index] == '"'){
                countindex = index;
                while(index < lexInput.size()){
                    ++index;
                    if(lexInput[index] == '"' && lexInput[index - 1] != '\\'){
                        ++index;
                        break;
                    }
                }
                returnedTokens.emplace_back(lexInput.substr(countindex, index - countindex));
                if(returnedTokens.back().back() == ' '){
                    returnedTokens.back().pop_back();
                }
            }
            if(lexInput[index] == '\''){
                countindex = index;
                while(index < lexInput.size()){
                    ++index;
                    if(lexInput[index] == '\'' && lexInput[index - 1] != '\\'){
                        ++index;
                        break;
                    }
                    
                }
                returnedTokens.emplace_back(lexInput.substr(countindex, index - countindex));
                if(returnedTokens.back().back() == ' '){
                    returnedTokens.back().pop_back();
                }
            }
            if(ispunct(lexInput[index]) &&
               lexInput[index] != '_' &&
               lexInput[index] != '.' &&
               lexInput[index] != ','){
                   
                if(index+1 < lexInput.size()){
                    dualchar = lexInput.substr(index,2);
                    if(dualchar == "**"){
                        index += 2;
                        returnedTokens.emplace_back("**");
                    }else if(dualchar == "//"){
                        index += 2;
                        returnedTokens.emplace_back("//");
                    }else if(dualchar == "||"){
                        index += 2;
                        returnedTokens.emplace_back("||");
                    }else if(dualchar == "&&"){
                        index += 2;
                        returnedTokens.emplace_back("&&");
                    }else if(dualchar == "<<"){
                        index += 2;
                        returnedTokens.emplace_back("<<");
                    }else if(dualchar == ">>"){
                        index += 2;
                        returnedTokens.emplace_back(">>");
                    }else if(dualchar == "=="){
                        index += 2;
                        returnedTokens.emplace_back("==");
                    }else if(dualchar == "!="){
                        index += 2;
                        returnedTokens.emplace_back("!=");
                    }else if(dualchar == ">="){
                        index += 2;
                        returnedTokens.emplace_back(">=");
                    }else if(dualchar == "<="){
                        index += 2;
                        returnedTokens.emplace_back("<=");
                    }
                }
                if(lexInput[index] == '+'){
                    ++index;
                    returnedTokens.emplace_back("+");
                }else if(lexInput[index] == '-'){
                    ++index;
                    returnedTokens.emplace_back("-");
                }else if(lexInput[index] == '*'){
                    ++index;
                    returnedTokens.emplace_back("*");
                }else if(lexInput[index] == '/'){
                    ++index;
                    returnedTokens.emplace_back("/");
                }else if(lexInput[index] == '%'){
                    ++index;
                    returnedTokens.emplace_back("%");
                }else if(lexInput[index] == '^'){
                    ++index;
                    returnedTokens.emplace_back("^");
                }else if(lexInput[index] == '!'){
                    ++index;
                    returnedTokens.emplace_back("!");
                }else if(lexInput[index] == '('){
                    ++index;
                    returnedTokens.emplace_back("(");
                }else if(lexInput[index] == ')'){
                    ++index;
                    returnedTokens.emplace_back(")");
                }else if(lexInput[index] == '='){
                    ++index;
                    returnedTokens.emplace_back("=");
                }else if(lexInput[index] == '>'){
                    ++index;
                    returnedTokens.emplace_back(">");
                }else if(lexInput[index] == '<'){
                    ++index;
                    returnedTokens.emplace_back("<");
                }else if(lexInput[index] == '&'){
                    ++index;
                    returnedTokens.emplace_back("&");
                }else if(lexInput[index] == '|'){
                    ++index;
                    returnedTokens.emplace_back("|");
                }
                

            }else if(isalpha(lexInput[index]) || lexInput[index] == '_'){
                // check if variable first
                countindex = index;
                while(countindex < lexInput.size() &&
                    (isalpha(lexInput[countindex]) ||
                     isdigit(lexInput[countindex]) ||
                     lexInput[countindex] == '_'   ||
                     lexInput[countindex] == '`' )){
                    ++countindex;
                }
                if(lexInput[countindex] == '('){
                    // if functions call
                    returnedTokens.emplace_back(lexInput.substr(index,countindex-index + 1));
                    index += countindex-index + 1;
                    returnedTokens.emplace_back("(");
                }else{
                    returnedTokens.emplace_back(lexInput.substr(index,countindex-index));
                    index += countindex-index;
                }
            }else if(isdigit(lexInput[index]) || lexInput[index] == '.'){
                countindex = index;
                decimal:
                    while(countindex < lexInput.size() &&
                          (isdigit(lexInput[countindex]) ||
                         (isxdigit(lexInput[countindex]) &&
                          hexmode))){

                        ++countindex;
                    }
                    if(lexInput[countindex] == '.'){
                        ++countindex;
                        goto decimal;
                    }else if(lexInput[countindex] == 'e' ||
                             lexInput[countindex] == 'E'){
                        
                        if(countindex + 1 < lexInput.size()){
                            if(lexInput[countindex + 1] == '+' ||
                               lexInput[countindex + 1] == '-' ){
                                   ++countindex;
                                   ++countindex;
                                   goto decimal;
                               }else if(isdigit(lexInput[countindex + 1])){
                                   ++countindex;
                                   goto decimal;                                   
                               }
                        }
                        --countindex;
                    }else if(lexInput[countindex] == 'b'){
                        ++countindex;
                        goto decimal;
                    }else if(lexInput[countindex] == 'o'){
                        ++countindex;
                        goto decimal;
                    }else if(lexInput[countindex] == 'x'){
                        ++countindex;
                        hexmode = true;
                        goto decimal;
                    }else if(lexInput[countindex] == 'i'){
                        ++countindex;
                    }
                returnedTokens.emplace_back(lexInput.substr(index,countindex-index));
                hexmode = false;
                index = countindex;
            }else if(lexInput[index] == ','){
                returnedTokens.emplace_back(","); // default sep
                ++index;
            }else{
                ++index;
            }
            loopcount++;
            if(loopcount >= 2*lexInput.size()){
                break; // Error
            }
        }
        for(unsigned long comb_index = 1; comb_index < returnedTokens.size(); comb_index++){
            if(returnedTokens[comb_index-1] == "==" &&
               returnedTokens[comb_index] == "="){
                
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "===";
            }else if(returnedTokens[comb_index-1] == "**" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "**=";
            }else if(returnedTokens[comb_index-1] == "<<" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "<<=";
            }else if(returnedTokens[comb_index-1] == ">>" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = ">>=";
            }else if(returnedTokens[comb_index-1] == "+" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "+=";
            }else if(returnedTokens[comb_index-1] == "-" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "-=";
            }else if(returnedTokens[comb_index-1] == "*" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "*=";
            }else if(returnedTokens[comb_index-1] == "/" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "/=";
            }else if(returnedTokens[comb_index-1] == "%" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "%=";
            }else if(returnedTokens[comb_index-1] == "^" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "^=";
            }else if(returnedTokens[comb_index-1] == "=" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "==";
            }else if(returnedTokens[comb_index-1] == "!" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "!=";
            }else if(returnedTokens[comb_index-1] == ">" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = ">=";
            }else if(returnedTokens[comb_index-1] == "<" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "<=";
            }else if(returnedTokens[comb_index-1] == "|" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "|=";
            }else if(returnedTokens[comb_index-1] == "&" &&
                     returnedTokens[comb_index] == "="){
            
                returnedTokens.erase(returnedTokens.begin()+comb_index-1);
                returnedTokens[comb_index-1] = "&=";
            }else{
                // Error
            }
        }
        lexInput.clear();
        std::string().swap(lexInput);
        return returnedTokens;
    }

/*
    0 - NUM
    1 - OPERATOR
    2 - LEFT BRACKET
    3 - RIGHT BRACKET
    4 - FUNCTION
    5 - VARIABLE
    6 - R FUNC
    7 - Separator / Func End
*/

    std::vector<token> tokenComp(std::vector<std::string> tokensInput){
        std::vector<token> output;
        output.reserve(tokensInput.size());
        for(unsigned long int index = 0; index < tokensInput.size(); ++index){
            if(!isdigit(tokensInput[index][0])){
                if(tokensInput[index] == ","){
                    if(index - 1 < index){
                        if(tokensInput[index - 1].back() == '(' ||
                            tokensInput[index - 1] == ","){
                            output.emplace_back(token("", tok::hold));
                        }
                    }
                    token tk("SEP", tok::sep);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "+"){
                    token tk("ADD", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "-"){
                    token tk("SUB", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "*"){
                    token tk("MUL", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "/"){
                    token tk("DIV", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "%"){
                    token tk("MOD", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "^"){
                    token tk("BITXOR", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "!"){
                    token tk("NOT",tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "("){
                    if(index - 1 < index){
                        if((tokensInput[index - 1] == ")" ||
                            string_isnum(tokensInput[index - 1]) ||
                            isalpha(tokensInput[index - 1].front()) ||
                            tokensInput[index - 1].front() == '_') &&
                            tokensInput[index - 1].back() != '(' &&
                            tokensInput[index - 1].back() != '.'){
                            output.emplace_back(token("MUL", tok::op));
                        }
                    }
                    token tk("L_BRAC", tok::lbrac);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == ")"){
                    if(index - 1 < index){
                        if(tokensInput[index - 1] == ","){
                            output.emplace_back(token("", tok::hold));
                        }
                    }                    
                    token tk("R_BRAC", tok::rbrac);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "="){
                    token tk("ASN", tok::asn);
                    output.emplace_back(tk); // Assign
                }else if(tokensInput[index] == "**"){
                    token tk("POW", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "//"){
                    token tk("FLOORDIV", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "||"){
                    token tk("LOR", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "&&"){
                    token tk("LAND", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "&"){
                    token tk("BITAND", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "|"){
                    token tk("BITOR", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == ">>"){
                    token tk("SHR", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "<<"){
                    token tk("SHL", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "==="){
                    token tk("TEQL",100); // Special
                    output.emplace_back(tk); // Equal
                }else  if(tokensInput[index] == "=="){
                    token tk("EQL", tok::op);
                    output.emplace_back(tk); // Equal
                }else if(tokensInput[index] == "!="){
                    token tk("NQL", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == ">="){
                    token tk("GQL", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "<="){
                    token tk("LQL", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == ">"){
                    token tk("GRT", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "<"){
                    token tk("LST", tok::op);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "+="){
                    token tk("ADDASN", tok::asn);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "-="){
                    token tk("SUBASN", tok::asn);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "*="){
                    token tk("MULASN", tok::asn);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "/="){
                    token tk("DIVASN", tok::asn);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "^="){
                    token tk("BITXORASN", tok::asn);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "<<="){
                    token tk("SHLASN", tok::asn);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == ">>="){
                    token tk("SHRASN", tok::asn);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "**="){
                    token tk("POWASN", tok::asn);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "|="){
                    token tk("BITORASN", tok::asn);
                    output.emplace_back(tk);
                }else if(tokensInput[index] == "&="){
                    token tk("BITANDASN", tok::asn);
                    output.emplace_back(tk);
                }else if(tokensInput[index].front() == '.'){
                    if(tokensInput[index].back() == 'i'){
                        tokensInput[index].pop_back();
                        output.emplace_back(token(tokensInput[index], tok::num, 1));
                    }else{
                        output.emplace_back(token(tokensInput[index], tok::num, 0));
                    }
                    
                }else if(tokensInput[index].back() != '(' &&
                        (isalpha(tokensInput[index].front()) ||
                        tokensInput[index].front() == '_')){

                    if(index - 1 < index){
                        if((tokensInput[index - 1] == ")" ||
                            string_isnum(tokensInput[index - 1]) ||
                            isalpha(tokensInput[index - 1].front()) ||
                            tokensInput[index - 1].front() == '_') &&
                            tokensInput[index - 1].back() != '(' &&
                            tokensInput[index - 1].back() != '.'){
                            output.emplace_back(token("MUL", tok::op));
                        }
                    }

                    token tk(tokensInput[index],tok::var);
                    output.emplace_back(tk);
                }else if(tokensInput[index].back() == '('){
                    if(index - 1 < index){
                        if((tokensInput[index - 1] == ")" ||
                            string_isnum(tokensInput[index - 1]) ||
                            isalpha(tokensInput[index - 1].front()) ||
                            tokensInput[index - 1].front() == '_') &&
                            tokensInput[index - 1].back() != '(' &&
                            tokensInput[index - 1].back() != '.'){
                            output.emplace_back(token("MUL", tok::op));
                        }
                    }
                    size_t d = std::count(tokensInput[index].begin(), tokensInput[index].end(), '`');
                    tokensInput[index].erase(std::remove(tokensInput[index].begin(), tokensInput[index].end(), '`'), tokensInput[index].end());
                    token tk(tokensInput[index], tok::func);
                    if(index+2 < tokensInput.size() && tokensInput[index+2] == ")"){
                        tk.reserved = 0;
                    }else{
                        tk.reserved = 1;
                    }
                    if(d == std::string::npos){
                        tk.special = 0;
                    }else{
                        tk.special = static_cast<long int>(d);
                    }
                    output.emplace_back(tk);
                }else if((tokensInput[index].front() == '"' && tokensInput[index].back() == '"') ||
                    (tokensInput[index].front() == '\'' && tokensInput[index].back() == '\'')){
                    token tk;
                    tk.type = tok::str;
                    if(tokensInput[index].size() >= 2){
                        tk.data = tokensInput[index].substr(1,tokensInput[index].size()-2);
                    }else{
                        tk.data = "";
                    }
                
                    output.emplace_back(tk);
                }else{
                    token tk("",tok::hold);
                    output.emplace_back(tk);
                }
            }else if(tokensInput[index].back() == '('){
                if(index - 1 < index){
                    if((tokensInput[index - 1] == ")" ||
                        string_isnum(tokensInput[index - 1]) ||
                        isalpha(tokensInput[index - 1].front()) ||
                        tokensInput[index - 1].front() == '_') &&
                        tokensInput[index - 1].back() != '(' &&
                        tokensInput[index - 1].back() != '.'){
                        output.emplace_back(token("MUL", tok::op));
                    }
                }
                size_t d = std::count(tokensInput[index].begin(), tokensInput[index].end(), '`');
                tokensInput[index].erase(std::remove(tokensInput[index].begin(), tokensInput[index].end(), '`'), tokensInput[index].end());
                token tk(tokensInput[index],tok::func);
                if(index+2 < tokensInput.size() && tokensInput[index+2] == ")"){
                    tk.reserved = 0;
                }else{
                    tk.reserved = 1;
                }
                if(d == std::string::npos){
                    tk.special = 0;
                }else{
                    tk.special = static_cast<long int>(d);
                }
                output.emplace_back(tk);
            }else if(ttype(tokensInput[index]) == tok::num){
                if(index - 1 < index){
                    if((tokensInput[index - 1] == ")" ||
                        string_isnum(tokensInput[index - 1]) ||
                        isalpha(tokensInput[index - 1].front()) ||
                        tokensInput[index - 1].front() == '_') &&
                        tokensInput[index - 1].back() != '(' &&
                        tokensInput[index - 1].back() != '.'){
                        output.emplace_back(token("MUL", tok::op));
                    }
                }
                token tk(tokensInput[index], tok::num);
                output.emplace_back(tk);
            }else if((tokensInput[index].front() == '"' &&
                      tokensInput[index].back() == '"') ||
                     (tokensInput[index].front() == '\'' &&
                      tokensInput[index].back() == '\'')){

                token tk;
                tk.type = tok::str;
                if(tokensInput[index].size() >= 2){
                    tk.data = tokensInput[index].substr(1,tokensInput[index].size()-2);
                }else{
                    tk.data = "";
                }

                output.emplace_back(tk);
            }else{
                if(index - 1 < index){
                    if((tokensInput[index - 1] == ")" ||
                        string_isnum(tokensInput[index - 1]) ||
                        isalpha(tokensInput[index - 1].front()) ||
                        tokensInput[index - 1].front() == '_') &&
                        tokensInput[index - 1].back() != '(' &&
                        tokensInput[index - 1].back() != '.'){
                        output.emplace_back(token("MUL", tok::op));
                    }
                }
                token tk(tokensInput[index],tok::var);
                output.emplace_back(tk);
            }
        }

        // Check for negation on first token
        unsigned long int replidx = 1;
        if(output.front().type == tok::op &&
           output.front().data == "SUB"){
            output.front().data = "NEG"; 
            output.front().type = tok::op;
        }else if(output.front().type == tok::op &&
                 output.front().data == "ADD"){
            output.front().data = "POS"; 
            output.front().type = tok::op;
        }
        if(output.size() >= 2){
            if(output.front().type == tok::op &&
              (output.front().data == "SUB" ||
              output.front().data == "ADD")){
                if(output[1].type == tok::op ||
                   output[1].type == tok::var ||
                   output[1].type == tok::str ||
                   output[1].type == tok::func ||
                   output[1].type == tok::lbrac){
                    if(output.front().data == "SUB"){
                        output.front().data = "NEG"; 
                        output.front().type = tok::op;
                    }else if(output.front().data == "ADD"){
                        output.front().data = "POS"; 
                        output.front().type = tok::op;
                    }
                }
            }
        }

        // Check for negation
        while(replidx < output.size()){
            if(replidx + 1 < output.size() &&
              output[replidx].type == tok::op &&
             (output[replidx].data == "SUB" ||
              output[replidx].data == "ADD")){
                if((output[replidx + 1].type == tok::lbrac || 
                    output[replidx + 1].type == tok::op ||
                    output[replidx - 1].type == tok::hold ||
                    output[replidx - 1].type == tok::sep ||
                    output[replidx - 1].type == tok::lbrac || 
                    output[replidx - 1].type == tok::op) &&
                    output[replidx - 1].type != tok::num &&
                    output[replidx - 1].type != tok::var &&
                    output[replidx - 1].type != tok::str &&
                    output[replidx - 1].type != tok::rbrac){
                    if(output[replidx].data == "SUB"){
                        output[replidx].data = "NEG"; 
                        output[replidx].type = tok::op;
                    }else if(output[replidx].data == "ADD"){
                        output[replidx].data = "POS"; 
                        output[replidx].type = tok::op;
                    }
                }
            }
            replidx++;
        }
        tokensInput.clear();
        std::vector<std::string>().swap(tokensInput);
        return output;
    }


    int assigntype(std::string str){
        size_t eqpos = str.find('=');
        if(eqpos == std::string::npos){
            return -1;
        }else{
            int ieqpos = static_cast<int>(eqpos);
            if(ieqpos - 2 >= 0){
                if(str[ieqpos - 2] == '<' && str[ieqpos - 1] == '<'){
                    return 8;
                }else if(str[ieqpos - 2] == '>' && str[ieqpos - 1] == '>'){
                    return 9;
                }else if(str[ieqpos - 2] == '*' && str[ieqpos - 1] == '*'){
                    return 7;
                }else{

                }
            }
            if(ieqpos - 1 >= 0){
                if(str[ieqpos - 1] == '+'){
                    return 1;
                }else if(str[ieqpos - 1] == '-'){
                    return 2;
                }else if(str[ieqpos - 1] == '*'){
                    return 3;
                }else if(str[ieqpos - 1] == '/'){
                    return 4;
                }else if(str[ieqpos - 1] == '%'){
                    return 5;
                }else if(str[ieqpos - 1] == '^'){
                    return 6;
                }else{

                }
            }
        }

        return 0;
    }

    // Removes all whitespace that 
    std::string removeWhiteSpace(std::string str){
        bool instring = false;
        for(unsigned long int rindx = 0; rindx < str.size(); rindx++){
            if(!instring && isspace(str[rindx])){
                str.erase(str.begin()+rindx);
            }
            if(str[rindx] == '"' || str[rindx] == '\''){
                if(!instring){
                    instring = true;
                }else{
                    if(rindx >= 1){
                        if(str[rindx - 1] == '\\') continue;
                    }else{
                        instring = false;
                    }
                }
            }
        }
        return str;
    }

    std::string stripcomment(std::string str){
        bool take = true;
        std::string out;
        out.reserve(str.size());
        for(unsigned long int idx = 0; idx < str.size(); ++idx){
            if(idx+1 < str.size()){
                if(str[idx] == '/' && str[idx + 1] == '/'){
                    return str.substr(0, idx);
                }
                if(str[idx] == '/' && str[idx + 1] == '*'){
                    take = false;
                }else if(str[idx] == '*' && str[idx + 1] == '/'){
                    ++idx;
                    take = true;
                }else{
                    if(take){
                        out.push_back(str[idx]);
                    }
                }
            }else{
                if(take){
                    out.push_back(str[idx]);
                }
            }

        }
        str.clear();
        std::string().swap(str);
        return out;
    }

    /*
        0 Calculate
        1 Assign
        2 Assign Function
    */
    int execType(std::string str){
        long long int bcount = 0;
        unsigned long int index = 0;
        bool squote = false;
        bool dquote = false;
        bool hasbracket = false;
        while(index < str.size()){
            if(str[index] == '\''){
                while(index < str.size() && squote){
                    if(index - 1 < index && str[index] == '\''){
                        if(str[index - 1] == '\\') ++index;
                        else squote = false;
                    }else if(str[index] == '\''){
                        squote = false;
                    }
                    ++index;
                }
            }
            if(str[index] == '"'){
                while(index < str.size() && dquote){
                    if(index - 1 < index && str[index] == '"'){
                        if(str[index - 1] == '\\') ++index;
                        else dquote = false;
                    }else if(str[index] == '"'){
                        dquote = false;
                    }
                    ++index;
                }
            }
            if(str[index] == '('){
                ++bcount;
                hasbracket = true;
            }
            if(str[index] == ')'){
                --bcount;
                hasbracket = true;
            }
            if(bcount == 0 && str[index] == '='){
                if(hasbracket){
                    if(index - 1 < index){

                        // basically a chained if ||
                        switch(str[index - 1]){
                            case '&':
                            case '|':
                            case '^':
                            case '%':
                            case '/':
                            case '*':
                            case '+':
                            case '-':
                            case '=':
                            case '!':
                            case '<':
                            case '>':
                                return 0;
                        
                        }
                    }
                    if(index + 1 < str.size()){
                        if(str[index + 1] == '='){
                            return 0;
                        }
                    }
                    return 2;
                }else{
                    return 0;
                }
                
            }
            ++index;
        }
        return 0;
    }

    dipart< std::vector<token> > spliteq(std::vector<token> str){
        dipart< std::vector<token> > out;
        long long int bracket_counter = 0;
        unsigned long int index = 0;
        while(index < str.size() && !(bracket_counter == 0 && str[index].type == tok::asn)){
            if(str[index].type == tok::lbrac) ++bracket_counter;
            if(str[index].type == tok::rbrac) --bracket_counter;
            ++index;
        }
        out.front = std::vector<token>(str.begin(), str.begin() + index);
        ++index;
        if(index < str.size()) out.back = std::vector<token>(str.begin() + index, str.end());

        return out;
        
    }

    std::string strchar_parse(const std::string& str){
        unsigned long int index = 0;
        std::string outstr;
        outstr.reserve(str.size());
        char next;
        bool nsuccess;
        while(index < str.size()){
            if(index + 1 < str.size()){
                next = str[index + 1];
                nsuccess = true;
            }else{
                nsuccess = false;
            }
            if(str[index] == '\\' && nsuccess){
                switch(next){
                    case 'n':
                        outstr.push_back('\n');
                        break;
                    case 'f':
                        outstr.push_back('\f');
                        break;
                    case 'r':
                        outstr.push_back('\r');
                        break;
                    case 'b':
                        outstr.push_back('\b');
                        break;
                    case 't':
                        outstr.push_back('\t');
                        break;
                    case '\\':
                        outstr.push_back('\\');
                        break;
                    case '\'':
                        outstr.push_back('\'');
                        break;
                    case '"':
                        outstr.push_back('"');
                        break;
                    default:
                        ++index;

                }
                ++index;
            }else{
                outstr.push_back(str[index]);
                ++index;
            }
        }
        return outstr;
    }


}
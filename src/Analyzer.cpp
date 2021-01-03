#include<string>
#include<vector>

#include "Analyzer.hpp"
#include "Token.hpp"

// Check if there is an unexpected token
// true = Pass, false = Fail
bool checkrpn(const std::vector<token> checktokens){

	// Must be Signed int
	signed long long int simulatestack = 0;
	for(unsigned long int index = 0; index < checktokens.size(); index++){
		switch(checktokens[index].type){
			case 9:
			case 5:
			case 0:
				simulatestack++;
				break;
			case 1:
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
			case 4:
				simulatestack -= (signed long long int)checktokens[index].reserved;
				simulatestack++;
				break;
			case 6:
				break;
			case 10:
				simulatestack--;
				simulatestack--;
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
bool squotecount(const std::string str){
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
bool quotecount(const std::string str){
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
int checkleftBrac(const std::string str){
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

int checkrightBrac(const std::string str){
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


#include <iostream>
#include <stack>
#include <string>
#include <algorithm>
#include <stdexcept>

class Calc {
private:
	bool calcTop(std::stack<double>& num, std::stack<char>& sym) {
		double temp1 = num.top();
		num.pop();
		double temp2 = num.top();
		num.pop();
		switch (sym.top()) {
		case '+': 
			num.push(temp2 + temp1); 
			break;
		case '-': 
			num.push(temp2 - temp1); 
			break;
		case '*': 
			num.push(temp2 * temp1);
			break;
		case '/':
			if (temp1 == 0.0) {
				throw std::invalid_argument("Division by zero!");
				return false;
			}
			num.push(temp2 / temp1); 
			break;
		}
		sym.pop();
		return true;
	}
	void dealExpr(std::string& expr){
		expr.erase(std::remove_if(expr.begin(), expr.end(), ::isspace), expr.end());
		std::stack<int> pare;
		if(expr.size() == 0){
			expr = "0";
			return;
		}
		for(int i = 0; i < expr.size(); ++i){
			if(expr[i] == '('){ 
				pare.push(1);
				if(i && isdigit(expr[i - 1])){
					expr.insert(i, 1, '*');
					i++;
				}
			}
			else if(expr[i] == ')'){
				if(pare.empty()) 
					throw std::invalid_argument("Is NOT an Standard Expr!");
				pare.pop();
			}
		}
		if(!pare.empty()) 
			throw std::invalid_argument("Is NOT an Standard Expr!");
		return;
	}
	
	double calcExpr(std::string expr, int &i){
		std::stack<double> num;
		std::stack<char> sym;
		expr += '=';
		int n = expr.size();
		while(i<n){
			if(isdigit(expr[i]) || expr[i] == '-'){
				std::string temp;
				for(int k = i; k < expr.size(); k++){
					if(isdigit(expr[k]) || expr[k] == '.' || (expr[k] == '-' && k == i))
						temp += expr[k];
					else{
						i = k;
						break;
					}
				}
				num.push(stod(temp));
			}
			else if(expr[i] == '('){
				++i;
				num.push(calcExpr(expr, i));
				++i;
			}
			if(expr[i] == '+' || expr[i] == '-'){
				while(1){
					if(sym.empty()){
						sym.push(expr[i]);
						break;
					}
					else calcTop(num, sym);
				}
			}
			else if(expr[i] == '*' || expr[i] == '/'){
				while(1){
					if(sym.empty() || sym.top() == '+' || sym.top() == '-'){
						sym.push(expr[i]);
						break;
					}
					else calcTop(num, sym);
				}
			}
			else if(expr[i] == ')' || expr[i] == '='){
				while(1){
					if(sym.empty()) return num.top();
					else calcTop(num, sym);
				}
			}
			++i;
		}
		return num.top();
	}
public:
	void run(void) {
		std::string expr;
		std::cout << "Press \"q\" to quit\n";
		while(true) {
			std::cout << "Input Expr:\n";
			std::getline(std::cin, expr);
			int i = 0;
			if(expr == "q") break;
			try {
				dealExpr(expr);
				double ans = calcExpr(expr, i);
				std::cout << "Answer: " << ans << std::endl;
			} catch (const std::exception& e) {
				std::cerr << "Error: " << e.what() << std::endl;
			}
			std::cout << std::endl;
		}
	}
	static double calc(std::string& expr) {
		Calc c;
		c.dealExpr(expr);
		int i = 0;
		return c.calcExpr(expr, i);
	}
};

int main() {
	Calc c;
	c.run();
	//std::cout << Calc::calc("7.65*(6/7-(7*9)/2.36+3)-5/(((7)))+-1.657*-2.37");
	return 0;
}

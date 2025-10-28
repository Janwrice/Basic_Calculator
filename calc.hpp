
//基本计算器
//By_Janwrice(QQ:2168197068)
//可正确计算操作数为实数，且带有 + - * / 以及嵌套括号的复杂表达式
//可处理表达式中含有空格、表达式中括号不匹配、表达式中除以零、在括号前省略 * 号的等复杂情况

#ifndef CALC_H
#define CALC_H

#include <iostream> //标准输入输出
#include <stack> //用于创建符号栈、数字栈以实现双栈法求解表达式
#include <string> //用于处理字符串
#include <algorithm> //会用到一些算法函数
#include <stdexcept> //处理异常
#include <cctype> //使用 isdigit() 函数

/*
* 核心计算类，提供两个接口
* 一个是交互计算函数run()
* 一个是可以被直接调用的静态计算函数calc()，接受一个表达式字符串
*/
class Calc {
private:
	//预处理表达式函数
	inline void dealExpr(std::string& expr);
	//计算栈顶的表达式
	inline void calcTop(std::stack<double>& num, std::stack<char>& sym);
	//核心计算逻辑
	inline double calcExpr(std::string expr, int &i);
public:
	//交互计算函数run()
	inline void run(void);
	//可被直接调用的静态计算函数calc()
	static inline double calc(std::string& expr);
};

inline void Calc::dealExpr(std::string& expr){
	//首先去除表达式中可能出现的空格
	expr.erase(std::remove_if(expr.begin(), expr.end(), ::isspace), expr.end());
	std::stack<int> pare;//用于检测括号是否合法
	//如果未输入任何值则默认结果为0
	if(expr.size() == 0){
		expr = "0";
		return;
	}
	//检测括号是否合法
	for(int i = 0; i < expr.size(); ++i){
		//遇到左括号入栈
		if(expr[i] == '('){ 
			pare.push(1);
			//顺便处理括号前省略 * 号的情况
			if(i && isdigit(expr[i - 1])){
				expr.insert(i, 1, '*');//如果括号前就是数字说明省略了 * 号，插入 * 号
				i++;//因为插入了 * 号，当前索引需要加一，否则左括号会被重复匹配
			}
		}
		//遇到右括号出栈
		else if(expr[i] == ')'){
			if(pare.empty()) //如果没有可以匹配右括号的左括号则抛出异常
				throw std::invalid_argument("Is NOT a Standard Expr!");
			pare.pop();//先判空后出栈，防止段错误
		}
	}
	if(!pare.empty()) //如果存在右括号没有被左括号匹配则抛出异常
		throw std::invalid_argument("Is NOT a Standard Expr!");
	return;
}

inline void Calc::calcTop(std::stack<double>& num, std::stack<char>& sym) {
	double temp1 = num.top();//获取后操作数
	num.pop();//出栈
	double temp2 = num.top();//获取前操作数
	num.pop();//出栈
	//获取操作符确定以要进行什么计算
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
		//如果出现除以零的情况则抛出除以零异常
		if (temp1 == 0.0) {
			throw std::invalid_argument("Division by zero!");
		}
		num.push(temp2 / temp1); 
		break;
	}
	sym.pop();//操作结束后操作符出栈
	return;
}

inline double Calc::calcExpr(std::string expr, int &i){
	std::stack<double> num; //操作数栈
	std::stack<char> sym; //运算符栈
	expr += '='; //在表达式后加入 = 以方便确定表达式是否遍历完毕
	int n = expr.size(); //确定表达式长度
	//开始遍历表达式
	while(i<n){
		//在这里检测 - 号实际上是安全的，
		//因为后面的代码可以保证重新循环到这里时的字符一定是可被操作的数字（或者是括号，括号会被递归处理成数字），而不是运算符
		//首先如果它是数字，或者是数字前的负号，就开始处理数字
		if(isdigit(expr[i]) || expr[i] == '-'){
			std::string temp; //用于临时存储当前的操作数
			//从当前索引开始遍历直到它不再是数字：
			for(int k = i; k < expr.size(); k++){
				//如果它是数字或者是小数点，或者当第一次遍历是 - 号时，接到临时存储变量 temp 中
				if(isdigit(expr[k]) || expr[k] == '.' || (expr[k] == '-' && k == i))
					temp += expr[k];
				else{
					//遍历完以后，更新索引i，跳出循环，接着处理数字后面的字符
					i = k;
					break;
				}
			}
			num.push(stod(temp)); //把存好到temp中的操作数转换为 double 类型用于计算
		}
		//如果它是左括号就开始递归
		else if(expr[i] == '('){
			++i; //递归时将索引加一，确保从括号中的表达式开始递归运算
			num.push(calcExpr(expr, i));
			++i; //递归后将索引加一，否则当前索引是右括号
		}
		//处理完数字后开始处理运算符：
		//处理数字和操作符是分开的操作，所以这里是 if 而不是 else if
		//如果是 + 和 -：
		if(expr[i] == '+' || expr[i] == '-'){
			/*注意，在遇到一个运算符时，如果只将栈顶的一个运算符弹出计算（如果满足优先级条件），
			而不是将所有优先级高于或等于当前运算符的运算符都弹出。这会导致计算顺序错误。*/
			//所以这里使用 while 循环确保运算可以清空栈
			while(1){
				//当栈为空的时候，将当前符号入栈，并跳出循环
				if(sym.empty()){
					sym.push(expr[i]);
					break;
				}
				//否则开始计算栈顶的基础表达式
				else calcTop(num, sym);
			}
		}
		//如果是 * 和 /：
		else if(expr[i] == '*' || expr[i] == '/'){
			while(1){
				//与 + - 逻辑几乎一致，只是多了遇到 + - 时就要将当前符号入栈，并跳出循环
				//这实现了处理运算符的优先级
				if(sym.empty() || sym.top() == '+' || sym.top() == '-'){
					sym.push(expr[i]);
					break;
				}
				else calcTop(num, sym);
			}
		}
		//如果遇到了右括号或者 = 号，就清空当前栈并返回
		else if(expr[i] == ')' || expr[i] == '='){
			while(1){
				if(sym.empty()) return num.top();
				else calcTop(num, sym);
			}
		}
		++i; //处理完数字后的符号后，将索引自增，接着处理下一个数字
	}
	return num.top(); //理论上这句应该不会被执行
}

inline void Calc::run(void) {
	std::string expr;
	std::cout << "Press \"q\" to quit\n";
	while(true) {
		std::cout << "Input Expr:\n";
		std::getline(std::cin, expr);//获取输入的表达式
		int i = 0;//初始化索引，计算函数以来表达式每个字符的索引用于递归
		if(expr == "q") break;//发现输入 q 退出交互界面
		//运行主要计算逻辑并正确处理异常：
		try {
			dealExpr(expr);//预处理表达式
			double ans = calcExpr(expr, i);//计算表达式
			std::cout << "Answer: " << ans << std::endl;//返回结果
		} catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;//若出现异常则返回异常
		}
		std::cout << std::endl;
	}
}

inline double Calc::calc(std::string& expr) {
	Calc c;
	c.dealExpr(expr);
	int i = 0;
	return c.calcExpr(expr, i);
}

#endif

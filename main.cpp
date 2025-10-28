//使用示例：

#include<iostream>
#include"calc.h"
int main(void){
	//创建计算对象并运行交互计算函数：
	Calc c;
	c.run();
	//一个静态函数测试用例：
	//std::cout << Calc::calc("7.65*(6/7-(7*9)/2.36+3)-5/(((7)))+-1.657*-2.37");
	return 0;
}

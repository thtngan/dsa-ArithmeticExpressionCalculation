#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <cmath>

using namespace std;

bool isOperator(char opt) { // check whether  it's a operator or not
	if (opt == '+' || opt == '-' || opt == '*' || opt == '/' || opt == '^')
		return true;
	return false;
}
bool isOperand(char opr) { // check whether it's operand or not
	if (isOperator(opr) || opr == '{' || opr == '}' || opr == '[' || opr == ']' || opr == '(' || opr == ')')
		return false;
	return true;
}
int setPriority(char opt) { // set priority for different operator
	if (opt == '+' || opt == '-')
		return 0;
	if (opt == '*' || opt == '/')
		return 1;
	if (opt == '^')
		return 2;
	return -1;
}
bool checkPriority(char opt1, char opt2) { // check priority of two operators (last item in the Stack and current item)
	int opt1Priority = setPriority(opt1);
	int opt2Priority = setPriority(opt2);
	if (opt1Priority == -1 || opt2Priority == -1) // check both of items are operators
		return false;
	if (opt1Priority >= opt2Priority) // if last item in the Stack has higher or equal priority than current item then return true
		return true;
	return false;
}
int checkToken(char a) { // Set the flag for each token in the expression
	if (a == ' ')
		return -1;
	if (isOperator(a))
		return 0;
	if (isOperand(a))
		return 1;
	return -1;
}
bool checkForValidity(string infix) { // check the expression is the expression infix
	int i = 0, prev = -2;
	if (isOperator(infix[0]) || isOperator(infix[infix.size() - 1])) // if the expression start or end with an operator
		return false; // then this expression is invalid. 
	while (i < infix.size()) {
		int n = checkToken(infix[i]);
		if (n == -1)
			i++;
		else if (prev == 0 && n == 0)  //if there are 2 operators that are adjacent
			return false; // then this expression is invalid. 
		else {
			prev = n;
			i++;
		}
	}
	return true;
}
void standardizeString(string& S) { // standardize the expression by replace {} and [] by ()
	for (int i = 0; i < S.length(); i++) {
		if (S[i] == '{')
			S[i] = '(';
		if (S[i] == '[')
			S[i] = '(';
		if (S[i] == '}')
			S[i] = ')';
		if (S[i] == ']')
			S[i] = ')';
	}
}
float calculateExpression(float a, float b, char opt) { // calculate the expression with two operand 
	switch (opt) {
	case '+': return(float)a + b;
	case '-': return(float)a - b;
	case '*': return(float)a * b;
	case '/': return(float)a / b;
	case '^': return(float)pow(a, b);
	}
}

string evaluateInfix(string a) {  // functions to evaluate the expression
	string expression = a;
	int len = expression.length();  // get string length

	standardizeString(expression);

	stack <float> values;  // stack to store float value
	stack <char> operators; // stack to store operand such as + - * /

	for (int i = 0; i < len; i++) {
		if (expression[i] == ' ') {  // skip the whitespace 
			continue;
		}
		else if (expression[i] == '(') {    // push the'(' into stack of operator
			operators.push(expression[i]);
		}
		else if (isOperand(expression[i]) == true) { // get and convert string of a float number and push it in value stack
			string temp1;  // string that cointain the float number string
			for (int j = i; j < len; j++) {
				if (isOperand(expression[j]) == 0) {   // escape the loop once encounter the end of an float number 
					i = j - 1;
					break;
				}
				temp1 = temp1 + expression[j];
			}
			values.push(stof(temp1));  // convert and push float number into stack 
		}
		else if (expression[i] == ')') {  // calculate the sub-expression between the '()'
			while (!operators.empty() && operators.top() != '(') {  // calculate with two value from value stack until encounter '(' in operator stack
				float val2 = values.top();
				values.pop();

				float val1 = values.top();
				values.pop();

				char op = operators.top();
				operators.pop();

				values.push(calculateExpression(val1, val2, op));
			}
			if (!operators.empty()) // pop '(' out once finishing calculating all expression in the ()
				operators.pop();
		}
		else
		{
			while (!operators.empty() && setPriority(operators.top()) >= setPriority(expression[i])) { // calculate the sub-expression with higher priority
				float val2 = values.top();
				values.pop();

				float val1 = values.top();
				values.pop();

				char op = operators.top();
				operators.pop();

				values.push(calculateExpression(val1, val2, op));
			}
			// Push current operator to operator stack.
			operators.push(expression[i]);
		}
	}

	while (!operators.empty()) {  // Here, we have eliminated all of the bracket in the expression
		float val2 = values.top();  // evaluate the remaining of expression 
		values.pop();

		float val1 = values.top();
		values.pop();

		char op = operators.top();
		operators.pop();

		values.push(calculateExpression(val1, val2, op));
	}

	// Top of 'values' contains result, convert it back to string return.
	stringstream ss;
	ss << values.top();
	string s = ss.str();
	return s;
}
string infixToPostfix(string infix) {
	string postfix = "", tempStack = "";
	char space = ' ';
	int i = 0, prev = 1;
	while (i < infix.size()) {
		if (infix[i] == ' ') // skip the whitespace 
			i++;
		if (isOperand(infix[i])) { // push back the operand into postfix
			if (prev != 1)
				postfix += ' ';
			postfix += infix[i];
		}
		else {
			if (isOperator(infix[i])) {
				//if the last operator in the tempStack has higher priority than the current item
				while (!tempStack.empty() && isOperator(tempStack[tempStack.size() - 1]) && checkPriority(tempStack[tempStack.size() - 1], infix[i])) {
					postfix += ' ';
					postfix += tempStack[tempStack.size() - 1]; // then push this last operator into postfix 
					tempStack.pop_back(); //and delete it from tempstack
				}
				tempStack += infix[i]; // push back the curent item into tempStack 
			}
			else if (infix[i] == '(' || infix[i] == '[' || infix[i] == '{') //push all '(', '[', '{' into tempStack
				tempStack += infix[i];
			else {
				// push all of operators between the '(_)', '[_]', '{_}' from tempStack into postfix. 
				while (!tempStack.empty() && isOperator(tempStack[tempStack.size() - 1])) {
					postfix += ' ';
					postfix += tempStack[tempStack.size() - 1];
					tempStack.pop_back();
				}
				tempStack.pop_back(); //Then, delete '(', '[', '{' from tempStack
			}
		}
		prev = checkToken(infix[i]);
		i++;
	}
	while (!tempStack.empty()) { // push all remaining items from tempStack to postfix
		postfix += ' ';
		postfix += tempStack[tempStack.size() - 1];
		tempStack.pop_back();
	}
	return postfix;
}

vector<string> readFile(string fileName, char action, string N) {
	vector<string> listInfix;
	ifstream f;
	f.open(fileName.c_str());
	if (!f.is_open()) {
		cout << "Could not open file input";
		return listInfix;
	}

	string line;
	int numberExpressions = stoi(N);
	int i = 0; //number of expressions

	if (action == 't') {//Convert from Infix to Postfix
		while (!f.eof() && (i < numberExpressions)) {
			getline(f, line);
			if (!checkForValidity(line))
				listInfix.push_back("E");
			else {
				line = infixToPostfix(line);
				listInfix.push_back(line);
			}
			i++;
		}
	}
	else {//Calculate the infix expression
		while (!f.eof() && (i < numberExpressions)) {
			getline(f, line);
			if (!checkForValidity(line))
				listInfix.push_back("E");
			else {
				line = evaluateInfix(line);
				listInfix.push_back(line);
			}
			i++;
		}
	}
	f.close();
	return listInfix;
}
bool writeFile(string fileName, vector<string> listInfix) {
	ofstream f;
	f.open(fileName.c_str());
	if (!f.is_open())
		return false;
	for (int i = 0; i < listInfix.size(); i++)
		f << listInfix[i] << endl;
	f.close();
	return true;
}

//Struct: A.exe InputPath N [-c: calculate, -t: convert] OutputPath
int main(int argc, char* argv[]) {
	if (argc != 5 || (argv[3][1] != 'c' && argv[3][1] != 't')) {
		cout << "Struct: A.exe InputPath N Action OutputPath" << endl;
		return 0;
	}
	vector<string> listInfix = readFile(argv[1], argv[3][1], argv[2]);
	if (listInfix.empty()) {
		cout << "Could not open file input";
		return 0;
	}

	if (writeFile(argv[4], listInfix))
		cout << "Running successfully";
	else
		cout << "Could not open file output";

	return 0;
}


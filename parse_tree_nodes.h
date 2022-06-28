/*********************************************************************
  Name: Caleb Kirkland             NetID: cok18
  Course: CSE 4714              
  Project Part 4 - Interpreter
  Purpose of File: Holds the nodes that hold information/interpreting about the statements,
  expressions, and values in our program.
**********************************************************************/

#ifndef PARSE_TREE_NODES_H
#define PARSE_TREE_NODES_H

#include <iostream>
#include <vector>
#include <string>
#include "lexer.h"

using namespace std;

// Map container that will hold the variables and their values
extern map<string, int> symbolTable;

// Forward declarations
class programNode;
class blockNode;
class compoundNode;
class assignmentNode;
class ExprNode;
class simpleExpressionNode;
class TermNode;
class FactorNode;
class ifNode;
class WhileNode;
class ReadNode;
class StatementNode;
class WriteNode;

// Forward declarations of operator<<
ostream& operator<<(ostream& os, ExprNode& en);
ostream& operator<<(ostream& os, StatementNode& node);
ostream& operator<<(ostream& os, blockNode& node);
ostream& operator<<(ostream& os, programNode& node);
ostream& operator<<(ostream& os, compoundNode& node);
ostream& operator<<(ostream& os, simpleExpressionNode& node);

//*****************************************************************************
// Abstract class. Base class for IdNode, IntLitNode, NestedExprNode.
class FactorNode {
public:
	virtual int interpret(bool negative, bool type);
    virtual void printTo(ostream &os) = 0; // pure virtual method, makes the class Abstract
    virtual ~FactorNode(); // labeling the destructor as virtual allows 
	                       // the subclass destructors to be called
};

FactorNode::~FactorNode() {}
int FactorNode::interpret(bool negative, bool type) {return 0;}

// Uses double dispatch to call the overloaded method printTo in the 
// FactorNodes: IdNode, IntLitNode, and NestedExprNode
ostream& operator<<(ostream& os, FactorNode& fn) {

	//os << "factor( ";
	fn.printTo(os);
	//os << ") ";
	return os;
}

//*****************************************************************************
// class IdNode (Identifier Node)
class IdNode : public FactorNode {
public:
    string* id = nullptr;

    IdNode(string name);
    ~IdNode();
    void printTo(ostream & os);
	int interpret(bool negative, bool type);
};
IdNode::IdNode(string name) {
	id = new string(name);
}
IdNode::~IdNode() {
	cout << "Deleting a factorNode\n";
	delete id;
	id = nullptr;
}
void IdNode::printTo(ostream& os) {
	os << "factor( " << *id << " ) ";
}
int IdNode::interpret(bool negative, bool type) {
	
	// If the factor is negated and the type is "minus" or "not"
	if (negative == true &&  type == true) {
		return symbolTable.find(*id)->second *-1;
	} else if (negative == true && type == false) {
		return !symbolTable.find(*id)->second;
	} else {
		return symbolTable.find(*id)->second;
	}
}
//*****************************************************************************
// class IntLitNode (Integer Literal Node)
class IntLitNode : public FactorNode {
public:
    int int_literal = 0;

    IntLitNode(int value);
    ~IntLitNode();
    void printTo(ostream & os);
	int interpret(bool negative, bool type);
};

IntLitNode::IntLitNode(int value) {
	int_literal = value;
}

IntLitNode::~IntLitNode() {
	cout << "Deleting a factorNode\n";
	// Nothing to do since the only member variable is not a pointer
}

void IntLitNode::printTo(ostream& os) {
	os << "factor( " << int_literal << " ) ";
}

int IntLitNode::interpret(bool negative, bool type) {
	// If the factor is negated and the type is "minus" or "not"
	if (negative == true &&  type == true) {
		return int_literal *-1;
	} else if (negative == true && type == false) {
		return !int_literal;
	} else {
		return int_literal;
	}
}

//*****************************************************************************
// class FloatLitNode (Integer Literal Node)
class FloatLitNode : public FactorNode {
public:
    float float_literal = 0.0;

    FloatLitNode(float value);
    ~FloatLitNode();
    void printTo(ostream & os);
	int interpret(bool negative, bool type);
};

FloatLitNode::FloatLitNode(float value) {
	float_literal = value;
}

FloatLitNode::~FloatLitNode() {
	cout << "Deleting a factorNode\n";
	// Nothing to do since the only member variable is not a pointer
}

void FloatLitNode::printTo(ostream& os) {
	os << "factor( " << float_literal << " ) ";
}

int FloatLitNode::interpret(bool negative, bool type) {
	// If the factor is negated and the type is "minus" or "not"
	if (negative == true &&  type == true) {
		return int(float_literal) *-1;
	} else if (negative == true && type == false) {
		return !int(float_literal);
	} else {
		return int(float_literal);
	}
}

// STATEMENT -------------------------------------------------------------------------
class statementNode{
public:
	virtual void interpret();
    virtual void printTo(ostream &os) = 0; // pure virtual method, makes the class Abstract
    virtual ~statementNode(); // labeling the destructor as virtual allows 
	                          // the subclass destructors to be called
};

ostream& operator<<(ostream& os, statementNode& node){
    node.printTo(os);
    return os;
}

statementNode::~statementNode(){}
void statementNode::interpret() {}

//*****************************************************************************
// class TermNode (Terminal Node)
class  TermNode {
public:
    FactorNode* firstFactor = nullptr;
    vector<int> restFactorOps; // TOK_MULT_OP or TOK_DIV_OP
    vector<FactorNode*> restFactors;

	bool minusFactor;
	bool notFactor;

    ~TermNode();
	int interpret();
};

ostream& operator<<(ostream& os, TermNode& tn) {
	if (tn.minusFactor == true) {
		os << "term( factor( - ";
	} else if(tn.notFactor == true) {
		os << "term( factor( NOT ";
	} else {
		os << "term( ";
	}
	os << *(tn.firstFactor);

	int length = tn.restFactorOps.size();
	for (int i = 0; i < length; ++i) {
		int op = tn.restFactorOps[i];
		if (op == TOK_MULTIPLY)
			os << "* ";
		else
			os << "/ ";
		os << *(tn.restFactors[i]);
	}
	if(tn.minusFactor == true || tn.notFactor == true){
		os << ") ) ";
	} else {
		os << ") ";
	}
	return os;
}

TermNode::~TermNode() {
	cout << "Deleting a termNode\n";
	delete firstFactor;
	firstFactor = nullptr;

	int length = restFactorOps.size();
	for (int i = 0; i < length; ++i) {
		delete restFactors[i];
		restFactors[i] = nullptr;
	}
	// This is to make the test cases pass.
	// We are not creating new memory for negative factors, so this is similating it
	// if (minusFactor) {
	// 	FactorNode* garbageFactor = new IntLitNode(0);
	// 	delete garbageFactor;
	// 	garbageFactor = nullptr;
	// }
}

int TermNode::interpret() {
	int returnValue;
	// Interpret the first term of the expression
	// The first condition in the arugments for interpret represents if the factor is negated
	// True indicates yes, and false indicates no.
	// The second condition in the arguments represents the type of negation, minus or not,
	// true meaning minus, and false.
	if (minusFactor == true) {
		returnValue = firstFactor->interpret(true, true);
	} else if (notFactor == true) {
		returnValue == firstFactor->interpret(true, false);
	} else {
		returnValue = firstFactor->interpret(false, false);
	}
    

    int length = restFactorOps.size();
    for (int i = 0; i < length; ++i) {
        // get the value of the next Term
        int nextValue = restFactors[i]->interpret(false, false);

        // perform the operation (*, /, or AND) that separates the Terms
        if (restFactorOps[i] == TOK_MULTIPLY)
            returnValue = returnValue * nextValue;
        else if (restFactorOps[i] == TOK_DIVIDE)
            returnValue = returnValue / nextValue;
		else if (restFactorOps[i] == TOK_AND)
            returnValue = returnValue && nextValue;
    }

    return returnValue;
}

// SIMPLE EXPRESSION---------------------------------------------------------------------
class simpleExpressionNode {
public:
	TermNode* firstTerm = nullptr;
    vector<int> restTermOps; // TOK_ADD_OP or TOK_SUB_OP
    vector<TermNode*> restTerms;

	~simpleExpressionNode();
	int interpret();
};

ostream& operator<<(ostream& os, simpleExpressionNode& se) {
	os << "simple_expression( ";
	os << *(se.firstTerm);

	int length = se.restTermOps.size();
	for (int i = 0; i < length; ++i) {

		int op = se.restTermOps[i];
		if (op == TOK_PLUS) {
			os << "+ ";
		} else {
			os << "- ";
		}
		os << *(se.restTerms[i]);
	}
	os << ") ";
	return os;
}

simpleExpressionNode::~simpleExpressionNode() {
	cout << "Deleting a simpleExpressionNode\n";
	delete firstTerm;
	firstTerm = nullptr;

	int length = restTermOps.size();
	for (int i = 0; i < length; ++i) {
		delete restTerms[i];
		restTerms[i] = nullptr;
	}
}

int simpleExpressionNode::interpret() {
    // Interpret the first term of the expression
    int returnValue = firstTerm->interpret();

    int length = restTermOps.size();
    for (int i = 0; i < length; ++i) {
        // Interpret the value of the next term, 
		// values are ready to operate after this step
        int nextValue = restTerms[i]->interpret();

        // Perform the operation according to the operator (=, <, >, !=)
        if (restTermOps[i] == TOK_PLUS)
            returnValue = returnValue + nextValue;
        else if (restTermOps[i] == TOK_MINUS)
            returnValue = returnValue - nextValue;
		else if (restTermOps[i] == TOK_OR)
            returnValue = returnValue || nextValue;
    }

    return returnValue;
}

// EXPR NODE ***********************************************************************
class ExprNode {
public:
    simpleExpressionNode* firstSE = nullptr;
    vector<int> restSEOps; // TOK_ADD_OP or TOK_SUB_OP
    vector<simpleExpressionNode*> restSE;

    ~ExprNode();
	int interpret();
};

ostream& operator<<(ostream& os, ExprNode& en) {
	os << "expression( ";
	os << *(en.firstSE);

	int length = en.restSEOps.size();
	for (int i = 0; i < length; ++i) {

		int op = en.restSEOps[i];
		if (op == TOK_PLUS) {
			os << "+ ";
		} else if (op == TOK_NOTEQUALTO) {
			os << "<> ";
		} else if (op == TOK_GREATERTHAN) {
			os << "> ";
		} else if (op == TOK_LESSTHAN) {
			os << "< ";
		} else if (op == TOK_EQUALTO) {
			os << "= ";
		} else {
			os << "- ";
		}
		os << *(en.restSE[i]);
	}
	os << ")";
	return os;
}

ExprNode::~ExprNode() {
	cout << "Deleting an expressionNode\n";
	delete firstSE;
	firstSE = nullptr;

	int length = restSEOps.size();
	for (int i = 0; i < length; ++i) {
		delete restSE[i];
		restSE[i] = nullptr;
	}
}

int ExprNode::interpret() {
    // Interpret the first term of the expression
    int returnValue = firstSE->interpret();

    int length = restSEOps.size();
    for (int i = 0; i < length; ++i) {
        // Interpret the value of the next term, 
		// values are ready to operate after this step
        int nextValue = restSE[i]->interpret();

        // Perform the operation according to the operator (=, <, >, !=)
        if (restSEOps[i] == TOK_EQUALTO)
            returnValue = returnValue == nextValue;
        else if (restSEOps[i] == TOK_LESSTHAN)
            returnValue = returnValue < nextValue;
		else if (restSEOps[i] == TOK_GREATERTHAN)
            returnValue = returnValue > nextValue;
		else if (restSEOps[i] == TOK_NOTEQUALTO)
            returnValue = returnValue != nextValue;
    }

    return returnValue;
}

// ASSIGNMENT ------------------------------------------------------------------------
class assignmentNode : public statementNode{
public:
    string* name = nullptr;
    ExprNode* expression = nullptr;

	void printTo(ostream & os);
	assignmentNode(string assignmentName);
	~assignmentNode();
	void interpret();
};

assignmentNode::assignmentNode(string assignmentName) {
	name = new string(assignmentName);
}

assignmentNode::~assignmentNode() {
	cout << "Deleting an assignmentNode\n";
	delete name;
	name = nullptr;
	delete expression;
	expression = nullptr;
}

void assignmentNode::printTo(ostream& os) {
	os << "Assignment " << *name << " := " << *expression << endl;
}

ostream& operator<<(ostream& os, assignmentNode& node){
    os << "Assignment " << *(node.name) << " := " << *(node.expression) << endl;
    return os;
}

void assignmentNode::interpret() {
	int assignExpression = expression->interpret();
	symbolTable.erase(*name);
	symbolTable.insert({*name, assignExpression});
}

// COMPOUND -------------------------------------------------------------------------
class compoundNode : public statementNode {
public:
    vector<statementNode*> statements;
	void printTo(ostream & os);

	~compoundNode();
	void interpret();
};

ostream& operator<<(ostream& os, compoundNode& node){  
	os << "Begin Compound Statement\n";;

	int length = node.statements.size();
	for (int i = 0; i < length; ++i) {
		os << *(node.statements[i]);
	}
	os << "End Compound Statement\n";
	return os;
}

void compoundNode::printTo(ostream& os) {
	os << "Begin Compound Statement\n";
	int length = statements.size();
	for (int i = 0; i < length; ++i) {
		os << *(statements[i]);
	}
	os << "End Compound Statement\n";
}

compoundNode::~compoundNode() {
	cout << "Deleting a compoundNode\n";
	int length = statements.size();
	for (int i = 0; i < length; ++i) {
		delete statements[i];
		statements[i] == nullptr;
	}
}

void compoundNode::interpret() {
	int length = statements.size();
    for (int i = 0; i < length; ++i) {
        statements[i]->interpret();
    }
}

// IF -------------------------------------------------------------------------------
class ifNode : public statementNode {
public:
    ExprNode* expression = nullptr;
    statementNode* thenStatement = nullptr;
    statementNode* elseStatement = nullptr;

	void printTo(ostream & os);
	~ifNode();
	void interpret();
};

ostream& operator<<(ostream& os, ifNode& node){
	os << "If " << node.expression << endl;
	os << "%%%%%%%% True Statement %%%%%%%%" << endl;
	os << node.thenStatement;
	os << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	if (node.elseStatement != nullptr) {
		os << "%%%%%%%% False Statement %%%%%%%%" << endl;
		os << node.elseStatement;
		os << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	}
    return os;
}

void ifNode::printTo(ostream& os) {
	os << "If " << *expression << endl;
	os << "%%%%%%%% True Statement %%%%%%%%" << endl;
	os << *thenStatement;
	os << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	// Only display the else statement if it exists!
	if (elseStatement != nullptr) {
		os << "%%%%%%%% False Statement %%%%%%%%" << endl;
		os << *elseStatement;
		os << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	}	
}

ifNode::~ifNode() {
	cout << "Deleting an ifNode\n";
	delete expression;
	expression = nullptr;
	delete thenStatement;
	thenStatement = nullptr;
	delete elseStatement;
	elseStatement = nullptr;
}

void ifNode::interpret() {
	// Go into the then statement if and only if the expression returns true
	if (expression->interpret() != 0) {
		thenStatement->interpret();
	}
	else if (elseStatement != nullptr) {
		elseStatement->interpret();
	}
}

// WHILE -------------------------------------------------------------------------------
class whileNode : public statementNode {
public:
    ExprNode* expression = nullptr;
    statementNode* loopBody = nullptr;

	void printTo(ostream & os);
	~whileNode();

	void interpret();
};

void whileNode::printTo(ostream& os) {
	os << "While " << *expression << endl;
	os << "%%%%%%%% Loop Body %%%%%%%%" << endl;
	os << *loopBody;
	os << "%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
}

whileNode::~whileNode() {
	cout << "Deleting a whileNode\n";
	delete expression;
	expression = nullptr;
	delete loopBody;
	loopBody = nullptr;
}

void whileNode::interpret() {
	// While the expression is true (while the expr node returns true)
	// we want to go into the loop body
	while (expression->interpret() != 0) {
		loopBody->interpret();
	}
}

// READ ----------------------------------------------------------------------------------
class readNode : public statementNode {
public:
    string* name = nullptr;

	readNode(string readName);
    void printTo(ostream & os);
	~readNode();

	void interpret();
};

readNode::readNode(string readName) {
	name = new string(readName);
}
void readNode::printTo(ostream& os) {
	os << "Read Value " << *name << endl;
}

readNode::~readNode() {
	cout << "Deleting a readNode\n";
	delete name;
	name = nullptr;
}

void readNode::interpret() {
	int read;
	// taking in input from std::cin
	cin >> read;
	symbolTable.erase(*name);
	symbolTable.insert({*name, read});
}

// WRITE ----------------------------------------------------------------------------------
class writeNode : public statementNode {
public:
	bool writeType;
    string* name = nullptr;

	writeNode(string writeName);
    void printTo(ostream & os);
	~writeNode();

	void interpret();
};

writeNode::writeNode(string writeName) {
	name = new string(writeName);
}

void writeNode::printTo(ostream& os) {
	if (writeType){
		os << "Write String " << *name << endl;
	} else { 
		os << "Write Value " << *name << endl;
	}
}

writeNode::~writeNode() {
	cout << "Deleting a writeNode\n";
	delete name;
	name = nullptr;
}

void writeNode::interpret() {
	if (writeType) {
		string *output = new string(*name);
		output->pop_back();
		output->erase(output->begin());
		cout << *output << endl;
		delete output;
	} else {
		cout << symbolTable.find(*name)->second << endl;
	}
}

// BLOCK *****************************************************************************
class blockNode{
public:
    compoundNode* compoundStatement = nullptr;

    ~blockNode();
	void interpret();
};

ostream& operator<<(ostream& os, blockNode& node){  
    os << *(node.compoundStatement);
    return os;
}

blockNode::~blockNode(){
	cout << "Deleting a blockNode\n";
	delete compoundStatement;
	compoundStatement = nullptr;
}

void blockNode::interpret() {
	compoundStatement->interpret();
}

// PROGRAM *****************************************************************************
class programNode{
public:
    string* name = nullptr;
    blockNode* block = nullptr;

    programNode(string programName);
    ~programNode();

	void interpret();
};

programNode::programNode(string programName) {
	name = new string(programName);
}

ostream& operator<<(ostream& os, programNode& node){
    os << "Program Name " << *(node.name) << endl;
    os << *(node.block) << endl;
    return os;
}

programNode::~programNode() {
	cout << "Deleting a programNode\n";
	delete name;
	name = nullptr;
	delete block;
	block = nullptr;
}

void programNode::interpret() {
	block->interpret();
}

//*****************************************************************************
class NestedExprNode : public FactorNode {
public:
    ExprNode* exprPtr = nullptr;

    NestedExprNode(ExprNode* en);
	~NestedExprNode();
    void printTo(ostream & os);
	int interpret();
};

NestedExprNode::NestedExprNode(ExprNode* en) {
	exprPtr = en;
}
NestedExprNode::~NestedExprNode() {
	delete exprPtr;
	exprPtr == nullptr;
}

void NestedExprNode::printTo(ostream& os) {
	os << "nested_expression( " << *exprPtr << " ) ";
}

int NestedExprNode::interpret() {
	return exprPtr->interpret();
}


#endif /* PARSE_TREE_NODES_H */
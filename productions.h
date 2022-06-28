/*********************************************************************
  Name: Caleb Kirkland             NetID: cok18
  Course: CSE 4714              
  Project Part 4 - Interpreter
  Purpose of File: Holds the production rules and guides the parser
  to build the parse tree and symbol table. 
**********************************************************************/
#ifndef PRODUCTIONS_H
#define PRODUCTIONS_H

#include <iostream>
#include "parse_tree_nodes.h"

extern map<string, int> symbolTable;
int nextToken = 0;  // token returned from yylex
int level = 0;  // used to indent output to approximate parse tree
int declarationCount = 0;

extern "C"
{
	// Instantiate global variables used by flex
	extern char* yytext;       // text of current lexeme
	extern int   yylex();      // the generated lexical analyzer
}

// Production Functions
programNode* program();
blockNode* block();
readNode* read(); 
writeNode* write(); 
statementNode* statement();
assignmentNode* assignment();
compoundNode* compound_statement();
ifNode* If();
whileNode* While();
ExprNode* expression();
simpleExpressionNode* simple_expression();
TermNode* term();
FactorNode* factor();
void indent();

// Functions to check if the first token sets are correct.
bool first_of_read();
bool first_of_write();
bool first_of_program();
bool first_of_block();
bool first_of_statement();
bool first_of_expression();
bool first_of_if();
bool first_of_assignment();
bool first_of_while();
bool first_of_simple_expression();
bool first_of_term();
bool first_of_factor();
bool first_of_compound();

int lex() {
    nextToken = yylex(); // Get next token

    if( nextToken == TOK_EOF ) {
        yytext[0] = 'E'; yytext[1] = 'O'; yytext[2] = 'F'; yytext[3] = 0; 
    } else if (nextToken == TOK_IDENT) { // If its an identifier, add to table
        symbolTable.insert({yytext, 0});
    } 
    return nextToken;
}
inline void indent(){
    for (int i = 0; i<level; i++)
        cout << ("    ");
}
void output(){
    indent();
    cout << "-->found " << yytext << endl;
}

//********************** PRODUCTION FUNCTIONS ******************************
programNode* program() {
    if (!first_of_program()) {
        throw ("3: 'PROGRAM' expected");
    }
    cout << "enter <program>" << endl;
    indent();
    ++level;
    output();
    
    nextToken = yylex();
    programNode* newProgramNode = new programNode(string(yytext));

    output();

    // If the first token isnt an identifier, throw error
    if (nextToken != TOK_IDENT) {
        throw ("2: identifier expected");
    }
    
    nextToken = yylex();

    // If there isnt semi-colon after the ID, throw error
    if (nextToken != TOK_SEMICOLON) {
        throw ("14 : ';' expected");
    }
    output();
    
    nextToken = yylex();

    if (!first_of_block()) {
        // Block didnt start correctly
        throw ("18: error in declaration part OR 17: 'BEGIN' expected");
    }
    
    // As long as the current token isn't "End", block will be called recursively
    while (nextToken != TOK_END && nextToken != TOK_EOF) {
        newProgramNode->block = block();
    }
 
    --level;
    indent();
    cout << "exit <program>" << endl;
    nextToken = yylex();
    return newProgramNode;
}

blockNode* block() {
    indent();
    cout << "enter <block>" << endl;
    ++level;

    blockNode* newBlockNode = new blockNode;
    
    // First token must be var
    if (nextToken == TOK_VAR) {
        output();
        nextToken = yylex();
        // Must be an Identifier after
        if (nextToken != TOK_IDENT) {
            throw ("2: identifier expected");
        }
        
        if (symbolTable.find(yytext) == symbolTable.end()) {
            symbolTable.insert({yytext, 0});
        }
        else {
            throw "101: identifier declared twice";
        }

        output();

        nextToken = yylex();

        output();
        // Expecting colon
        if (nextToken != TOK_COLON) {
            throw ("5 : ':' expected");
        }
        nextToken = yylex();

        output();
        // Expecting int or real
        if (nextToken != TOK_INTEGER && nextToken != TOK_REAL){
            throw ("10: error in type");
        }
        nextToken = yylex();

        output();
        // Expecting Semicolon
        if (nextToken != TOK_SEMICOLON) {
            throw ("14: ';' expected");
        }

        nextToken = yylex();

        // Loop as long as not at start or end of block
        while (nextToken != TOK_SEMICOLON && nextToken != TOK_BEGIN) {
            cout << endl;
            // Token must be identifier
            if (nextToken != TOK_IDENT){
                throw ("2: identifier expected");
            }

            if (symbolTable.find(yytext) == symbolTable.end()) {
                symbolTable.insert({yytext, 0});
            }
            else {
                throw "101: identifier declared twice";
            }
            output();

            nextToken = yylex();

            output();
            // Expecting colon before the int or real
            if (nextToken != TOK_COLON) {
                throw ("5 : ':' expected");
            }
            nextToken = yylex();

            output();
            // Expecting an int or real
            if (nextToken != TOK_INTEGER && nextToken != TOK_REAL) {
                throw ("10: error in type");
            }
            nextToken = yylex();

            output();
            // Make sure theres a semi
            if (nextToken != TOK_SEMICOLON){
                throw ("14: ';' expected");
            }
            nextToken = yylex();
        }
        cout << endl;
    }
    if (nextToken != TOK_BEGIN)  {
        throw ("18: error in declaration part OR 17: 'BEGIN' expected");
    }
    // Go into compound statement part of the block
    newBlockNode->compoundStatement = compound_statement();
    //compound_statement(); 

    while (nextToken != TOK_END && nextToken != TOK_EOF) {
        if (nextToken != TOK_SEMICOLON) {
            throw ("14: ';' expected");
        }
        newBlockNode->compoundStatement = compound_statement();
        //compound_statement();
    }
    yylex(); // Weird thing that makes my code work and breaks everything if I touch it.

    --level;
    indent();
    cout << "exit <block>" << endl;

    return newBlockNode;  
}

// <compound> -> TOK_BEGIN <statement> { TOK_SEMICOLON <statement> } TOK_END 
compoundNode* compound_statement() {
    indent();
    cout  << "enter <compound_statement>" << endl;
    ++level; 
    output();

    nextToken = yylex();

    compoundNode* newCompoundNode = new compoundNode;

    // If it doesnt qualify as the first of a statement, the same applies for compound
    if (!first_of_statement()) {
        throw ("900: illegal type of statement");
    }
    // Go into statement of compound statemnet
    newCompoundNode->statements.push_back(statement());
    
    // As long as token is semicolon, then we want to go back into statement
    while (nextToken == TOK_SEMICOLON) {
        output();
        nextToken = yylex();

        // Expecting a statement start
        if (!first_of_statement()) {
            throw ("900: illegal type of statement");
        }
        newCompoundNode->statements.push_back(statement());
        // We found the end of the file if the current token is EOF
        if(nextToken == TOK_EOF) {
            indent();
            cout << "-->found END" << endl;
        }
    }
    // Output the find
    if (nextToken != TOK_EOF) {
        output();
    }
    --level;
    indent();
    cout << "exit <compound_statement>" << endl;

    return newCompoundNode; 
}

// <statement> -> <assignment> | <compound> | <if> | <while> | <read> | <write>
statementNode* statement() {
    indent();
    cout << "enter <statement>" << endl;
    ++level;

    statementNode* newStatementNode = nullptr;

    if (nextToken == TOK_IDENT) {
        // Want assignement if the token is an identifier
        newStatementNode = assignment();
    } else if (nextToken == TOK_BEGIN || nextToken == TOK_THEN) {
        // Want compound statement if the token is BEGIN or THEN
        newStatementNode = compound_statement();
    } else if (nextToken == TOK_IF) {
        
        // TODO Finish the class
        newStatementNode = If();

        // If the current token is END, then we want to lex, but not 
        // If the token after is EOF
        if (nextToken == TOK_END){
            nextToken = yylex();
        }
    } else if (nextToken == TOK_WHILE) {
        newStatementNode =While();
    } else if (nextToken == TOK_READ) {
        newStatementNode =read();
    } else if (nextToken = TOK_WRITE) {
        newStatementNode =write();
    } else {
        if (nextToken != TOK_SEMICOLON) {
            throw ("14: ';' expected");
        }
    }

    --level;
    indent();
    cout << "exit <statement>" << endl;

    return newStatementNode;
}
//<assignment> -> TOK_IDENT TOK_ASSIGN <expression>
assignmentNode* assignment() {
    indent();
    cout << "enter <assignment>" << endl;
    ++level;

    bool declared = false;
    
    // We want all assigened identifiers added to the symbol table
    //symbolTable.insert(yytext);

    if(nextToken == TOK_IDENT) {
        if (symbolTable.find(yytext) != symbolTable.end()) {
            declared = true;
        }
        else {
            throw ("104: identifier not declared"); 
        }
        output();
    }

    assignmentNode* newAssignmentNode = new assignmentNode(string(yytext));

    nextToken = yylex();
    output();

    // Expecting an walrus operator
    if (nextToken != TOK_ASSIGN) {
        throw ("51: ':=' expected");
    }

    nextToken = yylex();

    // Expecting to have the right expression start
    if (!first_of_expression()) {
        throw ("901: illegal type of simple expression");
    }
    // Go into expression part of the assignment
    newAssignmentNode->expression = expression();

    --level;
    indent();
    cout << "exit <assignment>" << endl;

    return newAssignmentNode;
}

//<if> -> TOK_IF <expression> TOK_THEN <statement> [ TOK_ELSE <statement> ]
ifNode* If() {
    indent();
    cout << "enter <if statement>" << endl;
    ++level;

    output();
    nextToken = yylex();

    ifNode* newIfNode = new ifNode;
    
    // Expecting an expression start
    if (!first_of_expression()) {
        throw ("144: illegal type of expression");
    }
    // Going into expression part of the If-Statement
    newIfNode->expression = expression();
    // Expecting a then to go with the If
    if (nextToken != TOK_THEN) {
        throw ("52: 'THEN' expected");
    }
    output();
    nextToken = yylex();

    if (!first_of_statement()) {
        throw ("900: illegal type of statement");
    }
    // Going into the next part of the If statement after the "then"
    newIfNode->thenStatement = statement();

    //If the token is else, then we want into another statement
    // We are expecting an else here, so we lex again to see the next token
    if (nextToken != TOK_ELSE && nextToken != TOK_SEMICOLON) {
        nextToken = yylex();

        if (nextToken == TOK_ELSE) {
            output();

            nextToken = yylex();

        
            // Make sure the first part of the else statement is good
            if (!first_of_statement()) {
                throw ("900: illegal type of statement");
            }
            newIfNode->elseStatement = statement();
        }
    } else if (nextToken != TOK_SEMICOLON) {
        output();
        // If its not, then just continue. This is weird but its to control
        // The amount of times we lex. I know, not the best design. 
        nextToken = yylex();

        // Make sure the first part of the else statement is good
        if (!first_of_statement()) {
            throw ("900: illegal type of statement");
        }
        newIfNode->elseStatement = statement();
    }

    --level;
    indent();
    cout << "exit <if statement>" << endl;

    return newIfNode;
}

// <while> ->TOK_WHILE <expression> <statement>
whileNode* While() { 
    indent();
    cout << "enter <while statement>" << endl;
    ++level;
    output();

    whileNode* newWhileNode = new whileNode;

    if (nextToken == TOK_WHILE) {
        nextToken = yylex();

        // Expecting a correct expression start
        if (!first_of_expression()) {
            throw ("144: illegal type of expression");
        }
        // Go into the condition part of the while loop
        newWhileNode->expression = expression();
        // Expecting a correct statement start
        if (!first_of_statement()) {
            throw ("900: illegal type of statement");
        }
        // Go into the statement of the while loop
        newWhileNode->loopBody = statement();
    }
    if (nextToken != TOK_SEMICOLON) {
        nextToken = yylex();
    }
    
    --level;
    indent();
    cout << "exit <while statement>" << endl;

    return newWhileNode;
}

//<read> -> TOK_READ TOK_OPENPAREN TOK_IDENT TOK_CLOSEPAREN
readNode* read(){
    indent();
    cout << "enter <read>" << endl;
    ++level;
    output();
    
    // Make sure the first part of the read statement starts with (
    lex();
    if (nextToken != TOK_OPENPAREN) {
        throw ("9: '(' expected");
    }
    output();
    // Make sure theres an identifier being printed
    lex();
    if(nextToken != TOK_IDENT) {
        throw ("2: identifier expected");
    }
    output();
    
    readNode* newReadNode = new readNode(string(yytext));

    // Make sure the read statement ends with )
    lex();
    if(nextToken != TOK_CLOSEPAREN) {
        throw ("4: ')' expected");
    }
    output();

    lex();
    --level;
    indent();
    cout << "exit <read>" << endl;

    return newReadNode;
}

// <write> -> TOK_WRITE TOK_OPENPAREN ( TOK_IDENT | TOK_STRINGLIT ) TOK_CLOSEPAREN
writeNode* write() {
    indent();
    cout << "enter <write>" << endl;
    ++level;
    output();

    // Make sure the first part of the read statement starts with (
    lex();
    if (nextToken != TOK_OPENPAREN) {
        throw ("9: '(' expected");
    }
    output();

    // Make sure theres an identifier being printed
    lex();
    if (nextToken != TOK_IDENT && nextToken != TOK_STRINGLIT){
        throw ("134: illegal type of operand(s)");
    }
    output();

    writeNode* newWriteNode = new writeNode(string(yytext));
    //
    if(nextToken == TOK_IDENT) {
        newWriteNode->writeType = false;
    } else {
        newWriteNode->writeType = true;
    }

    // Make sure the read statement ends with )
    lex();
    if (nextToken != TOK_CLOSEPAREN) {
        throw ("4: ')' expected");
    }
    output();

    lex();
    --level;
    indent();
    cout << "exit <write>" << endl;

    return newWriteNode;
}

ExprNode* expression() {
    indent();
    cout << "enter <expression>" << endl;
    ++level;
    
    ExprNode* newExprNode = new ExprNode;

    // Make sure the expression starts correctly
    if(!first_of_simple_expression()) {
        throw("144: illegal type of expression");
    }
    // Go into the simple expression portion of the expression
    newExprNode->firstSE = simple_expression();

    // If token is part of expression grammar, then output 
    if (nextToken == TOK_EQUALTO || nextToken == TOK_LESSTHAN || 
        nextToken == TOK_GREATERTHAN || nextToken == TOK_NOTEQUALTO) {

        newExprNode->restSEOps.push_back(nextToken);
        
        output();
        lex();
        // Make sure simple expression is starting correcly
        if(!first_of_simple_expression()) {
            throw("901: illegal type of simple expression");
        }
        newExprNode->restSE.push_back(simple_expression());
    }

    --level;
    indent();
    cout << "exit <expression>" << endl;

    return newExprNode;
}

// <simple expression> -> <term> { ( TOK_PLUS | TOK_MINUS | TOK_OR ) <term> }
simpleExpressionNode* simple_expression() {
    indent();
    cout << "enter <simple expression>" << endl;
    ++level;

    simpleExpressionNode* newSimpleExpressionNode = new simpleExpressionNode;

    // Make sure the first part of the term is correct
    if (!first_of_term()) {
        throw "902: illegal type of term";
    }
    // Go into term part of the simple expression
    newSimpleExpressionNode->firstTerm = term();
    // While a part of the simple expression grammar, keep going into term
    while (nextToken != TOK_SEMICOLON && nextToken != TOK_END && nextToken != TOK_EOF &&
           nextToken != TOK_EQUALTO && nextToken != TOK_GREATERTHAN && nextToken != TOK_LESSTHAN &&
           nextToken != TOK_NOTEQUALTO && nextToken != TOK_THEN && nextToken != TOK_ELSE &&
           nextToken != TOK_CLOSEPAREN && nextToken != TOK_BEGIN && nextToken != TOK_IDENT) {

        if (nextToken == TOK_PLUS) {
            output();
            newSimpleExpressionNode->restTermOps.push_back(nextToken);
            nextToken = yylex();

            if (!first_of_term()) {

                throw "902: illegal type of term";
            }
            newSimpleExpressionNode->restTerms.push_back(term());
        } else if (nextToken == TOK_MINUS) {
            output();
            newSimpleExpressionNode->restTermOps.push_back(nextToken);
            nextToken = yylex();

            if (!first_of_term()) {

                throw "902: illegal type of term";
            }
            newSimpleExpressionNode->restTerms.push_back(term());
        } else if (nextToken = TOK_OR) {
            output();
            newSimpleExpressionNode->restTermOps.push_back(nextToken);
            nextToken = yylex();

            if (!first_of_term()) {
                throw "902: illegal type of term";
            }
            newSimpleExpressionNode->restTerms.push_back(term());
        } else {
            break;
        }
    }
    Label1:
    --level;
    indent();
    cout << "exit <simple expression>" << endl;

    return newSimpleExpressionNode;
}


TermNode* term() {
    indent();
    cout << "enter <term>" << endl;

    ++level;
    
    TermNode* newTermNode = new TermNode;

    // Make sure the start of factor is correct
    if (!first_of_factor()) {
        throw "903 : illegal type of factor";
    }
    // Go into factor part of the term
    if (string(yytext) == "-") {
        newTermNode->minusFactor = true;
    }
    if (string(yytext) == "NOT"){
        newTermNode->notFactor = true;
    }
    newTermNode->firstFactor = factor();
    // While a part of the term grammar, check if token is multiply, divide, or and, 
    // and keep going into factor
    while (nextToken != TOK_NOT && nextToken != TOK_MINUS && nextToken != TOK_SEMICOLON &&
           nextToken != TOK_EOF && nextToken != TOK_GREATERTHAN && nextToken != TOK_LESSTHAN &&
           nextToken != TOK_EQUALTO && nextToken != TOK_NOTEQUALTO && nextToken != TOK_THEN &&
           nextToken != TOK_ELSE && nextToken != TOK_CLOSEPAREN && nextToken != TOK_BEGIN &&
           nextToken != TOK_PLUS && nextToken != TOK_END && nextToken != TOK_IDENT) {
        if (nextToken == TOK_MULTIPLY || nextToken == TOK_DIVIDE || nextToken == TOK_AND){
            output();

            newTermNode->restFactorOps.push_back(nextToken);
            nextToken = yylex();
            // Make sure factor part is starting correctly
            if (!first_of_factor()) {
                throw ("903: illegal type of factor");
            }
            newTermNode->restFactors.push_back(factor());
        }
    }
    --level;
    indent();
    cout << "exit <term>" << endl;

    return newTermNode;
}


FactorNode* factor() {
    indent();
    cout << "enter <factor>" << endl;
    ++level;

    FactorNode* newFactorNode = nullptr;
    
    // If int literal or float literal, then output and lex
    if (nextToken == TOK_INTLIT) {
        newFactorNode = new IntLitNode(atoi(yytext));
        output();
        nextToken = yylex();

    } else if (nextToken == TOK_FLOATLIT) {
        newFactorNode = new FloatLitNode(atof(yytext));
        output();
        nextToken = yylex();

    } else if (nextToken == TOK_IDENT) {
        
        if (symbolTable.find(yytext) == symbolTable.end()) {
            throw ("104: identifier not declared");
        }

        newFactorNode = new IdNode(string(yytext));
        output();
        nextToken = yylex();

    } else if (nextToken == TOK_OPENPAREN) {
        output();
        nextToken = yylex();


        if (!first_of_expression()) {
            throw ("144: illegal type of expression");
        }
        newFactorNode = new NestedExprNode(expression());
        output();
        if (nextToken != TOK_CLOSEPAREN) {
            throw ("4: ')' expected");
        }
        nextToken = yylex();

    } else if (nextToken == TOK_NOT){
        output();
        nextToken = yylex();

        if (!first_of_factor()){
            throw ("903: illegal type of factor");
        }
        // Go into factor after not
        newFactorNode = factor();

    } else if (nextToken == TOK_MINUS) {
        output();
        nextToken = yylex();


        if (!first_of_factor()) {
            throw "903: illegal type of factor";
        }

        if (nextToken == TOK_IDENT) {
            if (symbolTable.find(yytext) == symbolTable.end()) {
                throw ("104: identifier not declared");
            }
            else {
                throw "101: identifier declared twice";
            }
        }
        // Go into factor after minus
        newFactorNode = factor();
    } else {
        // If no match, then its not a correct factor type
        throw ("903: illegal type of factor");
    }

    --level;
    indent();
    cout << "exit <factor>" << endl;

    return newFactorNode;
}

//********************************************************************************
bool first_of_program() {
    return nextToken == TOK_PROGRAM;
}
bool first_of_block() {
    return nextToken == TOK_VAR || nextToken == TOK_BEGIN;
}
bool first_of_statement() {
    return nextToken == TOK_IDENT || nextToken == TOK_BEGIN || nextToken == TOK_IF ||
           nextToken == TOK_WHILE || nextToken == TOK_READ || nextToken == TOK_WRITE;
}
bool first_of_assignment() {
    return nextToken == TOK_IDENT;
}
bool first_of_compound() {
    return nextToken == TOK_BEGIN;
}
bool first_of_if() {
    return nextToken == TOK_IF;
}
bool first_of_while() {
    return nextToken == TOK_WHILE;
}
bool first_of_read() {
    return nextToken == TOK_READ;
}
bool first_of_write() {
    return nextToken == TOK_WRITE;
}
bool first_of_expression() {
    return nextToken == TOK_INTLIT || nextToken == TOK_FLOATLIT || nextToken == TOK_IDENT ||
           nextToken == TOK_OPENPAREN || nextToken == TOK_NOT || nextToken == TOK_MINUS;
}
bool first_of_simple_expression() {
    return nextToken == TOK_INTLIT || nextToken == TOK_FLOATLIT || nextToken == TOK_IDENT ||
           nextToken == TOK_OPENPAREN || nextToken == TOK_NOT || nextToken == TOK_MINUS;
}
bool first_of_term() {
    return nextToken == TOK_INTLIT || nextToken == TOK_FLOATLIT || nextToken == TOK_IDENT ||
           nextToken == TOK_OPENPAREN || nextToken == TOK_NOT || nextToken == TOK_MINUS;
}
bool first_of_factor() {
    return nextToken == TOK_INTLIT || nextToken == TOK_FLOATLIT || nextToken == TOK_IDENT ||
           nextToken == TOK_OPENPAREN || nextToken == TOK_NOT || nextToken == TOK_MINUS;
}
#endif
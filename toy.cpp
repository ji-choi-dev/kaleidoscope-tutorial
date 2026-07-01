#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <memory>
#include <vector>

enum Token {
    tok_eof = -1,

    tok_def = -2,
    tok_extern = -3,
    tok_identifier = -4,
    tok_number = -5,
}; //code that represents what kind of token is recognized

static std::string IdentifierStr;
static double NumVal;

static int gettok() {
    static int LastChar = ' '; //set empty space to enter the loop - this won't get reassigned even when invoked again

    while (isspace(LastChar)) //skips all whitespace
        LastChar = getchar(); 

    if (isalpha(LastChar)) { //check whether the token starts with alphabet
        IdentifierStr = LastChar;
        while (isalnum((LastChar = getchar())))
            IdentifierStr += LastChar; //collect all alphabets

        if (IdentifierStr == "def") // check if it is a recognized token
            return tok_def;;
        if (IdentifierStr == "extern")
            return tok_extern;
        return tok_identifier;
    }
    if (isdigit(LastChar) || LastChar == '.') { // handle numbers
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar) || LastChar == '.'); //2.22.2 will still be legal and ignored
        //but this is to make the lexer barebones

        NumVal = strtod(NumStr.c_str(), 0);
        return tok_number;
    }
    if (LastChar == '#') { //handle comments
        do {
            LastChar = getchar();
        } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
        if (LastChar != EOF)
            return gettok();
    }
    
    if (LastChar == EOF) // handle EOF 
        return tok_eof;

    int ThisChar = LastChar; // save the current char
    LastChar = getchar(); // read next character

    return ThisChar; //return the saved current character
}

class ExprAST {
public:
    virtual ~ExprAST() = default; 
};

class NumberExprAST : public ExprAST {
    double Val;
public:
    NumberExprAST(double Val) : Val(Val) {}
};

class VariableExprAST : public ExprAST {
    std::string Name;

public:
    VariableExprAST(const std::string &Name) : Name(Name) {}
};

class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
    BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
        std::unique_ptr<ExprAST> RHS)
    : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

public:
    CallExprAST(const std::string &Callee,
            std::vector<std::unique_ptr<ExprAST>> Args)
        :Callee(Callee), Args(std::move(Args)){}
};

class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;

public:
    PrototypeAST(const std::string &Name, std::vector<std::string> Args)
        : Name(Name), Args(std::move(Args)) {}

    const std::string &getName() const { return Name; }
};

class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::unique_ptr<ExprAST> Body)
            : Proto(std::move(Proto)), Body(std::move(Body)) {}
};
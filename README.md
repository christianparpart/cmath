# cmath

This is a pet project, implementing a programming language for mathematics and
related sciences.

### Core Language Features:

* comprehensive syntax
* static type system
* custom functions and variables

### Core Functions

### Frontends

It should be able to render to different frontends, such as: OS-native widgets,
Web, LaTeX, and plain text.

### SYNTAX:

```
SYMBOL            ::= [a-zA-Z]+
NUMBER            ::= [1-9][0-9]*
```

```
Program           ::= (ConstDef | MappingDef | StmtExpr)*

ConstDef          ::= SYMBOL ':=' StmtExpr
MappingDef        ::= SYMBOL ':' MappingParamList '->' Expr
MappingParamList  ::= SYMBOL | '(' SYMBOL (',' SYMBOL)* ')'

Expr              ::= StmtExpr

StmtExpr          ::= IfStmt | CaseStmt | CompoundStmt | ArithExpr
IfStmt            ::= 'if' RelExpr 'then' Expr 'else' Expr
CaseStmt          ::= 'case' Expr ('when' Expr 'then' Expr)* ['else' Expr]
CaseStmt          ::= CaseWhenClause CaseWhenClause* CaseElseClause
CaseWhenClause    ::= 'when' Expr 'then' Expr
CaseElseClause    ::= 'else' Expr
CompoundStmt      ::= '{' (Expr (LF | ';'))* Expr '}'

SolveExpr         ::= `solve' equExpr 'for' variableExpr
SumExpr           ::= 'sum' ['over'] Expr 'for' SYMBOL '=' Expr 'to' Expr
ProductExpr       ::= 'product' ['over'] Expr 'for' SYMBOL '=' Expr 'to' Expr

ArithExpr         ::= RelExpr
RelExpr           ::= AddExpr (REL_OP AddExpr)*
REL_OP            ::= '=' | '!=' | '<' | '>' | '<=' | '>='
AddExpr           ::= MulExpr ( SumExpr
                              | (('+' | '-') MulExpr)+ )?
MulExpr           ::= PowExpr ( ProductExpr
                              | (('*' | '/') PowExpr)+ )?
PowExpr           ::= FacExpr ('^' FacExpr)*
FacExpr           ::= PrimaryExpr ['!']
PrimaryExpr       ::= NUMBER
                    | '(' Expr ')'
                    | Variable
                    | Function '(' Params? ')'
Params            ::= Expr (',' Expr)*
```

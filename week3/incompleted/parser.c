/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken;
Token *lookAhead;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    printToken(lookAhead);
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
  assert("Parsing a Program ....");
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
  assert("Program parsed!");
}

void compileBlock(void) {
  assert("Parsing a Block ....");
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    compileConstDecl();
    compileConstDecls();
    compileBlock2();
  } 
  else compileBlock2();
  assert("Block parsed!");
}

void compileBlock2(void) {
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) {
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    compileVarDecl();
    compileVarDecls();
    compileBlock4();
  } 
  else compileBlock4();
}

void compileBlock4(void) {
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileConstDecls(void) {
  // CuongDD: 19/9/2014
  assert("Parsing ConstDecls...");
  while(lookAhead->tokenType == TK_IDENT) {
  	compileConstDecl();
  }
  assert("ConstDecls parsed!");
}

void compileConstDecl(void) {
  // CuongDD: 12/09/2014
  assert("Parsing ConstDecl...");
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
  assert("ConstDecl parsed!");
}

void compileTypeDecls(void) {
  assert("Parsing TypeDecls");
  // CuongDD: 19/9/2014
  while(lookAhead->tokenType == TK_IDENT) {
     compileTypeDecl();
  }
  assert("TypeDecls parsed!");
}

void compileTypeDecl(void) {
  // CuongDD: 19/9/2014
  assert("Parsing TypeDecl...");
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
  assert("TypeDecl parsed!");
}

void compileVarDecls(void) {
  // CuongDD: 19/9/2014
  assert("Parsing VarDecls...");
  while(lookAhead->tokenType == TK_IDENT) {
  	compileVarDecl();
  }
  assert("VarDecls parsed!");
}

void compileVarDecl(void) {
  // CuongDD: 19/9/2014
  assert("Parsing VarDecl...");
  eat(TK_IDENT);
  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
  assert("VarDecl parsed!");
}

void compileSubDecls(void) {
  assert("Parsing subtoutines ....");
  // CuongDD: 19/9/2014
  while((lookAhead->tokenType == KW_FUNCTION) || (lookAhead->tokenType == KW_PROCEDURE)) {
	if (lookAhead->tokenType == KW_FUNCTION) {
		compileFuncDecl();
	}
	if (lookAhead->tokenType == KW_PROCEDURE) {
		compileProcDecl();	
	}
  } 
  assert("Subtoutines parsed ....");
}

void compileFuncDecl(void) {
  assert("Parsing FuncDecl ...");
  // CuongDD: 19/9/2014
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  compileParams();
  eat(SB_COLON);
  compileBasicType();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("FuncDecl parsed!");
}

void compileProcDecl(void) {
  assert("Parsing ProcDecl...");
  // CuongDD: 19/9/2014
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_COLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("ProcDecl parsed!");
}

void compileUnsignedConstant(void) {
  assert("Parsing UnsignedConstant");
  // CuongDD: 19/9/2014
  if(lookAhead->tokenType == TK_NUMBER) {
	   eat(TK_NUMBER);
  } else {
	   eat(TK_IDENT);
  }
  assert("UsignedConstant parsed...");
}

void compileConstant(void) {
  // CuongDD: 12/09/2014
  assert("Parsing Constant...");
  if(lookAhead->tokenType == SB_PLUS) {
  	eat(SB_PLUS);
  	compileConstant2();
  } else if (lookAhead->tokenType == SB_MINUS) {
  	eat(SB_MINUS);
  	compileConstant2();   
  } else if (lookAhead->tokenType == TK_IDENT || lookAhead->tokenType == TK_NUMBER) {
  	compileConstant2();
  } else {
  	eat(TK_IDENT);
  }
  assert("Constant parsed!");
}

void compileConstant2(void) {
  // CuongDD: 12/09/2014
  if (lookAhead->tokenType == TK_IDENT) {
  	eat(TK_IDENT);
  } else if (lookAhead->tokenType == TK_NUMBER) {
  	eat(TK_NUMBER);
  }
}

void compileType(void) {
  assert("Parsing Type...");
  // CuongDD: 19/9/2014
  if (lookAhead->tokenType == KW_INTEGER) {
  	eat(KW_INTEGER);
  } else if (lookAhead->tokenType == KW_CHAR){
    eat(KW_CHAR);
  } else if (lookAhead->tokenType == KW_ARRAY) {
	eat(KW_ARRAY);
    eat(SB_LSEL);
	eat(TK_NUMBER);
	eat(SB_RSEL);
	eat(KW_OF);
	compileType();
  }
  assert("Type parsed!");
}

void compileBasicType(void) {
  assert("Parsing BasicType");
  // CuongDD: 19/9/2014
  if (lookAhead->tokenType == KW_INTEGER) {
	eat(KW_INTEGER);
  } else {
	
  }
  assert("BasicType parsed!");
}

void compileParams(void) {
  assert("Parsing Params...");
  // CuongDD: 19/9/2014
  while(lookAhead->tokenType == SB_LPAR) {
  	eat(SB_LPAR);
  	compileParam();
  	compileParams2();
  	eat(SB_RPAR);
  }
  assert("Params parsed!");
}

void compileParams2(void) {
  // CuongDD: 19/9/2014
  assert("Parsing Params2...");
  while(lookAhead->tokenType == SB_SEMICOLON) {
	eat(SB_SEMICOLON);
	compileParam();
  }
  assert("Params2 parsed!");
}

void compileParam(void) {
  // CuongDD: 19/9/2014
  assert("Parsing Param...");
  if(lookAhead->tokenType == TK_IDENT) {
  	eat(TK_IDENT);
  	eat(SB_COLON);
  	compileBasicType();
  } else {
	eat(KW_VAR);
	eat(TK_IDENT);
	eat(SB_COLON);
	compileBasicType();
  }
  assert("Param parsed!");
}

void compileStatements(void) {
  // CuongDD: 19/9/2014
  assert("Parsing Statements...");
  compileStatement();
  compileStatements2();
  assert("Statements parsed!");
}

void compileStatements2(void) {
  // CuongDD: 19/9/2014
  assert("Parsing Statment2...");
  while(lookAhead->tokenType == SB_SEMICOLON) {
  	compileStatement();
  }
  assert("Statment2 parsed!");
}

void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileAssignSt(void) {
  assert("Parsing AssignSt...");
  // CuongDD: 19/9/2014
  
  assert("AssignSt parsed!");
}

void compileCallSt(void) {
  assert("Parsing CallSt...");
  // CuongDD: 19/9/2014
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
  assert("CallSt parsed!");
}

void compileGroupSt(void) {
  assert("Parsing GroupSt...");
  // CuongDD: 19/9/2014
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
  assert("GroupSt parsed!");
}

void compileIfSt(void) {
  assert("Parsing an if statement ....");
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
  assert("If statement parsed ....");
}

void compileElseSt(void) {
  while (lookAhead->tokenType == KW_ELSE) {
	eat(KW_ELSE);
    compileStatement();
  }
}

void compileWhileSt(void) {
  assert("Parsing WhileSt...");
  // CuongDD: 19/9/2014
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  assert("While WhileSt!");
}

void compileForSt(void) {
  assert("Parsing ForSt...");
  // CuongDD: 19/9/2014
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
  assert("ForSt parsed!");
}

void compileArguments(void) {
  // CuongDD: 19/9/2014
  assert("Parsing Arguments...");
  while (lookAhead->tokenType == SB_LPAR) {
  	eat(SB_LPAR);
  	compileExpression();
  	compileArguments2();
  	eat(SB_RPAR);
  }
  assert("Arguments parsed!");
}

void compileArguments2(void) {
  // CuongDD: 19/9/2014
  assert("Parsing Arguments2...");
  while(lookAhead->tokenType == SB_COMMA) {
  	eat(SB_COMMA);
    compileExpression();
  }
  assert("Arguments2 parsed!");
}

void compileCondition(void) {
  // CuongDD: 19/9/2014
  assert("Parsing Condition...");
  compileExpression();
  compileCondition2();
  assert("Condition parsed!");
}

void compileCondition2(void) {
  // CuongDD: 19/9/2014
  assert("Parsing Condition2...");
  switch(lookAhead->tokenType) {
	case SB_EQ:
		eat(SB_EQ);
		compileExpression();
		break;
	case SB_NEQ:
		eat(SB_NEQ);
		compileExpression();
		break;
	case SB_LE:
		eat(SB_LE);
		compileExpression();
		break;
	case SB_GE:
		eat(SB_GE);
		compileExpression();
		break;
	case SB_GT:
		eat(SB_GT);
		compileExpression();
		break;
    default:
		break;
  }
  assert("Condition2 parsed!");
}

void compileExpression(void) {
  assert("Parsing Expression...");
  // CuongDD: 19/9/2014
  switch(lookAhead->tokenType) {
	case SB_PLUS:
		eat(SB_PLUS);
		compileExpression2();
		break;
	case SB_MINUS:
		eat(SB_MINUS);
		compileExpression2();
		break;
	default:
		compileExpression2();
		break;
  }
  assert("Expression parsed!");
}

void compileExpression2(void) {
  // CuongDD: 19/9/2014
  assert("Parsing Expression2...");
  compileTerm();
  compileExpression3();
  assert("Expression2 parsed!");
}


void compileExpression3(void) {
  // CuongDD: 19/9/2014
  assert("Parsing Expression3...");
  compileTerm();
  compileExpression3();
  assert("Expression3 parsed!");

}

void compileTerm(void) {
  // TODO
}

void compileTerm2(void) {
  // TODO
}

void compileFactor(void) {
  // TODO
}

void compileIndexes(void) {
  // TODO
}

int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  compileProgram();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}

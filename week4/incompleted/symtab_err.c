/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object* obj);
void freeScope(Scope* scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab* symtab;
Type* intType;
Type* charType;

/******************* Type utilities ******************************/

Type* makeIntType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

Type* makeCharType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

Type* makeArrayType(int arraySize, Type* elementType) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}

Type* duplicateType(Type* type) {
  // CuongDD: 9/10/2014
  Type* newType = (Type*)malloc(sizeof(type));
  newType->typeClass = type->typeClass;
  newType->arraySize = type->arraySize;
  newType->elementType = type->elementType;
  return newType;
}

int compareType(Type* type1, Type* type2) {
  // CuongDD: 9/10/2014
  // Compare typeClass first
  if (type1->typeClass == type2->typeClass)
  {
  	// Comparing array type
  	if (type1->typeClass == TP_ARRAY)
  	{
  		if (type1->arraySize == type2->arraySize)
  		{
  			return compareType(type1->elementType, type2->elementType);
  		}
  	}
  }
  return 0;
}

void freeType(Type* type) {
  // if (type->typeClass == TP_ARRAY)
  // {
  // 	freeType(type->elementType);
  // }
  // freeType(type);
  free(type);
  type = NULL;
}

/******************* Constant utility ******************************/

ConstantValue* makeIntConstant(int i) {
  // CuongDD: 9/10/2014
  ConstantValue *cv = (ConstantValue*)malloc(sizeof(ConstantValue));
  cv->type = TP_INT;
  cv->intValue = i;

  return cv;	
}

ConstantValue* makeCharConstant(char ch) {
  // CuongDD: 9/10/2014
  ConstantValue* cv = (ConstantValue*)malloc(sizeof(ConstantValue));
  cv->type = TP_CHAR;
  cv->charValue = ch;

  return cv;
}

ConstantValue* duplicateConstantValue(ConstantValue* v) {
  // CuongDD: 9/10/2014
  ConstantValue* cv = (ConstantValue*)malloc(sizeof(v));
  cv->type = v->type;
  if (v->type == TP_INT)
  	cv->intValue = v->intValue;
  else if (v->type == TP_CHAR)
  	cv->charValue = v->charValue;

  return cv;	
}

/******************* Object utilities ******************************/

Scope* createScope(Object* owner, Scope* outer) {
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

Object* createProgramObject(char *programName) {
  Object* program = (Object*) malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes*) malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = createScope(program,NULL);
  symtab->program = program;

  return program;
}

Object* createConstantObject(char *name) {
  // CuongDD: 9/10/2014
  Object* constant = (Object*)malloc(sizeof(Object));
  strcpy(constant->name, name);
  constant->kind = OBJ_CONSTANT;
  constant->constAttrs = (ConstantAttributes*)malloc(sizeof(ConstantAttributes));
  constant->constAttrs->value = NULL;

  return constant;
}

Object* createTypeObject(char *name) {
  // CuongDD: 9/10/2014
  Object* typeObj = (Object*)malloc(sizeof(Object));
  strcpy(typeObj->name, name);
  typeObj->kind = OBJ_TYPE;
  typeObj->typeAttrs = (TypeAttributes*)malloc(sizeof(TypeAttributes));
  typeObj->typeAttrs->actualType = NULL;	

  return typeObj;
}

Object* createVariableObject(char *name) {
  // CuongDD: 9/10/2014
  Object* varObj = (Object*)malloc(sizeof(Object));
  strcpy(varObj->name, name);
  varObj->kind = OBJ_VARIABLE;
  varObj->varAttrs = (VariableAttributes*)malloc(sizeof(VariableAttributes));
  varObj->varAttrs->type = NULL;
  varObj->varAttrs->scope = createScope(varObj, symtab->currentScope);

  return varObj;
}

Object* createFunctionObject(char *name) {
  // CuongDD: 9/10/2014
  Object* funcObj = (Object*)malloc(sizeof(Object));
  strcpy(funcObj->name, name);
  funcObj->kind = OBJ_FUNCTION;
  funcObj->funcAttrs = (FunctionAttributes*)malloc(sizeof(FunctionAttributes));
  funcObj->funcAttrs->paramList = (ParameterAttributes*)malloc(sizeof(ParameterAttributes));
  funcObj->funcAttrs->paramList->next = NULL;
  funcObj->funcAttrs->returnType = NULL;
  funcObj->funcAttrs->scope = createScope(funcObj, symtab->currentScope);

  return funcObj;
}

Object* createProcedureObject(char *name) {
  // CuongDD: 9/10/2014
  Object* procedure = (Object*)malloc(sizeof(Object));
  strcpy(procedure->name, name);
  procedure->kind = OBJ_PROCEDURE;
  procedure->procAttrs = (ProcedureAttributes*)malloc(sizeof(ProcedureAttributes));
  procedure->procAttrs->paramList = (ProcedureAttributes*)malloc(sizeof(ProcedureAttributes));
  procedure->procAttrs->paramList->next = NULL;
  procedure->procAttrs->scope = createScope(procedure, symtab->currentScope);

  return procedure;	
}

Object* createParameterObject(char *name, enum ParamKind kind, Object* owner) {
  // CuongDD: 9/10/2014
  Object* parameter = (Object*)malloc(sizeof(Object));
  strcpy(parameter->name, name);
  parameter->kind = OBJ_PARAMETER;
  parameter->paramAttrs = (ParameterAttributes*)malloc(sizeof(ParameterAttributes));
  parameter->paramAttrs->kind = kind;
  parameter->paramAttrs->type = NULL;
  parameter->paramAttrs->function = owner;

  return parameter;
}

// void freeObject(Object* obj) {
//   // CuongDD: 9/10/2014
//   if (obj != NULL) {
//   	switch(obj->kind) {
//   		case OBJ_PARAMETER:
//   			if (obj->paramAttrs->type != NULL) {
//   				freeType(obj->paramAttrs->type);
//   			}
//   			if (obj->paramAttrs->function != NULL) {
//   				free(obj->paramAttrs->function);
//   			}
//   			free(obj->paramAttrs);
//   		break;
//   		case OBJ_PROCEDURE:
//   			if (obj->procAttrs->paramList != NULL)
//   				freeObjectList(obj->procAttrs->paramList);
//   			if (obj->procAttrs->scope != NULL)
//   				freeScope(obj->procAttrs->scope);
//   			free(obj->procAttrs);
//   		break;
//   		case OBJ_FUNCTION:
// 			if (obj->funcAttrs->paramList != NULL)
// 				freeObjectList(obj->funcAttrs->paramList);
// 			if (obj->funcAttrs->returnType != NULL)
// 				freeType(obj->funcAttrs->returnType);
// 			if (obj->funcAttrs->scope != NULL)
// 				freeScope(obj->funcAttrs->scope);  			
// 			free(obj->funcAttrs);
//   		break;
//   		case OBJ_VARIABLE:
//   			if (obj->varAttrs->type != NULL)
//   				freeType(obj->varAttrs->type);
//   			if (obj->varAttrs->scope != NULL)
//   				freeScope(obj->varAttrs->scope);
//   			free(obj->varAttrs);
//   		break;
//   		case OBJ_TYPE:
//   			if (obj->typeAttrs->actualType != NULL)
//   				freeType(obj->typeAttrs->actualType);
//   			free(obj->typeAttrs);
//   		break;
//   		case OBJ_CONSTANT:
//   			if (obj->constAttrs->value != NULL)
//   				free(obj->constAttrs->value);
//   			free(obj->constAttrs);
//   		break;
//   		case OBJ_PROGRAM:
//   			if (obj->progAttrs->scope != NULL)
//   				freeScope(obj->progAttrs->scope);
//   			free(obj->progAttrs);
//   		break;
//   		default:
//   		break; 
//   	}
//   }	
//   free(obj);
// }
void freeObject(Object* obj) {
    if (obj != NULL) {
        if (obj->constAttrs != NULL) {
            switch (obj->kind) {
            case OBJ_CONSTANT:
                if (obj->constAttrs->value != NULL) {
                    free(obj->constAttrs->value);
                    obj->constAttrs->value = NULL;
                }
                break;
            case OBJ_VARIABLE:
                if (obj->varAttrs->type != NULL) {
                    free(obj->varAttrs->type);
                    obj->varAttrs->type = NULL;
                }
                break;
            case OBJ_TYPE:
                if (obj->typeAttrs->actualType != NULL) {
                    free(obj->typeAttrs->actualType);
                    obj->typeAttrs->actualType = NULL;
                }
                break;
            case OBJ_PROGRAM:
                if (obj->progAttrs->scope != NULL) {
                    freeScope(obj->progAttrs->scope);
                    obj->progAttrs->scope = NULL;
                }
                break;
            case OBJ_FUNCTION:
                freeScope(obj->funcAttrs->scope); // Free scope also free the param list
                break;
            case OBJ_PROCEDURE:
                freeScope(obj->procAttrs->scope); // Free scope also free the param list
                break;
            case OBJ_PARAMETER:
                if (obj->paramAttrs->type != NULL) {
                    free(obj->paramAttrs->type);
                    obj->paramAttrs->type = NULL;
                }
                break;
            default:
                break;
            }

            free(obj->constAttrs);
            obj->constAttrs = NULL;
        }
        free(obj);
        obj = NULL;
    }
}

void freeScope(Scope* scope) {
  // CuongDD: 9/10/2014
  if (scope->objList != NULL) 
  	freeObjectList(scope->objList);
  if (scope->owner != NULL)
  	free(scope->owner);
  if (scope->outer != NULL)
  	freeScope(scope->outer);
  free(scope);
}

void freeObjectList(ObjectNode *objList) {
  // CuongDD: 9/10/2014
  if (objList != NULL) {
  	freeObject(objList->object);
  	freeObjectList(objList->next);
  	objList = NULL;
  }	
}

void freeReferenceList(ObjectNode *objList) {
  // CuongDD: 9/10/2014
  if (objList != NULL) {
  	freeObject(objList->object);
  	freeReferenceList(objList->next);
  	objList = NULL;
  }	
}

void addObject(ObjectNode **objList, Object* obj) {
  ObjectNode* node = (ObjectNode*) malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL) 
    *objList = node;
  else {
    ObjectNode *n = *objList;
    while (n->next != NULL) 
      n = n->next;
    n->next = node;
  }
}

Object* findObject(ObjectNode *objList, char *name) {
  // CuongDD: 9/10/2014
  ObjectNode* curr = objList;
  while(curr != NULL) {
  	if (strcmp(curr->object->name,name))
  		return curr->object;
  	curr = curr->next;
  }
  return NULL;
}

/******************* others ******************************/

void initSymTab(void) {
  Object* obj;
  Object* param;

  symtab = (SymTab*) malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;
  
  obj = createFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEI");
  param = createParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEC");
  param = createParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  addObject(&(obj->procAttrs->paramList),param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITELN");
  addObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

void exitBlock(void) {
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object* obj) {
  if (obj->kind == OBJ_PARAMETER) {
    Object* owner = symtab->currentScope->owner;
    switch (owner->kind) {
    case OBJ_FUNCTION:
      addObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      addObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }
 
  addObject(&(symtab->currentScope->objList), obj);
}



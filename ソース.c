#define _USE_MATH_DEFINES
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_DATA 64

#define IDV_X			-  1
#define IDV_A			-  2
#define IDV_B			-  3
#define IDV_C			-  4
#define IDV_D			-  5

#define IDV_PI			- 21


#define IDF_ERR			-100

#define IDF_ADD			-111
#define IDF_SUB			-112
#define IDF_MUL			-113
#define IDF_DIV			-114
#define IDF_POW			-115

#define IDF_COS			-211
#define IDF_SIN			-212
#define IDF_TAN			-213
#define IDF_FLOOR		-214
#define IDF_ROUND		-215
#define IDF_CEIL		-216
#define IDF_EXP			-218
#define IDF_LN			-219

#define IDF_MOD - 220



/*
	数式の文法

	expr = mult ("+" mult | "-" mult)*
	mult = powr (powr | "/"powr)*
	powr = prim | prim "^" powr
	prim = (num | var | cst | "(" expr ")" | func args) ("^" prim)*
	args = mult | "(" expr ("," expr)* ")"
	func = "cos" | "sin" | "tan" | "floor" | "mod" | ...
	
*/



typedef enum {
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_POW,
	ND_NUM,
	ND_VAR,
	ND_FNC,
} NodeKind;

typedef struct tagNode Node;

struct tagNode {
	NodeKind kind;
	Node* lhs;
	Node* rhs;
	int val;
};

typedef enum {
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
} TokenKind;

typedef struct tagToken Token;

struct tagToken {
	TokenKind kind;
	Token* next;
	int val;
	char* str;
};



Node* expr(void);
Node* mult(void);
Node* powr(void);
Node* prim(void);
Node* func(int id);

void error(char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}


// 現在着目しているトークン
Token* token;

// スタック
typedef struct tagStack {
	double arr[MAX_DATA];
	int size;
} Stack;
int Push(Stack* stk, double input)
{
	if (stk->size < MAX_DATA) {
		//printf("push %f at %d\n", input, stk->size - 1);
		stk->size++;
		stk->arr[stk->size - 1] = input;
		return 0;
	}
	else {
		error("stack is full\n");
		return -1;
	}
}
double Pop(Stack* stk)
{
	if (0 < stk->size) {
		//printf("pop %f at %d\n", stk->arr[stk->size - 1], stk->size - 1);
		return stk->arr[--(stk->size)];
	}
	else {
		error("stack is empty\n");
		return -1;
	}
}

int func_arr[MAX_DATA];
int func_count;




char* GetVarName(int id)
{
	if (id == IDV_A)return  "a";
	if (id == IDV_B)return  "b";
	if (id == IDV_C)return  "c";
	if (id == IDV_D)return  "d";
	if (id == IDV_X)return  "x";
	if (id == IDV_PI)return "PI";
	return "???";
}

char* GetFuncName(int id)
{
	if (id == IDF_COS)return "cos";
	if (id == IDF_SIN)return "sin";
	if (id == IDF_TAN)return "tan";
	if (id == IDF_FLOOR)return "floor";
	if (id == IDF_ROUND)return "round";
	if (id == IDF_CEIL)return "ceil";
	if (id == IDF_EXP)return "exp";
	if (id == IDF_LN)return "ln";
	if (id == IDF_MOD)return "mod";
	return "???";
}







bool check(char* op)
{
	if (token->kind != TK_RESERVED || strncmp(token->str, op, strlen(op)) != 0)
		return false;
	return true;
}

bool consume(char* op)
{
	if (token->kind != TK_RESERVED || strncmp(token->str, op, strlen(op)) != 0)
		return false;
	token = token->next;
	return true;
}

bool consume_var(int* id)
{
	if (token->kind != TK_RESERVED) return false;
		 if (strncmp(token->str,  "a", 1) == 0) *id = IDV_A;
	else if (strncmp(token->str,  "b", 1) == 0) *id = IDV_B;
	else if (strncmp(token->str,  "c", 1) == 0) *id = IDV_C;
	else if (strncmp(token->str,  "d", 1) == 0) *id = IDV_D;
	else if (strncmp(token->str,  "x", 1) == 0) *id = IDV_X;
	else if (strncmp(token->str,  "t", 1) == 0) *id = IDV_X;
	else if (strncmp(token->str, "theta", 2) == 0) *id = IDV_X;
	else if (strncmp(token->str, "PI", 2) == 0) *id = IDV_PI;
	else return false;

	for (int i = 0; i < strlen(GetVarName(*id)); i++) token = token->next;
	return true;
}

bool consume_func(int* id)
{
	if (token->kind != TK_RESERVED) return false;
		 if (strncmp(token->str, "cos",    3) == 0) *id = IDF_COS;
	else if (strncmp(token->str, "sin",    3) == 0) *id = IDF_SIN;
	else if (strncmp(token->str, "tan",    3) == 0) *id = IDF_TAN;
	else if (strncmp(token->str, "floor",  5) == 0) *id = IDF_FLOOR;
	else if (strncmp(token->str, "round",  5) == 0) *id = IDF_ROUND;
	else if (strncmp(token->str, "ceil",   4) == 0) *id = IDF_CEIL;
	else if (strncmp(token->str, "exp",    3) == 0) *id = IDF_EXP;
	else if (strncmp(token->str, "ln",     2) == 0) *id = IDF_LN;
	else if (strncmp(token->str, "mod",    3) == 0) *id = IDF_MOD;
	else return false;

	for (int i = 0; i < strlen(GetFuncName(*id)); i++) token = token->next;
	return true;
}

void expect(char* op)
{
	if (token->kind != TK_RESERVED || strncmp(token->str, op, strlen(op)) != 0)
		error("'%s'ではありません", op);
	token = token->next;
}

int expect_number(void)
{
	if (token->kind != TK_NUM) {
		printf("/*****\\\n");
		printf("token->kind : %d\n", token->kind);
		printf("token->str : %s\n", token->str);
		printf("\\*****/\n");
		error("数ではありません");
	}
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof(void)
{
	return token->kind == TK_EOF;
}


Token* new_token(TokenKind kind, Token* cur, char* str)
{
	Token* tok = calloc(1, sizeof(Token));
	if (tok != NULL) {
		tok->kind = kind;
		tok->str = str;
		cur->next = tok;
		return tok;
	}
	error("tokのメモリ確保に失敗しました");
	return NULL;
}


Token* tokenize(char* p)
{
	Token head;
	head.next = NULL;
	Token* cur = &head;

	while (*p) {
		if (isspace(*p)) {
			p++;
			continue;
		}

		//if (*p == '+' || *p == '-') {
		//	cur = new_token(TK_RESERVED, cur, p++);
		//	continue;
		//}

		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		cur = new_token(TK_RESERVED, cur, p++);/**/
		continue;
		error("トークナイズできません");
	}

	new_token(TK_EOF, cur, p);
	return head.next;
}


Node* new_node(NodeKind kind, Node* lhs, Node* rhs)
{
	Node* node = calloc(1, sizeof(Node));
	if (node != NULL) {
		node->kind = kind;
		node->lhs = lhs;
		node->rhs = rhs;
		return node;
	}
	error("nodeのメモリ確保に失敗しました");
	return NULL;
}

Node* new_node_num(int val)
{
	Node* node = calloc(1, sizeof(Node));
	if (node != NULL) {
		node->kind = ND_NUM;
		node->val = val;
		return node;
	}
	error("nodeのメモリ確保に失敗しました");
	return NULL;
}

Node* new_node_var(int id)
{
	Node* node = calloc(1, sizeof(Node));
	if (node != NULL) {
		node->kind = ND_VAR;
		node->val = id;
		return node;
	}
	error("nodeのメモリ確保に失敗しました");
	return NULL;
}

Node* new_node_func(int id, Node* lhs, Node* rhs)
{
	Node* node = calloc(1, sizeof(Node));
	if (node != NULL) {
		node->kind = ND_FNC;
		node->lhs = lhs;
		node->rhs = rhs;
		node->val = id;
		return node;
	}
	error("nodeのメモリ確保に失敗しました");
	return NULL;
}

Node* expr(void)
{
	Node* node;
	
	if (consume("-"))node = new_node(ND_SUB, mult(), new_node_num(0));
	else {
		consume("+");
		node = mult();
	}

	for (;;) {
		if (consume("+"))
			node = new_node(ND_ADD, mult(), node);
		else if (consume("-"))
			node = new_node(ND_SUB, mult(), node);
		else
			return node;
	}
}

Node* mult(void)
{
	Node* node = powr();

	for (;;) {
		if (consume("*"))
			node = new_node(ND_MUL, powr(), node);
		else if (consume("/"))
			node = new_node(ND_DIV, powr(), node);
		else if (!check("+") && !check("-") && !check(",") && !check(")") && !at_eof())
			node = new_node(ND_MUL, powr(), node);
		else
			return node;
	}
}

Node* powr(void)
{
	Node* node = prim();

	if (consume("^"))return new_node(ND_POW, node, powr());

	return node;
}

Node* prim(void)
{
	Node* node;
	int id;

	if (consume("(")) {
		node = expr();
		expect(")");
		return node;
	}
	else if (consume_func(&id)) {
		return func(id);
	}
	else if (consume_var(&id)) {
		return new_node_var(id);
	}
	return new_node_num(expect_number());
}

Node* func(int id)
{
	Node* node;


	if (consume("(")) {
		node = expr();
		while (consume(",")) {
			node = new_node_func(id, node, expr());
		}
		expect(")");
		return node->kind == ND_FNC ? node : new_node_func(id, node, NULL);
	}
	return new_node_func(id, mult(), NULL);
}



void gen(Node* node)
{
	int* op = func_arr;
	int v = node->val;

	if (node->kind == ND_NUM) {
		op[++func_count - 1] = v;
		return;
	}
	if (node->kind == ND_VAR) {
		op[++func_count - 1] = v;
		return;
	}

	gen(node->lhs);
	if (node->kind != ND_FNC || v / 10 % 2 == 0) gen(node->rhs);

	switch (node->kind) {
	case ND_ADD: op[++func_count - 1] = IDF_ADD; break;
	case ND_SUB: op[++func_count - 1] = IDF_SUB; break;
	case ND_MUL: op[++func_count - 1] = IDF_MUL; break;
	case ND_DIV: op[++func_count - 1] = IDF_DIV; break;
	case ND_POW: op[++func_count - 1] = IDF_POW; break;
	case ND_FNC: op[++func_count - 1] = v; break;
	default    : op[++func_count - 1] = IDF_ERR; break;
	}
}


double Calc(double x, double a, double b, double c, double d)
{
	Stack stst;
	Stack* st = &stst;
	st->size = 0;
	int* op = func_arr;

	for (int i = 0; i < func_count; i++) {
		//printf("%d : %d\n", i, op[i]);
		switch (op[i]) {
		case IDV_A: Push(st, a); break;
		case IDV_B: Push(st, b); break;
		case IDV_C: Push(st, c); break;
		case IDV_D: Push(st, d); break;
		case IDV_X: Push(st, x); break;
		case IDV_PI: Push(st, M_PI); break;
		case IDF_ADD: Push(st, Pop(st) + Pop(st)); break;
		case IDF_SUB: Push(st, Pop(st) - Pop(st)); break;
		case IDF_MUL: Push(st, Pop(st) * Pop(st)); break;
		case IDF_DIV: Push(st, Pop(st) / Pop(st)); break;
		case IDF_POW: Push(st, pow(Pop(st), Pop(st))); break;
		case IDF_COS: Push(st, cos(Pop(st))); break;
		case IDF_SIN: Push(st, sin(Pop(st))); break;
		case IDF_TAN: Push(st, tan(Pop(st))); break;
		case IDF_FLOOR: Push(st, floor(Pop(st))); break;
		case IDF_ROUND: Push(st, round(Pop(st))); break;
		case IDF_CEIL: Push(st, ceil(Pop(st))); break;
		case IDF_EXP: Push(st, exp(Pop(st))); break;
		case IDF_LN: Push(st, log(Pop(st))); break;
		case IDF_MOD: Push(st, fmod(Pop(st), Pop(st))); break;
		default: Push(st, op[i]); break;
		}
	}

	return st->arr[0];
}



int main(void) {
	char str[30];

	printf("\n\n   f(x) = ");
	fgets(str, sizeof(str) / sizeof(char), stdin);

	token = tokenize(str);

	printf("\n");

	func_count = 0;
	gen(expr());

	printf("\n\n");

	for (double x = 0; x <= 2; x += 0.125) {
			printf("   f(%5.3f) = %8.5f\n", x, Calc(x, 0, 0, 0, 0));
	}

	printf("\n");
	return 0;
}

// standard libraries
#include <stdio.h>
#include <stdlib.h>

// third-parts libraries
#include "mpc.h"

typedef struct
{
	const mpc_parser_t** list;
	const char** grammarElements;
	const int length;
} parsersStruct;

const parsersStruct* setupLang(const char* grammar, const char** grammarElements, const int numberOfParsers)
{

	mpc_parser_t* Number = mpc_new(grammarElements[0]);
	mpc_parser_t* Operator = mpc_new(grammarElements[1]);
	mpc_parser_t* Expr = mpc_new(grammarElements[2]);
	mpc_parser_t* Mfrc = mpc_new(grammarElements[3]);

	mpca_lang(MPCA_LANG_DEFAULT, grammar, Number, Operator, Expr, Mfrc);

	const mpc_parser_t** arrayOfParsers = malloc(numberOfParsers * sizeof(arrayOfParsers));
	arrayOfParsers[0] = Number;
	arrayOfParsers[1] = Operator;
	arrayOfParsers[2] = Expr;
	arrayOfParsers[3] = Mfrc; // set the last as the lang parser

	const parsersStruct* _parsersStruct = malloc(sizeof(*_parsersStruct));
	// cast to initial set const structs' fields
	// when a struct is on the heap, and so it cannot be initialized
	// with a struct literal
	*(mpc_parser_t***)&_parsersStruct->list = arrayOfParsers;
	*(char***)&_parsersStruct->grammarElements = grammarElements;
	*(int*)&_parsersStruct->length = numberOfParsers;

	return _parsersStruct;
}

int main(int argc, char** argv)
{
	// Declare a buffer for user input of size 2048
	char input[2048];

	const char* grammarElements[] = {"number", "operator", "expr", "mfrc"};

	const char* grammar = " \
		number : /-?[0-9]+/ ; \
		operator : '+' | '-' | '*' | '/' ; \
		expr : <number> | '(' <expr> <operator> <expr> ')' ; \
		mfrc : /^/ <expr> /$/ ; \
	";

	const parsersStruct* MfrcStruct = setupLang(
		grammar,
		grammarElements,
		sizeof(grammarElements) / sizeof(char*)
	);

	// the lang parser is always the last
	mpc_parser_t* Mfrc = MfrcStruct->list[MfrcStruct->length - 1];

	while(1) {

		// Output our prompt
		fputs("mfrc> ", stdout);

		// Read a line of user input of maximum size 2048
		fgets(input, 2048, stdin);

		// Attempt to Parse the user Input

		// result of parsing operations
		mpc_result_t r;

		if(mpc_parse("<stdin>", input, Mfrc, &r)) {
			mpc_ast_print(r.output);
			mpc_ast_delete(r.output);
		} else {
			// Otherwise Print the Error
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}
	}

	mpc_cleanup(
		MfrcStruct->length,
		MfrcStruct->list[0],
		MfrcStruct->list[1],
		MfrcStruct->list[2],
		MfrcStruct->list[3],
		MfrcStruct->list[4]
	);

	free(MfrcStruct->list);
	free(MfrcStruct);

	return 0;
}

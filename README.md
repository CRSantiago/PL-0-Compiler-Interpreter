# PL-0-Complier-Interpreter

### PL/0 uses a compiler to transform the human readable code into assembly code, and then uses a virtual machine to execute it.

The Grammar:
Based on Wirth’s definition for EBNF we have the following rules:  
[ ] means an optional item.  
{ } means repeat 0 or more times.  
Terminal symbols are enclosed in quote marks. Identifiers and Numbers should be treated as terminal symbols as well.
A period is used to indicate the end of the definition of a syntactic class.

program ::= block "."  
block ::= declarations proc statement.	 
declarations ::= { const | var }.  
const ::= ["const" ident ":=" [“-“] number ";"].	
var ::= [ "var "ident “;"].  
proc ::= [ "procedure" ident “{“ block “}” ].  
statement ::= [ “def” ident ":=" expression
              | "call" ident    
	      	    | "begin" statement { “;” statement } "end"   
	      	    | "read" ident  
		          | "if" condition "then" statement ["else" statement]  
		          | "while" condition "do" statement  
		          | "write" expression ] .      
condition ::= expression  (“==“|“<>”|"<"|"<="|">"|">=“)  expression.  
expression ::= term { ("+"|"-") term}.  
term ::= factor {("*"|"/") factor}.   
factor ::= ident | number | "(" expression ")“ .  

To compile, use the command “gcc driver.c lex.c parser.c vm.c”.  
To execute, you’ll need an input written in PL/0 and use the command “./a.out input.txt -v -s -l -c”.

•	“-v” tells the driver to set trace_flag to true so the vm will print the stack trace  
•	“-s” tells the driver to set table_flag to true so the parser will print the symbol table  
•	“-l” (which is a lowercase L) tells the driver to set list_flag to true so the lexical analyzer will print the lexeme table  
•	“-c” tells the driver to set code_flag to true so the parser will print the assembly code

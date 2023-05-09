/*
Christopher Santiago
COP 3402
Dr. Euripides Montagne
HW2 - Scanner (Compiler Update)
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

#define NORW 14      /* number of reserved words */
#define INTMAX 5 /* maximum integer value */
#define IDMAX 11     /* maximum number of chars for idents */
#define SYMMAX 17    /* maximum number of symbols*/

int currentposition = 0;
char mainkey[] = "main";
char nullkey[] = "null";

// Defining word symbols integer array
int keyword[] = {keyword_const, keyword_var,   keyword_procedure, keyword_call,
                 keyword_begin, keyword_end,   keyword_if,        keyword_then,
                 keyword_else,  keyword_while, keyword_do,        keyword_read,
                 keyword_write, keyword_def};
// Defining reserved symbols
char *symbols_res[] = {".",  ":=", "-",  ";", "{", "}", "==", "<>", "<",
                   "<=", ">",  ">=", "+", "*", "/", "(",  ")"};

// Defining symbol symbols integer array
int symbol_int[] = {period,
                assignment_symbol,
                minus,
                semicolon,
                left_curly_brace,
                right_curly_brace,
                equal_to,
                not_equal_to,
                less_than,
                less_than_or_equal_to,
                greater_than,
                greater_than_or_equal_to,
                plus,
                times,
                division,
                left_parenthesis,
                right_parenthesis};

// Defining reserved words
char *word[] = {"const", "var",  "procedure", "call", "begin", "end",   "if",
                "then",  "else", "while",     "do",   "read",  "write", "def"};

int lexemeIndex = 0;
lexeme *lexeme_arr;

void handleAlpha(char *program, int startposition);
int isReserved(char *str);
void handleNumber(char *program, int startposition);
void handleSpecialCharacter(char *program, char ch);

lexeme *lex_analyze(int list_flag, char *input) {
  
  lexeme_arr = calloc(ARRAY_SIZE, sizeof(lexeme));
  char program[2000];
  int length = 0;
  int i = 0;
  while(input[i] != '\0'){
    program[i] = input[i];
    i++;
    length++;
  }

  // Process each character of the program array
  while (currentposition < length) {

    int startposition = currentposition;
    // Ident or Reserved Word - must start with alpha character
    if (isalpha(program[currentposition])) {
      handleAlpha(program, startposition);
    } else if (isdigit(program[currentposition])) {
      handleNumber(program, startposition);
    } else {
      char ch = program[currentposition];
      /*check is ch is a white space character, ' ', '\n'. '\t' etc. 
      If so ignore and increment current position*/
      if (isspace(ch)) {
        currentposition++;
      } else if (ch == '#') { // check if '# (comment)',loop through until new line
        while (program[currentposition] != '\n') {
          currentposition++;
        }
      } else {
        handleSpecialCharacter(program, ch);
      }
    }
  }
  if(list_flag == 1){
    printf("Lexeme List\n");
    printf("\tlexeme\ttoken\n");
    /*Loop through lexeme arr, check if error is zero, 
    if so then print to console according to type. else print error*/
    for (int i = 0; i < lexemeIndex; i++) {
      if (lexeme_arr[i].error == 0) {
        if (lexeme_arr[i].type == number) {
          printf("%8d%6d\n", lexeme_arr[i].number_value, lexeme_arr[i].type);
        } else {
          printf("%8s%6d\n", lexeme_arr[i].identifier_name, lexeme_arr[i].type);
        }
      } 
    }
  }

  int error = 0;
  for (int i = 0; i < lexemeIndex; i++) {
      if (lexeme_arr[i].error == 0) {
        continue;
      } else {
        error += 1;
        switch (lexeme_arr[i].error) {
        case 1:
          printf("Lexical Analyzer Error: maximum identifier length is 11\n");
          break;
        case 2:
          printf("Lexical Analyzer Error: maximum number length is 5\n");
          break;
        case 3:
          printf(
              "Lexical Analyzer Error: identifiers cannot begin with digits\n");
          break;
        case 4:
          printf("Lexical Analyzer Error: invalid symbol\n");
          break;
        case 5:
          printf("Lexical Analyzer Error: identifiers cannot be named 'null' or"
                "'main'\n");
          break;
        }
      }
    }
    if (error > 0){
      return NULL;
    } else {
      return lexeme_arr;
    }
}

// Handles tokens that start with a letter: reserved words and identifiers
void handleAlpha(char *program, int startposition) {
  // // Character after the initial alpha character
  currentposition++;
  // Keep looking forward until a character is not alphanumeric
  while (isalnum(program[currentposition])) {
    currentposition++;
  }
  // Get the length of the string by subtracting the first character
  // position from the last character position in the program array
  int strlength = currentposition - startposition;
  if (strlength > IDMAX) {
    lexeme_arr[lexemeIndex].error = 1;
    lexemeIndex++;
  } else {
    // Copy the string from the program array into a new char array
    // so that it can be compared to the reserved words
    char str[strlength];
    strncpy(str, &program[startposition], strlength);
    str[strlength] = '\0';
    int reserved = isReserved(str);
    if (reserved != -1) {
      // if reserved word, update lexeme at current lexemeIndex
      strcpy(lexeme_arr[lexemeIndex].identifier_name, word[reserved]);
      lexeme_arr[lexemeIndex].type = keyword[reserved];
      lexeme_arr[lexemeIndex].error = 0;
      lexemeIndex++;
    } else {
      // if identifier, check no invalid keywrods were used, else update lexemearr
      if (strcmp(str, mainkey) == 0 || strcmp(str, nullkey) == 0) {
        lexeme_arr[lexemeIndex].error = 5;
        lexemeIndex++;
      } else {
        strcpy(lexeme_arr[lexemeIndex].identifier_name, str);
        lexeme_arr[lexemeIndex].type = identifier;
        lexeme_arr[lexemeIndex].error = 0;
        lexemeIndex++;
      }
    }
  }
  return;
}

int isReserved(char *str) {
  int i;
  // loop through number of reserved words and compare to string passed from handleAplha()
  for (i = 0; i < NORW; i++) {
    if (strcmp(str, word[i]) == 0) {
      return i;
    }
  }
  return -1;
}

void handleNumber(char *program, int startposition) {
  // Convert the starting digit from a char to an int
  int num = program[startposition] - '0';
  // Advance to the next character after the saved character
  currentposition++;
  int idx = 0;
  // Keep advancing until a non-digit character is found
  while (isdigit(program[currentposition]) && idx < INTMAX) {
    // Adjust the number so that the new number is in the ones place
    //atio
    num = num * 10 + (program[currentposition] - '0');
    idx++;
    currentposition++;
  }
  // Error -ident that starts with a number
  if (isalpha(program[currentposition])) {
    lexeme_arr[lexemeIndex].error = 3;
    lexemeIndex++;
    //while isalum(program current position) current++
    while(isalnum(program[currentposition])){
      currentposition++;
    }
  } else if (isdigit(program[currentposition])) { // Error - Number is too big
    lexeme_arr[lexemeIndex].error = 2;
    lexemeIndex++;
    while(isalnum(program[currentposition])){
      currentposition++;
    }
  } else {
    lexeme_arr[lexemeIndex].number_value = num;
    lexeme_arr[lexemeIndex].type = number;
    lexeme_arr[lexemeIndex].error = 0;
    lexemeIndex++;
  }
}

void handleSpecialCharacter(char *program, char ch) {
  /*
    Check ch passed in with valid special characters, update lexeme arr with each acceptance
    Special characters that contain more than one char are compared currentposition + 1 to        compare the full string.
    if valid, currentposition is increment by 1 to offset counter and not repeat second           character validation
  */
  switch (ch) {
  case '.':
    strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[0]);
    lexeme_arr[lexemeIndex].type = period;
    lexeme_arr[lexemeIndex].error = 0;
    lexemeIndex++;
    break;
  case ':':
    if (program[currentposition + 1] == '=') {
      strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[1]);
      lexeme_arr[lexemeIndex].type = assignment_symbol;
      lexeme_arr[lexemeIndex].error = 0;
      lexemeIndex++;
      currentposition++;
    } else {
      lexeme_arr[lexemeIndex].error = 4;
      lexemeIndex++;
    }
    break;
  case '-':
    strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[2]);
    lexeme_arr[lexemeIndex].type = minus;
    lexeme_arr[lexemeIndex].error = 0;
    lexemeIndex++;
    break;
  case ';':
    strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[3]);
    lexeme_arr[lexemeIndex].type = semicolon;
    lexeme_arr[lexemeIndex].error = 0;
    lexemeIndex++;
    break;
  case '{':
    strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[4]);
    lexeme_arr[lexemeIndex].type = left_curly_brace;
    lexeme_arr[lexemeIndex].error = 0;
    lexemeIndex++;
    break;
  case '}':
    strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[5]);
    lexeme_arr[lexemeIndex].type = right_curly_brace;
    lexeme_arr[lexemeIndex].error = 0;
    lexemeIndex++;
    break;
  case '=':
    if (program[currentposition + 1] == '=') {
      strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[6]);
      lexeme_arr[lexemeIndex].type = equal_to;
      lexeme_arr[lexemeIndex].error = 0;
      lexemeIndex++;
      currentposition++;
    }
    else {
      lexeme_arr[lexemeIndex].error = 4;
      lexemeIndex++;
    }
    break;
  case '<':
    if (program[currentposition + 1] == '>') {
      strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[7]);
      lexeme_arr[lexemeIndex].type = not_equal_to;
      lexeme_arr[lexemeIndex].error = 0;
      lexemeIndex++;
      currentposition++;
    } else if (program[currentposition + 1] == '=') {
      strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[9]);
      lexeme_arr[lexemeIndex].type = less_than_or_equal_to;
      lexeme_arr[lexemeIndex].error = 0;
      lexemeIndex++;
      currentposition++;
    } else {
      strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[8]);
      lexeme_arr[lexemeIndex].type = less_than;
      lexeme_arr[lexemeIndex].error = 0;
      lexemeIndex++;
    }
    break;
  case '>':
    if (program[currentposition + 1] == '=') {
      strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[11]);
      lexeme_arr[lexemeIndex].type = greater_than_or_equal_to;
      lexeme_arr[lexemeIndex].error = 0;
      lexemeIndex++;
      currentposition++;
    } else {
      strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[10]);
      lexeme_arr[lexemeIndex].type = greater_than;
      lexeme_arr[lexemeIndex].error = 0;
      lexemeIndex++;
    }
    break;
  case '+':
    strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[12]);
    lexeme_arr[lexemeIndex].type = plus;
    lexeme_arr[lexemeIndex].error = 0;
    lexemeIndex++;
    break;
  case '*':
    strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[13]);
    lexeme_arr[lexemeIndex].type = times;
    lexeme_arr[lexemeIndex].error = 0;
    lexemeIndex++;
    break;
  case '/':
    strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[14]);
    lexeme_arr[lexemeIndex].type = division;
    lexeme_arr[lexemeIndex].error = 0;
    lexemeIndex++;
    break;
  case '(':
    strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[15]);
    lexeme_arr[lexemeIndex].type = left_parenthesis;
    lexeme_arr[lexemeIndex].error = 0;
    lexemeIndex++;
    break;
  case ')':
    strcpy(lexeme_arr[lexemeIndex].identifier_name, symbols_res[16]);
    lexeme_arr[lexemeIndex].type = right_parenthesis;
    lexeme_arr[lexemeIndex].error = 0;
    lexemeIndex++;
    break;
  default:
    lexeme_arr[lexemeIndex].error = 4;
    lexemeIndex++;
  }
  currentposition++;
}

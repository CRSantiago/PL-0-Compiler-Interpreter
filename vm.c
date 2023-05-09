/*
Christopher Santiago
COP 3402
Dr. Euripides Montagne
HW1 - VM (Compiler Update)
*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "compiler.h"

int base(int* pas, int BP, int L);

// size of process address space
#define ARRAY_SIZE 500

int pas[ARRAY_SIZE];

int PC; // program counter (points to next instruction in text)
int BP; // base ponter (points to beginings of activation record)
int SP; // stack pointer (points to top of activation record)
int IC; // instruction count (for initialization of BP and SP. Incremented by 3 per instruction)

void execute(int trace_flag, instruction *code) {

    printf("VM Execution\n");
    // Initialize pas values to zero
    memset(pas, 0, ARRAY_SIZE);
    int OP = 0, L = 0, M = 0, i = 0, IC = 0;
    while(code[i].op != -1) {
        pas[IC]         = code[i].op;
        pas[IC + 1]     = code[i].l;
        pas[IC + 2]     = code[i].m;

        IC += 3;
        i += 1;

    }
    PC = 0;
    SP = IC - 1; // index of final M value in last instruction
    BP = SP + 1; //the index immediately following the M value from the last instruction in the program
    printf("\t\t\tPC\tBP\tSP\tstack\n");
    printf("Initial values:\t\t%d\t%d\t%d\n", PC, BP, SP);

    int halt = 1;
    // loop through instructions until halt instruction is reached
    while(halt == 1){

        // fetch cycle

        OP = pas[PC];
        L = pas[PC + 1];
        M = pas[PC + 2];

        PC += 3;

        char printOP[4];

        // execution cycle
        switch(OP){
            // LIT
            case 1:
                SP = SP + 1;
                pas[SP] = M;
                strcpy(printOP, "LIT");
                break;
            // OPR
            case 2:
                switch(M){
                    // RTN
                    case 0:
                        SP = BP - 1;
                        BP = pas[SP + 2];
                        PC = pas[SP + 3];
                        strcpy(printOP, "RTN");
                        break;
                    // ADD
                    case 1:
                        pas[SP - 1] = pas[SP - 1] + pas[SP];
                        SP = SP - 1;
                        strcpy(printOP, "ADD");
                        break;
                    // SUB
                    case 2:
                        pas[SP - 1] = pas[SP - 1] - pas[SP];
                        SP = SP - 1;
                        strcpy(printOP, "SUB");
                        break;
                    // MUL
                    case 3:
                        pas[SP - 1] = pas[SP - 1] * pas[SP];
                        SP = SP - 1;
                        strcpy(printOP, "MUL");
                        break;
                    // DIV
                    case 4:
                        pas[SP - 1] = pas[SP - 1] / pas[SP];
                        SP = SP - 1;
                        strcpy(printOP, "DIV");
                        break;
                    // EQL
                    case 5:
                        pas[SP - 1] = pas[SP - 1] == pas[SP];
                        SP = SP - 1;
                        strcpy(printOP, "EQL");
                        break;
                    // NEQ
                    case 6:
                        pas[SP - 1] = pas[SP - 1] != pas[SP];
                        SP = SP - 1;
                        strcpy(printOP, "NEQ");
                        break;
                    // LSS
                    case 7:
                        pas[SP - 1] = pas[SP - 1] < pas[SP];
                        SP = SP - 1;
                        strcpy(printOP, "LSS");
                        break;
                    // LEQ
                    case 8:
                        pas[SP - 1] = pas[SP - 1] <= pas[SP];
                        SP = SP - 1;
                        strcpy(printOP, "LEQ");
                        break;
                    // GTR
                    case 9:
                        pas[SP - 1] = pas[SP - 1] > pas[SP];
                        SP = SP - 1;
                        strcpy(printOP, "GTR");
                        break;
                    // GEQ
                    case 10:
                        pas[SP - 1] = pas[SP - 1] >= pas[SP];
                        SP = SP - 1;
                        strcpy(printOP, "GEQ");
                        break;
                }
                break;
            // LOD
            case 3:
                SP = SP + 1;
                pas[SP] = pas[base(pas, BP, L) + M];
                strcpy(printOP, "LOD");
                break;
            // STO
            case 4:
                pas[base(pas, BP, L) + M] = pas[SP];
                SP = SP - 1;
                strcpy(printOP, "STO");
                break;
            // CAL
            case 5:
                pas[SP + 1] = base(pas, BP, L);
                pas[SP + 2] = BP;
                pas[SP + 3] = PC;
                BP = SP + 1;
                PC = M;
                strcpy(printOP, "CAL");
                break;
            // INC
            case 6:
                SP = SP + M;
                strcpy(printOP, "INC");
                break;
            // JMP
            case 7:
                PC = M;
                strcpy(printOP, "JMP");
                break;
            // JPC
            case 8:
                if(pas[SP] == 0){
                    PC = M;
                } 
                SP = SP - 1;
                strcpy(printOP, "JPC");
                break;
            // SYS
            case 9: 
                switch(M){
                    // WRT
                    case 1:
                        printf("Output result is: %d\n", pas[SP]);
                        SP = SP - 1;
                        strcpy(printOP, "WRT");
                        break;
                    // RED
                    case 2:
                        SP = SP + 1;
                        printf("Please enter an integer: ");
                        scanf("%d", &pas[SP]);
                        strcpy(printOP, "RED");
                        break;
                    // HLT
                    case 3:
                        halt = 0;
                        strcpy(printOP, "HLT");
                        break;
                }
                break;
        }
        printf("%8s %d %d %12d %8d %8d\t", printOP, L, M, PC, BP, SP);
        
        if(trace_flag){
          //print stack
          int i;
          for(i = IC; i <= SP; i++) {
              if (BP != IC && i == BP) printf("| "); // avoid printing bar when only one activation record
              printf("%d ", pas[i]);
          }
        }
        printf("\n");
    }
}

/*
Helpfer function to find a variable in a different Activation Record some L levels down:
*/
int base(int *pas, int BP, int L) {
    int arb = BP; // arb = activation record base
    while ( L > 0) {//find base L levels down
        arb = pas[arb];
        L--;
    }
    return arb;
}
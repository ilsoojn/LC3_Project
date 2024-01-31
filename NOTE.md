# REGISTERs

There are total 8 general purpose registers (**GPR**), *R0 ... R7*. That's it.\
Since there is no speical registers, such as stack pointer, we decided to use last three GPR as a special register.

- R5 // FP *(Frame Pointer)*
- R6 // SP *(Stack Pointer)*
- R7 // Return Address / Goto Address

We may need the special regsiters such as MAR *(Memory Adress Register)* & MDR *(Memory Data Register)*, in the future.
PC and SR *(Processor Status Register)* are defined and added in **class PSR**.

# INSTRUCTIONs

ISA is 16-bits, starting with 4-bits opcode (See README.md "## LC-3 REFERENCE").

<details>
<summary> <bold> Format & ISA </bold> </summary>

##### Immediate 6 Format
| OP    | OPC  | bit_3 | bit_3 | bit_6       |
| ----- | ---- | :---: | :---: |    :---:    |
| ADD   | 0001 | DR    | SR_1  | 000 : SR2   |
| AND   | 0101 | DR    | SR_1  | 000 : SR2   |
| ADD   | 0001 | DR    | SR_1  | 1 : imm5    |
| AND   | 0101 | DR    | SR_1  | 1 : imm5    |
| LDR   | 0110 | DR    | BaseR | imm6_offset |
| STR   | 0111 | SR    | BaseR | imm6_offset |
| NOT   | 1001 | DR    | SR_1  | 111111      |
| JSRR  | 0100 | 000   | BaseR | 000000      |
| JMP   | 1100 | 000   | BaseR | 000000      |
| RET   | 1100 | 000   | 111   | 000000      |
| RTI   | 1000 | 000   | 000   | 000000      |

##### Immediate 9 Format
| OP    | OPC  | bit_3 | bit_9       |
| ----- | ---- | :---: | :---: |
| LD    | 0010 | DR    | imm9_offset |
| LDI   | 1010 | DR    | imm9_offset |
| LEA   | 1110 | DR    | imm9_offset |
| ST    | 0011 | SR    | imm9_offset |
| STI   | 1011 | SR    | imm9_offset |

##### Branch Format
| OP    | OPC  | bit_3 | bit_9       |
| ----- | ---- | :---: | :---: |
| BR    | 0000 | CC    | imm9_offset |

##### Immediate 11 Format
| OP    | OPC  | bit_1 | bit_11       |
| ----- | ---- | :---: | :---: |
| JSR   | 0100 | 1     | imm11_offset |

##### Trap Format
| OP    | OPC  | bit_4 | bit_8       |
| ----- | ---- | :---: | :---: |
| TRAP  | 1111 | 0000  | 8: trapvect |

</details>

# Pseudo Assembler Directives

- **.ORIG**     : starting point of LC3 program
- **.FILL**     : word    - initilised
- **.BLKW**     : word    - unknown
- **.STRINGZ**  : string  - initialised
- **.END**      : end of the program
- **.EXRNAL**   : external 'library' function call 
<details>

<summary> <bold> Example (referenced book pg 179) </bold> </summary>

```
; DO some_NUMBER multiplies by 6
        .ORIG   x3050
        LD  R1, SIX         ; R1 <- SIX         // int SIX = 6;
        LD  R2, NUMBER      ; R2 <- NUMBER      // int NUMBER = ...?
        AND R3, R3, #0      ; R3 <- 0

AGAIN   ADD R3,     R3, R2  ; R3 <- R3 + R2     // for(SIX 0) NUMBER += NUMBER;
        ADD R1,     R1, #-1 ; R1 <- R1 - 1      
        BRp AGAIN           ; IF(above ADD result R1 0) 
                            ;   THEN (goto AGAIN) 
                            ;   ELSE (goto ELSE)
ELSE    HALT

NUMBER  .BLKW   1           ; save a word with variable NUMBER
SIX     .FILL   x0006       ; variable SIX holds real number 6
        .END
```

</details>

# Personal Note

<details> 
<summary> Expand Notes </summary>

- Does ***TargetMachine*** put everything together in the ***MY_TARGET*** folder ? ~~class LC3TargetMachine @ LC3.h~~ 

</details>
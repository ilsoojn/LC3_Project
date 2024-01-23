# REGISTERs

There are total 8 general purpose registers, R0 ... R7, and that's it. Since there is no speical processor (status) registers, we decided to use last three GPR as a special register.

- R0 // GPR
- R1 // GPR
- R2 // GPR
- R3 // GPR
- R4 // GPR
- R5 // FP (Frame Pointer) 
- R6 // sp : Stack Pointer
- R7 // Return Address / Goto Address

As we go thorugh, we may need the special regsiters such as MAR (Memory Adress Register) & MDR (Memory Data Register), as well as, PC and SR (Processor Status Register). We will see...

# INSTRUCTIONs

INSTR is 16-bits, starting with 4-bits opcode (See README.md "## LC-3 REFERENCE"). Listed all the Instructions set based on InstrFormat.

ADD     {4: opcode, 3: DR,  3: SR1,     000,  3: SR2}
AND     {4: opcode, 3: DR,  3: SR1,     000,  3: SR2}
ADD     {4: opcode, 3: DR,  3: SR1,     1, 5: imm5}
AND     {4: opcode, 3: DR,  3: SR1,     1, 5: imm5}

LDR     {4: opcode, 3: DR,  3: BaseR,   6: imm6_offset}
STR     {4: opcode, 3: SR,  3: BaseR,   6: imm6_offset}

NOT     {4: opcode, 3: DR,  3: SR,      111111}

LD      {4: opcode, 3: DR,  9: imm9_offset}
LDI     {4: opcode, 3: DR,  9: imm9_offset}
LEA     {4: opcode, 3: DR,  9: imm9_offset}
ST      {4: opcode, 3: SR,  9: imm9_offset}
STI     {4: opcode, 3: SR,  9: imm9_offset}

BR      {4: opcode, 3: CC,  9: imm9_offset}
JSR     {4: opcode, 1,      11: imm11_offset}

JMP     {4: opcode, 000,    3: BaseR,   000000}
JSRR    {4: opcode, 000,    3: BaseR,   000000}

RET     {4: opcode, 000,    111,    000000}
RTI     {4: opcode, 000,    000,    000000}

TRAP    {4: opcode, 0b0000, 8: trapvect}
reserved
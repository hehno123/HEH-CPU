# HEH-CPU
  Simple 16 bit CPU, done entirely in logisim
  
## Overall design
  1. 16 bit CPU
  2. Simple pipelining system
  3. Branch predictor - Always not taken
  4. Separted ROM and RAM - one memory cell has 16 bits
  5. I/O operations are not supported yet
  6. System has 8, 16 bit register A, B, C, D, E, F, G, H. They haven't any special purpose
  7. 16 bit address bus

## ISA
  ```
  FORMAT: OOOOCAAAXXXXXXXX

  OOOO: Opcode
  C: operand control bit
  AAA: Destination Register

  X:
  If C 0, then last 3 bits RD
  IF C 1, then bits IMM8

  Instructions:
  1. nop 
  2. hlt
  3. jz RD, RS/Imm8 (jump to RS/Imm8 address, if RD == 0)
  4. jnz RD, RS/Imm8 (jump to RS/Imm8 address, if RD != 0)
  5. jmp RS/Imm8
  6. add RD, RS/Imm8
  7. sub RD, RS/Imm8
  8. and RD, RS/Imm8
  9. or RD, RS/Imm8
  10. xor RD, RS/Imm8
  11. not RD, RS/Imm8
  12. sll RD, RS/Imm4
  13. srl RD, RS/Imm4
  14. mov RD, RS/Imm8
  15. lod RD, RS/Imm8  (M[RS] or M[Imm8] <- RD)
  16. str RD, RS/Imm8  (RD <- M[RS] or M[Imm8])

  In case of shfit instructions, only 4 lsb bits are considered
```

## Assembler
  Assembler is written in C and it's very straightforward.

  ```assembly
  Start:
	xor H, H
	xor B, B
	mov A, 7
	str A, H
	
	add H, 1
	mov C, halt
	str C, H
	jmp fib
```

  - instructions can be only written in lowercase
  - labels are ended by colon
  - Comma and space are separators between words (syntax xor,,,,,H,,,,H is valid as well as xor     H,    H)
  - comment begins with #

## Disadvantages of project 
  1. CPU doesn't use pc-relative addressing, so Imm8 jumps can only jump to first 256 lines of code.
  2. Absence of communication with I/O system
  3. CPU doesn't have any flags, so conditions jump are limited. Jumps conditions are calculated outside of ALU
  4. Not effective branch predictor, because it assumes that branch is always not taken
  5. Done entirely in logisim
  6. ISA doesn't utilize bits in effective way

## Advantages of project
  1. It works
  2. Simple pipeline system and branch predicting

## Plans for future
  1. Improve ISA
  2. Improve Branch predicting
  3. Add communication with I/O devices
  4. RAM and ROM unification (optionally)
  5. Add flags to processor  
  6. Write it in vhdl

# 1 "start.s"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "start.s"
# 10 "start.s"
# 1 "../userprog/syscall.h" 1
# 16 "../userprog/syscall.h"
# 1 "../threads/copyright.h" 1
# 17 "../userprog/syscall.h" 2
# 11 "start.s" 2

        .text
        .align 2
# 24 "start.s"
 .globl __start
 .ent __start
__start:
 jal main
 move $4,$0
 jal system_Exit
 .end __start
# 45 "start.s"
 .globl system_Halt
 .ent system_Halt
system_Halt:
 addiu $2,$0,0
 syscall
 j $31
 .end system_Halt

 .globl system_Exit
 .ent system_Exit
system_Exit:
 addiu $2,$0,1
 syscall
 j $31
 .end system_Exit

 .globl system_Exec
 .ent system_Exec
system_Exec:
 addiu $2,$0,2
 syscall
 j $31
 .end system_Exec

 .globl system_Join
 .ent system_Join
system_Join:
 addiu $2,$0,3
 syscall
 j $31
 .end system_Join

 .globl system_Create
 .ent system_Create
system_Create:
 addiu $2,$0,4
 syscall
 j $31
 .end system_Create

 .globl system_Open
 .ent system_Open
system_Open:
 addiu $2,$0,5
 syscall
 j $31
 .end system_Open

 .globl system_Read
 .ent system_Read
system_Read:
 addiu $2,$0,6
 syscall
 j $31
 .end system_Read

 .globl system_Write
 .ent system_Write
system_Write:
 addiu $2,$0,7
 syscall
 j $31
 .end system_Write

 .globl system_Close
 .ent system_Close
system_Close:
 addiu $2,$0,8
 syscall
 j $31
 .end system_Close

 .globl system_Fork
 .ent system_Fork
system_Fork:
 addiu $2,$0,9
 syscall
 j $31
 .end system_Fork

 .globl system_Yield
 .ent system_Yield
system_Yield:
 addiu $2,$0,10
 syscall
 j $31
 .end system_Yield

 .globl system_PrintInt
 .ent system_PrintInt
system_PrintInt:
        addiu $2,$0,11
        syscall
        j $31
 .end system_PrintInt

 .globl system_PrintChar
 .ent system_PrintChar
system_PrintChar:
        addiu $2,$0,12
        syscall
        j $31
 .end system_PrintChar

 .globl system_PrintString
 .ent system_PrintString
system_PrintString:
        addiu $2,$0,13
        syscall
        j $31
 .end system_PrintString

 .globl system_GetReg
 .ent system_GetReg
system_GetReg:
 addiu $2,$0,14
 syscall
 j $31
 .end system_GetReg

 .globl system_GetPA
 .ent system_GetPA
system_GetPA:
 addiu $2,$0,15
 syscall
 j $31
 .end system_GetPA

 .globl system_GetPID
 .ent system_GetPID
system_GetPID:
 addiu $2,$0,16
 syscall
 j $31
 .end system_GetPID

 .globl system_GetPPID
 .ent system_GetPPID
system_GetPPID:
 addiu $2,$0,17
 syscall
 j $31
 .end system_GetPPID

 .globl system_Sleep
 .ent system_Sleep
system_Sleep:
 addiu $2,$0,18
 syscall
 j $31
 .end system_Sleep

 .globl system_GetTime
 .ent system_GetTime
system_GetTime:
 addiu $2,$0,19
 syscall
 j $31
 .end system_GetTime

 .globl system_GetNumInstr
 .ent system_GetNumInstr
system_GetNumInstr:
 addiu $2,$0,50
 syscall
 j $31
 .end system_GetNumInstr

 .globl system_PrintIntHex
 .ent system_PrintIntHex
system_PrintIntHex:
 addiu $2,$0,20
 syscall
 j $31
 .end system_PrintIntHex


        .globl __main
        .ent __main
__main:
        j $31
        .end __main

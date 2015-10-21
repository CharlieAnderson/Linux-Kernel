#define ASM     1

#include "x86_desc.h"
/*
 In particular, the call number is placed in EAX, the first argument in EBX, then
ECX, and finally EDX
*/

.globl USER_LEVEL
.globl START_SYS_CALLS

.align   4

#getting to ring 3 to execute user program
USER_LEVEL: 
    movl 4(%esp), %eax
    pushl $USER_DS #user data segment with bottom 2 bits set for ring 3
    pushl %eax #push our current stack just for the heck of it    
    movl 16(%esp), %eax #get argument
    pushf       #store flags
    pushl $USER_CS; #user data segment with bottom 2 bits set for ring 3
    pushl %eax #push pointer to new process stack here
    sti
    iret
#user program makes a system call
START_SYS_CALLS:
    pushl %ECX
    pushl %EDX
    pushl %EBX
    pushl %esp
    pushl %ebp
    pushl %edi
    pushl %esi
    pushf
    cmpl $10, %eax #check if the cmd parameter is out of range (not from 0 to 3)
    jg ERROR     
    cmpl $1, %eax 
    jl ERROR
    # /if read or write, check if fd is in bounds
    cmpl $3, %eax
    je   FD_CHECK
    cmpl $4, %eax
    je   FD_CHECK
    
FD_PASS:
    subl $1, %eax

    PUSHL %EDX
    PUSHL %ECX
    PUSHL %EBX
    #jump to c functions
    call *sys_jump_table(, %eax,4)  

    popl %EBX
    popl %ECX
    popl %EDX

    popf
    popl %esi
    popl %edi
    popl %ebp
    popl %esp
    popl %ebx
    popl %EDX
    popl %ECX
    iret

ERROR: 
popf
popl %esi
popl %edi
popl %ebp
popl %esp
popl %ebx
popl %EDX
popl %ECX
movl $-1, %EAX
iret

FD_CHECK:
#check fd is in bounds
cmpl $7, %ebx 
jg ERROR     
cmpl $0, %ebx 
jl ERROR
jmp FD_PASS

#
sys_jump_table:
.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn


.end



#you can do you jumptable now, where sys_call_table contains your system calls
#jmp *syscall_table(, %eax,4)

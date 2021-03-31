  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #
.global delay
.global enable_interrupt

.text

enable_interrupt:
    ei
jr $ra

delay:
    PUSH $s0
    PUSH $s1
    PUSH $s2
    PUSH $s3
   
while:
    slt $s1, $0, $a0     #While $a0 > 2, $s2 = 0
    beq $s1, $0, done_while
    nop
    addi $a0, $a0, -1    #Decrement $a0
    addi $s0, $0, 0     #Declare i = $s0
for:
    slti $s2, $s0, 3800 #While i < const. $s3 = 1 [CHANGE HERE FOR LONGER DELAY]
    beq $s2, $0, while
    nop
    addi $s0, $s0, 1    #Increment i = $s0
    j for   #Branch to for if $s2 = 1 (while i < const.)
    nop
    
done_while:    
    
    POP $s3
    POP $s2
    POP $s1
    POP $s0
    
    jr $ra
    nop
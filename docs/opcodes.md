These are a list of bytecode opcodes that can be used.


* OP_PRINT       //print the value of stack[top] to stdout
* OP_PUSH x      //leaves x as the stack[top]
* OP_POP         //remove the stack[top]
* OP_NOT         //negates stack[top] (leaves boolean literal)
* OP_LESS        //if stack[top] less than stack[top-1] (leaves boolean value)
* OP_GREATER     //if stack[top] greater than stack[top-1] (leaves boolean value)
* OP_CONDITIONAL //if stack[top] is true, execute the next opcode, otherwise skip it
* OP_JUMP x      //alter the program counter by x (positive or negative) TODO: could use stack top


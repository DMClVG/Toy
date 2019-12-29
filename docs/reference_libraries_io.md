# IO

This is the basic Input and Output library for Toy. This library is only available in the unsafe build.

## ReadLine()

This function reads an entire line from stdin, until a newline character is entered, and returns that line as a string.

## ReadChar()

This function reads a single character from stdin, and returns it as a string.

## ReadNumber()

This function reads an entire line from stdin, until a newline character is entered, then tries parsing it into a number. If the parsing succeeded, that number is returned, otherwise null is returned.

## Write(str)

This function prints the value of "str" to the command line. It is essentially identical to the `print` keyword, except it doesn't append a newline character.


# Libraries

Several libraries are provided by default with Toy, and can be accessed via the `import` [keyword](tutorial.md#Import).

```
import "Standard";
```

Libraries insert their contents directly into the global scope when the `import` keyword is used. The functions provided are implemented in native code, rather than written in Toy.

# Standard

Standard is exactly that - the standard library for Toy.

## Clock()

Returns a number representing unix time, including milliseconds.

## Random()

Returns a pseudo-random number between 0 (inclusive) and 1 (exclusive). This relies on the underlying pseudo-random number generator implementation, and uses a seed based on unix time in milliseconds.

Coming Soon: Math library!
Coming Soon: JSON library!
Coming Soon: IO library!
Coming Soon: Type casting library!

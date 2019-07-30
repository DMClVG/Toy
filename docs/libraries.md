# Libraries

Several libraries are provided by default with Toy, and can be accessed via the `import` [keyword](tutorial.md#Import).

```
import "Standard";
```

The functions provided are implemented in native code, rather than written in Toy.

# Standard

Standard is exactly that - the standard library for Toy. When imported without an alias, the following functions are added to the global scope. When an alias is used, they are bundled into the alias.

## Clock()

Returns a number representing unix time, including milliseconds.

## Random()

Returns a pseudo-random number between 0 (inclusive) and 1 (exclusive). This relies on the underlying pseudo-random number generator implementation, and uses a seed based on unix time in milliseconds.

Coming Soon: Math library!
Coming Soon: JSON library!
Coming Soon: IO library!
Coming Soon: Type casting library!

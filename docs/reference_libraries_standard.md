# Standard

Standard is exactly that - the standard library for Toy. When imported without an alias, the following functions are added to the global scope. When an alias is used, they are bundled into the alias as normal.

## Clock()

Returns a number representing unix time, including milliseconds.

## Random()

Returns a pseudo-random number between 0 (inclusive) and 1 (exclusive). This relies on the underlying pseudo-random number generator implementation, and uses a seed based on unix time in milliseconds by default.

## RandomSeed(seed)

This sets the seed to be used by the Random() function to equal the first argument. It only accepts a number as an argument, and ignores any decimals.


# Libraries

Several libraries are provided by default with Toy, and can be accessed via the `import` [keyword](reference_language.md#Import).

```
import "Standard";
```

The functions provided are implemented in native code, rather than written in Toy.

# Toy

This is the meta-library, holding information about the Toy language itself. Use this for determining version information.

## version

This string is equal to "major.minor.patch".

## major

This number is the language's major version number.

## minor

This number is the language's minor version number.

## patch

This number is the language's patch version number.

## VersionGreater(x, y, z)

This is a helper function - it returns true of the current version is greater than "x.y.z", otherwise it returns false.

## VersionEqual(x, y, z)

This is a helper function - it returns true of the current version is equal to "x.y.z", otherwise it returns false.

## VersionLess(x, y, z)

This is a helper function - it returns true of the current version is less than "x.y.z", otherwise it returns false.

# Standard

Standard is exactly that - the standard library for Toy. When imported without an alias, the following functions are added to the global scope. When an alias is used, they are bundled into the alias.

## Clock()

Returns a number representing unix time, including milliseconds.

## Random()

Returns a pseudo-random number between 0 (inclusive) and 1 (exclusive). This relies on the underlying pseudo-random number generator implementation, and uses a seed based on unix time in milliseconds by default.

## RandomSeed(seed)

This sets the seed to be used by the Random() function to equal the first argument. It only accepts a number as an argument, and ignores any decimals.

Coming Soon: Math library!
Coming Soon: JSON library!
Coming Soon: IO library!
Coming Soon: Type casting library!

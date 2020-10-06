The C# version of Toy is officially deprecated, in favour of the branch [0.4.0](https://github.com/Ratstail91/Toy/tree/0.4.0).

# Toy

This is the Toy programming language interpreter, written in C#. It can be tested out here: [toylang.com](https://www.toylang.com)

My goal for writing this language is simply to have fun and to learn, but also to create something that *could* be usable in an actual dev environment, particularly as an extension for the Unity game engine.

The docs can be found under docs/, or you could start [here](docs/reference_language.md).

To see an example of Toy working in Unity, checkout my [flappybirb example](https://github.com/Ratstail91/flappybirb).

Special thanks to http://craftinginterpreters.com/ for their fantastic book that I followed to write this.

## Building

To build the project, run `make` in the root directory - this will build the safe option by default.

To build with the full set of features, including the IO library, run `make unsafe`, which will include all libraries in the `src/.unsafe` folder.


# Unity

This is the guide for setting up the Toy programming language to run in the Unity game engine.

This guide was tested in Unity 2019.2.0f1 with Toy version 0.1.0.

## Setup

Clone or copy/paste the toy code to the Assets directory in a new project. If you see an error such as this:

```
error CS0656: Missing compiler required member
```

Then you need to enable .NET version 4.x compatability. Open player settings -> player -> other settings, set "API Compatability Level" to 4.x or later.

You must also define "TOY_UNITY" under player settings -> player -> other settings -> Scripting Define Symbols, otherwise the console will not work.

## Important Notes

TODO


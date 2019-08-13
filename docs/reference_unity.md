# Unity

This is the guide for setting up the Toy programming language to run in the Unity game engine.

This guide was tested in Unity 2019.2.0f1 with Toy version 0.1.1.

## Setup

Clone or copy/paste the Toy repo to the Assets directory in a new project. If you see an error such as this:

```
error CS0656: Missing compiler required member
```

Then you need to enable .NET version 4.x compatability. Open player settings -> player -> other settings, set "API Compatability Level" to 4.x or later.

You must also define "TOY_UNITY" under player settings -> player -> other settings -> Scripting Define Symbols, otherwise the console will not work.

## Important Notes

The Unity components are not exposed to the raw language - instead, wrappers are used so that the language can interface with each component. Keep this in mind when working with the plugin in C#.

## Reference

Here are a set of references for each component of the Unity API.

* [Unity Plugin](reference_unity_plugin.md)
* [Unity GameObject](reference_unity_gameobject.md)
* [Unity Toy Behaviour](reference_unity_behaviour.md)
* [Unity Transform](reference_unity_transform.md)
* [Unity Rigidbody 2D](reference_unity_rigidbody2d.md)

## Tutorial

* [Toy Unity Tutorial](tutorial_unity.md)
* [Working With The Plugin](reference_unity_implementation.md)

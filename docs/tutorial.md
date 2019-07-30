# Tutorial

Welcome to the tutorial for the Toy programming language!

I'm still developing this language and discovering all of it's incedental nooks and crannys, but hopefully the core of the language is solid enough that I can begin to teach people to use it. This guide on how to use the language will also double as the official documentation for the time being.

If you're familiar with JavaScript, you will see a lot of similarities. However, there are also differences that I hope will make my language stand out in time. I don't expect it to reach JavaScript's popularity, but I would like it to be useful.

# Other Documents

A full break down of the language's [grammar is available](grammar) for those who are interested.

One of the more interesting features of Toy are the [plugins](plugins.md), which are detailed separately from the language. Array and Dictionary are included as examples for people who want to write their own.

# Basics

## Print

Let's start with the most fundemental keyword - "print".

```
print "hello world";
```

`print` is followed by a single argument that resolves to either a literal or a variable, which is converted to a string and then outputted to the console, followed by a newline.

`print` is a keyword, rather than a function. This is partially due to Toy's origin as a [lox derivative](http://craftinginterpreters.com/), but I've decided to keep it, as it makes it easy to debug code without requiring a library of some kind (and all of the baggage that comes with).


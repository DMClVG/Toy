# OP_IF_FALSE_JUMP

This is something interesting I discovered a while back - the most fundemental control structures can be built using just one jumping opcode:

"If the value X (consumed) is false, then jump to a specific point"

However doing so requires some bending over backwards by pushing a false value onto the stack, just to be consumed. So, instead, it might just be easier to have a generic jump code.

## if

```
OP_PUSH condition
OP_IF_FALSE_JUMP end
{
	//true code
}
end:
```

## if, else

```
OP_PUSH condition
OP_IF_FALSE_JUMP else
{
	//true code
}
OP_PUSH false
OP_IF_FALSE_JUMP end
else:
{
	//false code
}
end:
```

## while

```
begin:
OP_PUSH expression
OP_IF_FALSE_JUMP end
{
	//loop
}
OP_PUSH not
OP_IF_FALSE_JUMP begin
end:
```

## do-while

```
begin:
{
	//loop
}
OP_PUSH expression
OP_IF_FALSE_JUMP end
OP_PUSH false
OP_IF_FALSE_JUMP begin
end:
```

## for

```
{
	//init code

	begin:
	OP_PUSH condition
	OP_IF_FALSE_JUMP end
	{
		//body
	}
	{
		//increment
	}

	OP_PUSH false
	OP_IF_FALSE_JUMP begin

	end:
}
```


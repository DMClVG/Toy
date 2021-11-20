# OP_IF_FALSE_JUMP

This is something interesting I discovered a while back - the most fundemental control structures can be built using just one opcode:

"If the value X is false, then jump to a specific point"

## if

```
push expression
OP_IF_FALSE_JUMP end
{
	//true code
}
end:
```

## if, else

```
push expression
OP_IF_FALSE_JUMP else
{
	//true code
}
else:
OP_IF_FALSE_JUMP end
{
	//false code
}
end:
```

## while

```
begin:
push expression
OP_IF_FALSE_JUMP end
{
	//loop
}
OP_IF_FALSE_JUMP begin
end:
```

## do-while

```
begin:
{
	//loop
}
push expression
OP_IF_FALSE_JUMP end
OP_IF_FALSE_JUMP begin
end:
```

## for

```
{
	//init code
	begin:
	push expression
	OP_IF_FALSE_JUMP end
	{
		//body
	}
	{
		//increment
	}
	OP_IF_FALSE_JUMP begin
	end:
}
```


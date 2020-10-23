if:

```
push expression
OP_IF_FALSE_JUMP end
{
	//true code
}
end:
```

if, else:

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

while:

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

do-while:

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

for:

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

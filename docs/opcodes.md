if:

```
push expression
IF_NOT_JMP end
{
	//true code
}
end:
```

if, else:

```
push expression
IF_NOT_JMP else
{
	//true code
}
else:
IF_NOT_JMP end
{
	//false code
}
end:
```

while:

```
begin:
push expression
IF_NOT_JMP end
{
	//loop
}
IF_NOT_JMP begin
end:
```

do-while:

```
begin:
{
	//loop
}
push expression
IF_NOT_JMP end
IF_NOT_JMP begin
end:
```

for:

```
{
	//init code
	begin:
	push expression
	IF_NOT_JMP end
	{
		//body
	}
	{
		//increment
	}
	IF_NOT_JMP begin
	end:
}
```

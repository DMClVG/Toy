if:

```
push expression
IF_NOT -> JMP end
{
	//true code
}
end:
```

if, else:

```
push expression
IF_NOT -> JMP else
{
	//true code
}
JMP end
else:
{
	//false code
}
end:
```

while:

```
begin:
push expression
IF_NOT -> JMP end
{
	//loop
}
JMP begin
end:
```

do-while:

```
begin:
{
	//loop
}
push expression
IF_NOT -> JMP end
JMP begin
end:
```

for:

```
{
	//init code
	begin:
	push expression
	IF_NOT -> JMP end
	{
		//body
	}
	//increment
	JMP begin
	end:
}
```

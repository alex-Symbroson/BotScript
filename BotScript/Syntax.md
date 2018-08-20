
# Botscript

## Keywords
Keyword  | Usage
-------  | -------
```import```   | import file
```var```      | declare variable with dynamic type
```loop```     | counting loop
```while```    | condition loop
```continue``` | jump to next loop cycle
```break```    | leave loop
```return```   | leave function
```(null)```   | no value (default value for var)

## Operators
-> see http://www.cplusplus.com/doc/tutorial/operators

## Datatypes

#### simple datatypes:
Datatype | Notation | member functions
-------- | -------- | ----------------
```Bool```     | ```true, false```          | ```reverse()->Bool```
```Int```      | ```0-9, SYSx0-9```         | ```toStr([SYS])->Str``` <br>
```Float```    | ```0-9.0-9, SYSx0-9.0-9``` | ```isInt()->Bool```
```String```   | ```"" ''```                | ```find(Str, [start, [end]])->int``` <br> ```insert(Str, pos)``` <br> ```replace(Str, Str)->Str``` <br> ```split(Str, max)->Lst```

#### complex datatypes:
Datatype |  Notation  | member functions
-------- | ---------- | ----------------
```List```     | ```[var, ]```    | ```find(var, [start, [end]])->Int``` <br> ```insert(var, pos)``` <br> ```pop()->var``` <br> ```push(var)```
```Object```   | ```[str: var]``` | ```keys()->Lst``` <br> ```values()->Lst``` <br> ```pop(key)->var```
```Function``` | ```{ ... }```    | ``` ```
```Pin```      | ```Pin(Int)```   | ```setInput()``` <br> ```Read()->Int``` <br> ```setOutput()``` <br> ```Write(Int)``` <br> ```Stop()``` <br> ```...```

## Namespaces
#### Math
- ```sin, cos, tan, asin, acos, atan, pow, sqrt, ...```

#### RaspiBot
- ```getMotors() -> [left, right]```
- ```setMotors(left, right)``` <br>
- ```getIR()     -> [left, right]``` <br>
- ```getEncoder  -> [left, right]```
- ```setEncoder(left, right)```  <br>
- ```buttonPressed(i) -> bool```
- ```setButtonColor(i, r, g)```  <br>
- ```drive(velocity)```
- ```curve(radius, angle)```
- ```camera:```
    - ```capture(file)```
    - ```record(file) //file=null -> stop record```


#### Thread
- ```create(function, var) -> Int```
- ```join(Int) -> var```
- ```cancel(Int)```


## Builtins
- ```print(var)```
- ```input(var)->var```
- ```delay(Int)```
- ```clock()->Int```

## Comments
```c
//comment

/* multiline
   comment \*/
```

## Examples
```
```

# simpl

A programming language

## Sample

### Fibonacci.sip

``` simpl
main: int = Fib(index = 8).res, // result is 21

module Fib {
    index,

    a = 0,
    b = 1, 
    c = a + b,

    if = If(v = index > 2).extract,
    res = Fib(index: if = index - 1, a = b, b = c).res,
    res: !if = c,
},
```

## Build

``` pwsh
# windows
cmake -G Ninja
ninja
```

## Run

``` pwsh
./simplc Test.sip
```

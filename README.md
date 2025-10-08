# simpl

A programming language

## Sample

### Fibonacci.sip

``` simpl
main = res : Fib(index = 8), // result is 21

module Fib {
    index,

    a = 0,
    b = 1, 
    c = a + b,
    idx: if = index - 1,

    if = extract : If(v = index > 2),
    else = extract : If(v = index <= 2),
    res = res : Fib(index = idx, a = b, b = c),
    res: else = c,
},
```

## Build

``` pwsh
cmake . -Bbuild
cmake --build build
```

## Run (Windows)

``` pwsh
./build/Debug/simplc Test.sip
```

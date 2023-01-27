# simpl

A programming language

## Sample

Main.sip

``` simpl
main = Add(x = two, y = four).extract,
two = Unary(val = 1).succ,
four = Unary(val = 2).square,

module Unary {
    succ = Add(x = val, y = 1).extract,
    square = Mul(x = val, y = val).extract,
},
```

result

``` pwsh
6
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

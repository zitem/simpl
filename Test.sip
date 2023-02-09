main: int = res : Fib(index = four * 2), // result is 21

four = val : Get(mod = Four),

module Get { val = val : mod() },

module Four { val = 4 },

module Fib {
    index,

    a = 0,
    b = 1, 
    c = a + b,
    idx: if = index - 1,

    if = extract : If(v = index > 2),
    res = res : Fib(index = idx, a = b, b = c),
    res: !if = c,
},
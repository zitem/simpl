main: int = Fib(index = 8).res, // result is 21

module Unary {
    succ = Add(x = val, y = 1).extract,
    pred = Sub(x = val, y = 1).extract,
},

module Fib {
    index,

    a = 0,
    b = 1, 
    c = Add(x = a, y = b).extract,

    run = Gt(x = index, y = 2).extract,
    if = If(v = run).extract,
    else = Not(v = if).extract,

    pred: if = Unary(val = index).pred,
    res = Fib(a = b, b = c, index = pred).res,
    res: else = c,
},
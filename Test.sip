main = res : Fib(index = 8),

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

module Eq {
    extract = extract: Eq_Int(x = x, y = y),
    extract = extract: Eq_Bool(x = x, y = y),
},

module Noteq {
    extract = extract: Noteq_Int(x = x, y = y),
    extract = extract: Noteq_Bool(x = x, y = y),
},
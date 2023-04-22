// main = x:b:a(x=1),
// module a {
//     module b {
//         x = 2
//     },
// },

// module Eq {
//     extract = extract: Eq_Int(x = x, y = y),
//     extract = extract: Eq_Bool(x = x, y = y),
// },

// module Noteq {
//     extract = extract: Noteq_Int(x = x, y = y),
//     extract = extract: Noteq_Bool(x = x, y = y),
// },
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
    // TODO: try to replace to this
    // res = c: else,
},

// flags = {
//     useBit = enum.size <= 16,
//     cont: if useBit  = 0x00000000,
//     cont: if !useBit = array(size = enum.size, super = bool),
// },

// flags(enum = [hasA, hasB]),                  -- enum.size = 2, useBit = true
// flags(enum = [hasA, hasB], useBit = false),  -- enum.size = 2, useBit = false
// flags(enum = [hasA, ...]),                   -- enum.size = 17, useBit = false
// flags(enum = [hasA, ...], useBit = true),    -- failed
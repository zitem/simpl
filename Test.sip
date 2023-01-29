// module Test
main: bool = Eq(x = b, y = b).extract,
b = false,

n27 = Arithmetic(lhs = three, rhs = nine).mul,

three = Arithmetic(lhs = 1, rhs = 2).add,
nine = Unary(val = 3).square,

module Arithmetic {
    lhs,
    rhs,
    add = Add(x = lhs, y = rhs).extract,
    sub = Sub(x = lhs, y = rhs).extract,
    mul = Mul(x = lhs, y = rhs).extract,
    div = Div(x = lhs, y = rhs).extract,
},

module Unary {
    square = Mul(x = val, y = val).extract,
},

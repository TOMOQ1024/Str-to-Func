# Str-to-Func
数式文字列を解析し，実行するサンプルです
入力された数式は下記文法に従ったものと仮定しています

# 文法
数式全体はexprとみなします
また，"t", "h"は"x"とみなして計算します
```
expr = ("+" | "-")? mult ("+" mult | "-" mult)*
mult = powr (powr | "*" powr | "/" powr)*
powr = prim ("^" prim)*
prim = "(" expr ")" | func | v | n
func = f mult | f "(" expr ("," expr)* ")"
n = 0以上の整数
v = "a" | "b" | "c" | "d" | "x" | "t" | "h" | "PI"
f = "cos" | "sin" | "tan" | "floor" | "round" | "ceil" | "exp" | "ln" | "mod"
```

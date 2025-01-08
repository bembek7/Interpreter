# Implementation of a General-Purpose Programming Language

*Bartłomiej Puciłowski*

## Assumed Features

- Arithmetic, relational, and logical operations
- Variable creation and scope handling
- Function creation and invocation, including recursive functions
- A function may or may not return a value - `return` statement
- Variables are constant by default, passed by copy
- Conditional statement `if else`
- Loop statement `while`
- Comments preceded by the `#` symbol

### Data Types

- Basic numeric types (`int`, `float`)
- Boolean type - `bool`
- String type - `string`
- Dynamic, weak typing
- Function as a data type, i.e., a function can be an argument or a return value of another function.

#### Type Conversion

The order of types does not matter unless the output type is a string, in which case concatenation is done from the left or right.

Since typing is dynamic, in the case of assignment, the variable simply becomes the assigned type.

If an operator is not described, it means that the conversion is impossible - error.

All relational operators = rel

| Type in 1  | Type In 2   | Operator     | Type Out                                                                                                                                                       |
|------------|-------------|--------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **bool**   | **int**     | &&, \|\|, == | **bool** if **int** is convertible (0, 1),<br> error                                                                                                         |
| **bool**   | **float**   |              | error                                                                                                                                                          |
| **bool**   | **string**  | &&, \|\|, == | **bool** if string is convertible ("true", "false"),<br> error                                                                                               |
| **bool**   | **string**  | +            | **string** convert bool to ("true", "false") and concatenate to string                                                                                          |
| **int**    | **float**   | +,-,*,/, rel | **float**                                                                                                                                                     |
| **int**    | **string**  | -,*,/, rel   | **int** if string is convertible to int,<br> **float** if string is convertible to float,<br> error                                                      |
| **int**    | **string**  | +            | **int** if string is convertible to int,<br> **float** if string is convertible to float,<br> convert int to string and concatenate from the appropriate side |
| **float**  | **string**  | -,*,/, rel   | **float** if string is convertible to int or float,<br> error                                                                                             |
| **bool**   | **function**|              | error                                                                                                                                                          |
| **int**    | **function**|              | error                                                                                                                                                          |
| **float**  | **function**|              | error                                                                                                                                                          |
| **string** | **function**|              | error                                                                                                                                                          |

The only standard operator working on functions is ==, which returns true only if it is exactly the same defined function (comparison is impossible in the case of lambda).

### Operators Working on Functions

- ">>" - composition, combines two functions into one, where the result of the first function is the argument of the second.

```plaintext
func Square(x)
{
    return x * x;
}

func Increment(x)
{
    return x + 1;
}

func main()
{
    var composedFunction = square >> increment;
}
```

equivalent to:

```plaintext
func Compose(f, g)
{
    return func(x){ return g(f(x));}
}

func Square(x)
{
    return x * x;
}

func Increment(x)
{
    return x + 1;
}

func main()
{
    var composedFunction = Compose(square, increment);
    var result = composedFunction(3);
    print(result);
}
```

- "<<" - allows creating a new function and passing default arguments to it

```plaintext
func Fizz(a, b, c)
{
    return a + b + c;
}

func main()
{
    var buzz = Fizz << (3, 2);
    # buzz (x) == Fizz (3, 2, x)
}
```

### Examples

Creating and maintaining variables

```plaintext
mut var a;
a = 10;

var b = "123";

var c = a + b; # c == 133 prioritizing numbers over strings
```

```plaintext
var a = 10;

var b = a * a;

print(b);  # b automatically converted to string and printed
```

conditional statement if... else

```plaintext
var b = false;

if(b)
{
 # do sth
}
else
{
 # do sth else
}
```

while loop statement

```plaintext
mut var a = 0;
while(a < 10)
{
 # do stuff

 a++;
}
```

Function declaration and definition

```plaintext
func Fizz(a, b)
{
 print(a, b);
 if(a > b)
 {
  Fizz(a - b, b);
 }
}
```

```plaintext
func Add(a, b)
{
 return a + b;
}
```

Main

```plaintext
func Main()
{
}
```

Functions as variables

```plaintext
func LogDecorator(func)
{
    return func(args...)
    {
        print("Called function: " + func.name);
        print("Arguments: " + args);
        var result = func(args...);
        print("Result: " + result);
        return result;
    }
}

func Add(a, b)
{
    return a + b;
}

func main()
{
    var decoratedAdd = LogDecorator(Add);
    var result = decoratedAdd(3, 4);
}
```

## Grammar

Syntax part

```plaintext
program               = { function_definition };
function_definition   = "func", identifier, "(", parameters, ")", block;
parameters            = [ parameter, { ",", parameter } ];
parameter             = ["mut"], identifier 

block                 = "{", { statement }, "}";
statement             = function_call_statement
                      | conditional
                      | loop
                      | return_statement
                      | block
                      | declaration
                      | assignment

function_call_statement = function_call, ";";

declaration           = ["mut"], "var", identifier, [ "=", expression ], ";";
assignment            = identifier, "=", expression, ";";

function_call         = identifier, "(", arguments, ")";
arguments             = [ expression, { ",", expression } ];

conditional           = "if", "(", standard_expression, ")", block,
                        [ "else", block ];

loop                  = "while", "(", standard_expression, ")", block;
return_statement      = "return", [ expression ], ";";

expression            = standard_expression 
                      | "[", func_expression, "]";

standard_expression   = conjunction, { "||", conjunction }       
conjunction           = relation_term, { "&&", relation_term };
relation_term         = additive_term, [ relation_operator, additive_term ];
additive_term         = ["-"], (multiplicative_term, { ("+" | "-"), multiplicative_term });
multiplicative_term   = factor, { ("*" | "/"), factor };
factor                = [ "!" ], (literal | "(", standard_expression, ")" | identifier | function_call);

func_expression       = composable, { ">>", composable };
composable            = bindable, [ "<<", "(", arguments, ")" ];
bindable              = (function_lit | identifier |  "(", func_expression, ")");

function_lit          = "(", parameters, ")", block;
literal               = number | string | boolean;
number                = integer | float;
integer               = nonZeroDigit, { digit } | "0";
float                 = integer, ".", [ digit, { digit } ];
string                = '"', { any_character }, '"';
boolean               = "true" | "false";

identifier            = letter, { letter | digit | "_" };

comment               = "#", { any_character_except_newline }, ("\n" | EOF);
```

Lexical part

```plaintext
relation_operator     = ">=" | ">" | "<=" | "<" | "==" | "!=";
nonZeroDigit          = "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
digit                 = "0" | nonZeroDigit
letter                = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z"
                      | "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z";
symbol                = "!" | "@" | "#" | "$" | "%" | "^" | "&" | "*" | "(" | ")" | "-" | "_" | "+" | "=" | "{" | "}" | "[" | "]" | ":" | ";" | "'" | "<" | ">" | "," | "." | "/" | "?" | "\\" | "|" | "\" | "\"";
whitespace            = " " | "\t" | "\n" | "\r";
any_character         = letter | digit | symbol | whitespace;
any_character_except_newline = any_character - "\n";
```

## Error Handling

### Format

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: (message)`

The language does not distinguish between error severity levels. Every error is treated with the same "caution" - resulting in the program's execution being halted.

### Error Examples

- definition of a non-mutable variable without initialization,

```plaintext
var a;
```

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Defining const variable without initalization`

- changing the value of a non-mutable variable,

```plaintext
var a = 10;
a = "Hi";
```

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Trying to change const variable`

- redefinition of a variable,

```plaintext
var a = 10;
var a = "Hi";
```

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Redefiniton of variable: a`

- wykonywanie operacji na niedozwolonej kombinacji typów,

```plaintext
var a = "Hi" + 3;
```

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: The variable type does not support such operation`

- referencing a variable that does not exist in the current scope,

```plaintext
func Fizz()
{
 mut var b;
}

func main()
{
    b = 10;
}
```

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: is not recognized`

- definition of variables outside the function body,

```plaintext
mut var c;

func main()
{
    
}
```

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Cannot declare variable in global scope`

- calling a function with an incorrect number of arguments,

```plaintext
func Fizz()
{
 
}

func main()
{
    Fizz(5, 3);
}
```

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Invalid number of arguments in the function call`

- unknown syntax,

```plaintext
funky main()
{
}
```

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Invalid syntax`

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Invalid syntax`

## Implementation

I do not know the details at this stage of the project.

The interpreter will consist of four key modules:

- lexical analyzer,
- syntax analyzer,
- object structure interpreter.

### Lexical Analyzer (Lexer)

The lexer transforms the source code into tokens according to the language grammar. Each token represents specific elements, such as keywords or special symbols, and contains information about its position in the code (line and column number).

The lexer reports errors in specific cases, such as:

- excessive zeros at the beginning of a number,
- exceeding the maximum length of a number or identifier,
- number overflow.

Tokens generated by the lexer:

- Identifier  
- Integer  
- Float  
- String  
- Boolean  
- Semicolon  
- LParenth  
- RParenth  
- LBracket  
- RBracket  
- Comma  
- Comment  
- EndOfFile  
- Unrecognized  
- Assign  
- Plus  
- Minus  
- Asterisk  
- Slash  
- LogicalNot  
- Less  
- Greater  
- LogicalAnd  
- LogicalOr  
- Equal  
- LessEqual  
- GreaterEqual  
- NotEqual  
- PlusAssign  
- MinusAssign  
- AsteriskAssign  
- SlashAssign  
- AndAssign  
- OrAssign  
- Mut  
- Var  
- While  
- If  
- Else  
- Return  
- Func  
- FunctionBind  
- FunctionCompose

If the lexer encounters a sequence of characters it cannot decode, it will generate a special `Unrecognized` token.

### Syntax Analyzer

The syntax analyzer (parser) receives a stream of tokens from the lexer and creates an object structure of the source code. The parser processes tokens according to the language grammar, creating a structure that represents the hierarchy and relationships between code elements.

The parser reports errors in specific cases, such as:

- missing parentheses,
- incorrect use of operators,
- incorrect use of keywords,
- generally checks the syntactic correctness of the code.

### Object Structure Interpreter

The interpreter executes the code based on the generated object structure and checks the correctness of the code:

- whether the given 'expressions' have the appropriate type.
- whether variables are declared.
- whether variables are used in appropriate contexts.
- whether functions are called with the correct number of arguments.
- whether functions are declared.
- whether functions return a value according to the declaration.
- presence of the `Main()` function.

## Testing

Unit tests for all modules and integration tests covering earlier stages. Ongoing manual tests will also be performed.

- **Lexical Analyzer**: Check if the given input string returns the expected sequence of tokens.
- **Syntax Analyzer**: Check if the given source code or token sequence generates the expected object hierarchy.
- **Object Hierarchy Interpreter**: Check if the interpreter generates the expected results.

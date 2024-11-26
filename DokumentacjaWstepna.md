# Implementacja języka programowania ogólnego przeznaczenia

*Bartłomiej Puciłowski 325212*

Jest to dokumentacja wstępna, więc będzie zmieniana i uzupełniana, nie wszystkie rzeczy ostatecznie zostaną zaimplementowane **dokładnie** tak jak opisano

## Zakładane funkcjonalności

- Operacje arytmetyczne, relacyjne i logiczne
- Tworzenie zmiennych oraz obsługiwanie zakresu widoczności
- Tworzenie oraz wywoływanie funkcji, również rekurencyjnych
- Funkcja może, ale nie musi zwracać wartość - instrukcja `return`
- Zmienne domyślnie stałe, przekazywane przez kopię
- Instrukcja warunkowa `if else`
- Instrukcja pętli `while`
- Komentarze poprzedzone symbolem `#`

### Typy danych

- Podstawowe typy liczbowe (`**int**`, `float`)
- Typ logiczny - `bool`
- Typ tekstowy - `string`
- Typowanie dynamiczne, słabe
- Funkcja jako typ danych, tzn. funkcja może być argumentem lub wartością zwracaną przez inną funkcję.

#### Konwersja typów

Kolejność typów bez znaczenia, chyba że typ out to string wtedy doklejenie od lewej lub prawej.

Jako, że typowanie dynamiczne w przypadku assignmentu po prostu zmienna staja się assignowanym typem.

Jeśli operator nie jest opisany znaczy że konwersja jest niemożliwa - błąd,

Wszystkie operatory relacji = rel

| Typ in 1      | Typ In 2  | Operator      | Typ Out                                                                                                                                                       |
|---------------|-----------|---------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **bool**      | **int**   | &&, \|\|, ==  | **bool** jeśli **int** konwertowalny (0, 1),<br> błąd                                                                                                         |
| **bool**      | float     |               | błąd                                                                                                                                                          |
| **bool**      | string    | &&, \|\|, ==  | **bool** jeśli string konwertowalny ("true", "false"),<br> błąd                                                                                               |
| **bool**      | string    | +             | **string** zmiana boola na ("true", "false") i doklejenie do stringa                                                                                          |
| **int**       | float     | +,-,*,/, rel  | **float**                                                                                                                                                     |
| **int**       | string    | -,*,/, rel    | **int** jeśli string konwertowalny na inta,<br> **float** jeśli string konwertowalny na floata,<br> błąd                                                      |
| **int**       | string    | +             | **int** jeśli string konwertowalny na inta,<br> **float** jeśli string konwertowalny na floata,<br> zmiana inta na stringa i doklejenie z odpowiedniej strony |
| float         | string    | -,*,/, rel    | **float** jeśli string konwertowalny na inta lub floata,<br> błąd                                                                                             |
| float         | string    | +             | **string** zmiana floata na stringa i doklejenie z odpowiedniej strony                                                                                        |
| **bool**      | funkcja   |               | błąd                                                                                                                                                          |
| **int**       | funkcja   |               | błąd                                                                                                                                                          |
| float         | funkcja   |               | błąd                                                                                                                                                          |
| string        | funkcja   |               | błąd                                                                                                                                                          |

Jedyny standardowy operator działający na funkcjach, to ==, zwracające prawdę tylko jeśli chodzi o dokładnie tą samą zdefiniowaną funkcję (porówwnanie niemożliwe w przypadku lambdy).

### Operatory działające na funkcjach

- ">>" - kompozycja, łączy dwie funkcje w jedną, gdzie wynik pierwszej funkcji jest argumentem drugiej.

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

równe temu:

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

- "<<" - pozwala na stworzenie nowej funkcji i przekazanie do niej domyślnych argumentów

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

### Przykłady

Tworzenie, zachowanie zmiennych

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

instrukcja warunkowa if... else

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

instrukcja pętli - while

```plaintext
mut var a = 0;
while(a < 10)
{
 # do stuff

 a++;
}
```

Deklaracja i definicja funkcji

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
func main()
{
}
```

Funkcje jako zmienne

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

## Gramatyka

Część składniowa

```plaintext
program               = { function_definition };
function_definition   = "func", identifier, "(", parameters, ")", block;
parameters            = [ parameter, { ",", parameter } ];
parameter             = ["mut"], identifier 

block                 = "{", { statement }, "}";
statement             = function_call
                      | conditional
                      | loop
                      | return_statement
                      | block
                      | declaration
                      | assignment

declaration           = ["mut"], "var", identifier, [ "=", expression ], ";";
assignment            = identifier, "=", expression, ";";

function_call         = identifier, "(", arguments, ")";
arguments             = [ expression, { ",", expression } ];

conditional           = "if", "(", expression, ")", block,
                        [ "else", block ];

loop                  = "while", "(", expression, ")", block;
return_statement      = "return", [ expression ], ";";

expression            = conjunction, { "||", conjunction } 
                      | func_expression;
conjunction           = relation_term, { "&&", relation_term };
relation_term         = additive_term, [ relation_operator, additive_term ];
additive_term         = ["-"], (multiplicative_term, { ("+" | "-"), multiplicative_term });
multiplicative_term   = factor, { ("*" | "/"), factor };
factor                = [ "!" ], (literal | "(", expression, ")" | identifier | function_call);

func_expression       = possible_function, ">>", possible_function
                      | possible_function, "<<", "(", arguments, ")"
                      | possible_function;

possible_function     = (function_lit | identifier | func_expression);
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

Część leksykalna

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

## Obsługa błędów

### Format

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: (message)`

Język nie rozróżnia poziomów zagrożenia błędów. Każdy błąd traktowany jest z taką samą "ostrożnością" - skutkuje wstrzymaniem wykonania programu.

### Przykłady wywołania błędu

- definicja niemutowalnej zmiennej bez inicjalizacji,

```plaintext
var a;
```

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Defining const variable without initalization`

- zmiana wartości niemutowalnej zmiennej,

```plaintext
var a = 10;
a = "Hi";
```

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Trying to change const variable`

- redefinicja zmiennej,

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

- powołanie się na zmienną, która nie istnieje w obecnym zakresie,

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

- definicja zmiennych poza ciałem funkcji,

```plaintext
mut var c;

func main()
{
    
}
```

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Cannot declare variable in global scope`

- wywołanie funkcji z błędną ilością argumentów,

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

- nieznana składnia,

```plaintext
funky main()
{
}
```

`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Invalid syntax`

## Sposób uruchomienia

Program będzie aplikacją konsolową, uruchamianą poprzez wywołanie z argumentem zawierającym ścieżkę do kodu źródłowego do interpretacji
podanym na standardowym wejściu. Wynik poszczególnych kroków analizy oraz wynik samego uruchomienia programu zostanie
wyświetlony na standardowym wyjściu.

Przykład: `.\Interpreter sourceFile.xxx`

## Implementacja

Nie znam szczegółów na tym etapie projektu.

Interpreter będzie zbudowany z czterech kluczowych modułów:

- analizator leksykalny,
- analizator składniowy,
- analizator semantyczny,
- interpreter struktury obiektowej.

### Analizator leksykalny (lekser)

Lekser przekształca kod źródłowy na tokeny zgodne z gramatyką języka. Każdy token reprezentuje określone elementy, jak słowa kluczowe czy symbole specjalne, i zawiera informacje o swoim położeniu w kodzie (numer linii i kolumny). Lekser zgłasza błędy w konkretnych przypadkach, takich jak:

- nadmiarowe zera na początku liczby,
- przekroczenie maksymalnej długości liczby (15 znaków) lub identyfikatora (50 znaków).
- overflow liczb.

Tokeny generowane przez lekser (aktualnie, na pewno będzie ich więcej):

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


Jeżeli lekser napotka sekwencję znaków, której nie może zdekodować, wygeneruje specjalny token `Unrecognized`.

### Analizator składniowy

Parser odbiera strumień tokenów z leksera i tworzy strukturę obiektową kodu źródłowego. Metoda, którą parser będzie przetwarzał tokeny, zostanie ustalona w późniejszym etapie projektu. W przypadku błędów składniowych parser rzuca wyjątki z informacjami o położeniu błędnych elementów w kodzie.

### Analizator semantyczny

Analizator semantyczny sprawdza poprawność kodu na podstawie struktury obiektowej wygenerowanej przez parser. Weryfikuje obecność funkcji `main()` oraz analizuje użycie zmiennych – czy są zadeklarowane i czy stosowane w poprawnym kontekście. W przypadku znalezienia błędów semantycznych rzuca wyjątki, które zawierają informacje o położeniu i nazwie problematycznego elementu.

### Interpreter struktury obiektowej

Interpreter realizuje kod oparty na wygenerowanej strukturze obiektowej, ale przystępuje do pracy dopiero po pomyślnym zakończeniu analizy semantycznej. Zadaniem interpretera jest wykonywanie instrukcji, nadawanie wartości zmiennym oraz obsługa funkcji takich jak `print()`, która pozwala na wyświetlanie danych na standardowym wyjściu.

## Testowanie

Nie znam szczegółów na tym etapie projektu.

Testy jednostkowe dla wszystkich modułów jeśli tylko będzie to możliwe oraz testy integracyjne obejmujące wcześniejsze etapy. Na biężąco wykonywane będą też testy manualne.

- **Analizator leksykalny**: Sprawdzenie, czy dla danego wejściowego ciągu znaków zwróci oczekiwany ciąg tokenów.
- **Analizator składniowy**: Sprawdzenie, czy podany kod źródłowy lub ciąg tokenów generuje oczekiwaną hierarchię obiektów.
- **Analizator semantyczny**: Sprawdzenie, czy przygotowany niepoprawny kod zwraca odpowiednie komunikaty o błędach – testowany program musi zawierać wyjątek z oczekiwanym komunikatem.
- **Interpreter hierarchi obiektów**: Sprawdzenie, czy interpreter generuje oczekiwane wyniki

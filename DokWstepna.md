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
- Podstawowe typy liczbowe (`int`, `float`)
- Typ logiczny - `bool`
- Typ tekstowy - `string`
- Typowanie dynamiczne, słabe
- Funkcja jako typ danych, tzn. funkcja może być argumentem lub wartością zwracaną przez inną funkcję.

Kompozycja łączy dwie funkcje w jedną, gdzie wynik pierwszej funkcji jest argumentem drugiej.
```
func Compose(f, g)
{
    return func(x){ return f(g(x));}
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
    var composed_function = compose(square, increment);
    var result = composed_function(3);
    print(result);
}
```
LogDecorator zwraca nową funkcję, która rozszerza zachowanie przekazanej funkcji, dodając logowanie.
```
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
### Przykłady
Tworzenie, zachowanie zmiennych
```
mut var a;
a = 10;

var b = "123";

var c = a + b;	# c == 133 prioritizing numbers over strings
```
```
var a = 10;

var b = a * a;

print(b);		# b automatically converted to string and printed
```
instrukcja warunkowa if... else
```
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
```
mut var a = 0;
while(a < 10)
{
	# do stuff

	a++;
}
```
Deklaracja i definicja funkcji
```
func Fizz(a, b)
{
	print(a, b);
	if(a > b)
	{
		Fizz(a - b, b);
	}
}
```
```
func Add(a, b)
{
	return a + b;
}
```
Main
```
func main()
{
}
```
## Gramatyka
```
program               = { function_definition | comment };
comment               = "#", { any_character };

function_definition   = "func", identifier, "(", parameters, ")", block;
parameters            = [ identifier, { ",", identifier } ];

block                 = "{", { statement }, "}";
statement             = function_call
                      | conditional
                      | loop
                      | return_statement
                      | block
                      | declaration
                      | assignment
                      | comment;

declaration           = ["mut"] "var", identifier, [ "=", expression ], ";";
assignment            = identifier, "=", expression, ";";

function_call         = identifier, "(", arguments, ")", ";";
arguments             = [ expression, { ",", expression } ];

conditional           = "if", "(", expression, ")", block,
                        [ "else", block ];

loop                  = "while", "(", expression, ")", block;
return_statement      = "return", [ expression ], ";";

expression            = conjunction, { "or", conjunction };
conjunction           = relation_term, { "and", relation_term };
relation_term         = additive_term, [ relation_operator, additive_term ];
relation_operator     = ">=" | ">" | "<=" | "<" | "==" | "!=";
additive_term         = multiplicative_term, { ("+" | "-"), multiplicative_term };
multiplicative_term   = factor, { ("*" | "/"), factor };
factor                = literal | "(", expression, ")" | identifier;

literal               = number | string | boolean;
number                = integer | float;
integer               = ["-"], digit, { digit };
float                 = integer, ".", digit, { digit };
string                = '"', { any_character }, '"';
boolean               = "true" | "false";

any_character         = letter | digit | symbol | whitespace;

identifier            = letter, { letter | digit | "_" };
digit                 = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9";
letter                = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z"
                      | "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z";
symbol                = "!" | "@" | "#" | "$" | "%" | "^" | "&" | "*" | "(" | ")" | "-" | "_" | "+" | "=" | "{" | "}" | "[" | "]" | ":" | ";" | "'" | "<" | ">" | "," | "." | "/" | "?" | "\\" | "|" | "\";
whitespace            = " " | "\t" | "\n" | "\r";

```

## Obsługa błędów
### Format
`Error [line: (lineNr), column: (columnNr), file name: (filename)]: (message)`

Język nie rozróżnia poziomów zagrożenia błędów. Każdy błąd traktowany jest z taką samą "ostrożnością" - skutkuje wstrzymaniem wykonania programu.

### Przykłady wywołania błędu
- definicja niemutowalnej zmiennej bez inicjalizacji,
```
var a;
```
`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Defining const variable without initalization`
- zmiana wartości niemutowalnej zmiennej,
```
var a = 10;
a = "Hi";
```
`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Trying to change const variable`
- redefinicja zmiennej,
```
var a = 10;
var a = "Hi";
```
`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Redefiniton of variable: a`
- wykonywanie operacji na niedozwolonej kombinacji typów,
```
var a = "Hi" + 3;
```
`Error [line: (lineNr), column: (columnNr), file name: (filename)]: The variable type does not support such operation`
- powołanie się na zmienną, która nie istnieje w obecnym zakresie,
```
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
```
mut var c;

func main()
{
    
}
```
`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Cannot declare variable in global scope`
- wywołanie funkcji z błędną ilością argumentów,
```
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
```
funky main()
{
}
```
`Error [line: (lineNr), column: (columnNr), file name: (filename)]: Invalid syntax`

## Sposób uruchomienia
Program będzie aplikacją konsolową, uruchamianą poprzez wywołanie z argumentem zawierającym ścieżkę do kodu źródłowego do interpretacji
podanym na standardowym wejściu. Wynik poszczególnych kroków analizy oraz wynik samego uruchomienia programu zostanie
wyświetlony na standardowym wyjściu.

Przykład: `./Interpreter sourceFile.xxx`

## Wymagania funkcjonalne i niefunkcjonalne

## Implementacja ogólnie (tokeny):
 - (Dodaj szczegóły dotyczące implementacji tokenów)

## Testowanie
- (Opisz podejście do testowania)

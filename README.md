# lml
A Lisp Markup Language.

## Motivation
I really dislike xml syntax, so I wrote a parser for a lisp like syntax that compiles to html.

## Usage
```
lml
lml <input file>
```
With no parameters, lml will read from stdin and write to stdout.  With a filename passed as input, it will create a file in the same directory as the input file with the extension changed to `.html`.

## Syntax
```
(html (body //this is a commment.
  (p [bg-black body-paragraph]  //notice all the classes are specified all in one place
    "I like to eat " (a {href: "https://www.getapples.com"} "apples") ".")))
  //notice the spaces in quotes
```
turns into this:
```
<!DOCTYPE HTML>
<html>
  <body>
    <p class="bg-black body-paragraph">I like to eat <a href="https://www.getapples.com">apples</a>.</p>
  </body>
</html>
```
minus the indentation.  If you want it formatted, run it through an html formatter.

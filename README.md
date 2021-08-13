# Parser
This repository contains my custom parser that I built in C.

The program takes a text file as an input and breaks it into separate parts. Functionally, it reads the file line by line breaking each one into tokens, operators, keywords, strings, characters, comments, numerics, and identifiers. Then it prints the results. 

## Usage
Use the line below to compile the code and produce an excutable:
```
gcc -Wall parser.c -o parser
```
To run the program use the line below with any file you wish to parse:
```
./parser file.txt
```

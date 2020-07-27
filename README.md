# What is this?
Compact is a personal programming language I made to test my skills in C++ and my ability to create a lexical analyzer and parser.

# How does this work?
Compact uses a lexer/tokenizer to go through each character of a string and generates a vector to a bunch of tokens that represent the string. My parser then interprets the vector of tokens. Variables are stored using a std::map located within a compact_state* object which must be instantiated by the user. Also, basically everything is allocated on the heap because I thought that would make it faster. It probably didn't help. 

The definition for tokens is a disaster. Each compact_token contains a token type and a compact_object* which when dereferenced contains a void* and an object type. Object and token types are pretty much identical; in fact, I'm pretty sure the definition for the token type enumerator just extends the object type enumerator which I had already made. Tokens and objects are (almost) the exact same thing.

The parser has separate functions for statements and expressions. It assumes the string input is a statement and checks for an equal sign or an identifier at the beginning. If it doesn't have either of those, then execute the line as an expression, and otherwise, take whatever's after the identifier or after the equals sign and parse that as an expression, then take what was returned, replace the right side expression with it, and parse the statement. I'm aware this is redundant. I know better now.

Compact at least has an acceptable implementation for operator precedence. Adding operators to Compact is fairly easy.

# How do I use it?
Please don't use Compact. This language is really weird, and I plan on restarting it. Regardless, here's a breakdown of what you can do (trust me, it's not much)
```javascript
carve "What is your name? "
absorb x
carve "Hello, " + x + "!"

x = 7
b = x + 3
c = "abc" + "adc"
d = c + c + c
b = b + b + b;
x = b - 9;
dump
```

# Why did you create this abomination of a project?
I was looking at the source code for Lua and I thought that I could do something similar. I also wanted to work on my C++ because it had been years since I had used it.

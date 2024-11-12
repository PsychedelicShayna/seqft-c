# Sequential Formula Transform in C

This is is a recreational implementation of sequential formula transform in pure C that I've kept stored locally for a while now.
I was in the process of implementing variable length operators in the development branch, before other things took priority.
The master branch is based on a working commit.

I've taken the liberty of adding support for different bases via prefixes, e.g. the expression `20 * 0x0F ^ (0b0010 / 0o07) % 16 / 12.0` is a valid expression.
There's also rudimentary support for unary functions, like `floor()` and `ceil()`

Negative values are represented via `~` as opposed to `-` which is strictly for subtracting.

Feel free to browse the source, and view the development branch; this is a toy project I haven't worked on in a while.

The inspiration to write this came from [How the stack got stacked](https://www.youtube.com/watch?v=2vBVvQTTdXg) by Kay Lack.

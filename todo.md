# Todo
-------------------------------------------------------------------------------

- Prefix support for binary operators.
  * Paves the way for multi-argument functions.

- Variable length operators.
  * e.g. `>>`, `<<`, `&&`, `||`, `**`

- Bitwise Operators
  * (requires variable length operators)
  * `^` power of needs to be replaced with `**`
  * `~` negate needs to be `!` for bitwise.
  * Default datatype needs to be int, double
    only when providing a FPN.

- Mathematical constants
  * Pi
  * C / Speed of Light
  * Euler's Constant
  * etc

- Hashmap or BTree/Trie Implementation for function lookup.

- Meta Commands
  - !help, for documentation
  - !load, to load dynamically linked libs
  - !base, to change base display mode.
  - !precision, to change float precision.
  - !clear, to clear the screen.
  - .. etc

- Keyword Support

- Variable Support (needs keyword support)
  * Needs keyword support.
  * Needs hashmap or BTree/Trie implementation.

- User defined functions (needs kkeyword support)
    * Optionally, C functions defined in dynamically linked libraries
      could also be imported, which could provide a hashmap or lookup
      table to add additional functions into the global scope.

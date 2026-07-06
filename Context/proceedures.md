# procedures.md

## Purpose of the Document

In this file describes the development process and its ivariants

## Audience

- human developer
 
 - AI

# Author

 - AI (contrinuosly maintained)


## Content

- All source, including build scripts, designdocument  is  version controlled.

- Development does not occur in the master branch, only reintegrations allowed

- Development occures in features branches 

- Before reintegration coding rules must be enforced

- The master branch is always warning free and checked against static analysis and runs its unit test OK

- The unit test coverage shall be at least 80% measured on the source. 
  Note: Uncovered Exceotion paths 
  and unreachable code ned not be tested. Traoff effort versus gain-.

- 

## Toolset

### C++

  - g++15.2, std=c++23, no libs except standard lib, strct adherence to the c++ standard
    - compiler switches: default +switches for modules
    - relase -O2
    - debug -O0 

  - unit testing: doctest

  - coverage: gcov 

  - static analysis: likely sonarcube (panned not yet)

  - coding style: clang-format file will be provides
    dense, 2 space ident, doc coments 

  - source code commenting  doxygen style line comments 

  - headers use #pragma once not header guards

  - language features:

    - no runtime polymorphism in production code (-use mixins=

    - code for first maintaibility/extendability then performance 

    - keep, classes small, seperate conccerns

    - it us a non goal to achieve binary or source backwards compatibility


### Haskell

  - ghc 9.10.3
  - unit testing
  
  23  23  
  g++15.2  23  
 
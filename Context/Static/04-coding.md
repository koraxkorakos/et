# coding.md

## Toolchain

 - gcc 15.2 (GNU toolchain)
 - cmake 4.2.3
 - ninja 1.13.2 
 - doctest
 - clangformat 21
 - doxygen 1.15.0 
 - plantuml 1.2020.02
 - graphviz 14.1.2
 - ghc 9.10.3

### Out of Source Documentation

Placed close to documented entity.

- Markdown + Extensions
- Plantuml
- extensions for latex equations

Global Manual latex (?) (open question?)

## Build scripts

- keep build scripts simple
- avoid guess work and fallbacks for tool detections, instead fail early and hard
- provide sensible diagnostics if you fail

- pull in external dependencies like `doctest` in via an "external project. 
  i.e. download and install it in the build directory.
  Never take it from the Linux platform, because version ma not match or build switches be 
  incompatible.

## C++

### Language dialect

- C++23
- indiviual C++26 features may be opted in see below
- no compiler extensions

### Libraries

#### Production

- C++ standard library only
- doctest: in test code only

#### Test & Auxiliary

- C++ standard library 
- doctest: 2.5.3

## Coding Style

- adhere to C++23 Standard, but may use C++26 features (like pack indexing) 
  These C++26 features must individually be approved in this file 
- avoid RTTI and dynamic dispatch (virtual functions) an, virtual inheritance
- avoid platform dependencies, like size(long)
- avoid heap allocation if possible
- prefer compile time algorithms over runtime
- follow C++ Core Guidelines where you can, if you must deviate document it with a rationale.

### Code Quality

  - warning free compilation
  - warning suppression allowed an a case per case review in code marked with review date and 
    reviewer 
  - TBD choose static analysis tool, get warning free analysis with same rules for suppression as
    for compiler warnings
  - coverage: TBD use gcov?

### Source Formatting

  - snake case, as C++ std library also for templates and types
  - template type and template parameters and concepts star with upper case
  - MARCOS all capital (avoid macros if possible), 
  - use "#pragma once" for headers if headers exist, (but C++ modules preferred, 
    but might not be mature enough on the compiler
  - clang-format config file provided TBD
  - clang format as a commit hook
  - allow clang-format except sections from clangformat
  - 120 characters per line recommended,
     recommended 80 characters per line, then 40 chars for line comment,
     exceptions for otherwise hard to read lines (r.g. template in metaprogrammings)

### In Source Formatting Documentation

 - clangformat 21 (on commit hook, use no-clang-format section to suppress locally)
 - doxygen 1.15.0 (syntax) 
 - plantuml 1.2020.02
 - graphviz 14.1.2
 
  Guideline:   
     - keep comments complementary to source (DRY)
     - don't overly document 
     - Doxygen style with backslash as tag (\brief)
       - no stream comments (/*...*/)
       - brief to the right side of prototypes

## Haskell


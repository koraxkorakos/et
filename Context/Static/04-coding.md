# coding.md


The C++ code must

- strictly adhere to C++ Standard (no compiler extensions)
- adhere to C++23 Standard, but may use C++26 features (like pack indexing) 
  These C++26 features must individually be approved in this file 
- compile without warnings (reviewed and approved suppressions allowed)
- The build scripts 



# Platform:

 - gcc 15.2
 - cmake 4.2.3
 - ninja 1.13.2
 
 Source:
   - snake case, as C++ std library also for templates and types,
   - template type and template parameters and concepts star with upper case
   - MARCOS all capital (if needed), 
   - use "#pragma once" for headers if headers exist, C++ modules prefered, 
     but might not be mature enough on the compiler
   - clang-format provided
   - format as a commit hook
   - allow except sections from clangformat
   - 120 characters per line recommended,
     recommended 80 characters per line, then 40 chars for line comment,
     exceptions for otherwise hard to read lines (r.g. template in metaprogrammings)
   
 
 Documentation:
 
  Textfiles close to documented entity:
   
   - extended markdown, with plantuml
   
   Source comments:
   
     - keep comments complementary to source (DRY)
     - don't overly document 
     - Doxygen style with backslah as tag (\brief)
       - no stream comments (/*...*/)
       - brief to the right side of prototypes
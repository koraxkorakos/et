# roles.md

## Purpose of the document

This document describes the roles in the project team. The teams consists of human and Ai members,
both of which can take different roles in the development process at different times.
The roles will be used in requests to the AI, e.g. "As a DEVOPS perform the following task" to 
constrain the AI.

- PO product-owner: states the project goals in an informal way

- RE requirement-engineer: derives requirements from the project goals

- AR architect: sketches coarse system design and formulates proceedures to meet the functional and non 
            functional requirements 

- implementers

  - CI C++-implementer: designs and implements C++ code and documents design decisions linked to 
    architecture and requirements

  - HI Haskell-implementer: implements Haskell code and documents design decisions

- VER verificator: writes unit and module test against the documentation of the implementation

- VAL validator: examines for deviations from specification and procedures

- DEVOPS devops: maintains and automates the build and test tool chain chain, compiler, static analysis
          and prepares anbd other support tools 

## Audience

- AI

# Author

- Human developer
# Specification.md

## Pupose of the Document

In this file contains the steps of the current development activity should be listed. Each step 
requires an acknowledgement by the human user, unless the human user. The HUD can acknowledge 
individually or a range of steps.

## Audience
 
 - AI

# Author

 - HUD
 - AID (only if approved by HUD)

# Non functional Requirements

Priority (top first):

  - type safety
  - syntax readability (maintainability)
  - modularity (pay only for what is used)
  - performance (runtime and memory)
  - small code size

# Functional Requirements

  - At least 64 vector and 6 Multivector dimensions supported per default.
    The number of dimensions is a compiletime constant and may be altered by 
    a configure time define.
    
  - Operations 
    at least:
	- unary +,-
	- binary +,-,*
	- outer product (fully degenerate metric (Grassmann product))
	- inner products any metric, 
	- commuator and anticommutator
	- scalar product (scalar is the multi vectir with only index 0 populated)
	- intersection, spanning
	- any diagonal metric (signature)
	
	
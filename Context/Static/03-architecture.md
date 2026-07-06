# 02-architecture.md

The library has two layers:

  - Infrastructure
  - Algebra
  
 ## Infrastructure Layer
 
 consists of the following modules: 
 
   - Utility general helpers
   - CTS Indexset
   - Expression Template Framework
 
 ## Algebra Layer

Conceptially all vectors are sparse maximum dimensional. where the maximum 
dimension is a compile time constant. All algebraic (multi-)vectors are embedded
within this space.

The metric is passed as in a context during assignment and in the usual case
taken from the assigned variables type at expression template evaluation..

Extends algebraic structures in consecutive steps (modules).

Pattern; Leaky Abstraction

Idioms:
   - CTS containers (vectors, multivectors)
   - Expression Templates (for (multi-)vectors
     - passing a context on assignment
     
```plantuml
 @startuml
folder Infrstructure {
  [Expression Template] -up-> [Utilities]  : uses
  [Indexset] -up-> [Utilities]  : uses
}

folder Algebra {
[Metric] -up-> [Utilities]  : uses
[CTS Array] -up-> [Indexset]  : uses
[CTS Additive Group] -up-> [Expression Template]  : uses
[CTS Additive Group] -up-> [CTS Array]  : extends
[CTS Vector Space] -up-> [CTS Additive Group]  : extends
[CTS Multivector] -right-> [Metric]  : contains
[CTS Multivector] -up-> [CTS Vector Space]  : extends
[CTS Grassmann Algebra] -up-> [CTS Multivector]  : extends
[CTS Orthogonal Geometric Algebra] -up-> [CTS Grassmann Algebra]  : extends
[CTS Symplectic Geometric Algebra] -up-> [CTS Multivector]  : extends
}
@enduml
```

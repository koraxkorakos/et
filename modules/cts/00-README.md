# Module Structure

## Directory Structure

    cts/
    ├── 00-README.md                               # this file
    ├── cts.cppm                                   # Primary module
    ├── utilities/
    │   ├── expression_templates.ixx               # Expression templates
    │   ├── special_values.ixx                     # compile time special values for 0, 1, -1
    │   └── utilities.ixx                          # Base utilities
    ├── algebra/
    │   ├── metric.ixx                             # Metric tensor
    │   ├── array.ixx                              # CTS Array
    │   ├── additive_group.ixx                     # Additive Group
    │   ├── vector_space.ixx                       # Vector Space
    │   ├── multivector.ixx                        # Multivector
    │   ├── grassmann.ixx                          # Grassmann Algebra
    │   ├── orthogonal_ga.ixx                      # Orthogonal Geometric Algebra
    │   └── symplectic_ga.ixx                      # Symplectic Geometric Algebra
    └── valueset/
        └── valueset.ixx                           # Indexset (separate folder)

## Architecture

```plantuml
 @startuml
folder Infrstructure {
  [Expression Template] -up-> [Utilities]  : uses
  [Valueset] -up-> [Utilities]  : uses
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

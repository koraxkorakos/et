# Design of the compile-time sparse algebra system

## Purpose

This library represents algebraic values whose possible nonzero components are
known from their types. Typical examples are sparse vectors and multivectors.
The representation is intended to provide three things at once:

- convenient mathematical notation for building expressions;
- compile-time knowledge of which coefficients may be present; and
- evaluation that visits only those coefficients which can contribute.

The central difficulty is that notation alone does not always determine its
meaning. Multiplication can mean scalar multiplication, an exterior product,
or a metric-dependent geometric product. The library therefore separates the
description of an expression from the algebra used to interpret it.

## Expressions initially have syntax, not algebraic meaning

An expression such as

```text
a + reverse(b * c)
```

is first stored as a small tree describing only its written structure. At this
stage the tree knows that there is an addition, a multiplication, and a
reversal, but it does not decide what multiplication means or which
coefficients the result may contain.

This generic expression layer is deliberately simple. It owns its operands so
that temporary values remain valid, and it does not contain rules about
vectors, multivectors, metrics, or blade indices.

This separation is important because the same written expression can be
meaningful in different algebras. Choosing an interpretation too early would
make expressions less reusable and would unnecessarily couple ordinary
arithmetic notation to one algebra.

## The sink selects the semantic context

The algebra is selected as late as possible: when an expression is used to
construct or assign its destination, called the *sink*.

A sink belongs to a context such as:

- an additive group;
- a vector space;
- a Grassmann algebra; or
- an orthogonal geometric algebra with a particular metric.

The context interprets the generic expression recursively and produces a
semantic expression tree. Each node in this second tree knows both how to
compute a coefficient and which indices may be populated.

Conceptually, construction follows this sequence:

```text
generic expression + sink context
                 ↓
       semantic expression tree
                 ↓
         compile-time support set
                 ↓
       concrete sparse destination
```

The destination family supplies the context, while construction-time type
deduction supplies its index set and coefficient type. A user can therefore
name the desired algebraic kind without manually spelling out the result's
indices.

An already existing sink has a fixed type and therefore a fixed support set.
Assignment to it is intentionally strict: the semantic expression must have
the same structural support. The library must not silently discard components
which the user may have expected to store.

## Structural support is not numerical support

The most important invariant concerns the meaning of an index set.

For a value `x`, its compile-time index set describes coefficients which are
structurally present and may carry information. It does not claim that every
such coefficient is numerically nonzero at every evaluation.

The implication is one-way:

```text
index is unpopulated  ⇒  its coefficient is identically zero
```

The converse is false:

```text
coefficient is currently zero  ⇏  its index is unpopulated
```

For example, corresponding coefficients of `a - b` may cancel when `a` and
`b` happen to have equal values. That runtime coincidence does not remove the
coefficient from the type of the expression. At another evaluation the two
values may differ.

The distinction is even clearer when a coefficient is a function of time. A
time series may evaluate to zero at one instant and to a nonzero value at
another. The coefficient remains structurally present throughout the series.

Thus the index set is a conservative statement about possible or meaningful
coefficients. It may include coefficients whose current value is zero. An
index may be removed only when the coefficient is known to be identically zero
for every possible evaluation.

This is also a semantic distinction, not merely a performance device. A zero
scalar, a zero vector, a zero point, and an absent multivector blade can have
the same numerical representation while belonging to different mathematical
objects. This is loosely analogous to the distinction between zero metres and
zero seconds: numerical equality does not erase meaning.

## Structural zero

The library represents a coefficient at an unpopulated index with a special
structural-zero value, `zero_type`.

`zero_type` means more than “the result of this evaluation happened to be
zero.” It certifies that the coefficient is absent and identically zero under
the current structural interpretation.

This distinction supports both optimization and safety:

- calculations involving absent coefficients can be removed at compile time;
- a readable sparse value can still be queried at any index;
- an absent coefficient cannot accidentally be treated as writable storage;
- an assignment cannot silently route a meaningful coefficient into an
  absent destination component.

A populated coefficient, in contrast, has the ordinary coefficient type. It
may contain zero at runtime without becoming `zero_type`.

## Values and sinks have different rules

A *value* is readable. Reading any index is valid: populated indices return an
ordinary coefficient, and unpopulated indices return structural zero.

A *sink* is writable. Only its populated indices are writable. Writing an
unpopulated index is a type error because no storage and no semantic component
exist there.

A *variable* is both a value and a sink. It can be read everywhere under the
zero convention, but it can be written only at its declared populated
indices.

This asymmetry is intentional. Universal reading makes sparse algebra easy to
express, while restricted writing protects the representation.

## Projection and representation conversion

Projection is an explicit, read-only value adapter. It gives a value a
requested structural support set.

Projection may narrow support. Coefficients outside the requested set then
become structurally absent. This is the explicit operation used when the user
really intends to discard components.

Projection may also widen support. A source index which was structurally absent
becomes a populated coefficient holding an ordinary runtime zero. This does
not violate the support invariant: populated coefficients are allowed to be
zero.

Widening is useful when converting to dense vectors, matrices, serialization
formats, or external numerical libraries which require a fixed layout. For
example, a sparse value with support `{1, 3}` may be projected to
`{0, 1, 2, 3}`. Indices `0` and `2` then contain ordinary coefficient zeros,
not structural zeros.

Projection is restricted to values. It does not create writable aliases for
storage which was absent in the source, so it cannot weaken the sink contract.

## Strict assignment

When both the source expression and destination sink have known support sets,
assignment requires them to agree.

In particular, assigning an expression with a populated index to a sink where
that index is absent is an error. Silently dropping the coefficient would hide
a likely mistake.

If support conversion is intentional, it must be visible in the expression by
using projection first. The projected value then has an honest type describing
the representation which will be assigned.

This rule makes convenient construction and strict safety compatible:

- during construction, the sink context and expression determine the correct
  support automatically;
- during assignment to an existing fully specified sink, mismatched support is
  rejected;
- deliberate representation changes are expressed by an explicit projection.

## Context hierarchy and support propagation

Contexts form a hierarchy in which each level adds operations to the previous
one.

### Array context

The array context provides storage and evaluation over a known support set. It
does not introduce algebraic operations. Evaluation iterates over the
destination's compile-time indices and computes one coefficient for each.

### Additive-group context

Addition and subtraction use the union of their operands' support sets. A
coefficient may cancel numerically, but it remains structurally possible.

Unary plus and unary minus preserve the support of their operand.

### Vector-space context

The vector-space context adds multiplication by a scalar. A scalar is
represented as a multivector whose only possible index is `0`.

The accepted products are:

- scalar times multivector;
- multivector times scalar; and
- scalar times scalar.

A general multivector product is not assigned vector-space semantics because
that operation requires a richer algebra.

### Grassmann context

The Grassmann context adds the exterior product. Blade indices are represented
as bit sets. For input blades `m` and `p`:

- they contribute only when `m` and `p` do not share a basis vector;
- the result blade is `m xor p`; and
- the sign is determined by the parity of the permutation needed to put the
  combined basis vectors into canonical order.

Reversal preserves the support set and changes coefficient signs according to
blade grade.

### Orthogonal geometric-algebra context

An orthogonal geometric-algebra context additionally carries a diagonal
metric. Its geometric product allows overlapping basis vectors. For each pair
of input blades it combines:

- the canonical reordering sign;
- the metric factors belonging to repeated basis vectors; and
- the two input coefficients.

The exterior product is the part without repeated basis vectors. The inner
product selects the contributions required by the chosen contraction
convention. The geometric product contains both exterior and metric-dependent
contributions.

## Product coefficients

Products are the most computationally demanding expressions. For an output
blade `n`, its coefficient is a sum over contributing input-blade pairs:

```text
coefficient(n) =
    Σ sign(m, p)
      · metric_factor(m, p)
      · coefficient_left(m)
      · coefficient_right(p)
```

where the sum contains only pairs satisfying:

```text
m xor p = n
```

For the exterior product, pairs whose basis sets overlap
(`m ∩ p ≠ ∅`, represented by a nonzero bitwise intersection) are excluded.
For a geometric product they remain and receive the corresponding metric
factor.

The support set and contributing pairs are determined at compile time. Runtime
evaluation then performs only coefficient arithmetic. A future optimization
can precompute a contribution table grouped by output index, avoiding repeated
search through the Cartesian product of the operand supports.

## Two-stage evaluation

Evaluation deliberately occurs in two stages.

First, the selected context transforms the generic syntax tree into a semantic
CTS tree. During this transformation every node computes its structural
support and records how one output coefficient is obtained from its children.

Second, the destination iterates over the root support set. For each populated
index `n`, it requests `get<n>` from the semantic tree and stores the resulting
ordinary coefficient.

This organization keeps algebraic reasoning at compile time while leaving
only the necessary numeric work for runtime. It also allows the same generic
expression to be interpreted by different contexts without changing how the
expression was originally written.

## Summary of invariants

The design relies on the following rules:

1. Generic expressions describe syntax and do not choose an algebra.
2. A sink family selects the semantic context at construction or assignment.
3. An unpopulated index is guaranteed to be identically zero.
4. A populated index may evaluate to zero and remains populated when it does.
5. Structural zero is different from an ordinary coefficient containing zero.
6. Values are readable at every index; sinks are writable only at populated
   indices.
7. Assignment to an existing sink requires matching structural support.
8. Projection is the explicit operation for changing value support.
9. Projection may narrow or widen a value, but it never creates writable
   source storage.
10. Product evaluation sums only structurally contributing blade pairs.

Together these decisions make the interface concise without sacrificing the
mathematical distinctions or compile-time safety on which sparse evaluation
depends.

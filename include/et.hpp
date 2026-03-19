#pragma once

// FILE IS A SKETCH, NOT COMPILEABLE, NOT TESTED, NOT DOCUMENTED, NOT REVIEWED


#include <bitset>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <concepts>

/// \brief namespace for all code of the project
namespace et {

    constexpr std::size_t max_dim = 64;
    using dims_t = std::bitset<max_dim>;

    // AST -> ET -> Evaluation

    // Phase 1: construct just the AST (generic at construction time)
    // Phase 2: evaluation of the AST passing a context as parameter creates the expression template (DSL,
    //          explicit or via assignment or construction of result)
    // Phase 3: evaluation of the ET copies to the result (copy constructor or assignment operator)


// Benefits of this approach:
// - clean separation of concerns: we can separate the parsing and optimization of the AST from the evaluation of
// -  the ET, which makes it easier to maintain and extend the code
// - we can use the same AST for different evaluation contexts (e.g. different backends)
// - we can use the same AST for different result types (e.g. scalar, vector, matrix)
// - we can use the same AST for different operations (e.g. addition, multiplication, etc.)
// - we can use the same AST for different dimensions (e.g. 1D, 2D, 3D, etc.)
// - we can use the same AST for different data types (e.g. float, double, int, etc.)
// - we can use the same AST for different evaluation strategies (e.g. eager, lazy, etc.)
// - we can use the same AST for different optimization strategies (e.g. loop unrolling, vectorization, etc.)
// - we can use the same AST for different code generation strategies (e.g. JIT, AOT, etc.)
// - we can use the same AST for different debugging strategies (e.g. print the AST, print the ET, etc.)
// - AST is a tree, ET is a DAG (directed acyclic graph) -> we can share subexpressions in the ET (e.g. common
//   subexpression elimination)
// - AST is very lightweight, we can use it for parsing and optimization, ET is heavier (e.g. it can contain references
//   to the original AST nodes, it can contain additional information for optimization, etc.)
//   - we thus avoid referencing temporaries in the AST, which can lead to dangling references, we can use the AST for
//     parsing and optimization, and then we can use the ET for evaluation and code generation
//   - we still can have shred references at the AST level, but have to use proxies explicitly. like
//   - `std::reference_wrapper` or `std::shared_ptr`, which makes it clear that we are referencing a temporary and
//      thus we have to be careful with it.
// - ET is heavier (e.g. it can contain references to the original AST nodes, it can contain additional information for
//   optimization, etc.)

// Disadvantages of this approach:
// - we have to maintain two different representations of the same expression (AST and ET)
// - we have to implement the same operations for both AST and ET (e.g. addition, multiplication, etc.)
// - we have to choose, where to implement the same optimizations for AST and ET or both (e.g. common subexpression
// - elimination, loop unrolling, optimizations, etc.)
// - we  pay for higher genericity with higher complexity (e.g. we have to implement the same operations for both AST

// As a demonstration, we implement a simple expression template for addition of compile time sparse vectors and
// multiplication with scalars, i.e. the vector space operations. We limit the maximum to keep the compile time
// representation of sparsity simple. We do not provide division, as it is not a vector space operation, but it could
// be implemented as well later. Another reason for not providing division is that, when generalizing from vector spaces
// to Grassmann algebras or even Clifford or Tensor algebras, non commutativity of the multiplication would require to
// distinguish between left and right division, which would further complicate the implementation and does not map to
// the operations provided by C++.  We also do not provide unary plus and minus, as they are not really necessary, but
// they could be implemented as well later.
struct plus_op;
struct minus_op;
struct multiplies_op;
// struct divides_op;
struct uminus_op;
struct uplus_op;

template <typename T>
concept Expression = requires(T t) { 
    typename T::type; 
    { t.eval()}
};

template <typename op, typename... Exprs>
struct expression
{
    using type = expression;
    
};

    template<typename op, typename... Exprs>
    expression(op, Exprs &&) -> expression<op, std::forward<Exprs>(Exprs)...>;
 
}

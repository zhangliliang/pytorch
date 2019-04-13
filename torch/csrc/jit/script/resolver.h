#pragma once

#include <aten/src/ATen/core/jit_type.h>
#include <torch/csrc/jit/script/sugared_value.h>

namespace torch {
namespace jit {
namespace script {

struct Resolver;
using ResolverPtr = std::shared_ptr<Resolver>;

/**
 * class Resolver
 *
 * Represents an "outer environment" in which we an look up names and return
 * a corresponding SugaredValue. This is used during compilation to resolve
 * references to names which are not defined internal to the graph.
 *
 * Example: PythonResolver looks at the enclosing Python scope for `name`.
 */
struct Resolver {
  virtual ~Resolver() {}

  // Resolve a given name to a SugaredValue. This takes the method `m` that the
  // caller is currently constructing, since we may need to insert nodes into
  // the graph to create a value.
  virtual std::shared_ptr<SugaredValue> resolveValue(
      const std::string& name,
      Function& m,
      const SourceRange& loc) const = 0;
};

// A resolver that only understands "torch.foo()" lookups.
struct NativeResolver : public Resolver {
  std::shared_ptr<SugaredValue> resolveValue(
      const std::string& name,
      Function& m,
      const SourceRange& loc) const override {
    if (name == "torch") {
      return std::make_shared<BuiltinModule>("aten");
    }
    return nullptr;
  }
};
} // namespace script
} // namespace jit
} // namespace torch

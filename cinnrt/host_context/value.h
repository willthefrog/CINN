#pragma once
#include <glog/logging.h>

#include <llvm/ADT/SmallVector.h>
#include <utility>
#include <variant>
#include <vector>
#include "cinn/common/object.h"
#include "cinn/common/shared.h"
#include "cinnrt/host_context/function.h"
#include "cinnrt/tensor/dense_host_tensor.h"
#include "cinnrt/tensor/dense_tensor_view.h"
#include "cinnrt/tensor/tensor_shape.h"

namespace cinnrt {
namespace host_context {

struct MlirFunctionExecutable;

using ValueVariantType = std::variant<int16_t,
                                      int32_t,
                                      int64_t,
                                      float,
                                      double,
                                      bool,
                                      tensor::TensorShape,
                                      tensor::DenseHostTensor,
                                      MlirFunctionExecutable*,
                                      std::vector<int16_t>,
                                      std::vector<int32_t>,
                                      std::vector<int64_t>,
                                      std::vector<float>,
                                      std::vector<double>>;

//! Copy content from \param from to \param to.
void CopyTo(const Value& from, Value* to);

/**
 * Represents any data type for value in host context.
 */
class Value : public cinn::common::Object {
 public:
  using variant_type = ValueVariantType;

  explicit Value() {}  // NOLINT
  explicit Value(int32_t x) : data(x) {}
  explicit Value(int64_t x) : data(x) {}
  explicit Value(float x) : data(x) {}
  explicit Value(double x) : data(x) {}
  explicit Value(bool x) : data(x) {}
  explicit Value(std::vector<int16_t>&& x) : data(x) {}
  explicit Value(std::vector<int32_t>&& x) : data(x) {}
  explicit Value(std::vector<int64_t>&& x) : data(x) {}
  explicit Value(std::vector<float>&& x) : data(x) {}
  explicit Value(std::vector<double>&& x) : data(x) {}
  explicit Value(tensor::TensorShape&& x) : data(std::move(x)) {}
  explicit Value(tensor::DenseHostTensor&& x) : data(std::move(x)) {}
  explicit Value(MlirFunctionExecutable* x) : data(x) {}

  template <typename T>
  const T& get() const {
    return std::get<T>(data);
  }
  template <typename T>
  T& get() {
    return std::get<T>(data);
  }

  template <typename T>
  void set(T&& v) {
    data = std::move(v);
  }

  bool valid() const { return data.index() != std::variant_npos; }

  const char* type_info() const override;

  friend void CopyTo(const Value& from, Value* to);

 private:
  ValueVariantType data;
  static constexpr const char* __type_info__ = "host_context_value";
};

/**
 * Represents a counted reference of a Value.
 */
class ValueRef : cinn::common::Shared<Value> {
 public:
  ValueRef() = default;
  explicit ValueRef(Value* n) : cinn::common::Shared<Value>(n) {}
  explicit ValueRef(int32_t val);
  explicit ValueRef(int64_t val);
  explicit ValueRef(float val);
  explicit ValueRef(double val);
  explicit ValueRef(bool val);

  using cinn::common::Shared<Value>::get;
  using cinn::common::Shared<Value>::Reset;
  using cinn::common::Shared<Value>::operator->;
  using cinn::common::Shared<Value>::operator*;
  //! Get a readonly data.
  template <typename T>
  const T& get() const {
    CHECK(p_);
    return p_->get<T>();
  }

  template <typename T>
  T& get() {
    CHECK(p_);
    return p_->get<T>();
  }

  //! Assign a data.
  template <typename T>
  void Assign(const T& x) {
    if (!p_) {
      p_ = cinn::common::make_shared<Value>();
    }
    *p_ = x;
  }

  template <typename T, typename... Args>
  void Assign(Args... args) {
    p_ = cinn::common::make_shared<T>(std::forward<Args>(args)...);
  }

  inline bool IsValid() { return p_; }
};

}  // namespace host_context
}  // namespace cinnrt

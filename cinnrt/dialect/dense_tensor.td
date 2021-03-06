#ifdef DT_OPS
#else
#define DT_OPS

include "cinnrt/dialect/cinn_base.td"
include "cinnrt/dialect/tensor_shape_base.td"
include "mlir/Interfaces/SideEffectInterfaces.td"

def DT_Dialect : Dialect {
  let name = "dt";

  let description = [{
      The DenseTensor dialect.
  }];

  let cppNamespace = "::cinnrt::dt";
}

class DT_Op<string mnemonic, list<OpTrait> traits = []> :
      Op<DT_Dialect, mnemonic, traits>;

class CreateUninitTensorOp<string dtype>
      : DT_Op<"create_uninit_tensor." # dtype, [NoSideEffect]> {
  let summary = "dt.create_uninit_tensor operation";

  let description = [{
      An operation that creates an uninitialized tensor.
  }];

  let arguments = (ins I64ArrayAttr:$shape);
  let results = (outs TensorType:$output);

  let parser  = [{ return cinnrt::dt::parseCreateUninitTensorOp(parser, result); }];
  let printer = [{ return cinnrt::dt::printCreateUninitTensorOp(p, *this); }];
}

class FillTensorWithConstantOp<string dtype> :
      DT_Op<"fill_tensor_with_constant." # dtype> {
  let summary = "dt.fill_tensor_with_constant operation";

  let description = [{
      An operation that fills an input tensor with a value.
  }];

  let arguments = (ins
      TensorType:$input,
      AnyAttr:$value
  );
  let results = (outs);

  // TODO: can be removed?
  //let parser  = [{ return cinnrt::dt::parseFillTensorWithConstantOp(parser, result); }];
  //let printer = [{ return cinnrt::dt::printFillTensorWithConstantOp(p, *this); }];
  let assemblyFormat = "`(` $input `:` type($input) `)`  attr-dict";
}

def PrintTensorOp : DT_Op<"print_tensor"> {
  let summary = "dt.print_tensor operation";

  let description = [{
    An operation that prints a tensor.
  }];

  let arguments = (ins TensorType:$input);
  let results = (outs);
  let assemblyFormat = "`(` $input `:` type($input) `)` attr-dict";
}

class SetTensorOp<string dtype> :
      DT_Op<"set_tensor_with_constant_values." # dtype> {
  let summary = "dt.set_tensor_with_constant_values operation";

  let description = [{
    An operation that sets an input tensor with given values.
  }];

  let arguments = (ins TensorType);
  let results = (outs);

  let parser  = [{ return cinnrt::dt::parseSetTensorOp(parser, result); }];
  let printer = [{ return cinnrt::dt::printSetTensorOp(p, *this); }];
}

def GetTensorShapeOp : DT_Op<"get_tensor_shape", [NoSideEffect]> {
  let summary = "dt.get_tensor_shape operation";

  let description = [{
      An operation that returns the shape of the input tensor.
  }];

  let arguments = (ins TensorType:$input);
  let results = (outs TS_Shape:$output);
  let assemblyFormat = "$input attr-dict `:` type($input) `->` type($output)";
}

foreach dtype = ["ui8", "ui16", "ui32", "ui64", "i32", "f32", "f64", "i64"] in {
  def DT_CreateUninitTensorOp_#dtype : CreateUninitTensorOp<dtype>;
  def DT_FillTensorOp_#dtype : FillTensorWithConstantOp<dtype>;
  def DT_SetTensorOp_#dtype : SetTensorOp<dtype>;
}

#endif  // DT_OPS

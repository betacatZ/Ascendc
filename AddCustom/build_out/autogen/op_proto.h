#ifndef OP_PROTO_H_
#define OP_PROTO_H_

#include "graph/operator_hiai_reg.h"
namespace hiai {

HIAI_REG_OP(AddCustom)
.HIAI_INPUT(x, TensorType({ALL}))
.HIAI_INPUT(y, TensorType({ALL}))
.HIAI_OUTPUT(z, TensorType({ALL}))
.HIAI_REQUIRED_ATTR(bias, AttrValue::INT)
.HIAI_OP_END()

}

#endif

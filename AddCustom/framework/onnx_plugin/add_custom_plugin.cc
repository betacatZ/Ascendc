#include <cstdint>
#include <cstddef>
#include "register/register.h"
namespace domi {
    REGISTER_CUSTOM_OP("AddCustom")
        .FrameworkType(ONNX)
        .OriginOpType("AddCustom")
        .ParseParamsByOperatorFn(AutoMappingByOpFn); // 用来注册解析算子属性的函数
}
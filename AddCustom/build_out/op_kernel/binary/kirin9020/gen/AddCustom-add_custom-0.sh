#!/bin/bash
echo "[Kirin9020] Generating AddCustom_23f3ef9de2394be0170f41b041981312 ..."
export PYTHONPATH=$PYTHONPATH:/home/zdm/code/Ascendc/tool/DDK-tools-next-6.0.1.0/tools/tools_ascendc/../../tools/platform/kirin9020/ops/impl
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/zdm/code/Ascendc/tool/DDK-tools-next-6.0.1.0/tools/tools_ascendc/../../tools/platform/kirin9020/lib64:/home/zdm/code/Ascendc/tool/DDK-tools-next-6.0.1.0/tools/tools_ascendc/../../tools/platform/kirin9020/simulator
debug_dir=$(dirname $2)/debug
opc $1 --main_func=add_custom --input_param=/home/zdm/code/Ascendc/AddCustom/AddCustom/build_out/op_kernel/binary/kirin9020/gen/AddCustom_23f3ef9de2394be0170f41b041981312_param.json --soc_version=Kirin9020 --output=$2 --impl_mode="" --simplified_key_mode=0 --op_mode=dynamic --debug_dir=$debug_dir --op_debug_config=dump_cce
if ! test -f $2/AddCustom_23f3ef9de2394be0170f41b041981312.json ; then
  echo "$2/AddCustom_23f3ef9de2394be0170f41b041981312.json not generated!"
  exit 1
fi

if ! test -f $2/AddCustom_23f3ef9de2394be0170f41b041981312.o ; then
  echo "$2/AddCustom_23f3ef9de2394be0170f41b041981312.o not generated!"
  exit 1
fi
echo "[Kirin9020] Generating AddCustom_23f3ef9de2394be0170f41b041981312 Done"

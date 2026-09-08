/* Copyright (C) 2020-2021. Huawei Technologies Co., Ltd. All
rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License Version 2.0.
 * You may not use this file except in compliance with the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Apache License for more details at
 * http://www.apache.org/licenses/LICENSE-2.0
 */


#include "kernel_operator.h"
#include "lib/hilog.h"

using namespace AscendC;
constexpr int32_t BUFFER_NUM = 2;
constexpr int32_t TILE_NUM = 8;

// 定义 hilog domain 和 tag
#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0xD001100
#define LOG_TAG "AddCustomKernel"

class KernelAdd {
public:
    __aicore__ inline KernelAdd() {}
    __aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z, uint32_t totalLength, uint32_t tileNum)
    {
        this->blockLength = totalLength / AscendC::GetBlockNum();
        this->tileNum = tileNum;
        this->tileLength = this->blockLength / tileNum / BUFFER_NUM;

        // 使用 hilog 打印
        OH_LOG_INFO(LOG_APP, "[op_kernel::KernelAdd::Init] BlockIdx=%d, BlockNum=%d", AscendC::GetBlockIdx(), AscendC::GetBlockNum());
        OH_LOG_INFO(LOG_APP, "[op_kernel::KernelAdd::Init] totalLength=%u, blockLength=%u", totalLength, this->blockLength);
        OH_LOG_INFO(LOG_APP, "[op_kernel::KernelAdd::Init] tileNum=%u, tileLength=%u", this->tileNum, this->tileLength);

        xGm.SetGlobalBuffer((__gm__ DTYPE_X *)x + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
        yGm.SetGlobalBuffer((__gm__ DTYPE_Y *)y + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
        zGm.SetGlobalBuffer((__gm__ DTYPE_Z *)z + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
        pipe.InitBuffer(inQueueX, BUFFER_NUM, this->tileLength * sizeof(DTYPE_X));
        pipe.InitBuffer(inQueueY, BUFFER_NUM, this->tileLength * sizeof(DTYPE_Y));
        pipe.InitBuffer(outQueueZ, BUFFER_NUM, this->tileLength * sizeof(DTYPE_Z));
    }
    __aicore__ inline void Process()
    {
        int32_t loopCount = this->tileNum * BUFFER_NUM;
        OH_LOG_INFO(LOG_APP, "[op_kernel::KernelAdd::Process] Starting, loopCount=%d", loopCount);
        for (int32_t i = 0; i < loopCount; i++) {
            CopyIn(i);
            Compute(i);
            CopyOut(i);
        }
        OH_LOG_INFO(LOG_APP, "[op_kernel::KernelAdd::Process] Completed");
    }

private:
    __aicore__ inline void CopyIn(int32_t progress)
    {
        AscendC::LocalTensor<DTYPE_X> xLocal = inQueueX.AllocTensor<DTYPE_X>();
        AscendC::LocalTensor<DTYPE_Y> yLocal = inQueueY.AllocTensor<DTYPE_Y>();
        AscendC::DataCopy(xLocal, xGm[progress * this->tileLength], this->tileLength);
        AscendC::DataCopy(yLocal, yGm[progress * this->tileLength], this->tileLength);
        inQueueX.EnQue(xLocal);
        inQueueY.EnQue(yLocal);
    }
    __aicore__ inline void Compute(int32_t progress)
    {
        AscendC::LocalTensor<DTYPE_X> xLocal = inQueueX.DeQue<DTYPE_X>();
        AscendC::LocalTensor<DTYPE_Y> yLocal = inQueueY.DeQue<DTYPE_Y>();
        AscendC::LocalTensor<DTYPE_Z> zLocal = outQueueZ.AllocTensor<DTYPE_Z>();

        OH_LOG_DEBUG(LOG_APP, "[op_kernel::KernelAdd::Compute] Iteration %d, tileLength=%u", progress, this->tileLength);

        AscendC::Add(zLocal, xLocal, yLocal, this->tileLength);
        DumpTensor(xLocal, 0, 32);
        DumpTensor(yLocal, 1, 32);
        DumpTensor(zLocal, 2, 32);
        outQueueZ.EnQue<DTYPE_Z>(zLocal);
        inQueueX.FreeTensor(xLocal);
        inQueueY.FreeTensor(yLocal);
    }
    __aicore__ inline void CopyOut(int32_t progress)
    {
        AscendC::LocalTensor<DTYPE_Z> zLocal = outQueueZ.DeQue<DTYPE_Z>();
        AscendC::DataCopy(zGm[progress * this->tileLength], zLocal, this->tileLength);
        outQueueZ.FreeTensor(zLocal);
    }

private:
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::QuePosition::VECIN, BUFFER_NUM> inQueueX, inQueueY;
    AscendC::TQue<AscendC::QuePosition::VECOUT, BUFFER_NUM> outQueueZ;
    AscendC::GlobalTensor<DTYPE_X> xGm;
    AscendC::GlobalTensor<DTYPE_Y> yGm;
    AscendC::GlobalTensor<DTYPE_Z> zGm;
    uint32_t blockLength;
    uint32_t tileNum;
    uint32_t tileLength;
};

extern "C" __global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z,
                                                 GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tiling_data, tiling);

    OH_LOG_INFO(LOG_APP, "========================================");
    OH_LOG_INFO(LOG_APP, "[op_kernel::add_custom] Kernel started");
    OH_LOG_INFO(LOG_APP, "[op_kernel::add_custom] bias=%d, size=%u", tiling_data.bias, tiling_data.size);
    OH_LOG_INFO(LOG_APP, "========================================");

    KernelAdd op;
    op.Init(x, y, z, tiling_data.size, TILE_NUM);
    op.Process();

    OH_LOG_INFO(LOG_APP, "========================================");
    OH_LOG_INFO(LOG_APP, "[op_kernel::add_custom] Kernel completed successfully");
    OH_LOG_INFO(LOG_APP, "========================================");
}
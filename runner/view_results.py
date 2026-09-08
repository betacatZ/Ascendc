#!/usr/bin/env python3
"""
AddCustom Runner 结果查看和验证脚本
读取 fp16 二进制文件并显示数据
"""

import numpy as np
import sys
import os
from pathlib import Path

def read_fp16_bin(file_path):
    """读取 fp16 二进制文件"""
    if not os.path.exists(file_path):
        print(f"Error: File not found: {file_path}")
        return None

    data = np.fromfile(file_path, dtype=np.float16)
    return data

def print_array_info(name, data):
    """打印数组的基本信息"""
    if data is None:
        return

    print(f"\n{'='*60}")
    print(f"{name}")
    print(f"{'='*60}")
    print(f"Shape: {data.shape}")
    print(f"Total elements: {data.size}")
    print(f"Data type: {data.dtype}")
    print(f"Min value: {data.min():.6f}")
    print(f"Max value: {data.max():.6f}")
    print(f"Mean value: {data.mean():.6f}")
    print(f"Std deviation: {data.std():.6f}")

    # 显示前 10 个元素
    print(f"\nFirst 10 elements:")
    print(data[:10])

    # 显示后 10 个元素
    if data.size > 10:
        print(f"\nLast 10 elements:")
        print(data[-10:])

def verify_add_custom(x, y, z, bias=0):
    """验证 AddCustom 算子的结果
    假设 AddCustom 实现的是: z = x + y + bias
    """
    print(f"\n{'='*60}")
    print("Verification: z = x + y (+ bias)")
    print(f"{'='*60}")

    if x is None or y is None or z is None:
        print("Error: Cannot verify, missing data")
        return

    if x.shape != y.shape or x.shape != z.shape:
        print(f"Warning: Shape mismatch!")
        print(f"  x.shape = {x.shape}")
        print(f"  y.shape = {y.shape}")
        print(f"  z.shape = {z.shape}")
        return

    # 计算期望结果
    expected = x + y + bias

    # 计算差异
    diff = np.abs(z - expected)

    print(f"\nBias parameter: {bias}")
    print(f"Max absolute error: {diff.max():.6e}")
    print(f"Mean absolute error: {diff.mean():.6e}")
    print(f"RMS error: {np.sqrt((diff**2).mean()):.6e}")

    # 检查相对误差
    relative_error = diff / (np.abs(expected) + 1e-8)
    print(f"Max relative error: {relative_error.max():.6e}")
    print(f"Mean relative error: {relative_error.mean():.6e}")

    # 统计匹配情况
    tolerance = 1e-3  # fp16 精度约为 1e-3
    matches = np.sum(diff < tolerance)
    match_rate = matches / z.size * 100
    print(f"\nElements within tolerance ({tolerance}): {matches}/{z.size} ({match_rate:.2f}%)")

    if match_rate > 99.9:
        print("\n✓ Verification PASSED!")
    elif match_rate > 95:
        print("\n⚠ Verification PARTIAL (most elements match)")
    else:
        print("\n✗ Verification FAILED")
        print("\nShowing first 10 mismatches:")
        mismatch_indices = np.where(diff >= tolerance)[0][:10]
        for idx in mismatch_indices:
            print(f"  [{idx}] x={x[idx]:.6f}, y={y[idx]:.6f}, "
                  f"expected={expected[idx]:.6f}, got={z[idx]:.6f}, "
                  f"error={diff[idx]:.6f}")

def main():
    # 默认结果目录
    script_dir = Path(__file__).parent
    results_dir = script_dir / "results"

    # 支持命令行参数指定目录
    if len(sys.argv) > 1:
        results_dir = Path(sys.argv[1])

    print(f"Reading results from: {results_dir}")

    # 读取三个二进制文件
    x_file = results_dir / "add_custom_x.bin"
    y_file = results_dir / "add_custom_y.bin"
    z_file = results_dir / "add_custom_z.bin"

    x = read_fp16_bin(x_file)
    y = read_fp16_bin(y_file)
    z = read_fp16_bin(z_file)

    # 显示每个数组的信息
    print_array_info("Input X", x)
    print_array_info("Input Y", y)
    print_array_info("Output Z", z)

    # 验证结果
    # 如果你的 AddCustom 算子有 bias 参数，可以在这里指定
    bias = 0  # 根据实际情况修改
    verify_add_custom(x, y, z, bias)

    print(f"\n{'='*60}")
    print("Done!")
    print(f"{'='*60}\n")

if __name__ == "__main__":
    main()

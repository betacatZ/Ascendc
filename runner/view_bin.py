#!/usr/bin/env python3
"""
快速查看 fp16 bin 文件内容
"""

import numpy as np
import sys

def view_bin(file_path, num_elements=20):
    """查看 bin 文件的内容"""
    try:
        data = np.fromfile(file_path, dtype=np.float16)
        print(f"\n文件: {file_path}")
        print(f"总元素数: {data.size}")
        print(f"形状: {data.shape}")
        print(f"数据类型: {data.dtype}")
        print(f"最小值: {data.min():.6f}")
        print(f"最大值: {data.max():.6f}")
        print(f"平均值: {data.mean():.6f}")
        print(f"\n前 {min(num_elements, data.size)} 个元素:")
        print(data[:num_elements])

        if data.size > num_elements:
            print(f"\n后 {min(num_elements, data.size)} 个元素:")
            print(data[-num_elements:])
    except Exception as e:
        print(f"错误: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("用法: python3 view_bin.py <bin文件路径> [显示元素数]")
        print("示例: python3 view_bin.py results/add_custom_x.bin 30")
        sys.exit(1)

    file_path = sys.argv[1]
    num_elements = int(sys.argv[2]) if len(sys.argv) > 2 else 20
    view_bin(file_path, num_elements)

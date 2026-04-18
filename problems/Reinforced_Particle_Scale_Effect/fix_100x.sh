#!/bin/bash

# ================= 核心配置区 =================
# 1. 自动获取脚本所在的绝对路径
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# 2. 定位 MOOSE 可执行文件 (向上退两层)
MOOSE_APP="$SCRIPT_DIR/../../cdf_update-opt"

# 3. 运行参数
CORES=12
MAX_CONCURRENT=10

METHODS=("Upwind" "DG")
MORPHOLOGIES=("morphology_I" "morphology_II")

# 4. 100x 专属硬编码物理参数
FIX_SCALE="100x"
FIX_WIDTH="0.34640001"
FIX_DISP="0.002"  # 100x 对应的位移
INIT_I="3.06e7"
INIT_II="5.0e7"

# ==========================================

echo "=== 启动 100x 专项并发计算 (绝对路径 & 精确容差模式) ==="
echo "脚本目录: $SCRIPT_DIR"
echo "程序位置: $MOOSE_APP"

# 预查可执行文件是否存在
if [ ! -f "$MOOSE_APP" ]; then
    echo "错误：找不到可执行文件 $MOOSE_APP"
    echo "请检查层级关系是否正确。"
    exit 1
fi

for method in "${METHODS[@]}"; do
    # 确定母版路径
    if [ "$method" == "Upwind" ]; then
        MASTER_I="$SCRIPT_DIR/templates/master_upwind.i"
    else
        MASTER_I="$SCRIPT_DIR/templates/master_dg.i"
    fi

    for morph in "${MORPHOLOGIES[@]}"; do
        CURRENT_INIT=$([ "$morph" == "morphology_I" ] && echo "$INIT_I" || echo "$INIT_II")
        
        # 结果输出根目录
        RESULT_ROOT="$SCRIPT_DIR/results/${method}/${morph}"
        mkdir -p "$RESULT_ROOT"

        # 进入形态文件夹
        cd "$SCRIPT_DIR/$morph" || { echo "跳过: 找不到文件夹 $morph"; continue; }

        # 寻找 100x inp 文件
        MESH_FILE=$(ls *_${FIX_SCALE}.inp 2>/dev/null | head -n 1)

        if [[ -n "$MESH_FILE" ]]; then
            BASE_NAME=$(basename "$MESH_FILE" .inp)
            LOG_FILE="${RESULT_ROOT}/${BASE_NAME}_fixed.log"
            
            # 向量格式化：最外层双引号，内部空格（MOOSE 标准解析格式）
            DISP_VEC="0.0 $FIX_DISP 0.0"
            TRANS_VEC="$FIX_WIDTH 0 0"
            
            echo "[提交] $method | $morph | 100x | 宽度: $FIX_WIDTH"

            # 核心执行命令：
            # 注意：此处周期性边界块名使用 x_dir，请确保模板文件 master_*.i 中对应的块也叫 [x_dir]
            mpiexec -n $CORES "$MOOSE_APP" -i "$MASTER_I" \
                Mesh/read/file="$SCRIPT_DIR/$morph/$MESH_FILE" \
                Variables/rho_edge_pos_1/initial_condition="$CURRENT_INIT" \
                Variables/rho_edge_neg_1/initial_condition="$CURRENT_INIT" \
                Functions/disp_load/y="$DISP_VEC" \
                BCs/Periodic/auto_boundary_x/translation="$TRANS_VEC" \
                BCs/Periodic/auto_boundary_y/translation="$TRANS_VEC" \
                BCs/Periodic/auto_rho_edge_pos_boundary_x_1/translation="$TRANS_VEC" \
                BCs/Periodic/auto_rho_edge_neg_boundary_x_1/translation="$TRANS_VEC" \
                Outputs/file_base="${RESULT_ROOT}/${BASE_NAME}" \
                > "$LOG_FILE" 2>&1 &
            
            # 并发控制
            while [ $(jobs -p | wc -l) -ge $MAX_CONCURRENT ]; do
                wait -n
            done
        fi
        # 返回主目录，准备下一个循环
        cd "$SCRIPT_DIR"
    done
done

wait
echo "=== 100x 专项计算任务已全部完成！ ==="
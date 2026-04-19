#!/bin/bash

# ================= 核心配置区 =================
# 1. 自动获取脚本所在的绝对路径
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# 2. 定位 MOOSE 可执行文件 (向上退两层)
MOOSE_APP="$SCRIPT_DIR/../../cdf_update-opt"

# 3. 运行参数
CORES=12
MAX_CONCURRENT=10

METHODS=("DG")
MORPHOLOGIES=("morphology_I" "morphology_II")
SCALES=("2x" "5x" "10x" "100x" "1000x")

# 4. 物理参数
INIT_I="3.06e7"
INIT_II="5.0e7"
BASE_DISP=0.00002
BASE_TRANS_X=0.003464

# ==========================================

echo "=== 启动并发批量计算 (绝对路径模式) ==="
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

        for scale_str in "${SCALES[@]}"; do
            scale_num=$(echo $scale_str | tr -d 'x')
            
            # 物理量缩放计算
            current_disp=$(awk -v b="$BASE_DISP" -v s="$scale_num" 'BEGIN {printf "%.8f", b * s}')
            CURRENT_Y="'0.0 $current_disp 0.0'"
            current_trans_x=$(awk -v b="$BASE_TRANS_X" -v s="$scale_num" 'BEGIN {printf "%.6e", b * s}')
            CURRENT_TRANS_STR="'$current_trans_x 0.0 0.0'"

            # 寻找 inp 文件
            MESH_FILE=$(ls *_${scale_str}.inp 2>/dev/null | head -n 1)

            if [[ -n "$MESH_FILE" ]]; then
                BASE_NAME=$(basename "$MESH_FILE" .inp)
                LOG_FILE="${RESULT_ROOT}/${BASE_NAME}.log"
                
                echo "[提交] $method | $morph | $scale_str"

                # 核心执行命令：全部采用绝对路径
                mpiexec -n $CORES "$MOOSE_APP" -i "$MASTER_I" \
                    Mesh/read/file="$SCRIPT_DIR/$morph/$MESH_FILE" \
                    Variables/rho_edge_pos_1/initial_condition="$CURRENT_INIT" \
                    Variables/rho_edge_neg_1/initial_condition="$CURRENT_INIT" \
                    Functions/disp_load/y="$CURRENT_Y" \
                    BCs/Periodic/auto_boundary_x/translation="$CURRENT_TRANS_STR" \
                    BCs/Periodic/auto_boundary_y/translation="$CURRENT_TRANS_STR" \
                    BCs/Periodic/auto_rho_edge_pos_boundary_x_1/translation="$CURRENT_TRANS_STR" \
                    BCs/Periodic/auto_rho_edge_neg_boundary_x_1/translation="$CURRENT_TRANS_STR" \
                    Outputs/file_base="${RESULT_ROOT}/${BASE_NAME}" \
                    > "$LOG_FILE" 2>&1 &
                
                # 并发控制
                while [ $(jobs -p | wc -l) -ge $MAX_CONCURRENT ]; do
                    wait -n
                done
            fi
        done
        cd "$SCRIPT_DIR"
    done
done

wait
echo "=== 所有计算任务已完成！ ==="

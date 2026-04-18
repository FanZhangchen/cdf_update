#!/bin/bash

# ================= 核心配置 (与主脚本一致) =================
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MOOSE_APP="$SCRIPT_DIR/../../cdf_update-opt"

CORES=12
MAX_CONCURRENT=4   # 因为主脚本占了一半，这里我们可以开小一点，或者也开 10
METHODS=("Upwind" "DG")
MORPHOLOGIES=("morphology_I" "morphology_II")
FIX_SCALE="100x"   # 只盯着 100x 跑

INIT_I="3.06e7"
INIT_II="5.0e7"
BASE_DISP=0.00002
BASE_TRANS_X=0.003464
# =========================================================

echo "=== 启动 100x 专项修复任务 (容差模式: 1e-6) ==="

for method in "${METHODS[@]}"; do
    MASTER_I="$SCRIPT_DIR/templates/master_$([ "$method" == "Upwind" ] && echo "upwind" || echo "dg").i"

    for morph in "${MORPHOLOGIES[@]}"; do
        CURRENT_INIT=$([ "$morph" == "morphology_I" ] && echo "$INIT_I" || echo "$INIT_II")
        RESULT_ROOT="$SCRIPT_DIR/results/${method}/${morph}"
        mkdir -p "$RESULT_ROOT"

        # 进入形态文件夹
        cd "$SCRIPT_DIR/$morph" || continue

        # 1. 计算 100x 对应的数值
        scale_num=100
        current_disp=$(awk -v b="$BASE_DISP" -v s="$scale_num" 'BEGIN {printf "%.8f", b * s}')
        current_trans_x=$(awk -v b="$BASE_TRANS_X" -v s="$scale_num" 'BEGIN {printf "%.12e", b * s}')
        
        # 2. 寻找 100x 的网格
        MESH_FILE=$(ls *_${FIX_SCALE}.inp 2>/dev/null | head -n 1)

        if [[ -n "$MESH_FILE" ]]; then
            BASE_NAME=$(basename "$MESH_FILE" .inp)
            LOG_FILE="${RESULT_ROOT}/${BASE_NAME}_fixed.log"

            # 并发控制：等待空位
            while [ $(jobs -p | wc -l) -ge $MAX_CONCURRENT ]; do
                sleep 2
            done

            echo "[补丁提交] $method | $morph | 100x"

            # 核心执行：增加 search_tolerance=1e-6
            mpiexec -n $CORES "$MOOSE_APP" -i "$MASTER_I" \
                Mesh/read/file="$SCRIPT_DIR/$morph/$MESH_FILE" \
                Variables/rho_edge_pos_1/initial_condition="$CURRENT_INIT" \
                Variables/rho_edge_neg_1/initial_condition="$CURRENT_INIT" \
                Functions/disp_load/y="'0.0 $current_disp 0.0'" \
                BCs/Periodic/auto_boundary_x/translation="'$current_trans_x 0.0 0.0'" \
                BCs/Periodic/auto_boundary_x/search_tolerance=1e-6 \
                BCs/Periodic/auto_boundary_y/translation="'$current_trans_x 0.0 0.0'" \
                BCs/Periodic/auto_boundary_y/search_tolerance=1e-6 \
                BCs/Periodic/auto_rho_edge_pos_boundary_x_1/translation="'$current_trans_x 0.0 0.0'" \
                BCs/Periodic/auto_rho_edge_pos_boundary_x_1/search_tolerance=1e-6 \
                BCs/Periodic/auto_rho_edge_neg_boundary_x_1/translation="'$current_trans_x 0.0 0.0'" \
                BCs/Periodic/auto_rho_edge_neg_boundary_x_1/search_tolerance=1e-6 \
                Outputs/file_base="${RESULT_ROOT}/${BASE_NAME}" \
                > "$LOG_FILE" 2>&1 &
        fi
        cd "$SCRIPT_DIR"
    done
done

wait
echo "=== 100x 修复任务全部完成！ ==="
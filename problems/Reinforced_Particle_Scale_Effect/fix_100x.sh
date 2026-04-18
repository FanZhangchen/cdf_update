#!/bin/bash

# ================= 核心配置 =================
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MOOSE_APP="$SCRIPT_DIR/../../cdf_update-opt"

CORES=12
METHODS=("Upwind" "DG")
MORPHOLOGIES=("morphology_I" "morphology_II")

# 手动锁定 100x 的精确值
FIX_WIDTH="0.34640001"
FIX_DISP="0.002"  # 0.00002 * 100

# ==========================================

for method in "${METHODS[@]}"; do
    MASTER_I="$SCRIPT_DIR/templates/master_$([ "$method" == "Upwind" ] && echo "upwind" || echo "dg").i"
    
    for morph in "${MORPHOLOGIES[@]}"; do
        INIT=$([ "$morph" == "morphology_I" ] && echo "3.06e7" || echo "6.0e7")
        OUT_ROOT="$SCRIPT_DIR/results/${method}/${morph}"
        mkdir -p "$OUT_ROOT"

        # 寻找 100x 网格的绝对路径
        MESH_FILE=$(ls "$SCRIPT_DIR/$morph"/*_100x.inp 2>/dev/null | head -n 1)

        if [[ -n "$MESH_FILE" ]]; then
            BASE_NAME=$(basename "$MESH_FILE" .inp)
            LOG_FILE="${OUT_ROOT}/${BASE_NAME}_fixed.log"

            echo "[100x 专项提交] $method | $morph | 宽度: $FIX_WIDTH"

            # 核心改进点：
            # 1. 使用逗号分隔向量 (X,Y,Z)，绝对不带空格，不带额外单引号
            # 2. translation 直接使用 0.34640001
            # 3. 容差放宽到 1e-4

            mpiexec -n $CORES "$MOOSE_APP" -i "$MASTER_I" \
                Mesh/read/file="$MESH_FILE" \
                Variables/rho_edge_pos_1/initial_condition="$INIT" \
                Variables/rho_edge_neg_1/initial_condition="$INIT" \
                Functions/disp_load/y="0.0,$FIX_DISP,0.0" \
                BCs/Periodic/auto_boundary_x/translation="$FIX_WIDTH,0,0" \
                BCs/Periodic/auto_boundary_x/search_tolerance=1e-4 \
                BCs/Periodic/auto_boundary_y/translation="$FIX_WIDTH,0,0" \
                BCs/Periodic/auto_boundary_y/search_tolerance=1e-4 \
                BCs/Periodic/auto_rho_edge_pos_boundary_x_1/translation="$FIX_WIDTH,0,0" \
                BCs/Periodic/auto_rho_edge_pos_boundary_x_1/search_tolerance=1e-4 \
                BCs/Periodic/auto_rho_edge_neg_boundary_x_1/translation="$FIX_WIDTH,0,0" \
                BCs/Periodic/auto_rho_edge_neg_boundary_x_1/search_tolerance=1e-4 \
                Outputs/file_base="${OUT_ROOT}/${BASE_NAME}" \
                > "$LOG_FILE" 2>&1 &
        fi
    done
done

wait
echo "=== 100x 任务已全部补齐！ ==="
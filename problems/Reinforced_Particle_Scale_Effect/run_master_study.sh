#!/bin/bash

# ================= 配置区 =================
MOOSE_APP="../../../cdf_update-opt"
CORES=12
MAX_CONCURRENT=10  # 最大并发任务数 (10个任务 * 12核 = 120核)

METHODS=("Upwind" "DG")
MORPHOLOGIES=("morphology_I" "morphology_II")
SCALES=("2x" "5x" "10x" "100x" "1000x")

INIT_I="3.06e7"
INIT_II="5.0e7"

BASE_DISP=0.00002
BASE_TRANS_X=0.003464
# ==========================================

echo "=== 启动并发批量计算 (最大并发数: $MAX_CONCURRENT) ==="

# 定义一个计数器，记录已提交的后台任务数
running_jobs=0

for method in "${METHODS[@]}"; do
    if [ "$method" == "Upwind" ]; then
        MASTER_I="./templates/master_upwind.i"
    else
        MASTER_I="./templates/master_dg.i"
    fi

    for morph in "${MORPHOLOGIES[@]}"; do
        if [ "$morph" == "morphology_I" ]; then
            CURRENT_INIT=$INIT_I
        else
            CURRENT_INIT=$INIT_II
        fi

        OUT_DIR="./results/${method}/${morph}"
        mkdir -p "$OUT_DIR"

        for scale_str in "${SCALES[@]}"; do
            scale_num=$(echo $scale_str | tr -d 'x')
            
            current_disp=$(awk -v b="$BASE_DISP" -v s="$scale_num" 'BEGIN {printf "%.8f", b * s}')
            CURRENT_Y="'0.0 $current_disp 0.0'"
            
            current_trans_x=$(awk -v b="$BASE_TRANS_X" -v s="$scale_num" 'BEGIN {printf "%.6e", b * s}')
            CURRENT_TRANS_STR="'$current_trans_x 0.0 0.0'"

            MESH_FILE=$(ls ./${morph}/*_${scale_str}.inp 2>/dev/null)

            if [[ -f "$MESH_FILE" ]]; then
                BASE_NAME=$(basename "$MESH_FILE" .inp)
                LOG_FILE="${OUT_DIR}/${BASE_NAME}.log"
                
                echo "提交任务 -> [$method] [$morph] 尺度: ${scale_str}"

                # 【并发核心修改】：在结尾加上 & 让任务进入后台
                mpiexec -n $CORES $MOOSE_APP -i "$MASTER_I" \
                    Mesh/read/file="$MESH_FILE" \
                    Variables/rho_edge_pos_1/initial_condition="$CURRENT_INIT" \
                    Variables/rho_edge_neg_1/initial_condition="$CURRENT_INIT" \
                    Functions/disp_load/y="$CURRENT_Y" \
                    BCs/Periodic/auto_boundary_x/translation="$CURRENT_TRANS_STR" \
                    BCs/Periodic/auto_boundary_y/translation="$CURRENT_TRANS_STR" \
                    BCs/Periodic/auto_rho_edge_pos_boundary_x_1/translation="$CURRENT_TRANS_STR" \
                    BCs/Periodic/auto_rho_edge_neg_boundary_x_1/translation="$CURRENT_TRANS_STR" \
                    Outputs/file_base="${OUT_DIR}/${BASE_NAME}" \
                    > "$LOG_FILE" 2>&1 &
                
                # 获取当前后台正在运行的任务数
                # jobs -p 打印后台进程PID，wc -l 统计行数
                running_jobs=$(jobs -p | wc -l)

                # 如果当前运行的任务数达到了设定的上限
                if (( running_jobs >= MAX_CONCURRENT )); then
                    # wait -n 会暂停脚本循环，直到任意一个后台任务完成
                    wait -n
                fi
            else
                echo "[跳过] 未找到网格: ${morph} 下的 ${scale_str} 文件"
            fi
        done
    done
done

# 循环结束后，等待最后几批还在后台运行的任务全部完成
echo "所有任务已分配完毕，正在等待最后一批任务计算结束..."
wait

echo "=== 全部并发计算任务完美收官！ ==="
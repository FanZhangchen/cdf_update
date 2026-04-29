#!/bin/bash

# ================= 核心配置区 =================
# 1. 自动获取脚本所在的绝对路径
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# 2. 定位 MOOSE 可执行文件 (向上退两层)
MOOSE_APP="$SCRIPT_DIR/../../cdf_update-opt"

# 3. 运行参数
CORES=12
MAX_CONCURRENT=10

METHODS=("Upwind")
MORPHOLOGIES=("morphology_I" "morphology_II")
SCALES=("2x" "5x" "10x" "100x" "1000x")

# 4. 物理参数
INIT_I="3.06e7"
INIT_II="5.0e7"
BASE_DISP=0.00002
BASE_TRANS_X=0.003464

# ==========================================

echo "=== 启动并发批量计算 (动态源项 + 精度守护模式) ==="
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
        MASTER_I="$SCRIPT_DIR/templates/master_upwind_source.i"
    else
        MASTER_I="$SCRIPT_DIR/templates/master_dg_source.i"
    fi

    for morph in "${MORPHOLOGIES[@]}"; do
        # 动态分配初始条件和位错源项
        CURRENT_INIT=$([ "$morph" == "morphology_I" ] && echo "$INIT_I" || echo "$INIT_II")
        CURRENT_SOURCE=$([ "$morph" == "morphology_I" ] && echo "$INIT_I" || echo "$INIT_II")
        
        # 结果输出根目录
        RESULT_ROOT="$SCRIPT_DIR/results_with_source/${method}/${morph}"
        mkdir -p "$RESULT_ROOT"

        # 进入形态文件夹
        cd "$SCRIPT_DIR/$morph" || { echo "跳过: 找不到文件夹 $morph"; continue; }

        for scale_str in "${SCALES[@]}"; do
            scale_num=$(echo $scale_str | tr -d 'x')
            
            # 物理量缩放计算 (位移)
            current_disp=$(awk -v b="$BASE_DISP" -v s="$scale_num" 'BEGIN {printf "%.8f", b * s}')
            # 使用双引号+空格的标准向量格式，杜绝解析错误
            CURRENT_Y="0.0 $current_disp 0.0"
            
            # ---------------- 核心精度守护 ----------------
            # 如果是 100x，强行使用带 1 的精确值，否则正常缩放
            if [ "$scale_str" == "100x" ]; then
                current_trans_x="0.34640001"
            else
                current_trans_x=$(awk -v b="$BASE_TRANS_X" -v s="$scale_num" 'BEGIN {printf "%.6e", b * s}')
            fi
            # 同样使用标准向量格式
            CURRENT_TRANS_STR="$current_trans_x 0 0"
            # ----------------------------------------------

            # 寻找 inp 文件
            MESH_FILE=$(ls *_${scale_str}.inp 2>/dev/null | head -n 1)

            if [[ -n "$MESH_FILE" ]]; then
                BASE_NAME=$(basename "$MESH_FILE" .inp)
                LOG_FILE="${RESULT_ROOT}/${BASE_NAME}.log"
                
                echo "[提交] $method | $morph | $scale_str | Source: $CURRENT_SOURCE | Width: $current_trans_x"

                # 核心执行命令：全部采用绝对路径，注入 Materials 覆盖参数
                mpiexec -n $CORES "$MOOSE_APP" -i "$MASTER_I" \
                    Mesh/read/file="$SCRIPT_DIR/$morph/$MESH_FILE" \
                    Variables/rho_edge_pos_1/initial_condition="$CURRENT_INIT" \
                    Variables/rho_edge_neg_1/initial_condition="$CURRENT_INIT" \
                    Materials/ssd/dislo_source_edge="$CURRENT_SOURCE" \
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

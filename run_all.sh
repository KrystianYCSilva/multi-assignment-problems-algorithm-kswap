#!/bin/bash
#
# run_all.sh - Executa todos os modos para todos os tamanhos e restricoes.
#
# Uso:
#   bash run_all.sh                  # Executa tudo (4 modos x 6 tamanhos x 8 restricoes)
#   bash run_all.sh --modes "mapa mapa-vnd"       # Apenas modos especificos
#   bash run_all.sh --sizes "N25 N30"              # Apenas tamanhos especificos
#   bash run_all.sh --constraints "1 2 3"          # Apenas restricoes especificas
#   bash run_all.sh --max-files 10                 # Limitar instancias por combinacao
#   bash run_all.sh --iterations 3                 # Criterio de parada
#   bash run_all.sh --dry-run                      # Mostra comandos sem executar
#   bash run_all.sh --parallel 4                   # Maximo de processos paralelos
#
set -e

BIN="./build/mapa_kswap.exe"
NSP_BASE="./files"
CONSTRAINT_DIR="./files/casos-1-8"
RESULT_BASE="./resultados"
LOG_DIR="./logs"

ALL_MODES="mapa mapa-vnd vns vns-vnd"
ALL_SIZES="N25 N30 N50 N60 N75 N100"

MODES=""
SIZES=""
CONSTRAINTS=""
MAX_FILES=""
ITERATIONS="5"
DRY_RUN=false
PARALLEL=1

while [ $# -gt 0 ]; do
    case "$1" in
        --modes)       MODES="$2"; shift 2 ;;
        --sizes)       SIZES="$2"; shift 2 ;;
        --constraints) CONSTRAINTS="$2"; shift 2 ;;
        --max-files)   MAX_FILES="$2"; shift 2 ;;
        --iterations)  ITERATIONS="$2"; shift 2 ;;
        --dry-run)     DRY_RUN=true; shift ;;
        --parallel)    PARALLEL="$2"; shift 2 ;;
        --help)
            head -14 "$0" | tail -12
            exit 0
            ;;
        *) echo "Opcao desconhecida: $1"; exit 1 ;;
    esac
done

[ -z "$MODES" ] && MODES="$ALL_MODES"
[ -z "$SIZES" ] && SIZES="$ALL_SIZES"

get_nurses() {
    case "$1" in
        N25)  echo 25 ;;
        N30)  echo 30 ;;
        N50)  echo 50 ;;
        N60)  echo 60 ;;
        N75)  echo 75 ;;
        N100) echo 100 ;;
    esac
}

get_days() {
    case "$1" in
        N25|N50|N75|N100) echo 7 ;;
        N30|N60)          echo 28 ;;
    esac
}

get_constraints() {
    case "$1" in
        N25|N50|N75|N100) echo "1 2 3 4 5 6 7 8" ;;
        N30|N60)          echo "9 10 11 12 13 14 15 16" ;;
    esac
}

get_max_files() {
    case "$1" in
        N25|N50|N75|N100) echo 7290 ;;
        N30|N60)          echo 960 ;;
    esac
}

if [ ! -f "$BIN" ]; then
    echo "Binario nao encontrado. Compilando..."
    bash build.sh
fi

mkdir -p "$LOG_DIR"

TOTAL=0
DONE=0
FAILED=0
SKIPPED=0
PIDS=()

for size in $SIZES; do
    nurses=$(get_nurses "$size")
    days=$(get_days "$size")
    file_count=${MAX_FILES:-$(get_max_files "$size")}
    cons_list=${CONSTRAINTS:-$(get_constraints "$size")}

    for constraint in $cons_list; do
        for mode in $MODES; do
            TOTAL=$((TOTAL + 1))
        done
    done
done

echo "============================================"
echo "  MAPA NSP Solver - Execucao em Lote"
echo "============================================"
echo "  Modos:       $MODES"
echo "  Tamanhos:    $SIZES"
echo "  Iteracoes:   $ITERATIONS"
echo "  Max files:   ${MAX_FILES:-auto}"
echo "  Paralelo:    $PARALLEL"
echo "  Total jobs:  $TOTAL"
echo "============================================"
echo ""

START_TIME=$(date +%s)

wait_for_slot() {
    while [ ${#PIDS[@]} -ge "$PARALLEL" ]; do
        NEW_PIDS=()
        for pid in "${PIDS[@]}"; do
            if kill -0 "$pid" 2>/dev/null; then
                NEW_PIDS+=("$pid")
            else
                wait "$pid" 2>/dev/null && DONE=$((DONE + 1)) || FAILED=$((FAILED + 1))
            fi
        done
        PIDS=("${NEW_PIDS[@]}")
        [ ${#PIDS[@]} -ge "$PARALLEL" ] && sleep 1
    done
}

run_job() {
    local size=$1 nurses=$2 constraint=$3 mode=$4 file_count=$5
    local result_dir="$RESULT_BASE/$size"
    local nsp_dir="$NSP_BASE/$size"
    local logfile="$LOG_DIR/${size}_c${constraint}_${mode}.log"

    local cmd="$BIN --nurses $nurses --nsp-dir $nsp_dir --constraint-dir $CONSTRAINT_DIR"
    cmd="$cmd --constraint $constraint --max-files $file_count"
    cmd="$cmd --mode $mode --iterations $ITERATIONS"
    cmd="$cmd --result-dir $result_dir"

    if $DRY_RUN; then
        echo "[DRY] $cmd"
        return 0
    fi

    echo "[START] $size | caso=$constraint | modo=$mode | files=$file_count -> $logfile"
    mkdir -p "$result_dir"

    if [ "$PARALLEL" -gt 1 ]; then
        ($cmd > "$logfile" 2>&1) &
        PIDS+=($!)
    else
        local job_start=$(date +%s)
        if $cmd > "$logfile" 2>&1; then
            local job_end=$(date +%s)
            local elapsed=$((job_end - job_start))
            local last_line=$(tail -1 "$logfile" 2>/dev/null)
            DONE=$((DONE + 1))
            echo "[  OK] $size | caso=$constraint | modo=$mode | ${elapsed}s | $last_line"
        else
            local job_end=$(date +%s)
            local elapsed=$((job_end - job_start))
            FAILED=$((FAILED + 1))
            echo "[FAIL] $size | caso=$constraint | modo=$mode | ${elapsed}s | ver $logfile"
        fi
    fi
}

for size in $SIZES; do
    nurses=$(get_nurses "$size")
    days=$(get_days "$size")
    file_count=${MAX_FILES:-$(get_max_files "$size")}
    cons_list=${CONSTRAINTS:-$(get_constraints "$size")}

    echo ""
    echo "--- $size ($nurses enfermeiros, $days dias, $file_count instancias) ---"

    for constraint in $cons_list; do
        for mode in $MODES; do
            wait_for_slot
            run_job "$size" "$nurses" "$constraint" "$mode" "$file_count"
        done
    done
done

for pid in "${PIDS[@]}"; do
    wait "$pid" 2>/dev/null && DONE=$((DONE + 1)) || FAILED=$((FAILED + 1))
done

END_TIME=$(date +%s)
TOTAL_TIME=$((END_TIME - START_TIME))
HOURS=$((TOTAL_TIME / 3600))
MINS=$(( (TOTAL_TIME % 3600) / 60 ))
SECS=$((TOTAL_TIME % 60))

echo ""
echo "============================================"
echo "  Execucao Completa"
echo "============================================"
echo "  Concluidos:  $DONE"
echo "  Falhados:    $FAILED"
echo "  Tempo total: ${HOURS}h ${MINS}m ${SECS}s"
echo "  Resultados:  $RESULT_BASE/"
echo "  Logs:        $LOG_DIR/"
echo "============================================"

[ "$FAILED" -eq 0 ] || exit 1

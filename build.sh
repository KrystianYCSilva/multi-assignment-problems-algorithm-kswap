#!/bin/bash
set -e

GCC="C:/Users/kryst/scoop/apps/gcc/15.2.0/bin/gcc.exe"

if [ ! -f "$GCC" ]; then
    echo "ERROR: gcc not found at $GCC"
    exit 1
fi

CFLAGS="-std=c99 -Wall -Wextra -O2"
INCLUDES="-Isrc"
OUTDIR="build"

mkdir -p "$OUTDIR"

LIB_SOURCES=(
    src/core/schedule.c
    src/core/cost.c
    src/solvers/lap_solver.c
    src/heuristics/constructive.c
    src/heuristics/pcr.c
    src/heuristics/kswap.c
    src/heuristics/vns.c
    src/heuristics/vnd.c
    src/heuristics/shift_relocate.c
    src/heuristics/swap_exchange.c
    src/heuristics/two_opt.c
    src/heuristics/operator_registry.c
    src/io/nsp_reader.c
    src/io/result_writer.c
    src/benchmark/benchmark.c
)

OBJECTS=()
echo "=== Compiling library sources ==="
for src in "${LIB_SOURCES[@]}"; do
    obj="$OUTDIR/$(basename "${src%.c}.o")"
    echo "  CC $src -> $obj"
    "$GCC" $CFLAGS $INCLUDES -c "$src" -o "$obj"
    OBJECTS+=("$obj")
done

echo "=== Compiling main ==="
MAIN_OBJ="$OUTDIR/main.o"
"$GCC" $CFLAGS $INCLUDES -c src/main.c -o "$MAIN_OBJ"

echo "=== Linking mapa_kswap ==="
"$GCC" $CFLAGS "${OBJECTS[@]}" "$MAIN_OBJ" -lm -o "$OUTDIR/mapa_kswap.exe"
echo "Binary: $OUTDIR/mapa_kswap.exe"

if [ "$1" = "--tests" ] || [ "$1" = "-t" ]; then
    echo ""
    echo "=== Compiling tests ==="
    TESTS=(tests/test_schedule.c tests/test_solvers.c tests/test_integration.c)
    for tsrc in "${TESTS[@]}"; do
        tname="$(basename "${tsrc%.c}")"
        echo "  CC $tsrc -> $OUTDIR/$tname.exe"
        "$GCC" $CFLAGS $INCLUDES -Itests "$tsrc" "${OBJECTS[@]}" -lm -o "$OUTDIR/$tname.exe"
    done

    echo ""
    echo "=== Running tests ==="
    PASS=0
    FAIL=0
    for tsrc in "${TESTS[@]}"; do
        tname="$(basename "${tsrc%.c}")"
        echo -n "  RUN $tname ... "
        if "$OUTDIR/$tname.exe"; then
            echo "PASS"
            PASS=$((PASS + 1))
        else
            echo "FAIL"
            FAIL=$((FAIL + 1))
        fi
    done
    echo ""
    echo "Results: $PASS passed, $FAIL failed"
    [ "$FAIL" -eq 0 ] || exit 1
fi

echo "Done."

---
description: |
  Long-term project memory: completed features, releases, hotfixes, and accumulated metrics.
  Use when: reviewing project history, understanding what was built, or onboarding new contributors.
---

# MEMORY.md - Long-Term Project History

> Historico de features, releases e hotfixes do projeto.
> Atualizado quando: feature completa, release, hotfix critico.
> Append-only: nunca apague entradas.

---

## Project

- **Nome:** MAPA NSP Solver (mapa_kswap)
- **Stack:** C99, GCC 15.2.0, Bash (build.sh), NSPLib benchmarks

---

## T0: Regra de Atualizacao (OBRIGATORIA)

SEMPRE atualize este arquivo quando:
1. Feature completa (implementada e testada)
2. Release (versao publicada ou fase concluida)
3. Hotfix critico aplicado
4. Breaking change introduzido

Protocolo:
1. Proponha a atualizacao ao final da implementacao
2. Mostre o diff completo
3. AGUARDE aprovacao antes de escrever
4. NUNCA modifique este arquivo sem aprovacao

---

## Features Implementadas

### F1: Rewrite Completo - Arquitetura Modular
- **Data:** 2026-02-13
- **Arquivos:** Todos os arquivos em `src/` (16 .c, 15 .h), `tests/` (3 .c, 1 .h)
- **Testes:** 176 testes (99 schedule + 13 solvers + 64 integration), todos passando
- **Notas:**
  - Codigo original completamente reescrito: linked-lists substituidas por int** 2D arrays
  - 9 bugs criticos corrigidos do codigo original:
    1. k-swap sobrescrevia incondicionalmente (sem manter melhor)
    2. Condicao impossivel na logica de melhoria
    3. Memory leaks massivos (sem free)
    4. Variaveis nao inicializadas
    5. sizeof errado em calloc
    6. Use-after-free
    7. BAP ausente
    8. VNS ausente
    9. Operacoes backward inconsistentes
  - Arquitetura LEGO-block: cada operador em arquivo separado
  - `NeighbourhoodFn` typedef como interface padrao para todos os operadores
  - `LapSolverFn` function pointer permite trocar Hungarian/APC/BAP em runtime

### F2: Novos Operadores de Busca Local
- **Data:** 2026-02-13
- **Arquivos:** `shift_relocate.h/.c`, `swap_exchange.h/.c`, `two_opt.h/.c`
- **Testes:** Cobertos nos testes de integracao
- **Notas:** 3 novos operadores alem dos originais PCR e kSwap

### F3: VND Engine (Variable Neighbourhood Descent)
- **Data:** 2026-02-13
- **Arquivos:** `vnd.h/.c`
- **Testes:** Testado via modos mapa-vnd e vns-vnd no piloto N25
- **Notas:**
  - 8 vizinhancas: N1:Shift -> N2:Swap -> N3:2-Opt -> N4:kSwap1(PRT) -> N5:kSwap2 -> N6:kSwap3 -> N7:kSwap4 -> N8:PCR
  - Se melhoria em Nk -> reinicia para N1; se nao -> avanca para Nk+1
  - Resultado piloto N25: custo 358 -> 307 (melhoria de 14% vs MAPA classico)

### F4: Operator Registry
- **Data:** 2026-02-13
- **Arquivos:** `operator_registry.h/.c`
- **Testes:** `--list-operators` funcional
- **Notas:** 13 operadores plugaveis registrados

### F5: CLI com 5 Modos de Execucao
- **Data:** 2026-02-13
- **Arquivos:** `main.c`
- **Testes:** Piloto executado em todos os modos
- **Notas:** Flags: `--mode mapa|mapa-vnd|vns|vns-bap|vns-vnd`, `--verbose`, `--list-operators`, `--solver apc|hungarian`

### F6: Build System (build.sh)
- **Data:** 2026-02-13
- **Arquivos:** `build.sh`, `CMakeLists.txt`
- **Testes:** Compilacao e testes OK via `bash build.sh --tests`
- **Notas:**
  - build.sh compila direto com gcc (cmake nao disponivel no ambiente)
  - gcc path: `C:/Users/kryst/scoop/apps/gcc/15.2.0/bin/gcc.exe`
  - Foi necessario extrair `mingw-w64+gcc.7z` manualmente (scoop nao extraiu)
  - Flag `--tests` / `-t` compila e roda os 3 suites de teste

### F7: Benchmark Harness
- **Data:** 2026-02-13
- **Arquivos:** `benchmark.h/.c`
- **Notas:** Harness com export CSV para comparacao de operadores

### F8: Result Writer com auto-criacao de diretorios
- **Data:** 2026-02-13
- **Arquivos:** `result_writer.c`
- **Notas:** Cria automaticamente a arvore de diretorios de saida (resultados/N{nurses}/)

---

## Releases

| Versao | Data | Descricao |
|--------|------|-----------|
| v2.0.0 | 2026-02-13 | Rewrite completo: arquitetura modular, VND, VNS, 5 modos, 13 operadores, 176 testes |

---

## Hotfixes

| Data | Problema | Solucao |
|------|----------|---------|
| 2026-02-13 | gcc cc1.exe ausente (scoop nao extraiu 7z) | Extraido mingw-w64+gcc.7z manualmente com 7z |
| 2026-02-13 | test_full_mapa_pipeline falhava (assertava custo monotonicamente decrescente) | Corrigido para rastrear best cost ao longo do pipeline |
| 2026-02-13 | result_writer falhava quando diretorio nao existia | Adicionado mkdirs() para criar diretorios automaticamente |
| 2026-02-13 | result_dir default fixo em N50 independente de --nurses | Agora ajusta dinamicamente para N{nurses} |

---

## Metricas Acumuladas

- **LOC total (src):** 2229
- **LOC total (tests):** 456
- **Testes:** 176 (99 + 13 + 64)
- **Cobertura:** todos os modulos core, solvers, heuristics, io testados
- **Arquivos fonte:** 16 .c + 15 .h
- **Operadores registrados:** 13
- **Instancias NSPLib:** N25 (7290), N30 (960), N50 (7290), N60, N75, N100 + 16 restricoes

---

## Ambiente de Desenvolvimento

- **OS:** Windows (Git Bash)
- **Compilador:** GCC 15.2.0 via scoop (`C:/Users/kryst/scoop/apps/gcc/15.2.0/bin/gcc.exe`)
- **Build:** `bash build.sh` (sem cmake/make)
- **IDE:** CLion (LSP clangd mostra erros falsos de sysroot - ignorar)
- **Projeto:** `C:\Users\kryst\CLionProjects\mapa_kswap`

---

## Decisoes Tecnicas

1. **int** em vez de double** para schedules (enfermeiros x dias) - shifts sao inteiros pequenos
2. **LapSolverFn** function pointer para abstrair solver LAP - permite trocar em runtime
3. **NeighbourhoodFn** typedef para interface padrao de operadores
4. **build.sh** em vez de Makefile/CMake - cmake nao instalado, make nao disponivel
5. **VND com kSwap como vizinhanca** - kSwap(PRT) pode piorar custo temporariamente (by design do paper), VND mantem best

---

## Proximos Passos Potenciais

- Atualizar testes de integracao para exercitar novos operadores (shift, swap, 2-opt) individualmente
- Benchmark comparativo entre modos (mapa vs mapa-vnd vs vns-vnd) em multiplas instancias
- Paralelizacao do pipeline com OpenMP
- Integrar kSwap k=3 e k=4 backwards no registry
- Adicionar time limit como criterio de parada alternativo

---

*Ultima atualizacao: 2026-02-13.*

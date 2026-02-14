# MAPA - Multi-Assignment Problem-based Algorithm for Nurse Scheduling

Algoritmo heuristico baseado em procedimentos de multi-atribuicao para o Problema de Escalonamento de Enfermeiros (NSP), com extensoes VND e VNS.

Baseado em: Constantino et al., *"A heuristic algorithm based on multi-assignment procedures for nurse scheduling"*, Annals of Operations Research, 2013.

## Autores

Ademir Aparecido Constantino, Dario Landa-Silva, Everton Luiz de Melo, Candido Ferreira Xavier de Mendonca, Douglas Baroni Rizzato & Wesley Romao

## Arquitetura

```
src/
  core/           # Tipos centrais, schedule, avaliacao de custo
  solvers/        # Solvers LAP: Hungarian O(n^3), APC, BAP
  heuristics/     # Operadores modulares (LEGO-blocks)
    constructive   # Construcao dia-a-dia via LAP
    pcr            # PCR forward/backward (Cut-and-Recombine)
    kswap          # k-Swap forward/backward (k=1 e PRT)
    shift_relocate # Shift/Relocate (N1 - mais simples)
    swap_exchange  # Swap/Exchange (N2)
    two_opt        # 2-Opt (N3)
    vnd            # Variable Neighbourhood Descent (8 vizinhancas)
    vns            # Variable Neighbourhood Search
    operator_registry # Registro plugavel de operadores
  benchmark/      # Harness de benchmarking com export CSV
  io/             # Leitor NSPLib + escritor de resultados
  main.c          # CLI com 5 modos de execucao

tests/            # Testes unitarios e de integracao (176 testes)
files/            # Instancias NSPLib (N25, N30, N50, N60, N75, N100)
docs/             # Pseudocodigo de referencia do algoritmo
```

## Pre-requisitos

- **GCC** (testado com 15.2.0)
- **Bash** (Git Bash no Windows)

## Compilacao

```bash
bash build.sh
```

O binario sera gerado em `build/mapa_kswap.exe`.

Para compilar e rodar os testes automaticamente:

```bash
bash build.sh --tests
```

## Uso

```
./build/mapa_kswap.exe [opcoes]
```

### Opcoes de instancia

| Flag | Descricao | Padrao |
|------|-----------|--------|
| `--nurses N` | Numero de enfermeiros | 50 |
| `--days N` | Numero de dias | 7 |
| `--max-files N` | Maximo de arquivos de instancia | 7290 |
| `--start N` | Arquivo inicial (numero) | 1 |
| `--constraint N` | ID do arquivo de restricoes | 6 |
| `--nsp-dir PATH` | Diretorio das instancias NSP | `./files/N50` |
| `--constraint-dir PATH` | Diretorio de restricoes | `./files/casos-1-8` |
| `--result-dir PATH` | Diretorio de saida dos resultados | `./resultados/N{nurses}` |

### Opcoes de algoritmo

| Flag | Descricao | Padrao |
|------|-----------|--------|
| `--solver TYPE` | `apc` ou `hungarian` | `apc` |
| `--iterations N` | Iteracoes sem melhoria (criterio de parada) | 5 |
| `--mode MODE` | Modo de execucao (ver abaixo) | `mapa` |
| `--verbose` | Mostra detalhes de cada passo VND/VNS | - |
| `--list-operators` | Lista todos os operadores registrados | - |
| `--help` | Exibe ajuda | - |

### Modos de execucao (`--mode`)

| Modo | Descricao |
|------|-----------|
| `mapa` | MAPA classico (PCR + kSwap, sem VND) |
| `mapa-vnd` | MAPA com VND como busca local |
| `vns` | VNS metaheuristica |
| `vns-bap` | VNS com solver BAP (binary search) |
| `vns-vnd` | VNS com shaking kSwap + VND como busca local |

### Exemplos

Executar MAPA classico em N25, restricao 1, 1 instancia:
```bash
./build/mapa_kswap.exe --nurses 25 --nsp-dir files/N25 --constraint 1 --max-files 1 --mode mapa
```

Executar MAPA-VND com saida detalhada:
```bash
./build/mapa_kswap.exe --nurses 25 --nsp-dir files/N25 --constraint 1 --max-files 1 --mode mapa-vnd --verbose
```

Executar VNS-VND em N50 com solver Hungarian:
```bash
./build/mapa_kswap.exe --nurses 50 --constraint 6 --max-files 10 --mode vns-vnd --solver hungarian --verbose
```

Listar todos os operadores disponiveis:
```bash
./build/mapa_kswap.exe --list-operators
```

## VND - Ordenacao de Vizinhancas

O VND percorre as vizinhancas da mais simples para a mais complexa. Se houver melhoria em Nk, reinicia para N1; se nao, avanca para Nk+1:

| # | Vizinhanca | Operador |
|---|-----------|----------|
| N1 | Shift/Relocate | `shift_relocate` |
| N2 | Swap/Exchange | `swap_exchange` |
| N3 | 2-Opt | `two_opt` |
| N4 | kSwap k=1 (PRT) | `kswap` |
| N5 | kSwap k=2 | `kswap` |
| N6 | kSwap k=3 | `kswap` |
| N7 | kSwap k=4 | `kswap` |
| N8 | PCR (CRP) | `pcr` |

## Resultados

Os resultados sao gravados em CSV no diretorio `resultados/N{nurses}/` com formato:

```
instancia;restricao;iteracoes;custo_inicial;custo_final;hcv;scv;tempo_segundos
```

## Testes

```bash
bash build.sh --tests
```

Suites:
- `test_schedule` - 99 testes (CRUD, copy, swap_block, reverse, forbidden sequences)
- `test_solvers` - 13 testes (Hungarian, APC, BAP, concordancia entre solvers)
- `test_integration` - 64 testes (construcao, custo, PCR, kSwap, pipeline completo)

## Operadores Registrados

13 operadores plugaveis via `operator_registry`:

```
shift_fwd, shift_bwd, swap_fwd, swap_bwd, 2opt_fwd, 2opt_bwd,
pcr_fwd, pcr_bwd, kswap1_fwd, kswap1_bwd, kswap2_fwd, kswap3_fwd, kswap4_fwd
```

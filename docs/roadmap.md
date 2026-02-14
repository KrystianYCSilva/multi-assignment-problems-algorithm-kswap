# Roadmap — GRASP + Busca Tabu

## Visao Geral

Duas extensoes ao MAPA para melhorar a qualidade das solucoes:

1. **GRASP** — diversidade na fase construtiva (multi-start)
2. **Busca Tabu** — fuga de minimos locais na fase de melhoria

Combinadas, formam o modo `grasp-vnd-tabu`: multiplas solucoes iniciais diversas
refinadas com busca local que escapa minimos locais.

---

## Fase 1: GRASP na Fase Construtiva

**Problema:** O MAPA e deterministico. Mesma instancia → mesma solucao inicial → mesmo minimo local.

**Solucao:** GRASP (Greedy Randomized Adaptive Search Procedure) na construcao.

### Algoritmo (por dia)

1. Montar matriz de custo `cost[nurse][shift]` (identico ao `constructive_build`)
2. Resolver LAP normalmente → obter custo otimo `c*`
3. Calcular `c_max` = pior custo entre assignments possiveis
4. Threshold: `t = c* + alpha * (c_max - c*)`
5. Construir RCL (Restricted Candidate List): assignments com custo <= t
6. Selecionar aleatoriamente um assignment da RCL
7. Aplicar ao schedule

### Parametro alpha

| alpha | Comportamento |
|-------|---------------|
| 0.0   | Guloso puro (MAPA original) |
| 0.1-0.3 | Perturbacao leve (recomendado) |
| 0.5   | Perturbacao moderada |
| 1.0   | Totalmente aleatorio |

### Multi-start GRASP-VND

```
best = NULL
para i = 0..n_starts:
    s = constructive_grasp(nsp, c, solver, alpha, seed+i)
    vnd_solve(s, nsp, c, solver, &vnd_cfg)
    se s.cost < best.cost:
        best = s
retornar best
```

### Arquivo novo

`src/heuristics/constructive_grasp.h`:
```c
Schedule *constructive_grasp(const NspInstance *nsp, const Constraints *c,
                             LapSolverFn solver, double alpha, unsigned int seed);
```

### CLI

```
--mode grasp-vnd
--alpha 0.2          # parametro RCL (default 0.2)
--starts 10          # numero de construcoes GRASP (default 10)
```

### Arquivos impactados

| Arquivo | Acao |
|---------|------|
| `src/heuristics/constructive_grasp.h/.c` | Novo (~120 LOC) |
| `src/main.c` | Adicionar MODE_GRASP_VND, --alpha, --starts |
| `build.sh` | Adicionar constructive_grasp.c ao LIB_SOURCES |
| `CMakeLists.txt` | Adicionar constructive_grasp.c |
| `run_all.sh` | Adicionar grasp-vnd ao ALL_MODES |
| `tests/test_integration.c` | Novo teste: seeds diferentes → schedules diferentes |

### Estimativa

~120 linhas de codigo novo. 1 sessao.

---

## Fase 2: Busca Tabu nos Operadores de Vizinhanca

**Problema:** O VND e estritamente descendente. Quando atinge um minimo local, para.

**Solucao:** Busca Tabu permite aceitar pioras temporarias para escapar minimos locais,
usando memoria de curto prazo para evitar ciclos.

### Estrutura da Lista Tabu

```c
typedef struct {
    int nurse;
    int day;
    int old_shift;
    int new_shift;
} TabuMove;

typedef struct {
    TabuMove *moves;
    int capacity;
    int head;
    int count;
    int tenure;       // iteracoes que um move fica proibido
    bool aspiration;  // aceitar tabu se melhor que best global
} TabuList;

typedef struct {
    TabuList tabu;
    int max_iterations;
    bool verbose;
} TabuConfig;
```

### Algoritmo

```
melhor_global = s.cost
para iter = 0..max_iterations:
    melhor_vizinho = +INF
    melhor_move = NULL

    para cada nurse, day:
        para cada target (shift_relocate ou swap):
            move = (nurse, day, shift_atual, novo_shift)

            se move na lista tabu E custo > melhor_global:
                pular (proibido, sem aspiracao)

            avaliar custo
            se custo < melhor_vizinho:
                melhor_vizinho = custo
                melhor_move = move

    aplicar melhor_move (mesmo que piore o custo)
    adicionar move inverso na lista tabu

    se melhor_vizinho < melhor_global:
        melhor_global = melhor_vizinho
        salvar best
```

### Decisoes de design

| Aspecto | Proposta | Justificativa |
|---------|----------|---------------|
| Escopo | Operadores Shift e Swap | Granularidade natural (1 nurse x 1 day). kSwap/PCR sao grossos demais |
| Tenure | `sqrt(n_nurses * n_days)` | Adaptativo. N25/7d→13, N100/28d→53 |
| Aspiracao | Sim | Padrao na literatura, sempre aceitar se melhor que best global |
| Integracao | N9/N10 dentro do VND | Ultimo recurso antes de desistir |
| Modo CLI | `--mode vnd-tabu` | VND com Tabu como vizinhanca final |

### Ordenacao VND com Tabu

```
N1: Shift
N2: Swap
N3: 2-Opt
N4: kSwap k=1 (PRT)
N5: kSwap k=2
N6: kSwap k=3
N7: kSwap k=4
N8: PCR (CRP)
N9: Tabu-Shift       ← novo
N10: Tabu-Swap       ← novo
```

Quando N1-N8 exaurem sem melhoria, Tabu tenta escapar do minimo local.
Se Tabu encontrar custo melhor que a entrada, VND reinicia de N1.

### CLI

```
--mode vnd-tabu
--tabu-tenure 15       # override do tenure (default: adaptativo)
--tabu-iter 100        # iteracoes tabu por chamada (default: 100)
```

### Arquivos impactados

| Arquivo | Acao |
|---------|------|
| `src/heuristics/tabu_search.h/.c` | Novo (~200 LOC) |
| `src/heuristics/vnd.c` | Adicionar wrappers wrap_tabu_shift, wrap_tabu_swap |
| `src/heuristics/operator_registry.c` | Registrar tabu_shift, tabu_swap |
| `src/main.c` | Adicionar MODE_VND_TABU, --tabu-tenure, --tabu-iter |
| `build.sh` | Adicionar tabu_search.c |
| `CMakeLists.txt` | Adicionar tabu_search.c |
| `tests/test_integration.c` | Testar que Tabu aceita pioras e depois melhora |

### Estimativa

~200 linhas de codigo novo. 1-2 sessoes.

---

## Fase 3: Combinacao Final

**Modo `grasp-vnd-tabu`:** o mais poderoso.

```
best = NULL
para i = 0..n_starts:
    s = constructive_grasp(nsp, c, solver, alpha, seed+i)    # diversidade
    vnd_tabu_solve(s, nsp, c, solver, &cfg)                  # busca profunda
    se s.cost < best.cost:
        best = s
retornar best
```

### Arquivos impactados

| Arquivo | Acao |
|---------|------|
| `src/main.c` | Adicionar MODE_GRASP_VND_TABU, wiring |
| `run_all.sh` | Adicionar ao ALL_MODES |

### Estimativa

~30 linhas (apenas wiring). Depende de Fase 1 + Fase 2.

---

## Resumo

| Fase | Feature | LOC | Sessoes | Dependencia |
|------|---------|-----|---------|-------------|
| 1 | GRASP | ~120 | 1 | Nenhuma |
| 2 | Busca Tabu | ~200 | 1-2 | Nenhuma |
| 3 | Combinacao | ~30 | <1 | Fase 1 + 2 |
| **Total** | | **~350** | **2-3** | |

### Modos CLI apos implementacao completa

| Modo | Construcao | Busca Local | Shaking |
|------|-----------|-------------|---------|
| `mapa` | Deterministico | PCR + kSwap fixo | — |
| `mapa-vnd` | Deterministico | VND (8 vizinhancas) | — |
| `vns` | Deterministico | PCR + kSwap fixo | kSwap |
| `vns-vnd` | Deterministico | VND | kSwap |
| `grasp-vnd` | **GRASP (RCL)** | VND | Multi-start |
| `vnd-tabu` | Deterministico | **VND + Tabu** | — |
| `grasp-vnd-tabu` | **GRASP (RCL)** | **VND + Tabu** | Multi-start |

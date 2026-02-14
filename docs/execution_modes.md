# Modos de Execucao - Diagramas

## Visao Geral: Todos os Modos

```mermaid
flowchart LR
    INPUT["Instancia NSP\n+ Restricoes"] --> CONSTR["Fase Construtiva\n(LAP dia-a-dia)"]
    CONSTR --> MODE{--mode}
    MODE -->|mapa| MAPA["MAPA Classico"]
    MODE -->|mapa-vnd| MAPAVND["MAPA + VND"]
    MODE -->|vns| VNS["VNS"]
    MODE -->|vns-bap| VNSBAP["VNS + BAP"]
    MODE -->|vns-vnd| VNSVND["VNS + VND"]
    MAPA --> OUT["Resultado CSV"]
    MAPAVND --> OUT
    VNS --> OUT
    VNSBAP --> OUT
    VNSVND --> OUT
```

## Fase Construtiva (Comum a Todos)

```mermaid
flowchart TD
    START(["Inicio"]) --> READ["Ler instancia NSP\n+ restricoes"]
    READ --> INIT["Criar schedule vazio\n(todos SHIFT_FREE)"]
    INIT --> LOOP{"Para cada dia d\n(0 .. n_days-1)"}
    LOOP -->|proximo dia| COST["Montar matriz de custo\ncost[nurse][shift]\n= preferencia + penalidades"]
    COST --> LAP["Resolver LAP\n(Hungarian ou APC)"]
    LAP --> ASSIGN["Atribuir shifts\nconforme solucao LAP"]
    ASSIGN --> LOOP
    LOOP -->|todos os dias| EVAL["Avaliar custo total\n(hard + soft violations)"]
    EVAL --> DONE(["Schedule inicial pronto"])

    style START fill:#2d5016,color:#fff
    style DONE fill:#2d5016,color:#fff
    style LAP fill:#1a3a5c,color:#fff
```

## Modo 1: `mapa` - MAPA Classico

```mermaid
flowchart TD
    S(["Schedule inicial"]) --> COPY["candidate = copia(current)"]
    COPY --> PCR_F["PCR Forward"]
    PCR_F --> KS1_F["kSwap k=1 Forward (PRT)"]
    KS1_F --> PCR_B["PCR Backward"]
    PCR_B --> KS1_B["kSwap k=1 Backward"]
    KS1_B --> KS4["kSwap k=4 Forward"]
    KS4 --> KS3["kSwap k=3 Forward"]
    KS3 --> KS2["kSwap k=2 Forward"]
    KS2 --> EVAL["Avaliar custo"]
    EVAL --> CMP{"candidate.cost\n< current.cost?"}
    CMP -->|Sim| ACCEPT["current = candidate\nno_improve = 0\natualizar best"]
    CMP -->|Nao| REJECT["descartar candidate\nno_improve++"]
    ACCEPT --> STOP{"no_improve\n>= max?"}
    REJECT --> STOP
    STOP -->|Nao| COPY
    STOP -->|Sim| FIM(["Retornar best"])

    style S fill:#2d5016,color:#fff
    style FIM fill:#2d5016,color:#fff
    style CMP fill:#7a4f1e,color:#fff
    style STOP fill:#7a4f1e,color:#fff
```

## Modo 2: `mapa-vnd` - MAPA com VND

```mermaid
flowchart TD
    S(["Schedule inicial"]) --> COPY["candidate = copia(current)"]
    COPY --> VND["VND Solve\n(8 vizinhancas)"]
    VND --> EVAL["Avaliar custo"]
    EVAL --> CMP{"candidate.cost\n< current.cost?"}
    CMP -->|Sim| ACCEPT["current = candidate\nno_improve = 0\natualizar best"]
    CMP -->|Nao| REJECT["descartar candidate\nno_improve++"]
    ACCEPT --> STOP{"no_improve\n>= max?"}
    REJECT --> STOP
    STOP -->|Nao| COPY
    STOP -->|Sim| FIM(["Retornar best"])

    style S fill:#2d5016,color:#fff
    style FIM fill:#2d5016,color:#fff
    style VND fill:#1a3a5c,color:#fff
    style CMP fill:#7a4f1e,color:#fff
```

## Motor VND (usado por `mapa-vnd` e `vns-vnd`)

```mermaid
flowchart TD
    INIT(["Entrada: schedule s"]) --> K0["k = 0"]
    K0 --> CHECK{"k < 8?"}
    CHECK -->|Nao| FIM(["Retornar s"])
    CHECK -->|Sim| SAVE["before = s.cost"]
    SAVE --> APPLY["Aplicar vizinhanca Nk"]
    APPLY --> EVALN["Avaliar custo"]
    EVALN --> BETTER{"s.cost < before?"}
    BETTER -->|"Sim (melhoria)"| RESTART["k = 0\n(reinicia do mais simples)"]
    BETTER -->|"Nao"| NEXT["k = k + 1\n(tenta proximo)"]
    RESTART --> CHECK
    NEXT --> CHECK

    style INIT fill:#2d5016,color:#fff
    style FIM fill:#2d5016,color:#fff
    style BETTER fill:#7a4f1e,color:#fff

```

### Vizinhancas VND em Ordem

```mermaid
flowchart LR
    N1["N1\nShift\nRelocate"] --> N2["N2\nSwap\nExchange"]
    N2 --> N3["N3\n2-Opt"]
    N3 --> N4["N4\nkSwap\nk=1 PRT"]
    N4 --> N5["N5\nkSwap\nk=2"]
    N5 --> N6["N6\nkSwap\nk=3"]
    N6 --> N7["N7\nkSwap\nk=4"]
    N7 --> N8["N8\nPCR\nCRP"]

    N1 -.- LABEL1(["Mais simples\nMais rapido"])
    N8 -.- LABEL2(["Mais complexo\nMais lento"])

    style N1 fill:#2d8659,color:#fff
    style N2 fill:#2d7a54,color:#fff
    style N3 fill:#2d6e4f,color:#fff
    style N4 fill:#3a5c6e,color:#fff
    style N5 fill:#3a506b,color:#fff
    style N6 fill:#3a4468,color:#fff
    style N7 fill:#4a3a65,color:#fff
    style N8 fill:#5a2d5c,color:#fff
    style LABEL1 fill:none,stroke:none
    style LABEL2 fill:none,stroke:none
```

## Modo 3: `vns` - VNS Metaheuristica

```mermaid
flowchart TD
    S(["Schedule inicial"]) --> COPY["candidate = copia(current)"]
    COPY --> PCR_F["PCR Forward"]
    PCR_F --> KS1_F["kSwap k=1 Forward"]
    KS1_F --> PCR_B["PCR Backward"]
    PCR_B --> KS1_B["kSwap k=1 Backward"]
    KS1_B --> KS4["kSwap k=4..2 Forward\n(decrescente)"]
    KS4 --> EVAL["Avaliar custo"]
    EVAL --> CMP{"candidate.cost\n< current.cost?"}
    CMP -->|Sim| ACCEPT["current = candidate\nno_improve = 0\natualizar best"]
    CMP -->|Nao| REJECT["descartar candidate\nno_improve++"]
    ACCEPT --> STOP{"no_improve >= max\nou iter >= max_iter?"}
    REJECT --> STOP
    STOP -->|Nao| COPY
    STOP -->|Sim| FIM(["Retornar best"])

    style S fill:#2d5016,color:#fff
    style FIM fill:#2d5016,color:#fff
    style CMP fill:#7a4f1e,color:#fff
```

## Modo 4: `vns-bap` - VNS com Solver BAP

```mermaid
flowchart TD
    S(["Schedule inicial"]) --> SEL["solver = BAP\n(Binary search sobre APC)"]
    SEL --> COPY["candidate = copia(current)"]
    COPY --> PCR_F["PCR Forward\n(com BAP)"]
    PCR_F --> KS1_F["kSwap k=1 Forward"]
    KS1_F --> PCR_B["PCR Backward"]
    PCR_B --> KS1_B["kSwap k=1 Backward"]
    KS1_B --> KS4["kSwap k=4..2 Forward"]
    KS4 --> EVAL["Avaliar custo"]
    EVAL --> CMP{"candidate.cost\n< current.cost?"}
    CMP -->|Sim| ACCEPT["current = candidate\nno_improve = 0"]
    CMP -->|Nao| REJECT["descartar candidate\nno_improve++"]
    ACCEPT --> STOP{"criterio\nde parada?"}
    REJECT --> STOP
    STOP -->|Nao| COPY
    STOP -->|Sim| FIM(["Retornar best"])

    style S fill:#2d5016,color:#fff
    style FIM fill:#2d5016,color:#fff
    style SEL fill:#5c1a1a,color:#fff
    style CMP fill:#7a4f1e,color:#fff
```

### Diferenca: APC vs BAP

```mermaid
flowchart LR
    subgraph APC["APC (Approximate)"]
        A1["Resolver LAP\ncom custo completo"] --> A2["Uma solucao"]
    end
    subgraph BAP["BAP (Binary Search)"]
        B1["Filtrar custos\n> threshold"] --> B2["Resolver LAP\ncom custos filtrados"]
        B2 --> B3{"Solucao\nviavel?"}
        B3 -->|Sim| B4["Diminuir threshold\n(binary search)"]
        B3 -->|Nao| B5["Aumentar threshold"]
        B4 --> B1
        B5 --> B1
    end

    style APC fill:#1a3a5c,color:#fff
    style BAP fill:#5c1a1a,color:#fff
```

## Modo 5: `vns-vnd` - VNS com Shaking + VND

```mermaid
flowchart TD
    S(["Schedule inicial"]) --> COPY["candidate = copia(current)"]
    COPY --> SHAKE["SHAKING: kSwap Forward\nk = (no_improve % 4) + 1"]
    SHAKE --> VND["VND Solve\n(8 vizinhancas completas)"]
    VND --> EVAL["Avaliar custo"]
    EVAL --> CMP{"candidate.cost\n< current.cost?"}
    CMP -->|Sim| ACCEPT["current = candidate\nno_improve = 0\natualizar best"]
    CMP -->|Nao| REJECT["descartar candidate\nno_improve++"]
    ACCEPT --> STOP{"no_improve\n>= max?"}
    REJECT --> STOP
    STOP -->|Nao| COPY
    STOP -->|Sim| FIM(["Retornar best"])

    style S fill:#2d5016,color:#fff
    style FIM fill:#2d5016,color:#fff
    style SHAKE fill:#5c1a1a,color:#fff
    style VND fill:#1a3a5c,color:#fff
    style CMP fill:#7a4f1e,color:#fff
```

### Ciclo Shaking do VNS-VND

```mermaid
flowchart LR
    I0["no_improve=0\nk=1"] --> I1["no_improve=1\nk=2"]
    I1 --> I2["no_improve=2\nk=3"]
    I2 --> I3["no_improve=3\nk=4"]
    I3 --> I4["no_improve=4\nk=1"]
    I4 --> I5["...cicla"]

    style I0 fill:#2d8659,color:#fff
    style I1 fill:#3a5c6e,color:#fff
    style I2 fill:#4a3a65,color:#fff
    style I3 fill:#5c1a1a,color:#fff
    style I4 fill:#2d8659,color:#fff
```

## Comparacao entre Modos

```mermaid
flowchart TD
    subgraph COMMON["Comum a Todos"]
        C1["Construcao inicial via LAP"]
        C2["Loop com criterio no_improve"]
        C3["Manter best global"]
    end

    subgraph MAPA_G["mapa"]
        M1["Busca local fixa:\nPCR + kSwap sequencial"]
    end

    subgraph MAPAVND_G["mapa-vnd"]
        MV1["Busca local adaptativa:\nVND (8 vizinhancas)\nreinicia ao melhorar"]
    end

    subgraph VNS_G["vns / vns-bap"]
        V1["Mesmo pipeline que MAPA\nmas vns-bap usa solver BAP\n(mais preciso, mais lento)"]
    end

    subgraph VNSVND_G["vns-vnd"]
        VV1["Shaking: kSwap(k variavel)\n+ VND como busca local\nMais poderoso"]
    end

    COMMON --> MAPA_G
    COMMON --> MAPAVND_G
    COMMON --> VNS_G
    COMMON --> VNSVND_G

    style COMMON fill:#1a3a5c,color:#fff
    style MAPA_G fill:#2d5016,color:#fff
    style MAPAVND_G fill:#2d6e4f,color:#fff
    style VNS_G fill:#5c4a1a,color:#fff
    style VNSVND_G fill:#5c1a3a,color:#fff
```

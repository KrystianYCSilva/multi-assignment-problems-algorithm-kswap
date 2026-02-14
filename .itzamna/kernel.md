---
description: "Cognitive decision engine deployed to target projects. Simplified version of content/core/ modules."
---

# Itzamna Kernel v4

> Sistema de decisao cognitivo para agentes de IA.
> Este arquivo define COMO voce deve processar cada pedido.

---

## Classificacao de Tarefa

Antes de executar qualquer tarefa, classifique pelo custo cognitivo:

### Reflexo (trivial, sem contexto extra)

**Quando:** Tarefa trivial, factual, ou mecanica.
**Custo:** ~500 tokens. Sem leitura de memoria.
**Acao:** Execute diretamente.

Exemplos: corrigir typo, renomear variavel, rodar build, responder pergunta factual.

**Regra:** Se resolve em <30 segundos mentais, e reflexo.

---

### Deliberado (precisa de contexto)

**Quando:** Requer entender o projeto ou historico.
**Custo:** ~2-5K tokens.
**Obrigatorio antes de executar:**
1. Leia `.itzamna/memory.md` (estado, decisoes recentes)
2. Leia `.context/` (project.md, tech.md, rules.md)
3. Carregue skills relevantes de `.<cli>/skills/`
4. Consulte `.itzamna/constitution.md` se envolver regras

Exemplos: implementar feature, code review, bug fix, refatorar.

---

### Profundo (pesquisa e planejamento)

**Quando:** Decisao complexa, arquitetura nova, trade-offs.
**Custo:** ~10-15K tokens.
**Obrigatorio antes de executar:**
1. Tudo do Deliberado
2. Pesquise em documentacao oficial ou web
3. Planeje com multiplas opcoes e trade-offs
4. Apresente opcoes ao humano ANTES de implementar

Exemplos: projetar arquitetura, criar skill, decisao tecnica com trade-offs.

**Delegacoes:**
- **Criar/validar skills** -> `/itzamna.skill.create`, `/itzamna.skill.validate`
- **Specs/plans/tasks** -> `/speckit.specify`, `/speckit.plan`, `/speckit.tasks`

---

## Gestao de Memoria

### 5 Camadas

```
Working Memory                    = Sua janela de contexto atual
Project Context                   = .context/ (projeto, stack, regras especificas)
Short-Term (Episodic Memory)      = .itzamna/memory.md (estado atual, sessoes, proximos passos)
Long-Term  (Episodic Memory)      = MEMORY.md (root) (features completas, releases, hotfixes)
Semantic Memory                   = .<cli>/skills/ (conhecimento reutilizavel)
```

### Dual Memory

| Arquivo | Tipo | Conteudo | Quando atualizar |
|---------|------|----------|------------------|
| `.itzamna/memory.md` | Short-term | Estado atual, decisoes, sessoes, proximos passos | Final de sessao significativa |
| `MEMORY.md` (root) | Long-term | Features completas, releases, hotfixes, metricas | Feature/release/hotfix completo |

### .context/ - Contexto do Projeto

| Arquivo | Conteudo | Quando ler |
|---------|----------|------------|
| `project.md` | O que o projeto e, por que existe, escopo | Sempre em Deliberado+ |
| `tech.md` | Stack, arquitetura, dependencias | Ao escrever/revisar codigo |
| `rules.md` | Regras especificas deste projeto, ADRs | Ao tomar decisoes |

### Regras de memoria

| Quando | O que fazer |
|--------|-------------|
| Inicio de sessao (Deliberado+) | Leia `.itzamna/memory.md` |
| Decisao arquitetural | Registre em `.itzamna/memory.md` |
| Fim de sessao significativa | Atualize `.itzamna/memory.md` (short-term) |
| Feature/release/hotfix completo | Proponha update em `MEMORY.md` root (long-term) |
| Conhecimento reutilizavel | Proponha criar skill |

Ambas memorias sao append-only. Nunca apague entradas.

---

## Roteamento

```
Pedido do usuario
     |
     v
 E trivial? --sim--> Reflexo: Execute direto
     | nao
     v
 Precisa de contexto? --sim--> Deliberado: Leia memoria + skills
     | nao (precisa mais)
     v
 Precisa pesquisa? --sim--> Profundo: Pesquise + planeje
                                  |
                                  v
                            Apresente opcoes ao humano
                            Delegue: /itzamna.skill.* ou /speckit.*
```

---

## Regras T0 (inviolaveis)

1. NUNCA escreva/modifique/delete arquivos sem aprovacao humana
2. NUNCA faca commit ou push automatico
3. NUNCA inclua secrets no codigo
4. NUNCA afirme que funciona sem verificar
5. SEMPRE classifique a tarefa antes de agir
6. SEMPRE leia `.itzamna/memory.md` em tarefas Deliberado+
7. SEMPRE atualize `.itzamna/memory.md` ao final de sessoes significativas
8. SEMPRE proponha update em `MEMORY.md` (root) ao completar features/releases/hotfixes

> Regras completas: `.itzamna/constitution.md`

---

*Itzamna Kernel v4.1 | Reflexo, Deliberado, Profundo.*

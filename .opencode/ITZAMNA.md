---
description: "Template for the ITZAMNA.md agent file deployed to each CLI directory by itzamna init."
---

# Itzamna v4

> Voce e um agente de codigo trabalhando em colaboracao com um humano.
> Leia `.itzamna/kernel.md` para entender como processar pedidos.

## Protocolo

1. Leia `.itzamna/kernel.md` - define classificacao e roteamento
2. Classifique cada pedido (T0/T1/T2) antes de agir
3. Para tarefas nao-triviais: leia `.itzamna/memory.md` e `.context/`
4. Carregue 2-5 skills relevantes de `skills/` neste diretorio (JIT)
5. Execute (PDCA: Plan > Do > Check > Act)
6. Human Gate: confirme antes de acoes destrutivas (delete, deploy, overwrite)
7. Atualize `.itzamna/memory.md` ao final de tarefas significativas
8. Para features/releases/hotfixes: proponha update em `MEMORY.md` (root, long-term)

## Regras T0 (inviolaveis)

1. NUNCA escreva/modifique/delete arquivos sem aprovacao humana
2. NUNCA faca commit ou push automatico
3. NUNCA inclua secrets no codigo
4. NUNCA afirme que funciona sem verificar (rode testes/lint)
5. SEMPRE classifique a tarefa antes de agir
6. SEMPRE leia `.itzamna/memory.md` em tarefas nao-triviais
7. SEMPRE proponha update em `MEMORY.md` (root) ao completar features/releases/hotfixes

Regras completas: `.itzamna/constitution.md`

## Estrutura

```
MEMORY.md              <- Memoria long-term (features, releases, hotfixes)
.itzamna/              <- Diretorio central Itzamna
  kernel.md            <- Como processar pedidos (classificacao + roteamento)
  constitution.md      <- Regras hierarquicas (T0/T1/T2)
  memory.md            <- Memoria short-term (estado atual, sessoes, proximos passos)
  templates/           <- Templates operacionais (skill-template, quality-checklist, etc.)
.context/              <- Contexto do projeto
  project.md           <- O que o projeto e e por que
  tech.md              <- Stack e arquitetura
  rules.md             <- Regras especificas + ADRs
.<cli>/                <- Este diretorio
  skills/              <- Conhecimento especializado (skill-name/SKILL.md)
  commands/            <- Sub-agentes e slash commands
```

## Skills

Skills estao em `skills/` neste diretorio, formato `skill-name/SKILL.md`.
Carregue sob demanda (JIT) conforme a tarefa exigir.
Cada SKILL.md tem `description` + "Use when:" no frontmatter - use para decidir quais carregar.

## Comandos

Slash commands disponiveis em `commands/`:
- `/itzamna.init` - Bootstrap do projeto
- `/itzamna.status` - Status do sistema
- `/itzamna.context` - Gerenciar .context/
- `/itzamna.memory` - Gerenciar memoria dual (short-term + long-term)
- `/itzamna.skill.create` - Criar skill
- `/itzamna.skill.validate` - Validar skill
- `/itzamna.skill.list` - Listar skills
- `/itzamna.agent` - Gerenciar sub-agentes

## Integracoes

- **Hefesto:** Absorvido - operacoes de skill via `/itzamna.skill.*`
- **spec-kit:** Workflow spec-first via `/speckit.specify`, `/speckit.plan`, `/speckit.tasks`, `/speckit.implement`

*Itzamna v4.1 | Menos e mais.*

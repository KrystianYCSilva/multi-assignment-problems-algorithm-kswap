---
description: "Inicializa Itzamna num projeto: verifica arquivos, sincroniza com arquivos gerados pelo CLI, preenche contexto."
---

# /itzamna.init

Inicialize ou verifique a instalacao do Itzamna neste projeto.

## Phase 1: Verificar Estrutura

1. Verifique se `.itzamna/` existe com os arquivos essenciais:
   - `.itzamna/kernel.md`
   - `.itzamna/constitution.md`
   - `.itzamna/memory.md`
   - `.itzamna/templates/` (skill-template, agent-template, quality-checklist, cli-compatibility)

2. Verifique se `.context/` existe com project.md, tech.md, rules.md

3. Verifique integracoes:
   - Hefesto: absorvido (comandos `/itzamna.skill.*`)
   - spec-kit: procure `.specify/` na raiz

4. Se algo faltando, sugira rodar `itzamna init` no terminal

## Phase 2: Sync com Arquivo do CLI

O CLI nativo (opencode, Claude, Copilot, etc.) gera um arquivo principal no root ou na pasta do CLI.
Detecte qual CLI esta ativo e localize o arquivo gerado:

| CLI | Arquivo gerado | Local |
|-----|---------------|-------|
| opencode | `AGENTS.md` | root |
| Claude Code | `CLAUDE.md` | root |
| Gemini CLI | `GEMINI.md` | root |
| Cursor | `.cursorrules` | root |
| Codex | `AGENTS.md` | root |
| Copilot | `copilot-instructions.md` | `.github/` |
| Qwen | `AGENTS.md` | root |
| Windsurf | `AGENTS.md` | root |

**Nota:** O Itzamna cria `ITZAMNA.md` dentro de cada `.<cli>/` — isso e DIFERENTE do arquivo nativo do CLI no root.

**Se o arquivo existir e NAO referenciar o Itzamna:**

1. Informe o usuario:
   ```
   Detectei [arquivo] gerado pelo [CLI]. Ele nao referencia o Itzamna.
   Posso adicionar uma secao de referencia ao protocolo Itzamna?
   ```

2. Se aprovado, adicione ao FINAL do arquivo existente (NUNCA sobrescreva o conteudo):
   ```markdown

   ---

   ## Itzamna Protocol

   Este projeto utiliza o [Itzamna](https://github.com/KrystianYCSilva/itzamna) como orquestrador cognitivo.

   **Antes de executar qualquer tarefa nao-trivial:**
   1. Leia `.itzamna/kernel.md` (classificacao e roteamento)
   2. Leia `.itzamna/memory.md` (estado atual do projeto)
   3. Leia `.context/` (project.md, tech.md, rules.md)

   **Ao finalizar tarefas significativas:**
   - Atualize `.itzamna/memory.md` (estado atual, append-only)
   - Para features/releases/hotfixes: proponha update em `MEMORY.md` (root, long-term)

   Regras completas: `.itzamna/constitution.md`
   Slash commands: veja `commands/` neste diretorio
   ```

3. Se o arquivo NAO existir, nao crie — o CLI nativo deve gera-lo

## Phase 3: Verificar MEMORY.md Root (Long-Term)

1. Verifique se `MEMORY.md` existe no root do projeto
2. Se NAO existir, proponha criar usando o template de long-term memory:
   - Estrutura: Features implementadas, releases, hotfixes, metricas
   - Append-only, atualizado quando features/releases completam

3. Se existir, verifique se `.itzamna/memory.md` tambem existe:
   - `MEMORY.md` (root) = Long-term (features, releases, historico)
   - `.itzamna/memory.md` = Short-term (estado atual, sessoes, proximos passos)

## Phase 4: Preencher .context/ (se vazio)

Se `.context/` existir mas estiver com templates nao preenchidos (contem `*(preencha)*` ou `*(ex:`):

1. Informe o usuario:
   ```
   .context/ esta com templates vazios. Posso analisar o codebase e preencher?
   ```

2. Se aprovado, analise o codebase:
   - `README.md`, `package.json`/`pyproject.toml`/`Cargo.toml`/`CMakeLists.txt`
   - Estrutura de diretorios (`src/`, `tests/`, `docs/`)
   - Arquivos de configuracao (`.gitignore`, CI/CD, linters)
   - Codigo fonte (imports, patterns, frameworks)

3. Preencha cada arquivo com Human Gate:
   - `.context/project.md`: O que, por que, escopo, status
   - `.context/tech.md`: Stack, build, test, arquitetura
   - `.context/rules.md`: Regras detectadas, ADRs iniciais

4. Mostre cada arquivo antes de escrever. Peca aprovacao.

## Phase 5: Preencher .itzamna/memory.md (se vazio)

Se `.itzamna/memory.md` estiver com template vazio:

1. Baseado na analise do codebase (Phase 4), proponha estado inicial:
   - Nome e stack do projeto
   - Estado atual (o que ja foi feito)
   - Proximos passos (baseado em TODOs, issues, roadmap)

2. Mostre a proposta. Peca aprovacao.

## Phase 6: Report Final

Mostre o status completo:
```
Itzamna Status
==============
.itzamna/:       OK (kernel, constitution, memory)
.context/:       OK (preenchido) | VAZIO (precisa preencher)
MEMORY.md:       OK (long-term) | MISSING
[CLI] AGENTS.md: OK (synced) | NEEDS SYNC | NOT FOUND
Hefesto:         absorbed (built-in)
spec-kit:        linked | not found
```

## Regras

- NUNCA sobrescreva arquivos gerados pelo CLI — apenas adicione referencia Itzamna
- NUNCA crie o arquivo principal do CLI (AGENTS.md root, CLAUDE.md, etc.) — o CLI faz isso
- NUNCA modifique .context/ ou memory sem aprovacao humana
- Human Gate em TODAS as escritas

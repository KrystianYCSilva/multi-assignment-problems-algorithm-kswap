---
description: "Mostra estado do sistema Itzamna: kernel, memoria dual, integracoes."
---

# /itzamna.status

Mostre o estado atual do sistema Itzamna para o usuario.

## Passos

1. Verifique se `.itzamna/` existe com kernel.md, constitution.md, memory.md
2. Verifique se `.itzamna/templates/` existe com os 4 templates operacionais
3. Verifique se `MEMORY.md` existe no root do projeto (long-term memory)
4. Verifique se `skills/` existe neste diretorio CLI e liste quantas skills ha
5. Detecte integracoes:
   - Hefesto: absorvido (comandos `/itzamna.skill.*`)
   - spec-kit: procure `.specify/` na raiz do projeto
6. Mostre o resultado em formato tabular

## Formato de saida

```
Itzamna Status
==============
.itzamna/:
  kernel.md:       OK | MISSING
  constitution.md: OK | MISSING
  memory.md:       OK (X bytes) | MISSING (short-term)
  templates/:      4/4 OK | X/4
MEMORY.md:         OK (long-term) | MISSING
ITZAMNA.md:        OK | MISSING (in this CLI dir)
Skills:            X skills instaladas
Hefesto:           absorbed (built-in)
spec-kit:          linked | not found
```

## Regras

- Este comando e somente leitura (nao modifica nada)
- Se algo estiver faltando, sugira `itzamna init` ou `itzamna update` para corrigir

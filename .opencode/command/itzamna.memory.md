---
description: "Gerencia a memoria dual do projeto: MEMORY.md (long-term) e .itzamna/memory.md (short-term)."
---

# /itzamna.memory

Gerencie a memoria do projeto. O Itzamna usa **dual memory**:

| Arquivo | Tipo | Conteudo | Atualizar quando |
|---------|------|----------|------------------|
| `MEMORY.md` (root) | Long-term | Features completas, releases, hotfixes, metricas | Feature/release/hotfix completo |
| `.itzamna/memory.md` | Short-term | Estado atual, decisoes recentes, sessoes, proximos passos | Final de sessao significativa |

## Uso

```
/itzamna.memory              <- Mostra estado atual (ambas memorias)
/itzamna.memory update       <- Atualiza short-term com sessao atual
/itzamna.memory release      <- Registra feature/release em long-term
/itzamna.memory check        <- Verifica se memorias estao atualizadas
```

## Subcomando: (sem argumento) - Ler

1. Leia `.itzamna/memory.md` (short-term)
2. Leia `MEMORY.md` no root (long-term), se existir
3. Apresente resumo:
   - Estado atual do projeto
   - Ultimas 3 decisoes
   - Proximos passos pendentes
   - Numero de sessoes registradas
   - Ultimas features do long-term (se houver)

## Subcomando: update (short-term)

1. Leia `.itzamna/memory.md` atual
2. Analise o que foi feito nesta sessao (baseado no historico da conversa)
3. Proponha as atualizacoes:
   - Nova entrada na tabela de Sessoes
   - Novas decisoes (se houve)
   - Atualizacao do estado atual
   - Atualizacao dos proximos passos
4. Mostre o diff proposto
5. **Peca aprovacao antes de escrever**

## Subcomando: release (long-term)

Use quando uma feature, release ou hotfix foi completado.

1. Leia `MEMORY.md` no root
2. Analise o que foi completado (feature, release, hotfix)
3. Proponha nova entrada no formato:
   ```markdown
   ### [Nome da Feature/Release]
   - **Data:** YYYY-MM-DD
   - **Arquivos:** principais arquivos criados/modificados
   - **Testes:** X testes, cobertura Y%
   - **Notas:** observacoes relevantes
   ```
4. Atualize metricas acumuladas (LOC, testes, cobertura)
5. Mostre o diff proposto
6. **Peca aprovacao antes de escrever**
7. Tambem atualize `.itzamna/memory.md` com o novo estado

## Subcomando: check

1. Leia ambas memorias
2. Verifique:
   - `.itzamna/memory.md`: ultima atualizacao recente? (avise se > 3 sessoes sem update)
   - `MEMORY.md`: features recentes registradas? Metricas atualizadas?
   - Proximos passos estao marcados? (itens [ ] vs [x])
   - Estado atual reflete a realidade do projeto?
3. Se desatualizada, sugira o subcomando adequado

## Regras

- NUNCA modifique nenhuma memoria sem aprovacao humana
- NUNCA apague entradas existentes (append-only em ambas)
- NUNCA atualize `MEMORY.md` (long-term) para sessoes comuns — apenas features/releases/hotfixes
- Formato de data: YYYY-MM-DD
- Mantenha resumos concisos (1-2 linhas por sessao, 3-5 linhas por feature)

---
description: "13-point validation checklist for skill quality (5 CRITICAL + 7 WARNING + 1 INFO)."
---

# Quality Checklist (Auto-Critica)

> Used by AI agents during `/itzamna.skill.create` and `/itzamna.skill.validate`
> to self-review generated skills. Every item MUST pass before presenting to the user.

---

## 13-Point Checklist

### 1. Frontmatter Valid

- [ ] YAML frontmatter present (between `---` markers)
- [ ] `name` field: lowercase, hyphens, max 64 chars, pattern `^[a-z0-9]+(-[a-z0-9]+)*$`
- [ ] `description` field: non-empty, max 1024 chars
- [ ] Directory name matches `name` field exactly

**Severity:** CRITICAL

---

### 2. Frontmatter Strict

- [ ] Frontmatter contains ONLY `name` and `description` fields
- [ ] No `license`, `metadata`, `version`, `compatibility`, or any extra field
- [ ] No `metadata.yaml` or external metadata files generated

**Severity:** CRITICAL

---

### 3. Size Limits

- [ ] SKILL.md < 500 lines
- [ ] SKILL.md < ~5000 tokens (estimate: lines * 10)
- [ ] No single section exceeds 100 lines

**Severity:** CRITICAL

---

### 4. Description Quality

- [ ] Description is specific enough for skill discovery (~100 tokens)
- [ ] Starts with action verb (e.g., "Guides", "Validates", "Generates")
- [ ] Contains "Use when:" trigger phrase
- [ ] Does NOT repeat the skill name verbatim

**Severity:** CRITICAL

---

### 5. No "When to Use" in Body

- [ ] Body does NOT contain a "When to Use" or "When to use this skill" section
- [ ] All trigger/discovery info is in the frontmatter `description` only
- [ ] Body starts with brief introduction, then goes straight to "How to" sections

**Severity:** WARNING

---

### 6. Task-Oriented Sections

- [ ] Body uses "How to [task]" sections (e.g., "How to model entities", "How to validate input")
- [ ] Does NOT use generic "Instructions > Step 1, Step 2" pattern
- [ ] Each section title answers: "What does the agent DO?"
- [ ] Does NOT include tutorial-style generic steps ("Analyze context", "Write code")

**Severity:** WARNING

---

### 7. Token Economy

- [ ] Basic concepts Claude already knows are cited in 1 sentence, not explained
- [ ] No teaching well-documented patterns (polymorphism, Streams API, try-catch, etc.)
- [ ] Examples are reserved for non-obvious or project-specific patterns
- [ ] No compatibility tables, version footers, or license footers in body
- [ ] Rule of thumb: "Would a senior developer need this explanation?" — if no, cut it

**Severity:** WARNING

---

### 8. Conciseness

- [ ] No obvious explanations (e.g., "HTML is a markup language")
- [ ] No filler phrases ("It is important to note that...")
- [ ] Prefers tables/lists over prose paragraphs
- [ ] Each sentence adds unique value

**Severity:** WARNING

---

### 9. Examples

- [ ] Examples only for non-obvious or project-specific patterns
- [ ] Examples use realistic data (not "foo", "bar", "example.com")
- [ ] Basic concept examples are omitted (Claude knows getter/setter, for-each, etc.)

**Severity:** WARNING

---

### 10. Terminology Consistency

- [ ] Same concept uses same term throughout
- [ ] No mixing of synonyms (e.g., "function" vs "method" without reason)
- [ ] Technical terms match official documentation

**Severity:** WARNING

---

### 11. Degrees of Freedom

- [ ] High freedom: suggestions with "consider", "you may"
- [ ] Medium freedom: recommendations with "prefer", "recommended"
- [ ] Low freedom: rules with "MUST", "NEVER", "always"
- [ ] Freedom level is appropriate for skill type

**Severity:** INFO

---

### 12. Progressive Disclosure

- [ ] If SKILL.md > 200 lines, advanced content moved to `references/`
- [ ] References are max 1 level deep (no nested references)
- [ ] JIT resources in `references/` are truly optional
- [ ] Core workflow instructions remain in SKILL.md
- [ ] Referenced files actually exist (no broken references)

**Severity:** WARNING

---

### 13. Security

- [ ] No API keys, tokens, passwords, or secrets
- [ ] No hardcoded credentials
- [ ] No personally identifiable information (PII)
- [ ] No internal/private URLs

**Severity:** CRITICAL

---

## Grading

| Grade   | Criteria                              |
|---------|---------------------------------------|
| PASS    | 0 CRITICAL fails, 0-2 WARNING        |
| PARTIAL | 0 CRITICAL fails, 3+ WARNING         |
| FAIL    | 1+ CRITICAL fail                      |

## Auto-Critica Process

1. Run all 13 checks against generated skill
2. For each FAIL: fix immediately before proceeding
3. Document corrections in "Auto-Critica Notes" section
4. Re-run failed checks after correction
5. Only present to user when grade >= PARTIAL
6. If PARTIAL: warn user about remaining warnings

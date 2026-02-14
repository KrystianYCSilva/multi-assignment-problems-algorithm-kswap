---
description: "Canonical SKILL.md template following agentskills.io specification."
---

# Skill Template Reference

> Canonical template for generating skills compatible with the
> [agentskills.io](https://agentskills.io) specification. AI agents MUST follow
> this structure when creating new skills via `/itzamna.skill.create`.

---

## Frontmatter (Required)

```yaml
---
name: <skill-name>
description: |
  <Action verb> <what it does> following <standard/practice>.
  Use when: <specific trigger conditions for discovery>.
---
```

### Frontmatter Rules

| Field       | Constraint                                          |
|-------------|-----------------------------------------------------|
| name        | lowercase, hyphens only, max 64 chars               |
| name format | `^[a-z0-9]+(-[a-z0-9]+)*$` (no consecutive hyphens) |
| description | max 1024 chars, non-empty, ~100 tokens ideal        |
| description | MUST include "Use when:" trigger phrase              |
| ONLY fields | `name` and `description` — nothing else             |

**NEVER add** `license`, `metadata`, `version`, `compatibility`, or any other field.
The description is the **only** trigger mechanism — all "when to use" info goes here, not in the body.

---

## SKILL.md Structure

```markdown
---
name: pdf-processing
description: |
  Extract text and tables from PDF files, fill forms, merge documents.
  Use when: the user needs to work with PDF files for extraction,
  form filling, merging, or any document manipulation tasks.
---

# PDF Processing

<Brief introduction: what and why. 2-3 sentences max.>

## How to extract text

<Task-oriented section. Each "How to" covers one capability.
Use numbered steps for sequential procedures, bullets for guidance.>

## How to fill forms

<Another capability. Keep sections focused on what the agent DOES.>

## How to merge documents

<As many "How to" sections as the skill has capabilities.>
```

---

## Token Economy Principle

> **The context window is a public good.** Claude already knows basic concepts.
> Only add what Claude CANNOT know: project-specific rules, non-obvious patterns,
> domain-specific workflows, and edge cases.

### Classification of Knowledge

| Type | Treatment | Example |
|------|-----------|---------|
| **Basic** (Claude already knows) | One sentence + no example | "Use `private` fields with getters/setters for encapsulation." |
| **Intermediate** (Claude knows but may miss nuances) | One paragraph + 1 short example | Interface segregation in Java 6 legacy code |
| **Advanced** (Claude likely doesn't know) | Full section + examples + `references/` | Company-specific API schemas, custom framework patterns |
| **Project-specific** (Claude cannot know) | Full detail + examples | Internal conventions, business rules, proprietary formats |

### Anti-patterns

- Teaching polymorphism, inheritance, Streams API, or other well-documented concepts
- Including compatibility tables (Claude can check `javac -version`)
- Adding "Version", "Created", "License" footers
- Duplicating "When to Use" in the body (already in frontmatter description)
- Adding a "References" section with well-known URLs (Oracle docs, MDN, etc.)

---

## Body Sections Pattern

### DO: Task-oriented "How to" sections

```markdown
## How to model domain entities
## How to handle legacy migrations
## How to apply validation rules
```

Each "How to" section answers: **"What does the agent DO?"**

### DON'T: Tutorial-style generic steps

```markdown
## Instructions
### Step 1: Analyze context
### Step 2: Apply principles
### Step 3: Write code
```

Generic steps waste tokens. The agent already knows how to analyze and write code.

### When to include examples

- Include: When the pattern is **non-obvious** or **project-specific**
- Skip: When the concept is basic and well-documented (e.g., getter/setter, try-catch)
- Rule of thumb: "Would a senior developer need this example?" If no, skip it.

---

## Progressive Disclosure Rules

| Constraint           | Limit                    |
|----------------------|--------------------------|
| SKILL.md lines       | max 500                  |
| SKILL.md tokens      | max ~5000                |
| Reference depth      | 1 level only             |
| References files     | Only if SKILL.md > 200 lines |

### When to use `references/`

Create `references/*.md` files when:
- SKILL.md approaches 200+ lines
- Detailed lookup tables, API schemas, or complex patterns are needed
- Content is truly "just-in-time" (read only when needed)
- Advanced concepts that need many tokens to explain

Do NOT use references for:
- Core workflow instructions (keep in SKILL.md)
- Content that is always needed
- Well-documented public knowledge

### Directory Structure

```
<skill-name>/
  SKILL.md              # Required - main skill file
  references/           # Optional - JIT deep-dive resources
    <topic>.md
  scripts/              # Optional - executable helpers
    <script>.<ext>
  assets/               # Optional - images, data files
    <file>.<ext>
```

---

## Naming Convention

| Rule                  | Example                    |
|-----------------------|----------------------------|
| Use lowercase         | `validate-email`           |
| Use hyphens           | `java-fundamentals`        |
| Be descriptive        | `react-testing-patterns`   |
| Domain first          | `python-error-handling`    |
| Max 64 chars          | -                          |
| No consecutive hyphens| `my-skill` not `my--skill` |

---

## Quality Indicators

A good skill MUST:
- [ ] Have ONLY `name` + `description` in frontmatter
- [ ] Have a specific "Use when:" trigger in description
- [ ] Use "How to [task]" sections (not generic "Instructions > Steps")
- [ ] Apply Token Economy (no teaching what Claude already knows)
- [ ] Include examples only for non-obvious patterns
- [ ] Be concise (no filler phrases, no obvious explanations)
- [ ] Contain no credentials or secrets
- [ ] Stay under 500 lines / ~5000 tokens

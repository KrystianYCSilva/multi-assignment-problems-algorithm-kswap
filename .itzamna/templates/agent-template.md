---
description: "Canonical structure template for generating new sub-agent command files."
---

# Agent Template

This template defines the canonical structure for generated agents.

## Agent Format

```markdown
---
description: "<Purpose in 1-2 sentences>. Composes skills: <skill-1>, <skill-2>, ..."
---

# /<agent-name>

You are <persona with specific role>. You specialize in <specific domain>.

## Skills

Load these skills for context before proceeding:
- Read `.<cli>/skills/<skill-1>/SKILL.md` for <capability-1>
- Read `.<cli>/skills/<skill-2>/SKILL.md` for <capability-2>
- Read `.<cli>/skills/<skill-3>/SKILL.md` for <capability-3>

## Workflow

1. <Step 1: Initial action>
2. <Step 2: Processing>
3. <Step 3: Validation>
4. <Step 4: Output/Result>

## Rules

- <Rule 1: Specific constraint>
- <Rule 2: Quality requirement>
- <Rule 3: Error handling>
- <Rule 4: Output format>
```

---

## CLI-Specific Path Mapping

When generating agent commands, use the correct skill path pattern for each CLI:

| CLI | Skill Path Pattern | Example |
|-----|-------------------|---------|
| Claude | `.claude/skills/<skill-name>/SKILL.md` | `.claude/skills/java-fundamentals/SKILL.md` |
| Gemini | `.gemini/skills/<skill-name>/SKILL.md` | `.gemini/skills/java-fundamentals/SKILL.md` |
| Codex | `.codex/skills/<skill-name>/SKILL.md` | `.codex/skills/java-fundamentals/SKILL.md` |
| GitHub Copilot | `.github/skills/<skill-name>/SKILL.md` | `.github/skills/java-fundamentals/SKILL.md` |
| OpenCode | `.opencode/skills/<skill-name>/SKILL.md` | `.opencode/skills/java-fundamentals/SKILL.md` |
| Cursor | `.cursor/skills/<skill-name>/SKILL.md` | `.cursor/skills/java-fundamentals/SKILL.md` |
| Qwen | `.qwen/skills/<skill-name>/SKILL.md` | `.qwen/skills/java-fundamentals/SKILL.md` |

**Detection Logic**: Scan each CLI's skills directory to find available skills, then generate paths using that CLI's pattern.

---

## Copilot Dual-File Generation

**IMPORTANT**: For GitHub Copilot CLI, generate TWO files:

1. **Agent file** (`.github/agents/<agent-name>.md`):
   - Full agent specification with frontmatter, persona, skills, workflow, rules
   - This is the native Copilot agent format

2. **Command file** (`.github/prompts/<agent-name>.prompt.md`):
   - Simple stub that references the agent
   - Format:
     ```markdown
     ---
     agent: <agent-name>
     ---

     # /<agent-name>

     This prompt invokes the <agent-name> agent.
     See `.github/agents/<agent-name>.md` for the full specification.
     ```

For all other CLIs, generate a single command file in their respective directories:
- Claude: `.claude/commands/<agent-name>.md`
- Gemini: `.gemini/commands/<agent-name>.toml`
- Codex: `.codex/prompts/<agent-name>.md`
- OpenCode: `.opencode/command/<agent-name>.md`
- Cursor: `.cursor/commands/<agent-name>.md`
- Qwen: `.qwen/commands/<agent-name>.md`

---

## Naming Rules

- **Agent names** follow the same naming rules as skills: `^[a-z0-9]+(-[a-z0-9]+)*$`
- **Command prefix**: Agents do NOT use `hefesto.` prefix
  - ✅ Correct: `/code-reviewer`, `/data-analyzer`, `/test-generator`
  - ❌ Wrong: `/hefesto.code-reviewer`, `/hefesto.data-analyzer`
- **Max length**: 64 characters
- **Descriptive but concise**: `java-code-reviewer`, not `code-reviewer-for-java-projects`

---

## Token Economy for Agents

- **Agent command file**: < 200 lines (target ~150 lines)
- **Total tokens**: ~3000 tokens maximum
- **Persona**: 1-2 sentences
- **Skills section**: List of skill paths only, no duplication of skill content
- **Workflow**: 3-6 sequential steps
- **Rules**: 3-6 concise rules

If an agent requires more complexity, split into multiple specialized agents.

---

## Quality Guidelines

1. **Specific Persona**: Avoid generic descriptions like "helpful assistant"
   - ✅ "You are an expert Java code reviewer specializing in SOLID principles and design patterns"
   - ❌ "You are a helpful code reviewer"

2. **Actionable Workflow**: Each step should be clear and executable
   - ✅ "1. Read the code file and identify all public methods"
   - ❌ "1. Analyze the code"

3. **Measurable Rules**: Rules should be verifiable
   - ✅ "Maximum 3 issues per file, prioritize CRITICAL > WARNING"
   - ❌ "Report issues appropriately"

4. **Skill Reuse**: Never duplicate skill content, always reference existing skills
   - ✅ "Read `.claude/skills/java-fundamentals/SKILL.md` for Java best practices"
   - ❌ "Java best practices: always use final for constants, ..." (duplicating skill content)

---

## Example: Complete Agent

```markdown
---
description: "Expert Java code reviewer specializing in SOLID principles. Composes skills: java-fundamentals, design-patterns, code-reviewer."
---

# /java-solid-reviewer

You are an expert Java code reviewer with deep knowledge of SOLID principles and design patterns. You specialize in identifying architectural issues and suggesting refactoring improvements.

## Skills

Load these skills for context before reviewing:
- Read `.claude/skills/java-fundamentals/SKILL.md` for Java language best practices
- Read `.claude/skills/design-patterns/SKILL.md` for Gang of Four patterns
- Read `.claude/skills/code-reviewer/SKILL.md` for general code review guidelines

## Workflow

1. Read the provided Java file(s) and identify all classes and methods
2. Check each class against SOLID principles (Single Responsibility, Open/Closed, Liskov Substitution, Interface Segregation, Dependency Inversion)
3. Identify applicable design patterns and check for correct implementation
4. Report findings in priority order: CRITICAL (violates SOLID) > WARNING (suboptimal) > SUGGESTION (improvement)
5. Provide specific refactoring suggestions with code examples

## Rules

- Maximum 5 findings per file to maintain focus
- Always cite specific SOLID principle or design pattern violated
- Provide concrete refactoring code examples, not generic advice
- Flag security issues as CRITICAL
- Use Java naming conventions in all suggestions
```

# MVVM Skills Pack Manifest

## Core Skills

These skills are the MVVM framework series and should be installed together.

| Folder | Skill name | Purpose |
| --- | --- | --- |
| `create-mvvm-project` | `create-mvvm-project` | Create a new Qt MVVM project from the bundled framework template. |
| `refactor-to-mvvm-project` | `refactor-to-mvvm-project` | Audit and refactor an existing Qt/C++ project to MediatorCore MVVM in staged migration slices. |
| `design-mvvm-framework` | `design-mvvm-framework` | Plan MVVM feature scope, responsibilities, tag flow, startup relationships, and skill order. |
| `design-mediator-message` | `design-mediator-message` | Design Mediator tags, QVariant types, custom type registration, and publish/subscribe contracts. |
| `design-mediator-policy` | `design-mediator-policy` | Select or design subscription policies such as AlwaysPolicy, replay, ValueChangedPolicy, filters, and throttling. |
| `write-app-bootstrapper` | `write-app-bootstrapper` | Write main.cpp, ModelBootstrapper, AppContext startup, logger tags, and meta-type registration. |
| `write-model` | `write-model` | Write BaseModel subclasses with queue/drain, tag-to-private-handler mapping, publish output, lifecycle, and logs. |
| `write-viewmodel` | `write-viewmodel` | Write BaseViewModel subclasses with Q_PROPERTY UI state, command publishing, and tag-to-private-handler subscription mapping. |
| `write-view` | `write-view` | Write Qt QWidget/View classes, layouts, child views, ViewModel creation, binding, events, and style interfaces. |
| `write-view-style` | `write-view-style` | Write or optimize Qt QSS from reference images, screenshots, UI direction, or companion UI/UX skill output. |

## Companion Skills

These skills are included because the MVVM series may call them for UI quality or style generation.

| Folder | Skill name | Purpose |
| --- | --- | --- |
| `ui-ux-pro-max` | `ui-ux-pro-max` | UI/UX design intelligence and searchable design guidance. |
| `design-taste-frontend` | `design-taste-frontend` | Frontend taste and anti-template visual design guidance. |

## Governance Skills

These skills package the large-program AI constraints. They should be installed with the core MVVM skills when you want AI to follow strict design, planning, implementation, verification, review, and retrospective gates.

| Folder | Skill name | Purpose |
| --- | --- | --- |
| `project-design` | `project-design` | Constrain large-program design before coding and route into MVVM/Superpowers planning. |
| `project-planning` | `project-planning` | Write verifiable staged plans into `docs/todo.md`. |
| `project-mvvm-contract` | `project-mvvm-contract` | Enforce Model/ViewModel/View/Bootstrapper boundaries. |
| `project-message-design` | `project-message-design` | Govern Mediator tag/type/policy contracts before implementation. |
| `project-implementation` | `project-implementation` | Constrain coding to planned, small, MVVM-compliant changes. |
| `project-verification` | `project-verification` | Require tests, logs, build output, or behavior evidence before completion claims. |
| `project-review` | `project-review` | Review completed stages for bugs, regressions, missing tests, and MVVM violations. |
| `project-retrospective` | `project-retrospective` | Record user corrections and failures as project lessons. |

## Installers

| Folder | Target tool | Default destination |
| --- | --- | --- |
| `install/codex/install.ps1` | Codex | `%USERPROFILE%\.codex\skills` or `$CODEX_HOME\skills` |
| `install/claude-code/install.ps1` | Claude Code | `%USERPROFILE%\.claude\skills` |
| `install/opencode/install.ps1` | OpenCode | `%USERPROFILE%\.config\opencode\skills` or `$XDG_CONFIG_HOME\opencode\skills` |

## External Recommended Skills

Install Superpowers separately when the environment supports it:

- `superpowers:using-superpowers`
- `superpowers:brainstorming`
- `superpowers:writing-plans`
- `superpowers:verification-before-completion`

The MVVM skills reference the Superpowers planning/spec workflow, but this package intentionally does not vendor Superpowers.

## Important Assets

- `README.md`: GitHub-ready package overview and install guide.
- `LICENSE`: package license copied from the source repository.
- `skills/core/create-mvvm-project/assets/framework-template/`: Qt framework template copied by `create-mvvm-project`.
- `skills/core/create-mvvm-project/scripts/create_project.py`: project creation script.
- `skills/core/refactor-to-mvvm-project/SKILL.md`: existing-project migration workflow.
- `skills/governance/`: large-program AI constraint skills that orchestrate the core MVVM skills.
- `agents/openai.yaml`: Codex UI metadata. Claude Code can ignore it.
- `install/opencode/install.ps1`: OpenCode native installer for global or project `.opencode/skills` destinations.

﻿# MVVM Skills Pack

面向 Qt/C++ 的 MediatorCore / MediatorRuntime / MediatorBinding MVVM skills 包，适配 Codex、Claude Code、OpenCode 等支持 `SKILL.md` 的 AI 开发工具。

这个包的目标不是让 AI “多写代码”，而是让 AI 在写代码前先把职责、消息、启动顺序、验证方式想清楚，再按 Model / ViewModel / View / Bootstrapper 的边界落地。

## 特性

- 10 个核心 MVVM skills：覆盖新项目创建、旧项目重构、消息契约、Policy、启动器、Model、ViewModel、View、QSS。
- 8 个 governance skills：约束大型程序从设计、计划、实现、验证、审查到复盘的完整流程。
- 2 个可选 UI companion skills：用于 UI/UX 建议和视觉风格优化。
- 内置 Qt MVVM 框架模板：`create-mvvm-project` 可复制 `assets/framework-template/` 创建新项目。
- 多工具安装脚本：Codex、Claude Code、OpenCode 均可一键安装。

## 目录结构

```text
mvvm-skills-pack/
  skills/
    core/          # MVVM 核心技能
    governance/    # 大型程序 AI 约束与流程技能
    companions/    # 可选 UI/UX 辅助技能
  install/
    codex/
    claude-code/
    opencode/
  MANIFEST.md
  README.md
```

## 安装

建议先单独安装 Superpowers。此包会引用 Superpowers 的规格、计划和验证流程，但不内置 Superpowers，方便不同机器维护自己的版本。

### Codex

```powershell
.\install\codex\install.ps1
```

默认安装到：

```text
%USERPROFILE%\.codex\skills
```

如果设置了 `CODEX_HOME`，会安装到：

```text
%CODEX_HOME%\skills
```

### Claude Code

```powershell
.\install\claude-code\install.ps1
```

默认安装到：

```text
%USERPROFILE%\.claude\skills
```

项目级安装：

```powershell
.\install\claude-code\install.ps1 -Destination "D:\path\to\project\.claude\skills"
```

### OpenCode

```powershell
.\install\opencode\install.ps1
```

默认安装到：

```text
%USERPROFILE%\.config\opencode\skills
```

如果设置了 `XDG_CONFIG_HOME`，会安装到：

```text
%XDG_CONFIG_HOME%\opencode\skills
```

项目级安装：

```powershell
.\install\opencode\install.ps1 -Destination "D:\path\to\project\.opencode\skills"
```

## 安装选项

只安装核心 MVVM skills：

```powershell
.\install\codex\install.ps1 -CoreOnly
.\install\claude-code\install.ps1 -CoreOnly
.\install\opencode\install.ps1 -CoreOnly
```

安装 core + governance，跳过 UI companion：

```powershell
.\install\codex\install.ps1 -NoCompanions
.\install\claude-code\install.ps1 -NoCompanions
.\install\opencode\install.ps1 -NoCompanions
```

安装 core + companions，跳过 governance：

```powershell
.\install\codex\install.ps1 -NoGovernance
.\install\claude-code\install.ps1 -NoGovernance
.\install\opencode\install.ps1 -NoGovernance
```

## 推荐使用流程

### 从零设计大型程序

```text
$project-design
$project-planning
$design-mvvm-framework
$design-mediator-message
$design-mediator-policy
$write-app-bootstrapper
$write-model
$write-viewmodel
$write-view
$project-verification
```

### 创建新项目骨架

```text
$create-mvvm-project
```

创建完成后再使用：

```text
$design-mvvm-framework
```

### 重构已有 Qt/C++ 项目

```text
$refactor-to-mvvm-project
```

该 skill 会先审计旧项目，再分阶段迁移，避免一次性大爆炸式重写。

### 新增一个跨层功能

```text
$design-mvvm-framework
$design-mediator-message
$design-mediator-policy
$write-model
$write-viewmodel
$write-view
```

### 优化 Qt Widgets 样式

```text
$write-view-style
```

可配合：

```text
$ui-ux-pro-max
$design-taste-frontend
```

## Skill 分组

### Core

| Skill | 用途 |
| --- | --- |
| `create-mvvm-project` | 从框架模板创建新的 Qt MVVM 项目 |
| `refactor-to-mvvm-project` | 将已有 Qt/C++ 项目分阶段重构为 MVVM |
| `design-mvvm-framework` | 做跨层功能总纲设计和 skill 编排 |
| `design-mediator-message` | 设计 tag、QVariant 类型和 Qt 元对象注册 |
| `design-mediator-policy` | 选择或设计 Always / ValueChanged / replay / filter Policy |
| `write-app-bootstrapper` | 编写 main.cpp、ModelBootstrapper 和全局注册 |
| `write-model` | 编写 BaseModel 子类和 tag -> 私有 handler 映射 |
| `write-viewmodel` | 编写 BaseViewModel、Q_PROPERTY、命令发布和状态订阅 |
| `write-view` | 编写 QWidget/View、控件、绑定、事件转发和样式接口 |
| `write-view-style` | 编写或优化 Qt QSS 样式表 |

### Governance

| Skill | 用途 |
| --- | --- |
| `project-design` | 大型程序设计前置约束 |
| `project-planning` | 把任务写成可验证阶段计划 |
| `project-mvvm-contract` | 检查 MVVM 分层红线 |
| `project-message-design` | 约束 tag/type/policy 契约先行 |
| `project-implementation` | 约束代码实现阶段 |
| `project-verification` | 完成前强制验证证据 |
| `project-review` | 以问题优先方式做审查 |
| `project-retrospective` | 把用户纠正和失败经验沉淀到文档 |

### Companions

| Skill | 用途 |
| --- | --- |
| `ui-ux-pro-max` | UI/UX 搜索型设计建议 |
| `design-taste-frontend` | 前端审美、反模板化和界面优化建议 |

## 核心架构约束

- `main.cpp` 只做应用壳。
- Model 由 `ModelBootstrapper` / `AppContext` 启动和托管。
- View 由 `main.cpp` 或父 View 创建。
- View 在自身构造流程中创建并注册 ViewModel。
- ViewModel 不由启动器统一创建。
- Model 不直接调用 View/ViewModel。
- ViewModel 不直接调用 Model/View。
- View 不写业务逻辑，不直接调用 Model。
- 自定义类型发布到 Mediator 前必须注册到 Qt 元对象系统。
- `SetupSubscriptions()` 只做 `Subscribe(...)` 和 `RegisterHandlers({...})`。
- `RegisterHandlers({...})` 只保留 tag 到私有 handler 的映射，lambda 只转发。

## 发布建议

发布到 GitHub 时，建议仓库根目录直接使用本目录内容：

```text
README.md
MANIFEST.md
install/
skills/
```

推荐在 release 中附带压缩包：

```text
dist/mvvm-skills-pack.zip
```

## 兼容性说明

- Codex、Claude Code、OpenCode 都可以读取包含 `SKILL.md` 的 skill 目录。
- Codex 会使用 `agents/openai.yaml` 作为 UI 元数据。
- Claude Code 和 OpenCode 可以安全忽略 `agents/openai.yaml`。
- OpenCode 原生目录为 `.opencode/skills` 或全局 `.config/opencode/skills`。

## License

建议随 GitHub 仓库一起发布项目 license。若直接发布本仓库内容，可沿用根目录 `LICENSE`。

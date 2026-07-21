﻿---
name: create-mvvm-project
description: "Use when creating a new Qt MVVM project from the bundled MediatorCore/MediatorRuntime/MediatorBinding framework template; 用于复制 assets/framework-template、替换项目名、可选包含 demo/tests/modbus，并提示下一步使用 design-mvvm-framework。"
---

# Create MVVM Project

## 职责

从本 skill 的 `assets/framework-template/` 复制框架骨架，创建新的 MediatorCore Qt MVVM 项目。
只负责项目骨架创建，不负责功能设计、Model/ViewModel/View 编写、Policy 选择或 tag 契约设计。

## 使用流程

1. 确认新项目名和目标目录。
2. 确认是否包含 `demo/`、`tests/`、`model/modbus/`。
3. 运行 `scripts/create_project.py`。
4. 检查新项目中的 `CMakeLists.txt`、`README.md`、应用名是否已替换。
5. 创建完成后，如需增加功能，继续使用 `$design-mvvm-framework`。

## 模板规则

模板必须排除：
- `.git/`
- `build/`
- `outputs/`
- `.cache/`
- `*.obj`
- `*.pdb`
- `*.exe`
- `*.lib`
- `*.ilk`
- 临时日志、数据库和会话产物

## 脚本

```powershell
python scripts/create_project.py --name MyProject --target D:\Work\MyProject --include-demo --include-tests --include-modbus
```

参数：
- `--name`：新项目名。
- `--target`：目标目录。
- `--include-demo`：复制 `demo/`。
- `--include-tests`：复制 `tests/`。
- `--include-modbus`：复制 `model/modbus/`。

## 自查清单

- [ ] 是否排除 `.git/`、`build/`、`outputs/`、`.cache/`。
- [ ] 是否排除编译产物。
- [ ] 是否替换 CMake `project(...)`。
- [ ] 是否按选项包含 demo/tests/modbus。
- [ ] 是否输出下一步 skill 建议。

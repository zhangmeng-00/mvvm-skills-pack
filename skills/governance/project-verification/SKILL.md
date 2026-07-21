﻿---
name: project-verification
description: "Use when a feature, bug fix, refactor stage, commit, or completion claim needs build, test, log, or behavior evidence before saying the Qt/MVVM task is done."
---

# Project Verification

## 职责

在完成声明前强制验证。此 skill 应配合 `superpowers:verification-before-completion`。

## 验证要求

- 优先运行相关测试。
- 没有测试时，提供可复现行为验证。
- 检查构建、日志或错误输出。
- 检查本次修改是否符合 MVVM 分层。
- 更新 `docs/todo.md` 的评审与验证证据。

## 完成前必须输出

- 执行的命令。
- 结果摘要。
- 失败项或未验证项。
- 剩余风险。

## 禁止事项

- 未验证不得声称完成。
- 不得把“没有运行测试”包装成已验证。
- 不得忽略失败测试。
- 不得只凭主观判断说“应该没问题”。

## 自查清单

- [ ] 是否运行了相关验证命令。
- [ ] 是否读取并理解验证输出。
- [ ] 是否记录失败和剩余风险。
- [ ] 是否更新 `docs/todo.md`。

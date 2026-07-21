#!/usr/bin/env python3
"""
Create a new MediatorCore MVVM project from the bundled framework template.
"""

from __future__ import annotations

import argparse
import shutil
from pathlib import Path


EXCLUDED_DIRS = {".git", "build", "outputs", ".cache", "__pycache__"}
EXCLUDED_SUFFIXES = {".obj", ".pdb", ".exe", ".lib", ".ilk", ".log", ".db"}
TEXT_SUFFIXES = {".txt", ".md", ".cmake", ".in", ".h", ".hpp", ".cpp", ".ui", ".json"}


def write_text_with_project_encoding(path: Path, text: str) -> None:
    encoding = "utf-8-sig" if any("\u4e00" <= char <= "\u9fff" for char in text) else "utf-8"
    path.write_text(text, encoding=encoding)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Create a MediatorCore MVVM project.")
    parser.add_argument("--name", required=True, help="New project name.")
    parser.add_argument("--target", required=True, help="Target project directory.")
    parser.add_argument("--include-demo", action="store_true", help="Include demo programs.")
    parser.add_argument("--include-tests", action="store_true", help="Include tests.")
    parser.add_argument("--include-modbus", action="store_true", help="Include model/modbus reference code.")
    parser.add_argument("--force", action="store_true", help="Allow writing into an existing empty directory.")
    return parser.parse_args()


def should_skip(path: Path, template_root: Path, args: argparse.Namespace) -> bool:
    rel = path.relative_to(template_root)
    parts = set(rel.parts)
    if parts & EXCLUDED_DIRS:
        return True
    if path.is_file() and path.suffix.lower() in EXCLUDED_SUFFIXES:
        return True
    if rel.parts and rel.parts[0] == "demo" and not args.include_demo:
        return True
    if rel.parts and rel.parts[0] == "tests" and not args.include_tests:
        return True
    if len(rel.parts) >= 2 and rel.parts[0] == "model" and rel.parts[1] == "modbus" and not args.include_modbus:
        return True
    return False


def copy_template(template_root: Path, target: Path, args: argparse.Namespace) -> None:
    if target.exists():
        if not args.force:
            raise SystemExit(f"Target already exists. Use --force only for an existing empty directory: {target}")
        if any(target.iterdir()):
            raise SystemExit(f"Refusing to write into non-empty target: {target}")
    target.mkdir(parents=True, exist_ok=True)

    for src in template_root.rglob("*"):
        if should_skip(src, template_root, args):
            continue
        dst = target / src.relative_to(template_root)
        if src.is_dir():
            dst.mkdir(parents=True, exist_ok=True)
            continue
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, dst)


def replace_project_name(target: Path, project_name: str) -> None:
    for path in target.rglob("*"):
        if not path.is_file() or path.suffix.lower() not in TEXT_SUFFIXES:
            continue
        try:
            text = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            continue
        updated = text.replace("MediatorMVAQt", project_name)
        if path.name == "CMakeLists.txt":
            updated = updated.replace("project(MediatorMVAQt LANGUAGES CXX)", f"project({project_name} LANGUAGES CXX)")
        if updated != text:
            write_text_with_project_encoding(path, updated)


def adjust_root_cmake(target: Path, args: argparse.Namespace) -> None:
    cmake = target / "CMakeLists.txt"
    if not cmake.exists():
        return

    text = cmake.read_text(encoding="utf-8")
    lines = text.splitlines()
    filtered = []
    for line in lines:
        if not args.include_demo and "add_subdirectory(demo)" in line:
            continue
        if not args.include_tests and "add_subdirectory(tests)" in line:
            continue
        filtered.append(line)
    text = "\n".join(filtered) + "\n"

    if not args.include_modbus:
        start = text.find("# MediatorModbus - Modbus communication library")
        if start != -1:
            end_marker = ")\n\n# ============================================================\n# Tests"
            end = text.find(end_marker, start)
            if end != -1:
                text = text[:start] + "# ============================================================\n# Tests" + text[end + len(end_marker):]

    write_text_with_project_encoding(cmake, text)


def main() -> None:
    args = parse_args()
    skill_root = Path(__file__).resolve().parents[1]
    template_root = skill_root / "assets" / "framework-template"
    target = Path(args.target).resolve()

    if not template_root.exists():
        raise SystemExit(f"Template not found: {template_root}")

    copy_template(template_root, target, args)
    replace_project_name(target, args.name)
    adjust_root_cmake(target, args)

    print(f"created={target}")
    print(f"project_name={args.name}")
    print(f"include_demo={args.include_demo}")
    print(f"include_tests={args.include_tests}")
    print(f"include_modbus={args.include_modbus}")
    print("next=Use $design-mvvm-framework to plan the first feature.")


if __name__ == "__main__":
    main()

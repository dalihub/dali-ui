import re
import sys
import os

# API Description:
# C++ Fluent API 매크로 생성기 (SetXXX 이름 보존 버전)
# 1. 자동 추출 및 Manual 태그 모두 원본 함수명(SetXXX)을 그대로 체이닝 이름으로 사용
# 2. View& SetXXX 형태의 Setter 수집 및 상속용 매크로 생성
# 3. @CHAIN_MANUAL: 주석/빈 줄 건너뛰고 선언 탐색, template<> 지원, 동일 이름 오버로드 수집

MANUAL_SEARCH_LIMIT = 60   # @CHAIN_MANUAL 이후 선언 탐색 시 최대 줄 수
DOC_COLLECT_LIMIT = 120   # doc 블록 수집 시 역방향 최대 스캔 줄 수 (무한루프 방지)

def _skip_comment_blank(lines, k, limit):
    """Skip comment and blank lines; return next index or limit."""
    while k < limit and k < len(lines):
        s = lines[k].strip()
        if s and not s.startswith(('/', '*')):
            return k
        k += 1
    return k

def _find_function_end(lines, start_idx):
    """From line start_idx, find the line index of the closing } of the function body (brace-balanced)."""
    depth = 0
    i = start_idx
    while i < len(lines):
        for c in lines[i]:
            if c == '{':
                depth += 1
            elif c == '}':
                depth -= 1
                if depth == 0:
                    return i
        i += 1
    return start_idx

def _extract_body_lines(lines, body_start, end_idx):
    """Extract and normalize body lines between { and }."""
    body_lines = []
    if body_start == end_idx:
        line = lines[body_start]
        open_idx = line.index('{')
        close_idx = line.rindex('}')
        inner = line[open_idx + 1:close_idx].strip()
        if inner:
            body_lines = [inner]
    else:
        first_content = lines[body_start][lines[body_start].index('{') + 1:].strip()
        if first_content:
            body_lines.append(first_content)
        for li in range(body_start + 1, end_idx):
            body_lines.append(lines[li].rstrip())
        last_content = lines[end_idx][:lines[end_idx].rindex('}')].strip()
        if last_content:
            body_lines.append(last_content)
    non_empty = [l for l in body_lines if l.strip()]
    if non_empty:
        min_indent = min(len(l) - len(l.lstrip()) for l in non_empty)
        body_lines = [l[min_indent:] if l.strip() else '' for l in body_lines]
    return body_lines

def _parse_one_declaration(lines, k, class_name):
    """
    At line k, optionally after a template line, parse 'Ret name(args)'.
    Returns (template_line_or_none, name, args_str, decl_line_idx) or None.
    """
    line = lines[k]
    stripped = line.strip()
    if stripped.startswith(('/', '*')):
        return None
    template_line = None
    decl_k = k
    # Optional template<...> on previous non-comment line
    prev = k - 1
    while prev >= 0 and lines[prev].strip().startswith(('/', '*')):
        prev -= 1
    if prev >= 0 and re.match(r'\s*template\s*<', lines[prev]):
        template_line = lines[prev].strip()
        # decl is current line
    m = re.search(r'(?P<ret>[\w&:*<>]+)\s+(?P<name>\w+)\s*\(', line)
    if not m:
        return None
    args_start = m.end()
    depth, args_str = 1, ""
    rmd = line[args_start:]
    for char in rmd:
        if char == '(':
            depth += 1
        elif char == ')':
            depth -= 1
        if depth == 0:
            break
        args_str += char
    if depth != 0:
        return None
    return (template_line, m.group('name'), args_str.strip(), decl_k)

# brace depth 기반으로 class의 실제 끝 위치를 찾는다.
# class 내부 struct/enum 등의 `};`를 class 종료로 오인하는 문제를 방지하기 위함
def _find_class_end(content, class_name):
    """Find the real end of a class body by brace matching."""
    m = re.search(r"class\s+.*?\b" + re.escape(class_name) + r"\b", content)
    if not m:
        return None
    brace_start = content.find('{', m.end())
    if brace_start == -1:
        return None
    depth = 0
    i = brace_start
    while i < len(content):
        c = content[i]
        if c == '{':
            depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0:
                semi = content.find(';', i)
                return (i, semi if semi != -1 else i)
        i += 1
    return None

def process_header(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    blocks = []
    is_in_block, current_block = False, None

    i = 0
    max_main_iter = max(len(lines) * 3, 10000)  # 무한루프 방지
    main_iter = 0
    prev_i = -1
    while i < len(lines) and main_iter < max_main_iter:
        main_iter += 1
        line = lines[i]

        # 블록 시작/끝 체크
        start_m = re.search(r'//\s*@CHAIN_START\((\w+)(?:\s*,\s*(\w*))?\)', line)
        if start_m:
            is_in_block, current_block = True, {'class': start_m.group(1), 'parent': start_m.group(2) or "", 'methods': []}
            i += 1
            continue
        if '// @CHAIN_END' in line:
            if current_block: blocks.append(current_block)
            is_in_block = False
            i += 1
            continue

        if not is_in_block:
            i += 1
            continue

        target_method = None
        manual_methods = []  # for MANUAL: list of (template_or_none, name, args, line_idx, doc)

        # 1. MANUAL_SELF 태그: 리턴 타입과 동일한 파라미터/body 타입을 ChildClass로 치환
        if '@CHAIN_MANUAL_SELF' in line:
            k = _skip_comment_blank(lines, i + 1, min(len(lines), i + MANUAL_SEARCH_LIMIT))
            inner_limit = 0
            while k < len(lines) and k < i + MANUAL_SEARCH_LIMIT and inner_limit < MANUAL_SEARCH_LIMIT:
                inner_limit += 1
                parsed = _parse_one_declaration(lines, k, current_block['class'])
                if parsed is None:
                    k += 1
                    continue
                template_line, name, args_str, decl_idx = parsed
                base_class = current_block['class']
                # 파라미터에서 BaseClass 타입(값/ref/const ref)을 ChildClass로 치환
                retyped_args = re.sub(
                    rf'\b{re.escape(base_class)}\b',
                    'ChildClass',
                    args_str
                )
                doc = []
                cursor = decl_idx - 1
                while cursor >= 0 and (decl_idx - cursor) <= DOC_COLLECT_LIMIT:
                    c_line = lines[cursor].strip()
                    if c_line.startswith(('/', '*')):
                        if '@CHAIN_MANUAL_SELF' not in c_line:
                            doc.insert(0, c_line)
                        if '/**' in c_line:
                            break
                        cursor -= 1
                    elif not c_line or re.match(r'^\s*template\s*<', c_line):
                        cursor -= 1
                    else:
                        break
                body_start = decl_idx
                while body_start < len(lines) and '{' not in lines[body_start]:
                    body_start += 1
                body_lines = []
                if body_start < len(lines):
                    end_idx = _find_function_end(lines, body_start)
                    raw_body = _extract_body_lines(lines, body_start, end_idx)
                    # body에서도 BaseClass 타입을 ChildClass로 치환
                    body_lines = [
                        re.sub(rf'\b{re.escape(base_class)}\b', 'ChildClass', bl)
                        for bl in raw_body
                    ]
                    i = end_idx + 1
                else:
                    i = decl_idx + 1
                current_block['methods'].append({
                    'name': name,
                    'args': retyped_args,
                    'doc': doc,
                    'template': template_line,
                    'body': body_lines,
                    'is_self': True
                })
                i = max(i, prev_i + 1)
                prev_i = i
                break

        # 2. MANUAL 태그 (주석/빈 줄 건너뛰기, template 지원; 태그당 메서드 1개)
        elif '@CHAIN_MANUAL' in line:
            k = _skip_comment_blank(lines, i + 1, min(len(lines), i + MANUAL_SEARCH_LIMIT))
            inner_limit = 0
            while k < len(lines) and k < i + MANUAL_SEARCH_LIMIT and inner_limit < MANUAL_SEARCH_LIMIT:
                inner_limit += 1
                parsed = _parse_one_declaration(lines, k, current_block['class'])
                if parsed is None:
                    k += 1
                    continue
                template_line, name, args_str, decl_idx = parsed
                doc = []
                cursor = decl_idx - 1
                while cursor >= 0 and (decl_idx - cursor) <= DOC_COLLECT_LIMIT:
                    c_line = lines[cursor].strip()
                    if c_line.startswith(('/', '*')):
                        if '@CHAIN_MANUAL' not in c_line:
                            doc.insert(0, c_line)
                        if '/**' in c_line:
                            break
                        cursor -= 1
                    elif not c_line or re.match(r'^\s*template\s*<', c_line):
                        cursor -= 1
                    else:
                        break
                current_block['methods'].append({
                    'name': name,
                    'args': args_str,
                    'doc': doc,
                    'template': template_line
                })
                body_start = decl_idx
                while body_start < len(lines) and '{' not in lines[body_start]:
                    body_start += 1
                if body_start < len(lines):
                    end_idx = _find_function_end(lines, body_start)
                    i = max(end_idx, decl_idx) + 1
                else:
                    i = decl_idx + 1
                i = max(i, prev_i + 1)  # 무한루프 방지: 반드시 한 줄 이상 진행
                prev_i = i
                break  # 한 개만 추가 후 내부 루프 탈출 → 끝에서 i += 1로 다음 줄로 진행

        # 3. 자동 Setter 추출 (Class& SetXXX)
        elif f"{current_block['class']}&" in line and "Set" in line:
            m = re.search(r'(?P<ret>[\w&:*<>]+)\s+(?P<name>Set\w+)\s*\(', line)
            if m:
                target_method = {
                    'name': m.group('name'), # 이름 그대로(Set 포함) 사용
                    'args_start': m.end(),
                    'line_idx': i
                }

        if target_method:
            idx = target_method['line_idx']
            # 괄호 짝 맞추기
            depth, args_str, rmd = 1, "", lines[idx][target_method['args_start']:]
            for char in rmd:
                if char == '(': depth += 1
                elif char == ')': depth -= 1
                if depth == 0: break
                args_str += char

            # 주석 수집 (역방향 스캔 상한으로 무한루프 방지)
            collected_docs = []
            cursor = idx - 1
            while cursor >= 0 and (idx - cursor) <= DOC_COLLECT_LIMIT:
                c_line = lines[cursor].strip()
                if c_line.startswith(('/', '*')):
                    if '@CHAIN_MANUAL' not in c_line:
                        collected_docs.insert(0, c_line)
                    if '/**' in c_line: break
                    cursor -= 1
                else:
                    break

            current_block['methods'].append({
                'name': target_method['name'],
                'args': args_str.strip(),
                'doc': collected_docs
            })
            if idx > i: i = idx

        prev_i = i
        i += 1

    if main_iter >= max_main_iter:
        sys.stderr.write(f"gen-chaining-macro: warning: main loop hit safety limit ({max_main_iter}), stopping.\n")

    if not blocks:
        return
    generate_files(file_path, blocks)

def generate_files(file_path, blocks):
    base_dir, base_name = os.path.dirname(file_path), os.path.splitext(os.path.basename(file_path))[0]
    autogen_path = os.path.join(base_dir, base_name + ".autogen.h")
    output = ["// Generated by dali-ui preprocessor. Do not modify.", "#pragma once\n"]

    for b in blocks:
        cls_upper = b['class'].upper()
        chain_macro = f"DALI_UI_CHAIN_{cls_upper}_METHODS"
        c_lines = [f"#define {chain_macro}(ChildClass) \\"]
        c_items = []
        if b['parent']:
            c_items.append(f"  DALI_UI_CHAIN_{b['parent'].upper()}_METHODS(ChildClass)")
        for m in b['methods']:
            names = extract_arg_names(m['args'])
            body = ["  " + d for d in m['doc']]
            if m.get('template'):
                body.append("  " + m['template'].strip())
            if m.get('is_self'):
                body.append(f"  ChildClass& {m['name']}({m['args']})")
                body.append("  {")
                for bl in m.get('body', []):
                    body.append("  " + bl if bl.strip() else "")
                body.append("  }")
            else:
                body.append(f"  ChildClass& {m['name']}({m['args']}) {{ {b['class']}::{m['name']}({', '.join(names)}); return *this; }}")
            c_items.append("\n".join(body))
        output += build_macro(c_lines, c_items) + [""]

    with open(autogen_path, 'w', encoding='utf-8') as f: f.write("\n".join(output))
    update_header(file_path, blocks, base_name)

def extract_arg_names(args_raw):
    names, curr, angle, paren = [], "", 0, 0
    parts = []
    for c in args_raw:
        if c == '<': angle += 1
        elif c == '>': angle -= 1
        elif c == '(': paren += 1
        elif c == ')': paren -= 1
        if c == ',' and angle == 0 and paren == 0: parts.append(curr.strip()); curr = ""
        else: curr += c
    parts.append(curr.strip())
    for p in parts:
        if p:
            clean = p.split('=')[0].strip()
            m = re.search(r'\*(\w+)\s*\)\s*\(', clean)
            if m:
                names.append(m.group(1))
            else:
                m = re.search(r'([\w]+)$', clean)
                if m:
                    names.append(m.group(1))
    return names

def build_macro(header, items):
    res = header
    for i, item in enumerate(items):
        sub = item.split('\n')
        for j, ln in enumerate(sub):
            is_last = (i == len(items)-1 and j == len(sub)-1)
            res.append(ln + ("" if is_last else " \\"))
    return res

def update_header(path, blocks, base_name):
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()
    content = re.sub(f'#include "{base_name}.autogen.h"\\n?', '', content)
    for b in blocks:
        content = re.sub(r'\n\s*public:\n\s*DALI_UI_CHAIN_.*_METHODS\(.*\)', '', content)
        c_def = re.search(r"class\s+.*?\b" + b['class'] + r"\b", content)
        if c_def:
            ins = c_def.start()
            search = content[:ins]
            doc = search.rstrip().rfind('/**')
            if doc != -1 and '*/' in search[doc:]: ins = doc
            content = content[:ins] + f'#include "{base_name}.autogen.h"\n' + content[ins:]
        if b['parent']:
            p_call = f"DALI_UI_CHAIN_{b['parent'].upper()}_METHODS({b['class']})"
            c_end = _find_class_end(content, b['class'])
            if c_end:
                idx, _ = c_end
                content = content[:idx] + f"\npublic:\n  {p_call}\n" + content[idx:]
    with open(path, 'w', encoding='utf-8') as f: f.write(content.strip() + "\n")

if __name__ == "__main__":
    if len(sys.argv) > 1: process_header(sys.argv[1])

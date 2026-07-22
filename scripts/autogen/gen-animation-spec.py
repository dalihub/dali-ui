#!/usr/bin/env python3
"""
Animation Bridge / Spec autogen code generator (v3).

Scans component headers for @ANIMATABLE / @ANIMATABLE_MANUAL tags and generates
animation Bridge, Spec, and Impl files using templates.

Usage:
  # Scan directory — finds @ANIMATION_CONFIG tags and processes all automatically
  python gen-animation-spec.py --scan-dir public-api/

  # Single class
  python gen-animation-spec.py --class View --header public-api/views/view.h
"""

import argparse
import glob
import os
import re
import sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
TEMPLATE_DIR = os.path.join(SCRIPT_DIR, 'templates')

# ---------------------------------------------------------------------------
# Type information
# ---------------------------------------------------------------------------

TYPE_INFO = {
    'float':      {'cpp': 'float',       'const_ref': False},
    'Vector3':    {'cpp': 'Vector3',     'const_ref': True},
    'Vector4':    {'cpp': 'Vector4',     'const_ref': True},
    'Quaternion': {'cpp': 'Quaternion',  'const_ref': True},
    'UiColor':    {'cpp': 'UiColor',     'const_ref': True},
}

# ---------------------------------------------------------------------------
# Tag parsers
# ---------------------------------------------------------------------------

# @ANIMATABLE(prop, type)
TAG_RE = re.compile(r'//\s*@ANIMATABLE\(\s*(.+?)\s*,\s*(\w+)\s*\)')

# @ANIMATABLE_MANUAL(PropertyName, type)
MANUAL_TAG_RE = re.compile(r'//\s*@ANIMATABLE_MANUAL\(\s*(\w+)\s*,\s*(\w+)\s*\)')

# @ANIMATION_CONFIG(Class) or @ANIMATION_CONFIG(Class, Parent)
CONFIG_TAG_RE = re.compile(r'//\s*@ANIMATION_CONFIG\(\s*(\w+)(?:\s*,\s*(\w+))?\s*\)')

SETTER_RE = re.compile(r'(?:\w+&|void)\s+Set(\w+)\s*\(')


def _camel_to_words(name):
    """PascalCase -> lowercase words."""
    return ' '.join(re.sub(r'([A-Z])', r' \1', name).strip().lower().split())


def _param_type(ti):
    return f"const {ti['cpp']}&" if ti['const_ref'] else ti['cpp']


def _class_to_filename(class_name):
    """PascalCase -> kebab-case."""
    return '-'.join(re.sub(r'([A-Z])', r'-\1', class_name).strip('-').lower().split('-'))


def parse_animatable_tags(header_path):
    """Parse @ANIMATABLE and @ANIMATABLE_MANUAL tags from a header file.

    Returns list of dicts with keys:
      name, prop_index (or None), type, type_info, is_manual
    """
    entries = []
    with open(header_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    for i, line in enumerate(lines):
        # Try @ANIMATABLE
        m = TAG_RE.search(line)
        if m:
            prop_index = m.group(1).strip()
            type_name = m.group(2).strip()

            if type_name not in TYPE_INFO:
                print(f"WARNING: Unknown type '{type_name}' at {header_path}:{i+1}", file=sys.stderr)
                continue

            for j in range(i + 1, min(i + 30, len(lines))):
                sm = SETTER_RE.search(lines[j])
                if sm:
                    entries.append({
                        'name': sm.group(1),
                        'prop_index': prop_index,
                        'type': type_name,
                        'type_info': TYPE_INFO[type_name],

                        'is_manual': False,
                    })
                    break
            continue

        # Try @ANIMATABLE_MANUAL
        m = MANUAL_TAG_RE.search(line)
        if m:
            prop_name = m.group(1).strip()
            type_name = m.group(2).strip()

            if type_name not in TYPE_INFO:
                print(f"WARNING: Unknown type '{type_name}' at {header_path}:{i+1}", file=sys.stderr)
                continue

            entries.append({
                'name': prop_name,
                'prop_index': None,
                'type': type_name,
                'type_info': TYPE_INFO[type_name],
                'is_manual': True,
            })

    return entries


def parse_animation_config(header_path):
    """Parse @ANIMATION_CONFIG tag. Returns dict or None."""
    with open(header_path, 'r', encoding='utf-8') as f:
        for line in f:
            m = CONFIG_TAG_RE.search(line)
            if m:
                return {'cls': m.group(1), 'parent': m.group(2), 'header': header_path}
    return None


# ---------------------------------------------------------------------------
# Doxygen generation
# ---------------------------------------------------------------------------

def _gen_doxygen(name_words, is_by):
    brief_name_overrides = {
        'shadow blur radius': 'first shadow blur radius',
        'shadow opacity': 'first shadow opacity',
    }
    brief_name = brief_name_overrides.get(name_words, name_words)
    brief = f'Animates the {brief_name} by a relative amount.' if is_by else f'Animates the {brief_name}.'
    params = [('relative' if is_by else 'target', 'The relative value' if is_by else 'The target value'),
              ('duration', 'The animation duration'),
              ('alpha', 'The alpha function (default: linear)'),
              ('delay', 'The delay before starting (default: 0)')]
    lines = ['/**', f' * @brief {brief}', ' *']
    for pname, pdesc in params:
        lines.append(f' * @param[in] {pname} {pdesc}')
    lines.append(' */')
    return '\n'.join(lines)


def _sig(ti, is_by, with_defaults=False):
    val = 'relative' if is_by else 'target'
    parts = [f'{_param_type(ti)} {val}', 'Duration duration']
    if with_defaults:
        parts.append('AlphaFunction alpha = AlphaFunction()')
        parts.append('Duration delay = Duration()')
    else:
        parts.append('AlphaFunction alpha')
        parts.append('Duration delay')
    return ', '.join(parts)


def _args(is_by):
    val = 'relative' if is_by else 'target'
    return f'{val}, duration, alpha, delay'


def _value_expr(entry, is_by):
    """C++ expression for the value passed to AddEntry/AnimateTo."""
    val = 'relative' if is_by else 'target'
    if entry['type'] == 'UiColor':
        return f'{val}.GetRgba()'
    return val


def _manual_apply_wrapper_name(view_cls, entry, is_by):
    apply_suffix = 'By' if is_by else 'To'
    return f'Apply{view_cls}{entry["name"]}{apply_suffix}'


# ---------------------------------------------------------------------------
# Method generation
# ---------------------------------------------------------------------------

def _bridge_body_lines(entry, is_by):
    """Generate the body lines shared by inline and out-of-line bridge methods."""
    val = _value_expr(entry, is_by)
    prop = entry['prop_index']
    anim_fn = 'AnimateBy' if is_by else 'AnimateTo'
    return [
        f'  ExtendIfNeeded(delay, duration);',
        f'  mAnimation.{anim_fn}(Property(mView, {prop}), {val}, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));',
    ]


def gen_bridge_method_inline(cls, entry, is_by):
    """Generate inline bridge method (no class:: prefix)."""
    ti = entry['type_info']
    name = entry['name'] + ('By' if is_by else '')
    lines = [f'{cls}& {name}({_sig(ti, is_by, with_defaults=True)})', '{']
    lines.extend(_bridge_body_lines(entry, is_by))
    lines.extend(['  return *this;', '}'])
    return '\n'.join(lines)


def gen_bridge_method_outofline(cls, entry, is_by):
    """Generate out-of-line bridge method (with class:: prefix, for .cpp)."""
    ti = entry['type_info']
    name = entry['name'] + ('By' if is_by else '')
    lines = [f'{cls}& {cls}::{name}({_sig(ti, is_by)})', '{']
    lines.extend(_bridge_body_lines(entry, is_by))
    lines.extend(['  return *this;', '}'])
    return '\n'.join(lines)


def gen_bridge_method_manual(cls, impl_cls, entry, is_by, view_expr='mView'):
    """Generate bridge method for @ANIMATABLE_MANUAL (calls Apply{Name}To/By from impl)."""
    ti = entry['type_info']
    name = entry['name'] + ('By' if is_by else '')
    sig = _sig(ti, is_by)
    val = _value_expr(entry, is_by)
    apply_suffix = 'By' if is_by else 'To'

    lines = [f'{cls}& {cls}::{name}({sig})', '{']
    lines.append(f'  ExtendIfNeeded(delay, duration);')
    lines.append(f'  Internal::{impl_cls}::Apply{entry["name"]}{apply_suffix}(mAnimation, {view_expr}, '
                 f'{{Dali::Property::INVALID_INDEX, {val}, duration, alpha, delay, nullptr}});')
    lines.extend(['  return *this;', '}'])
    return '\n'.join(lines)


def gen_bridge_method_decl(cls, entry, is_by, with_defaults=False):
    """Declaration for a bridge method."""
    ti = entry['type_info']
    name = entry['name'] + ('By' if is_by else '')
    return f'{cls}& {name}({_sig(ti, is_by, with_defaults=with_defaults)});'


def gen_spec_addentry(spec_cls, impl_cls, entry, is_by, apply_fn_override=None):
    """Generate Spec handle method that calls AddAnimateToEntry/AddAnimateByEntry on impl."""
    ti = entry['type_info']
    name = entry['name'] + ('By' if is_by else '')
    sig = _sig(ti, is_by)
    val = _value_expr(entry, is_by)
    add_method = 'AddAnimateByEntry' if is_by else 'AddAnimateToEntry'

    if entry['is_manual']:
        prop_idx = 'Dali::Property::INVALID_INDEX'
        apply_suffix = 'By' if is_by else 'To'
        apply_fn = apply_fn_override or f'&Internal::{impl_cls}::Apply{entry["name"]}{apply_suffix}'
        call = f'  Internal::GetImpl(*this).{add_method}({prop_idx}, {val}, duration, alpha, delay, {apply_fn});'
    else:
        prop_idx = entry['prop_index']
        call = f'  Internal::GetImpl(*this).{add_method}({prop_idx}, {val}, duration, alpha, delay);'

    return '\n'.join([
        f'{spec_cls}& {spec_cls}::{name}({sig})',
        '{',
        call,
        '  return *this;',
        '}',
    ])


def gen_spec_manual_apply_wrapper(impl_cls, root_impl_cls, root_view_cls, view_cls, entry, is_by):
    """Generate a child manual apply wrapper matching the root Entry::ApplyFunction."""
    apply_suffix = 'By' if is_by else 'To'
    wrapper_name = _manual_apply_wrapper_name(view_cls, entry, is_by)
    return '\n'.join([
        f'void {wrapper_name}(Animation& animation, {root_view_cls} view, const Internal::{root_impl_cls}::Entry& entry)',
        '{',
        f'  {view_cls} child = {view_cls}::DownCast(view);',
        f'  if(child)',
        f'  {{',
        f'    Internal::{impl_cls}::Apply{entry["name"]}{apply_suffix}(animation, child, entry);',
        f'  }}',
        '}',
    ])


# ---------------------------------------------------------------------------
# Composite method block generators
# ---------------------------------------------------------------------------

def _gen_methods_block(entries, parent_entries, cls, parent_cls, gen_own_fn, gen_parent_fn):
    """Generate all methods for a class (parent overrides + own properties)."""
    lines = []

    if parent_entries and parent_cls:
        lines.append('  ////////////////////////////////////////////////////////////////////////////')
        lines.append('  // Parent property overrides')
        lines.append('  ////////////////////////////////////////////////////////////////////////////')
        lines.append('')
        for entry in parent_entries:
            nw = _camel_to_words(entry['name'])
            for is_by in (False, True):
                lines.append(_indent(2, _gen_doxygen(nw, is_by)))
                lines.append(_indent(2, gen_parent_fn(cls, parent_cls, entry, is_by)))
            lines.append('')

    if entries:
        lines.append('  ////////////////////////////////////////////////////////////////////////////')
        lines.append('  // Own properties')
        lines.append('  ////////////////////////////////////////////////////////////////////////////')
        lines.append('')
        for entry in entries:
            nw = _camel_to_words(entry['name'])
            for is_by in (False, True):
                lines.append(_indent(2, _gen_doxygen(nw, is_by)))
                lines.append(_indent(2, gen_own_fn(cls, entry, is_by)))
            lines.append('')

    return '\n'.join(lines)


def _indent(spaces, text):
    """Indent each line of text."""
    prefix = ' ' * spaces
    return '\n'.join(prefix + line if line.strip() else '' for line in text.split('\n'))


# ---------------------------------------------------------------------------
# Template loading and rendering
# ---------------------------------------------------------------------------

def load_template(name):
    path = os.path.join(TEMPLATE_DIR, name)
    with open(path, 'r', encoding='utf-8') as f:
        return f.read()


def render(template_str, substitutions):
    result = template_str
    for key, value in substitutions.items():
        result = result.replace('{{' + key + '}}', value)
    return result


# ---------------------------------------------------------------------------
# File generation
# ---------------------------------------------------------------------------

def write_file(path, content):
    os.makedirs(os.path.dirname(path) or '.', exist_ok=True)
    if os.path.exists(path):
        with open(path, 'r', encoding='utf-8') as f:
            if f.read() == content:
                return False
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)
    return True


def _public_api_include(header, public_api_root):
    rel = os.path.relpath(header, public_api_root).replace(os.sep, '/')
    return f'dali-ui-foundation/public-api/{rel}'


def _root_class(cls, configs):
    """Return the topmost configured ancestor for cls."""
    root = cls
    while configs and root in configs:
        parent = configs[root].get('parent')
        if not parent or parent not in configs:
            break
        root = parent
    return root


def generate_for_class(cls, header, parent_cls=None, parent_header=None, configs=None, public_api_root=None):
    """Generate all files for one component class."""
    bridge = f'{cls}AnimationBridge'
    spec = f'{cls}AnimationSpec'
    impl = f'{cls}AnimationSpecImpl'
    view_instance = cls[0].lower() + cls[1:]

    bridge_fn = _class_to_filename(bridge)
    spec_fn = _class_to_filename(spec)
    impl_fn = _class_to_filename(impl)

    own_entries = parse_animatable_tags(header)
    parent_entries = parse_animatable_tags(parent_header) if parent_header else []
    manual_entries = [e for e in own_entries if e['is_manual']]

    if public_api_root is None:
        public_api_root = os.path.dirname(header)

    pub_anim_dir = os.path.join(public_api_root, 'animation')
    base_dir = os.path.dirname(public_api_root)
    int_anim_dir = os.path.join(base_dir, 'internal', 'animation')
    view_include = _public_api_include(header, public_api_root)

    os.makedirs(pub_anim_dir, exist_ok=True)
    os.makedirs(int_anim_dir, exist_ok=True)

    P = 'dali-ui-foundation'  # include path prefix
    has_parent = parent_cls is not None
    parent_bridge = f'{parent_cls}AnimationBridge' if has_parent else None
    parent_spec = f'{parent_cls}AnimationSpec' if has_parent else None
    parent_impl = f'{parent_cls}AnimationSpecImpl' if has_parent else None
    root_cls = _root_class(cls, configs)
    root_impl_cls = f'{root_cls}AnimationSpecImpl'

    def _out(directory, filename, content):
        p = os.path.join(directory, filename)
        if write_file(p, content):
            print(f'  Generated: {p}')

    # ===== 1. Bridge .autogen.h (declarations only) =====
    def _bridge_own_decl(c, entry, is_by):
        return gen_bridge_method_decl(c, entry, is_by, with_defaults=True)

    def _parent_override_decl(c, parent_c, entry, is_by):
        return gen_bridge_method_decl(c, entry, is_by, with_defaults=True)

    bridge_methods = _gen_methods_block(
        own_entries, parent_entries, bridge, parent_bridge,
        _bridge_own_decl, _parent_override_decl)

    if has_parent:
        h_tmpl = 'animation-bridge-child.h.tmpl'
    else:
        h_tmpl = 'animation-bridge-root.h.tmpl'

    _out(pub_anim_dir, f'{bridge_fn}.autogen.h', render(load_template(h_tmpl), {
        'BRIDGE_CLASS': bridge, 'VIEW_CLASS': cls,
        'VIEW_INSTANCE': view_instance,
        'PARENT_BRIDGE': parent_bridge or '',
        'PARENT_INCLUDE': f'#include <{P}/public-api/animation/{_class_to_filename(parent_bridge)}.autogen.h>' if has_parent else '',
        'VIEW_INCLUDE': f'#include <{view_include}>',
        'METHODS': bridge_methods,
    }))

    # ===== 2. Bridge .autogen.cpp (all implementations) =====
    bridge_cpp_methods = []

    # Parent overrides (out-of-line delegation)
    if parent_entries and parent_bridge:
        for entry in parent_entries:
            for is_by in (False, True):
                ti = entry['type_info']
                name = entry['name'] + ('By' if is_by else '')
                sig = _sig(ti, is_by)
                args = _args(is_by)
                bridge_cpp_methods.append('\n'.join([
                    f'{bridge}& {bridge}::{name}({sig})',
                    '{',
                    f'  {parent_bridge}::{name}({args});',
                    '  return *this;',
                    '}',
                ]))

    # Own properties
    for entry in own_entries:
        for is_by in (False, True):
            if entry['is_manual']:
                view_expr = f'{cls}::DownCast(mView)' if has_parent else 'mView'
                bridge_cpp_methods.append(gen_bridge_method_manual(bridge, impl, entry, is_by, view_expr))
            else:
                bridge_cpp_methods.append(gen_bridge_method_outofline(bridge, entry, is_by))

    if has_parent:
        cpp_tmpl = 'animation-bridge-child.cpp.tmpl'
    else:
        cpp_tmpl = 'animation-bridge-root.cpp.tmpl'

    _out(pub_anim_dir, f'{bridge_fn}.autogen.cpp', render(load_template(cpp_tmpl), {
        'BRIDGE_CLASS': bridge, 'VIEW_CLASS': cls,
        'PARENT_BRIDGE': parent_bridge or '',
        'BRIDGE_HEADER_PATH': f'{P}/public-api/animation/{bridge_fn}.autogen.h',
        'IMPL_INCLUDE': f'#include <{P}/internal/animation/{impl_fn}.autogen.h>' if manual_entries else '',
        'VIEW_HEADER_INCLUDE': f'#include <{view_include}>',
        'METHOD_IMPLEMENTATIONS': '\n\n'.join(bridge_cpp_methods),
    }))

    # ===== 3. Spec .autogen.h (declarations only) =====
    def _spec_decl(c, entry, is_by):
        name = entry['name'] + ('By' if is_by else '')
        return f'{c}& {name}({_sig(entry["type_info"], is_by, with_defaults=True)});'

    def _spec_parent_decl(c, parent_c, entry, is_by):
        name = entry['name'] + ('By' if is_by else '')
        return f'{c}& {name}({_sig(entry["type_info"], is_by, with_defaults=True)});'

    spec_methods = _gen_methods_block(
        own_entries, parent_entries, spec, parent_spec,
        _spec_decl, _spec_parent_decl)

    if has_parent:
        sh_tmpl = 'animation-spec-child.h.tmpl'
    else:
        sh_tmpl = 'animation-spec-root.h.tmpl'

    _out(pub_anim_dir, f'{spec_fn}.autogen.h', render(load_template(sh_tmpl), {
        'SPEC_CLASS': spec, 'VIEW_CLASS': cls, 'VIEW_INSTANCE': view_instance,
        'IMPL_CLASS': impl, 'METHODS': spec_methods,
        'PARENT_SPEC': parent_spec or '',
        'PARENT_INCLUDE': f'#include <{P}/public-api/animation/{_class_to_filename(parent_spec)}.autogen.h>' if has_parent else '',
    }))

    # ===== 4. Spec .autogen.cpp =====
    addentry_lines = []
    manual_wrapper_lines = []

    # Parent override implementations (out-of-line delegation)
    if parent_entries and parent_spec:
        for entry in parent_entries:
            for is_by in (False, True):
                ti = entry['type_info']
                name = entry['name'] + ('By' if is_by else '')
                sig = _sig(ti, is_by)
                fwd = _args(is_by)
                addentry_lines.append('\n'.join([
                    f'{spec}& {spec}::{name}({sig})',
                    '{',
                    f'  {parent_spec}::{name}({fwd});',
                    '  return *this;',
                    '}',
                ]))

    # Own property implementations (AddEntry calls)
    for entry in own_entries:
        for is_by in (False, True):
            apply_fn_override = None
            if has_parent and entry['is_manual']:
                wrapper_name = _manual_apply_wrapper_name(cls, entry, is_by)
                manual_wrapper_lines.append(gen_spec_manual_apply_wrapper(impl, root_impl_cls, root_cls, cls, entry, is_by))
                apply_fn_override = f'&{wrapper_name}'
            addentry_lines.append(gen_spec_addentry(spec, impl, entry, is_by, apply_fn_override))

    manual_wrappers = ''
    if manual_wrapper_lines:
        manual_wrappers = '\n'.join([
            'namespace',
            '{',
            '',
            '\n\n'.join(manual_wrapper_lines),
            '',
            '} // namespace',
            '',
        ])

    if has_parent:
        sc_tmpl = 'animation-spec-child.cpp.tmpl'
    else:
        sc_tmpl = 'animation-spec-root.cpp.tmpl'

    _out(pub_anim_dir, f'{spec_fn}.autogen.cpp', render(load_template(sc_tmpl), {
        'SPEC_CLASS': spec, 'VIEW_CLASS': cls, 'IMPL_CLASS': impl,
        'PARENT_SPEC': parent_spec or '',
        'SPEC_HEADER_PATH': f'{P}/public-api/animation/{spec_fn}.autogen.h',
        'IMPL_HEADER_PATH': f'{P}/internal/animation/{impl_fn}.autogen.h',
        'VIEW_HEADER_INCLUDE': f'#include <{view_include}>',
        'MANUAL_APPLY_WRAPPERS': manual_wrappers,
        'ADDENTRY_IMPLEMENTATIONS': '\n\n'.join(addentry_lines),
    }))

    # ===== 5. Impl .autogen.h =====
    manual_decls = []
    for e in own_entries:
        if e['is_manual']:
            manual_decls.append(f'  static void Apply{e["name"]}To(Animation& animation, {cls} view, const Entry& entry);')
            manual_decls.append(f'  static void Apply{e["name"]}By(Animation& animation, {cls} view, const Entry& entry);')

    if has_parent:
        tmpl_name = 'animation-spec-impl-child.h.tmpl'
        i_parent_inc = f'#include <{P}/internal/animation/{_class_to_filename(parent_impl)}.autogen.h>'
    else:
        tmpl_name = 'animation-spec-impl-root.h.tmpl'
        i_parent_inc = ''

    _out(int_anim_dir, f'{impl_fn}.autogen.h', render(load_template(tmpl_name), {
        'IMPL_CLASS': impl,
        'IMPL_PARENT': parent_impl if has_parent else 'BaseObject',
        'SPEC_CLASS': spec, 'VIEW_CLASS': cls,
        'PARENT_IMPL_INCLUDE': i_parent_inc,
        'SPEC_INCLUDE': f'#include <{P}/public-api/animation/{spec_fn}.autogen.h>',
        'MANUAL_APPLY_DECLARATIONS': '\n'.join(manual_decls),
    }))


# ---------------------------------------------------------------------------
# Scan mode
# ---------------------------------------------------------------------------

def scan_animation_configs(scan_dir):
    configs = {}
    for path in sorted(glob.glob(os.path.join(scan_dir, '**', '*.h'), recursive=True)):
        cfg = parse_animation_config(path)
        if cfg:
            configs[cfg['cls']] = cfg
    return configs


def _topo_sort(configs):
    visited, order = set(), []
    def visit(cls):
        if cls in visited:
            return
        visited.add(cls)
        parent = configs[cls].get('parent')
        if parent and parent in configs:
            visit(parent)
        order.append(cls)
    for cls in configs:
        visit(cls)
    return order


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description='Generate animation Bridge/Spec/Impl files')
    parser.add_argument('--scan-dir', default=None)
    parser.add_argument('--class', dest='cls', default=None)
    parser.add_argument('--header', default=None)

    args = parser.parse_args()

    if args.scan_dir:
        configs = scan_animation_configs(args.scan_dir)
        if not configs:
            print(f"No @ANIMATION_CONFIG tags found under {args.scan_dir}", file=sys.stderr)
            return
        order = _topo_sort(configs)
        for cls in order:
            cfg = configs[cls]
            parent = cfg.get('parent')
            parent_header = configs[parent]['header'] if parent and parent in configs else None
            print(f'[{cls}] {cfg["header"]}')
            generate_for_class(cls, cfg['header'], parent, parent_header, configs, args.scan_dir)
        return

    if not args.cls or not args.header:
        parser.error('--class and --header required (or use --scan-dir)')

    generate_for_class(args.cls, args.header)


if __name__ == '__main__':
    main()

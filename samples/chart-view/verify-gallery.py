#!/usr/bin/env python3
"""Capture the native gallery on X11; visual acceptance requires a separate review.

Requires Pillow, libX11, libXtst and an existing DISPLAY. Only the child gallery's
window is controlled. See WithAI/dali-ui/01-chart-view-improvement for scenarios.
"""

import argparse
import tempfile
import ctypes as c
import ctypes.util
import json
import os
from pathlib import Path
import re
import subprocess
import time

from PIL import ImageGrab


class WindowAttributes(c.Structure):
    _fields_ = [("x",c.c_int),("y",c.c_int),("width",c.c_int),("height",c.c_int),
                ("border_width",c.c_int),("depth",c.c_int),("visual",c.c_void_p),
                ("root",c.c_ulong),("window_class",c.c_int),("bit_gravity",c.c_int),
                ("win_gravity",c.c_int),("backing_store",c.c_int),("backing_planes",c.c_ulong),
                ("backing_pixel",c.c_ulong),("save_under",c.c_int),("colormap",c.c_ulong),
                ("map_installed",c.c_int),("map_state",c.c_int),("all_event_masks",c.c_long),
                ("your_event_mask",c.c_long),("do_not_propagate_mask",c.c_long),
                ("override_redirect",c.c_int),("screen",c.c_void_p)]


class NativeWindow:
    """Own a gallery process and address its window from its own runtime log."""

    def __init__(self, executable, output, case_id, freeze=True):
        self.output = Path(output).resolve()
        self.output.mkdir(parents=True, exist_ok=True)
        self.state_path = self.output / "live-state.json"
        self.state_path.unlink(missing_ok=True)
        self.log_path = self.output / "runtime.log"
        self.log = self.log_path.open("w")
        self.process = subprocess.Popen(
            [str(executable), f"--case={case_id}",
             *(["--freeze-animations"] if freeze else []),
             f"--dump-state={self.state_path}"], stdout=self.log, stderr=self.log,
            env=dict(os.environ, GCOV_PREFIX=tempfile.mkdtemp(prefix="chart-gallery-gcov-"))
        )
        (self.output / "process-id.txt").write_text(str(self.process.pid) + "\n")
        self.display = None
        self.window = None
        try:
            self._connect()
            self.wait_state(case_id or "vertical-bar-01", False)
            attributes=WindowAttributes()
            self.x.XGetWindowAttributes.argtypes=[c.c_void_p,c.c_ulong,c.POINTER(WindowAttributes)]
            deadline=time.monotonic()+10
            while time.monotonic()<deadline:
                self.x.XGetWindowAttributes(self.display,self.window,c.byref(attributes))
                if attributes.map_state==2:
                    break
                time.sleep(0.05)
            else:
                raise TimeoutError(f"Window {self.window} not viewable: {attributes.map_state}")
            self.x.XSetInputFocus(self.display, self.window, 2, 0)
            self.x.XFlush(self.display)
        except BaseException:
            self.close()
            raise

    def _connect(self):
        deadline = time.monotonic() + 20
        while time.monotonic() < deadline:
            matches = re.findall(r"WinId\s*\((\d+)\)", self.log_path.read_text(errors="replace"))
            if matches:
                self.window = int(matches[-1])
                break
            if self.process.poll() is not None:
                raise RuntimeError(f"Gallery exited: {self.process.returncode}; {self.log_path}")
            time.sleep(0.05)
        if self.window is None:
            raise TimeoutError(f"No native window in {self.log_path}")
        self.x = c.CDLL(ctypes.util.find_library("X11"))
        self.xt = c.CDLL(ctypes.util.find_library("Xtst"))
        display, window = c.c_void_p, c.c_ulong
        signatures = {
            "XOpenDisplay": ([c.c_char_p], display),
            "XCloseDisplay": ([display], c.c_int),
            "XDefaultRootWindow": ([display], window),
            "XSetInputFocus": ([display, window, c.c_int, c.c_ulong], c.c_int),
            "XRaiseWindow": ([display, window], c.c_int),
            "XFlush": ([display], c.c_int),
            "XStringToKeysym": ([c.c_char_p], c.c_ulong),
            "XKeysymToKeycode": ([display, c.c_ulong], c.c_uint),
            "XResizeWindow": ([display, window, c.c_uint, c.c_uint], c.c_int),
            "XGetGeometry": ([display, window, c.POINTER(window), c.POINTER(c.c_int),
                              c.POINTER(c.c_int), c.POINTER(c.c_uint), c.POINTER(c.c_uint),
                              c.POINTER(c.c_uint), c.POINTER(c.c_uint)], c.c_int),
            "XTranslateCoordinates": ([display, window, window, c.c_int, c.c_int,
                                       c.POINTER(c.c_int), c.POINTER(c.c_int),
                                       c.POINTER(window)], c.c_int),
        }
        for name, (args, result) in signatures.items():
            method = getattr(self.x, name)
            method.argtypes, method.restype = args, result
        self.xt.XTestFakeKeyEvent.argtypes = [display, c.c_uint, c.c_int, c.c_ulong]
        self.display = self.x.XOpenDisplay(os.fsencode(os.environ.get("DISPLAY", "")))
        if not self.display:
            raise RuntimeError("Cannot open DISPLAY")
        self.root = self.x.XDefaultRootWindow(self.display)
        self.x.XRaiseWindow(self.display, self.window)
        self.x.XFlush(self.display)

    def key(self, name, state=0):
        code = self.x.XKeysymToKeycode(self.display, self.x.XStringToKeysym(name.encode()))
        if not code:
            raise ValueError(f"Unknown key: {name}")
        # Address the owned client directly so desktop focus changes cannot lose keys.
        class KeyEvent(c.Structure):
            _fields_ = [("type",c.c_int),("serial",c.c_ulong),("send_event",c.c_int),
                        ("display",c.c_void_p),("window",c.c_ulong),("root",c.c_ulong),
                        ("subwindow",c.c_ulong),("time",c.c_ulong),("x",c.c_int),("y",c.c_int),
                        ("x_root",c.c_int),("y_root",c.c_int),("state",c.c_uint),
                        ("keycode",c.c_uint),("same_screen",c.c_int)]
        class Event(c.Union):
            _fields_ = [("key",KeyEvent),("padding",c.c_long*24)]
        self.x.XSendEvent.argtypes=[c.c_void_p,c.c_ulong,c.c_int,c.c_long,c.POINTER(Event)]
        for kind,mask in [(2,1),(3,2)]:
            event=Event()
            event.key=KeyEvent(type=kind,display=self.display,window=self.window,
                               root=self.root,keycode=code,state=state,same_screen=1)
            self.x.XSendEvent(self.display,self.window,0,mask,c.byref(event))
        self.x.XFlush(self.display)

    def click(self, x, y):
        left,top,child=c.c_int(),c.c_int(),c.c_ulong()
        self.x.XTranslateCoordinates(self.display,self.window,self.root,0,0,
                                     c.byref(left),c.byref(top),c.byref(child))
        self.x.XSetInputFocus(self.display,self.window,2,0)
        self.xt.XTestFakeMotionEvent.argtypes=[c.c_void_p,c.c_int,c.c_int,c.c_int,c.c_ulong]
        self.xt.XTestFakeButtonEvent.argtypes=[c.c_void_p,c.c_uint,c.c_int,c.c_ulong]
        self.xt.XTestFakeMotionEvent(self.display,0,left.value+x,top.value+y,0)
        self.xt.XTestFakeButtonEvent(self.display,1,1,0)
        self.xt.XTestFakeButtonEvent(self.display,1,0,0)
        self.x.XFlush(self.display)
        time.sleep(0.35)

    def drag(self, start, end):
        left,top,child=c.c_int(),c.c_int(),c.c_ulong()
        self.x.XTranslateCoordinates(self.display,self.window,self.root,0,0,c.byref(left),c.byref(top),c.byref(child))
        self.xt.XTestFakeMotionEvent.argtypes=[c.c_void_p,c.c_int,c.c_int,c.c_int,c.c_ulong]
        self.xt.XTestFakeButtonEvent.argtypes=[c.c_void_p,c.c_uint,c.c_int,c.c_ulong]
        self.xt.XTestFakeMotionEvent(self.display,0,left.value+start[0],top.value+start[1],0)
        self.xt.XTestFakeButtonEvent(self.display,1,1,0);self.x.XFlush(self.display)
        for step in range(1,9):
            x=round(start[0]+(end[0]-start[0])*step/8)
            y=round(start[1]+(end[1]-start[1])*step/8)
            self.xt.XTestFakeMotionEvent(self.display,0,left.value+x,top.value+y,0)
            self.x.XFlush(self.display);time.sleep(.04)
        self.xt.XTestFakeButtonEvent(self.display,1,0,0);self.x.XFlush(self.display)
        time.sleep(.35)

    def resize(self, width, height):
        self.x.XResizeWindow(self.display, self.window, width, height)
        self.x.XFlush(self.display)

    def wait_state(self, case_id, dark, **expected):
        deadline = time.monotonic() + 12
        last = None
        while time.monotonic() < deadline:
            if self.process.poll() is not None:
                raise RuntimeError(f"Gallery exited: {self.process.returncode}")
            try:
                last = json.loads(self.state_path.read_text())
                if (last.get("id") == case_id and last.get("dark") == dark
                        and last.get("ready") and all(last.get(k) == v for k, v in expected.items())):
                    return last
            except (FileNotFoundError, json.JSONDecodeError):
                pass
            time.sleep(0.025)
        raise TimeoutError(f"No ready state for {case_id}, dark={dark}, {expected}; last={last}")

    def capture(self, target, settle=0.15):
        # ContentUpdatedSignal is not a GPU presentation fence. Give X11 time to present.
        time.sleep(settle)
        root, child = c.c_ulong(), c.c_ulong()
        x, y, left, top = c.c_int(), c.c_int(), c.c_int(), c.c_int()
        width, height, border, depth = c.c_uint(), c.c_uint(), c.c_uint(), c.c_uint()
        if not self.x.XGetGeometry(self.display, self.window, c.byref(root), c.byref(x),
                                   c.byref(y), c.byref(width), c.byref(height), c.byref(border), c.byref(depth)):
            raise RuntimeError("Cannot read native window geometry")
        self.x.XTranslateCoordinates(self.display, self.window, self.root, 0, 0,
                                     c.byref(left), c.byref(top), c.byref(child))
        ImageGrab.grab(bbox=(left.value, top.value, left.value + width.value,
                             top.value + height.value), xdisplay=os.environ.get("DISPLAY")).save(target)
        return [width.value, height.value]

    def close(self):
        if self.process.poll() is None:
            if self.display and self.window:
                self.key("Escape")
            try:
                self.process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                self.process.terminate()
                try:
                    self.process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    self.process.kill()
                    self.process.wait()
        if self.display:
            self.x.XCloseDisplay(self.display)
            self.display = None
        self.log.close()


def run_overviews(executable, output):
    output.mkdir(parents=True,exist_ok=True)
    window=NativeWindow(executable,output,"")
    states=[]
    try:
        for width,height,card_width in [(480,800,448),(1440,1000,674),(1920,1000,600)]:
            window.resize(width,height)
            state=window.wait_state("vertical-bar-01",False,cardWidth=card_width)
            time.sleep(.5)
            window.capture(output/f"gallery-{width}.png")
            states.append(dict(width=width,status="PASS",state=state))
        listing=subprocess.check_output([str(executable),"--list-cases"],text=True)
        groups=[];last=None
        for line in listing.splitlines():
            if line.count("\t")!=2:continue
            case_id,category,title=line.split("\t",2)
            if category!=last:groups.append(case_id);last=category
        for _ in range(2):
            for case_id in groups[1:]:
                window.key("n");window.wait_state(case_id,False)
            for case_id in reversed(groups[:-1]):
                window.key("p");window.wait_state(case_id,False)
        states.append(dict(status="PASS",category_moves=4*(len(groups)-1)))
    finally:
        window.close()
        diagnostics=read_diagnostics(window.log_path)
        (output/"overview.json").write_text(json.dumps(dict(states=states,exit_code=window.process.returncode,diagnostics=diagnostics),ensure_ascii=False,indent=2)+"\n")
    if window.process.returncode or diagnostics:raise RuntimeError("Overview diagnostics require review")


def run_scenarios(executable, output):
    results=[]
    def run(case_id, action):
        window=NativeWindow(executable,output/case_id,case_id)
        try:
            window.wait_state(case_id,False)
            result=action(window)
            window.capture(output/case_id/"result.png")
        except BaseException:
            window.capture(output/case_id/"failure.png")
            raise
        finally:
            window.close()
        diagnostics=read_diagnostics(window.log_path)
        if window.process.returncode or diagnostics:
            raise RuntimeError(f"Gallery diagnostics for {case_id}: exit={window.process.returncode}, {diagnostics}")
        results.append(dict(id=case_id,status="PASS",state=result,exit_code=window.process.returncode,diagnostics=diagnostics))
    def selection(w):
        w.key("Right");w.wait_state("vertical-bar-01",False,hasSelection=True,selected=0)
        w.key("Right");before=w.wait_state("vertical-bar-01",False,selected=1)
        w.key("t");after=w.wait_state("vertical-bar-01",True,selected=1)
        assert after["selectionText"]==before["selectionText"]
        return after
    def page(w):
        w.key("Next");state=w.wait_state("line-14",False,page=1)
        assert state["viewport"][0]==5.5 and state["viewport"][2]==8.5
        return state
    def tabs(w):
        w.click(542,242);w.wait_state("custom-06",False,period=1,points=20)
        w.click(632,242);w.wait_state("custom-06",False,period=2,points=31)
        w.key("t");state=w.wait_state("custom-06",True,period=2,points=31)
        assert state["viewport"][3]==800
        return state
    def swipe(w):
        w.drag((800,390),(630,390))
        return w.wait_state("vertical-bar-23",False,page=1)
    def scroll(w):
        before=w.wait_state("vertical-bar-22",False)
        w.drag((800,390),(630,390))
        deadline=time.monotonic()+5
        while time.monotonic()<deadline:
            state=w.wait_state("vertical-bar-22",False)
            if state["viewport"][0]>before["viewport"][0]:
                initial=before["viewport"]
                current=state["viewport"]
                assert abs((current[2]-current[0])-(initial[2]-initial[0]))<.001
                assert current[1]==initial[1] and current[3]==initial[3]
                return state
            time.sleep(.05)
        raise AssertionError("Horizontal pan did not move the viewport")
    def number(w):
        def edit(text):
            w.click(513,628);w.key("a",4);w.key("BackSpace")
            for char in text:w.key(char)
            w.click(618,628)
        edit("nan")
        w.wait_state("interaction-01",False,numberInput="nan",numberError="Enter a finite number.")
        edit("3500")
        w.wait_state("interaction-01",False,metric=3500,numberInput="3500",numberError="")
        w.key("t");return w.wait_state("interaction-01",True,metric=3500,numberInput="3500")
    output.mkdir(parents=True,exist_ok=True)
    try:
        for case_id,action in [("vertical-bar-01",selection),("line-14",page),("custom-06",tabs),("interaction-01",number),("vertical-bar-23",swipe),("vertical-bar-22",scroll)]:
            run(case_id,action)
            print("SCENARIO PASS",case_id,flush=True)
    finally:
        (output/"scenarios.json").write_text(json.dumps(results,ensure_ascii=False,indent=2)+"\n")


def read_diagnostics(path):
    return [line for line in path.read_text(errors="replace").splitlines()
            if re.search(r"LogParkedWorkOnce|\b(?:WARNING|ERROR|Assert(?:ion)?)\b",line,re.I)]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", type=Path, default=Path(__file__).resolve().parent / "bin/chart-standard-gallery.example")
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--size", default="1440x1000")
    parser.add_argument("--overview",action="store_true",help="Check gallery columns and repeated category navigation")
    parser.add_argument("--scenarios",action="store_true",help="Exercise selection, pages, tabs and numeric input")
    parser.add_argument("--cases", nargs="+", help="Stable case IDs; omit for all cases")
    args = parser.parse_args()
    executable = args.executable.resolve()
    if args.overview:
        run_overviews(executable,args.output)
        return
    if args.scenarios:
        run_scenarios(executable,args.output)
        return
    listing = subprocess.check_output([str(executable), "--list-cases"], text=True,
                                      env=dict(os.environ,GCOV_PREFIX=tempfile.mkdtemp(prefix="chart-list-gcov-")))
    cases = [line.split("\t", 2) for line in listing.splitlines()]
    selected = set(args.cases or [case[0] for case in cases])
    unknown = selected - {case[0] for case in cases}
    if unknown:
        parser.error(f"Unknown cases: {sorted(unknown)}")
    first=next(i for i,case in enumerate(cases) if case[0] in selected)
    last=max(i for i,case in enumerate(cases) if case[0] in selected)
    cases=cases[first:last+1]
    width, height = map(int, args.size.split("x"))
    args.output.mkdir(parents=True, exist_ok=True)
    results = []
    window = NativeWindow(executable, args.output, cases[0][0])
    try:
        dark = False
        window.wait_state(cases[0][0], dark)
        window.resize(width, height)
        time.sleep(0.3)
        for index, (case_id, category, title) in enumerate(cases):
            state = window.wait_state(case_id, dark)
            if case_id in selected:
                for _ in range(2):
                    state = window.wait_state(case_id, dark)
                    stem = f"{case_id}-{'dark' if dark else 'light'}-{width}"
                    actual_size = window.capture(args.output / f"{stem}.png")
                    (args.output / f"{stem}.json").write_text(json.dumps(state, ensure_ascii=False, indent=2) + "\n")
                    results.append(dict(id=case_id, category=category, title=title, dark=dark,
                                        status="CAPTURED", image=f"{stem}.png", size=actual_size))
                    print(f"CAPTURED {stem}", flush=True)
                    if len(results) % 2:
                        window.key("t")
                        dark = not dark
            if index + 1 < len(cases):
                window.key("F6")
        report = dict(status="CAPTURED", visual_acceptance="NOT_REVIEWED", cases=len(selected),
                      captures=results, size=[width, height])
    except BaseException as error:
        report = dict(status="FAILED", error=str(error), captures=results)
        raise
    finally:
        window.close()
        report = locals().get("report", dict(status="FAILED", captures=results))
        report["exit_code"] = window.process.returncode
        report["diagnostics"] = read_diagnostics(window.log_path)
        (args.output / "report.json").write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n")
    if report["exit_code"] or report["diagnostics"]:
        raise RuntimeError("Gallery capture diagnostics require review")


if __name__ == "__main__":
    main()

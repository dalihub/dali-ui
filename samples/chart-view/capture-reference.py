#!/usr/bin/env python3
"""Capture matching reference cards from a Chrome DevTools page on localhost.

Start a dedicated Chrome with --remote-debugging-port=9333 first. The script
navigates that page and reads the original site; it does not publish anything.
"""
import argparse
import tempfile
import subprocess
from pathlib import Path
import socket,struct,json,os,base64,urllib.request,time

class CDP:
    def __init__(self, port=9333):
        pages=json.load(urllib.request.urlopen(f'http://127.0.0.1:{port}/json'))
        url=next(p['webSocketDebuggerUrl'] for p in pages if p['type']=='page')
        host,path=url[5:].split('/',1)
        self.socket=socket.create_connection((host.split(':')[0],int(host.split(':')[1])),timeout=30)
        key=base64.b64encode(os.urandom(16)).decode()
        self.socket.sendall(f'GET /{path} HTTP/1.1\r\nHost: {host}\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: {key}\r\nSec-WebSocket-Version: 13\r\n\r\n'.encode())
        response=b''
        while b'\r\n\r\n' not in response:
            response+=self.socket.recv(1)
        assert b'101' in response,response
        self.sequence=0
    def read(self,length):
        result=b''
        while len(result)<length:
            part=self.socket.recv(length-len(result))
            if not part: raise EOFError()
            result+=part
        return result
    def frame(self):
        first,second=self.read(2); length=second&127
        if length==126:length=struct.unpack('!H',self.read(2))[0]
        elif length==127:length=struct.unpack('!Q',self.read(8))[0]
        mask=self.read(4) if second&128 else None
        data=self.read(length)
        if mask:data=bytes(v^mask[i%4] for i,v in enumerate(data))
        return first&15,data
    def call(self, method, params=None):
        self.sequence+=1
        payload=json.dumps(dict(id=self.sequence,method=method,params=params or {})).encode()
        length=len(payload); mask=os.urandom(4)
        head=bytes([129,128|length]) if length<126 else bytes([129,254])+struct.pack('!H',length) if length<65536 else bytes([129,255])+struct.pack('!Q',length)
        self.socket.sendall(head+mask+bytes(v^mask[i%4] for i,v in enumerate(payload)))
        while True:
            opcode,data=self.frame()
            if opcode==1:
                response=json.loads(data)
                if response.get('id')==self.sequence:
                    if 'error' in response:raise RuntimeError(response['error'])
                    return response.get('result',{})
    def evaluate(self,expression):
        result=self.call('Runtime.evaluate',dict(expression=expression,returnByValue=True,awaitPromise=True))
        if 'exceptionDetails' in result:raise RuntimeError(result)
        return result['result'].get('value')



def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--port',type=int,default=9333)
    parser.add_argument('--url',default='https://pages.github.sec.samsung.net/SRIN-ST/standard-chart/')
    parser.add_argument('--executable',type=Path,default=Path(__file__).resolve().parent/'bin/chart-standard-gallery.example')
    args=parser.parse_args()
    args.output.mkdir(parents=True,exist_ok=True)
    listing=subprocess.check_output([str(args.executable),'--list-reference-cases'],text=True,env=dict(os.environ,GCOV_PREFIX=tempfile.mkdtemp(prefix='chart-reference-list-gcov-')))
    cases=[line.split('\t',2) for line in listing.splitlines() if line.count('\t')==2]
    c=CDP(args.port)
    c.call('Page.enable')
    c.call('Emulation.setDeviceMetricsOverride',dict(width=1440,height=1000,deviceScaleFactor=1,mobile=False))
    c.call('Page.addScriptToEvaluateOnNewDocument',dict(source='let chartSeed=1009;Math.random=()=>{chartSeed=(Math.imul(1664525,chartSeed)+1013904223)>>>0;return chartSeed/4294967296};'))
    c.call('Page.navigate',dict(url=args.url))
    for _ in range(150):
        if c.evaluate('document.querySelectorAll(".chart-section").length')==137:
            break
        time.sleep(.1)
    else:
        raise RuntimeError('Expected 137 reference cards')
    c.evaluate('document.fonts.ready.then(()=>true)')
    report=[]
    for theme in ['light','dark']:
        if c.evaluate('document.documentElement.dataset.colorScheme')!=theme:
            c.evaluate('document.querySelector("#dark-btn").click()')
            time.sleep(.3)
        assert c.evaluate('document.documentElement.dataset.colorScheme')==theme
        for case_id,category,title in cases:
            selector='Array.from(document.querySelectorAll(".chart-section")).find(x=>x.querySelector("h2,h3")?.textContent.trim()==='+json.dumps(title)+')'
            result=c.evaluate('(()=>{const card='+selector+';if(!card)return null;card.scrollIntoView({block:"center"});return true})()')
            if not result:
                report.append(dict(id=case_id,theme=theme,status='MISSING'))
                continue
            time.sleep(.15)
            # Match the native --freeze-animations capture: draw the final chart
            # frame after scrolling, including reference examples initially offscreen.
            c.evaluate('(()=>{const card='+selector+';for(const element of card.querySelectorAll("*")){const chart=element._chart;if(chart&&typeof chart.update==="function"){chart.stop();chart.update("none")}}return true})()')
            time.sleep(.1)
            rect=c.evaluate('(()=>{const r=('+selector+').getBoundingClientRect();return {x:r.x+scrollX,y:r.y+scrollY,width:r.width,height:r.height,scale:1}})()')
            data=c.call('Page.captureScreenshot',dict(format='png',captureBeyondViewport=True,clip=rect))
            filename=f'{case_id}-{theme}.png'
            (args.output/filename).write_bytes(base64.b64decode(data['data']))
            report.append(dict(id=case_id,title=title,theme=theme,status='CAPTURED',image=filename,rect=rect))
            print('CAPTURED',filename,flush=True)
    (args.output/'report.json').write_text(json.dumps(dict(url=args.url,animation="Chart.js stop + update(none) after scroll",captures=report),ensure_ascii=False,indent=2)+'\n')
    c.socket.close()

if __name__=='__main__':
    main()

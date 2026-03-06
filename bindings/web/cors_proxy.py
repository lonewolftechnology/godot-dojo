import http.server
import socketserver
import urllib.request
import urllib.error
import shutil
import sys

PORT = 8000
TARGET_API = "https://api.cartridge.gg"

class ProxyHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def do_POST(self):
        self.handle_proxy_request('POST')

    def do_GET(self):
        if self.path.startswith("/cartridge_api/"):
            self.handle_proxy_request('GET')
        else:
            super().do_GET()

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', '*')
        self.end_headers()

    def handle_proxy_request(self, method):
        if self.path.startswith("/cartridge_api/"):
            target_path = self.path.replace("/cartridge_api/", "/")
            url = TARGET_API + target_path
            
            print(f"Proxying {method} {self.path} -> {url}")

            post_data = None
            if method == 'POST':
                content_length = int(self.headers.get('Content-Length', 0))
                post_data = self.rfile.read(content_length)
            
            req = urllib.request.Request(url, data=post_data, method=method)
            
            # Copiar headers esenciales
            # Importante: Content-Type es vital para que la API sepa que es JSON
            for key, value in self.headers.items():
                if key.lower() in ['content-type', 'accept', 'authorization']:
                    req.add_header(key, value)
            
            # User-Agent de Chrome estándar para evitar bloqueos por bot
            req.add_header('User-Agent', 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36')
            req.add_header('Accept', 'application/json')

            try:
                with urllib.request.urlopen(req) as response:
                    self.send_response(response.status)
                    for key, value in response.headers.items():
                        if key.lower() not in ['access-control-allow-origin', 'content-encoding', 'content-length', 'transfer-encoding']:
                            self.send_header(key, value)
                    
                    self.send_header('Access-Control-Allow-Origin', '*')
                    self.end_headers()
                    shutil.copyfileobj(response, self.wfile)
                    
            except urllib.error.HTTPError as e:
                print(f"Remote API Error: {e.code}")
                # Intentar leer el cuerpo del error para ver qué dice la API
                error_body = e.read()
                print(f"Error Body: {error_body.decode('utf-8', errors='ignore')}")
                
                self.send_response(e.code)
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()
                self.wfile.write(error_body)
            except Exception as e:
                print(f"Proxy Error: {e}")
                self.send_error(500, str(e))
        else:
            self.send_error(404, "Not Found")

print(f"Iniciando servidor en http://localhost:{PORT}")
socketserver.TCPServer.allow_reuse_address = True

try:
    with socketserver.TCPServer(("", PORT), ProxyHTTPRequestHandler) as httpd:
        httpd.serve_forever()
except KeyboardInterrupt:
    print("\nServidor detenido.")

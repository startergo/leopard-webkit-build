#!/bin/bash
# Install the TLS 1.2 shim so WebKit.app's Safari negotiates AES-GCM / TLS 1.2
# sites (10.6's stock Security stack only does TLS 1.0). WebKit.app's launcher
# auto-loads it from /usr/local/lib when present.
DIR="$(cd "$(dirname "$0")" && pwd)"
echo "Installing TLS 1.2 shim to /usr/local/lib (requires your admin password)..."
sudo mkdir -p /usr/local/lib
sudo cp "$DIR/TLS12/libsecurity_ssl_tls12.dylib" /usr/local/lib/libsecurity_ssl_tls12.dylib
echo "Done. Launch WebKit.app — HTTPS sites needing modern ciphers now work."

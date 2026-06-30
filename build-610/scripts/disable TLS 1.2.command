#!/bin/bash
echo "Removing the TLS 1.2 shim (requires your admin password)..."
sudo rm -f /usr/local/lib/libsecurity_ssl_tls12.dylib
echo "Done. WebKit.app falls back to stock 10.6 TLS 1.0."

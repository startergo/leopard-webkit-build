# Bundled TLS 1.2 shim — libsecurity_ssl_tls12.dylib

`libsecurity_ssl_tls12.dylib` is a prebuilt **release binary**, not source. It
backports TLS 1.2 (AES-GCM, ECDSA, SHA-2 sig verify, session resumption) into
Apple's `libsecurity_ssl-55002` for Mac OS X 10.6.8 Snow Leopard (x86_64).

## Provenance
- Source project: `tls12-snow-leopard-merge` (separate repo; the merge target
  `sources/libsecurity_ssl-55002/` is committed there).
- Built with: `scripts/build-libsecurity-ssl.sh --patched`
- Install id: `/usr/local/lib/libsecurity_ssl_tls12.dylib`
- Arch: x86_64 (single-arch; matches WebKit.app's `arch -x86_64` Safari launch)
- Links only stock 10.6 system libs (Security, CoreFoundation, libSystem) and
  privately dlopens `libcrypto.0.9.8`.

## How it is used
WebKit.app's launcher sets `DYLD_INSERT_LIBRARIES=/usr/local/lib/libsecurity_ssl_tls12.dylib`
if that file exists, so its load-time constructor patches the SSL functions in
the Safari process. The DMG's "enable TLS 1.2.command" copies this dylib to
/usr/local/lib; "disable TLS 1.2.command" removes it.

To update this binary, rebuild it in tls12-snow-leopard-merge and re-copy.

# Hello-TLSSocket
A simple HTTPS example program for TLSSocket.

## Build
```
$ mbed import https://github.com/coisme/Hello-TLSSocket
$ mbed compile -t GCC_ARM -m K64F
```

## Sample Output

```
HelloTSLSocket, HTTPS example of TLSSocket
Connecting to network
Synchronizing time
Time set.
[INFO][TLSW]: mbedtls_ssl_config_defaults()
[INFO][TLSW]: mbedtls_ssl_conf_ca_chain()
[INFO][TLSW]: mbedtls_ssl_conf_rng()
[INFO][TLSW]: mbedtls_ssl_conf_authmode()
[INFO][TLSW]: mbedtls_ssl_setup()
[INFO][TLSW]: Starting TLS handshake with os.mbed.com
[INFO][TLSW]: TLS connection to os.mbed.com established

[DBG ][TLSW]: Server certificate:
    cert. version     : 3
    serial number     : 03:56:D4:79:41:63:31:CA:E0:56:06:61
    issuer name       : C=BE, O=GlobalSign nv-sa, CN=GlobalSign Organization Validation CA - SHA256 - G2
    subject name      : C=GB, ST=Cambridgeshire, L=Cambridge, O=Arm Ltd, CN=*.mbed.com
    issued  on        : 2018-05-04 15:36:03
    expires on        : 2019-06-06 10:31:02
    signed using      : RSA with SHA-256
    RSA key size      : 2048 bits
    basic constraints : CA=false
    subject alt name  : *.mbed.com, mbed.org, *.mbed.org, mbed.com
    key usage         : Digital Signature, Key Encipherment
    ext key usage     : TLS Web Server Authentication, TLS Web Client Authentication


[INFO][TLSW]: Certificate verification passed
Connected to os.mbed.com
socket_state_handler() next_state = 0
[DBG ][TLSW]: send 90
send(): GET /media/uploads/mbed_official/hello.txt HTTP/1.1
Host: os.mbed.com
Connection: close


socket_state_handler() next_state = 2
recv() HTTP/1.1 200 OK
Server: nginx/1.15.2
Date: Tue, 21 Aug 2018 07:29:15 GMT
Content-Type: text/plain
Content-Length: 14
Last-Modified: Fri, 27 Jul 2012 13:30:34 GMT
Connection: close
ETag: "501297fa-e"
Expires: Tue, 21 Aug 2018 17:29:15 GMT
Cache-Control: max-age=36000
Accept-Ranges: bytes

Hello world!

socket_state_handler() next_state = 3
[INFO][TLSW]: Closing TLS
connection closed! received total 315 bytes
socket_state_handler() next_stat
HelloTSLSocket DONE.
e = 4
socket_state_handler() next_state = 4
```

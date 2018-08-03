#include "mbed.h"
#include "easy-connect.h"
#include "TLSSocket.h"
#include "NTPClient.h"
#include "mbed-trace/mbed_trace.h"

const char* HOST_NAME = "os.mbed.com";
const int PORT = 443;
const char* HTTPS_PATH = "/media/uploads/mbed_official/hello.txt";

int main(int argc, char* argv[]) {
    mbed_trace_init();

    printf("HelloTSLSocket, HTTPS example of TLSSocket\n");

    // Open a network interface
    NetworkInterface* network = NULL;
    network = easy_connect(true);    // If true, prints out connection details.
    if (!network) {
        printf("Unable to open network interface.\n");
        return -1;
    }

    NTPClient ntp(network);
    ntp.set_server("time.google.com", 123);
    time_t now = ntp.get_timestamp();
    set_time(now);

    printf("Time set.\n");

    // Create a TLS socket
    TLSSocket socket(network);

    // Set root CA certificate
    socket.set_root_ca_cert(MBED_CONF_APP_ROOT_CA_CERT_PEM);

    // Connect to the server, including TLS handshake
    if(socket.connect(HOST_NAME, PORT) != 0) {
        printf("Failed to connect to the server.\n");
        return -1;
    }
    
    const size_t buf_size = 1024;
    char *buf = new char[buf_size];

    // Send HTTP request
    /* "Connection: close" header is specified to detect end of the body
     * contents by connection close notification. If this is not specified,
     * connection is kept, and need to detect end of the content in another
     * way.
     */
    int len = snprintf(buf, buf_size, 
                "GET %s HTTP/1.1\n"
                "Host: %s\n"
                "Connection: close\n"
                "\n", HTTPS_PATH, HOST_NAME);
    printf("\r\n%s", buf);
    int rc = 0;
    rc = socket.send(buf, len);
    if(rc < 0) {
        printf("send error.\n");
        return -1;
    }

    // Receive response from the server
    while((rc = socket.recv(buf, buf_size - 1)) > 0) {
        buf[rc] = '\0';
        printf("%s", buf);
    }
    if(rc < 0) {
        printf("\r\n! Read failed. err code = %d\n", rc);
    }

    // Done
    printf("\n");
    printf("HelloTSLSocket DONE.\n");
    delete[] buf;
    
    socket.close();
}

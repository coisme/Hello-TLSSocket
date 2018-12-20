#include "mbed.h"
#include "TLSSocket.h"
#include "NTPClient.h"
#include "mbed-trace/mbed_trace.h"

const char* HOST_NAME = "os.mbed.com";
const int PORT = 443;
const char* HTTPS_PATH = "/media/uploads/mbed_official/hello.txt";

static void trace_printer(const char* str)
{
    printf("%s\r\n", str);
}

void socket_state_handler(EventFlags *evt, Socket *socket)
{
    static char *buf;
    static int query_len;
    static int sent;
    static int received;
    int ret;
    static enum {
        INITIALIZING,
        //CONNECTING,
        SEND,
        RECEIVE,
        CLOSE,
        DONE,
    } next_state = INITIALIZING;

    printf("socket_state_handler() next_state = %d\n", next_state);

    switch (next_state) {
        case INITIALIZING:
            buf = new char[1024];
            query_len = snprintf(buf, 1024,
                "GET %s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Connection: close\r\n"
                "\r\n", HTTPS_PATH, HOST_NAME);
            sent = 0;
            received = 0;
            next_state = SEND;
            // Flow through to SEND

        case SEND:
            ret = socket->send(buf+sent, query_len-sent);
            if (ret == NSAPI_ERROR_WOULD_BLOCK) {
                break;
            } else if (ret < 0) {
                next_state = CLOSE; // Error
                break;
            }
            sent += ret;
            if (sent == query_len) {
                next_state = RECEIVE;
                printf("send(): %s\n", buf);
            }
            else
                next_state = SEND;
            break;

        case RECEIVE:
            do {
                ret = socket->recv(buf, 1023);
                if (ret == NSAPI_ERROR_WOULD_BLOCK) {
                    break;
                } else if (ret <= 0) {
                    next_state = CLOSE; // Error or connection closed
                    break;
                }
                received += ret;
                buf[ret] = 0;
                printf("recv() %s\n", buf);
            } while (ret > 0); // Need to read until WOULD_BLOCK is received, otherwise the state machine stops

            break;
        case CLOSE:
            socket->sigio(NULL); // Remove the event handler, we don't want spurious close events anymore.
            socket->close();
            printf("connection closed! received total %d bytes\n", received);
            evt->set(1); // Signal the main thread
            delete[] buf;
            next_state = DONE;
            break;
        case DONE:
            // Nothing to do, just allow queued events to flow through
            break;
    }
}


int main(int argc, char* argv[]) {
    mbed_trace_init();
    mbed_trace_print_function_set( trace_printer );

    printf("Hello-TLSSocket, HTTPS example of TLSSocket\n");

    // Open a network interface
    NetworkInterface* network = NetworkInterface::get_default_instance();
    if (!network) {
        printf("Error! No network inteface found.\n");
        return -1;
    }

    printf("Connecting to network\n");
    nsapi_size_or_error_t ret = network->connect();
    if (ret) {
        printf("Unable to connect! returned %d\n", ret);
        return -1;
    }

    printf("Synchronizing time\n");
    NTPClient ntp(network);
    ntp.set_server("time.google.com", 123);
    time_t now = ntp.get_timestamp();
    set_time(now);

    printf("Time set: %s\n", ctime(&now));

    // Create and open a TLS socket, allocate heap memory since TLS process may need large memory area.
    TLSSocket *socket = new TLSSocket;
    if(socket->open(network) != NSAPI_ERROR_OK) {
        printf("Failed to open the socket.\n");
        return -1;
    }

    // Set root CA certificate
    socket->set_root_ca_cert(MBED_CONF_APP_ROOT_CA_CERT_PEM);

    // Connect to the server, including TLS handshake
    if(socket->connect(HOST_NAME, PORT) != 0) {
        printf("Failed to connect to the server.\n");
        return -1;
    }
    printf("Connected to %s\n", HOST_NAME);

    EventFlags completed;
    EventQueue *queue = mbed_event_queue();
    Event<void()> handler = queue->event(socket_state_handler, &completed, socket);
    socket->set_blocking(false);
    socket->sigio(handler);
    handler();                   // Kick the state machine to start connecting

    completed.wait_any(1);

    // Close the socket and free memory.
    delete socket;

    // Done
    printf("\n");
    printf("HelloTSLSocket DONE.\n");
}

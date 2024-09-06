#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "flags.h"
#include "ping.h"
#include "print.h"
#include "time.h"

static void handle_sigint(int sig);
static inline bool is_timeout(flags_t flags, uint32_t start_time_in_seconds);

static bool is_running = true;
static icmp_ping_t ping = {0};

int main(int argc, char *argv[]) {
  flags_t flags = {0};

  int8_t return_code = parse_flags(argc, argv, &flags);
  if (return_code < 0) {
    return 1;
  } else if (return_code == 0) {
    return 0;
  }

  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handle_sigint;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("ft_ping: sigaction");
  }

  if (init_icmp_ping(&ping, flags) == -1) {
    return 1;
  }

  ping_result_t result;
  print_ping_start(&ping, &flags);
  uint32_t start_time = get_current_time_in_seconds();
  result = icmp_ping(&ping, flags);
  print_ping_result(&result, &flags);
  while (is_running) {
    if (is_timeout(flags, start_time)) {
      break;
    }
    if (!flags.options.flood) {
      sleep(flags.interval_value);
    }
    result = icmp_ping(&ping, flags);
    print_ping_result(&result, &flags);
    if (flags.options.count && ping.seq >= flags.count_value) {
      break;
    }
  }
  print_ping_statistics(&ping);
  close(ping.socket);
  return 0;
}

static void handle_sigint(int sig) {
  (void)sig;
  print_ping_statistics(&ping);
  close(ping.socket);
  exit(0);
}

static inline bool is_timeout(flags_t flags, uint32_t start_time_in_seconds) {
  return (flags.options.timeout &&
          get_current_time_in_seconds() - start_time_in_seconds + flags.interval_value >=
              flags.timeout_value);
}
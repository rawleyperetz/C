#include <errno.h>
#include <glib-2.0/glib-object.h>
#include <libnotify/notify.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ARGCOUNT 2
#define TITLE "Eye-strain"
#define MSG                                                                    \
  "You've been working for 20 minutes. Look at something 20 feet away for 20 " \
  "seconds"

volatile sig_atomic_t cancelprogram = 0;

void sigint_handler(int signo) {
  (void)signo;
  cancelprogram = 1;
}

int main(int argc, char **argv) {

  struct sigaction sa;
  sa.sa_handler = sigint_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  if (argc != ARGCOUNT) {
    fprintf(stderr, "Usage: %s <time interval (minutes)>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char *endptr;
  errno = 0;
  long timeInterval = strtol(argv[1], &endptr, 10) * 60;
  // this error checking is faulty
  if (errno != 0 || *endptr != '\0' || timeInterval <= 0) {
    fprintf(stderr, "Invalid time interval: %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  if (!notify_init("Eye-Strain-Reminder")) {
    fprintf(stderr, "Failed to initialize libnotify\n");
    exit(EXIT_FAILURE);
  }

  while (!cancelprogram) {

    sleep((unsigned int)timeInterval);

    NotifyNotification *notif = notify_notification_new(TITLE, MSG, NULL);
    if (!notify_notification_show(notif, NULL)) {
      fprintf(stderr, "Failed to display notification.\n");
    }

    g_object_unref(G_OBJECT(notif));
  }

  notify_uninit();

  return 0;
}

#include "main.h"

/* Parameters manager */
static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
        struct arguments *arguments = state->input;

        switch (key)
        {
        case 'd':
                arguments->daemonize    = TRUE;
                break;
        case 'f':
                arguments->use_file     = TRUE;
                arguments->file         = arg;
                break;
        case 'h':
                arguments->hibernate    = TRUE;
                break;
        case 'l':
                arguments->lid          = TRUE;
                break;
        case 'm':
                arguments->monitor      = TRUE;
                break;
        case 'p':
                arguments->poweroff     = TRUE;
                break;
        case 'r':
                arguments->restart      = TRUE;
                break;
        case 's':
                arguments->suspend      = TRUE;
                break;
        case 'v':
                arguments->verbose      = TRUE;
                break;
        case 'w':
                arguments->wait         = isdigit(arg[0]) ? atoi(arg) : -1;
                break;
        case ARGP_KEY_NO_ARGS:
                arguments->gui          = TRUE;
                break;
        default:
                return ARGP_ERR_UNKNOWN;
        }
        return 0;
}

/* Opt parser */
static const struct argp argp = { options, parse_opt, 0, doc, 0, 0, 0 };

static void
initialize_arguments(struct arguments *args)
{
        args->daemonize = FALSE;
        args->file      = "/dev/null";
        args->hibernate = FALSE;
        args->lid       = FALSE;
        args->monitor   = FALSE;
        args->poweroff  = FALSE;
        args->restart   = FALSE;
        args->suspend   = FALSE;
        args->use_file  = FALSE;
        args->verbose   = FALSE;
        args->wait      = -1;
}

int
main(int argc, char **argv)
{
        /* struct arguments args; */
        initialize_arguments(&args);
        argp_parse(&argp, argc, argv, 0, 0, &args);
        pid_t pid, sid;
        pthread_t lid_id, monitor_id;
        gint monitor = 0;

        if (args.daemonize) {
                pid = fork();
                if (pid < 0) {
                        logger("An error occurred while the process child was being created.", args.file);
                        return EXIT_FAILURE;
                } else if (pid > 0) {
                        return EXIT_SUCCESS;
                }

                umask(0);

                sid = setsid();
                if (sid < 0) {
                        logger("An error occurred while the child's SID was being allocated.", args.file);
                        return EXIT_FAILURE;
                }

                if ((chdir("/")) < 0) {
                        logger("An error occurred while the current directory was being changed.", args.file);
                        return EXIT_FAILURE;
                }

                close(STDIN_FILENO);
                close(STDOUT_FILENO);
                close(STDERR_FILENO);

                is_daemonized = TRUE;
        }

        if (args.monitor) {
                char *options[] = {
                        args.file,
                        args.verbose ? "1" : "0",
                        args.daemonize ? "1" : "0",
                };
                pthread_create(&monitor_id, NULL, battery_monitor, options);
                monitor++;
        }

        if (args.lid) {
                pthread_create(&lid_id, NULL, lid, args.file);
                monitor++;
        }

        if (monitor > 0) {
                if (args.lid) {
                        pthread_join(lid_id, NULL);
                }

                if (args.monitor) {
                        pthread_join(monitor_id, NULL);
                }
        } else {
                if (args.hibernate || args.poweroff || args.restart || args.suspend) {
                        if (args.wait > 0) {
                                while ((args.wait--) > 0) {
                                        if (args.verbose) {
                                                if (!args.daemonize) {
                                                        setbuf(stdout, NULL);
                                                        printf("\r%i seconds remaining.", args.wait);
                                                }
                                                /* Assuming the worst case:
                                                * the user has entered the maximum integer-allowed
                                                * value, this would a required size to store the
                                                * whole message.
                                                */
                                                char *msg = format(35, "%i seconds remaining.", args.wait);
                                                logger(msg, args.file);
                                                free(msg);
                                        }
                                        sleep(1);
                                }
                        }

                        if (args.hibernate) {
                                emit_signal(HIBERNATE, args.file);
                        } else if (args.poweroff) {
                                emit_signal(POWEROFF, args.file);
                        } else if (args.restart) {
                                emit_signal(RESTART, args.file);
                        } else if (args.suspend) {
                                emit_signal(SUSPEND, args.file);
                        }
                } else {
                        use_gui(args.file);
                }
        }

        return 0;
}

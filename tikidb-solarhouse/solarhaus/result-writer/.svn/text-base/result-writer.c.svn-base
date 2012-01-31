#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <mysql.h>
#include <sys/time.h>

#define BAUDRATE B57600
#define BAUDRATE_S "57600"
#ifdef linux
#define MODEMDEVICE "/dev/ttyS0"
#else
#define MODEMDEVICE "/dev/com1"
#endif /* linux */

#define BUFSIZE 32
#define RXBUFSIZE 128
#define QUERYSIZE 512

static char rxbuf[RXBUFSIZE];

static int usage(int result) {
	printf("Usage: serialdump [-bSPEED] [SERIALDEVICE]\n");
	return result;
}


int main(int argc, char **argv)
{
	int val[7];
	struct termios options;
	fd_set mask, smask;
	int fd, nfound, pos = 0;
	speed_t speed = BAUDRATE;
	char *speedname = BAUDRATE_S;
	char *device = MODEMDEVICE;
	unsigned char buf[BUFSIZE];
	char query[QUERYSIZE];
	FILE* logFile = NULL;
	const char *timeformat = "%Y-%m-%d %H:%M:%S";
	MYSQL *conn = NULL;
	time_t t;
	char timeBuf[20];
	  
	int index = 1;
	while (index < argc) {
		if (argv[index][0] == '-') {
		  switch(argv[index][1]) {
			case 'b':
				if (strcmp(&argv[index][2], "38400") == 0) {
					speed = B38400;
					speedname = "38400";
				} else if (strcmp(&argv[index][2], "19200") == 0) {
					speed = B19200;
					speedname = "19200";
				} else if (strcmp(&argv[index][2], "57600") == 0) {
					speed = B57600;
					speedname = "57600";
				} else if (strcmp(&argv[index][2], "115200") == 0) {
					speed = B115200;
					speedname = "115200";
				} else {
					fprintf(stderr, "unsupported speed: %s\n", &argv[index][2]);
					return usage(1);
				}
				break;
			case 'h':
				return usage(0);
			default:
				fprintf(stderr, "unknown option '%c'\n", argv[index][1]);
				return usage(1);
			}
			index++;
		} else {
			device = argv[index++];
			if (index < argc) {
				fprintf(stderr, "too many arguments\n");
				return usage(1);
			}
		}
	}
	
	logFile = fopen("result-writer.log", "a+");
	fprintf(stderr, "connecting to %s (%s)", device, speedname);
	fclose(logFile);
	
	t = time(&t);
	strftime(timeBuf, 20, timeformat, localtime(&t));
	logFile = fopen("result-writer.log", "a+");
	fprintf(logFile, "%s: connecting to %s (%s)\n", timeBuf, device, speedname);
	fclose(logFile);

	fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_SYNC );
	if (fd <0) {
		fprintf(stderr, "\n");
		perror(device);
		exit(-1);
	}
	fprintf(stderr, " [OK]\n");

	printf("MySQL client version: %s\n", mysql_get_client_info());

	if (fcntl(fd, F_SETFL, 0) < 0) {
		perror("could not set fcntl");
		exit(-1);
	}

	if (tcgetattr(fd, &options) < 0) {
		perror("could not get options");
		exit(-1);
	}

	cfsetispeed(&options, speed);
	cfsetospeed(&options, speed);
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~(CSIZE|PARENB|PARODD);
	options.c_cflag |= CS8;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;

	if (tcsetattr(fd, TCSANOW, &options) < 0) {
		perror("could not set options");
		exit(-1);
	}

	conn = mysql_init(NULL);
	
	if (conn == NULL) {
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}

	if (mysql_real_connect(conn, "hanau.dvs.informatik.tu-darmstadt.de", "solarhouse", "Noo9rai4", "wsn_solarhouse", 0, NULL, 0) == NULL) {
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		exit(1);
	}
							
	FD_ZERO(&mask);
	FD_SET(fd, &mask);
	FD_SET(fileno(stdin), &mask);

	index = 0;
	for (;;) {
		smask = mask;
		nfound = select(FD_SETSIZE, &smask, (fd_set*)0, (fd_set*)0, (struct timeval*)0);
		if (nfound < 0) {
			if (errno == EINTR) {
				fprintf(stderr, "interrupted system call\n");
				continue;
			}
			perror("select");
			exit(1);
		}
		
		if(FD_ISSET(fd, &smask)) {
			int i, n = read(fd, buf, sizeof(buf));
			if (n < 0) {
				perror("could not read");
				exit(-1);
			}
			
			// copy from temp-buf to line-buf
			for (i = 0; i < n; i++, pos++) {
				rxbuf[pos] = buf[i];
				if (rxbuf[pos] == '\n') {
					// print line
					printf("%s", rxbuf);
					// parse the line here
					if (rxbuf[0] == '|' && rxbuf[1] == ' ') {
						sscanf(rxbuf, "| %d | %d | %d | %d | %d | %d | %d |", &val[0], &val[1], &val[2], &val[3], &val[4], &val[5], &val[6]);
						if (!(val[0] == 40 && val[1] == 60)) {
							memset(query, '\0', sizeof(query));
							sprintf(query, "INSERT INTO sensorreadings (node_id, epoch, extern_CO2, extern_humidity, extern_temperature, rssi) VALUES ('%d', '%d', '%d', '%d', '%d', '%d')", val[0], val[2], val[3], val[4], val[5], val[6]);
							if (mysql_real_query(conn, query, strlen(query)) != 0) {
								printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
								t = time(&t);
								strftime(timeBuf, 20, timeformat, localtime(&t));
								logFile = fopen("result-writer.log", "a+");
								fprintf(logFile, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
								fclose(logFile);
								switch (mysql_errno(conn)) {
									case 2006:
									case 2013: {
										printf("Trying to reconnect MySQL database\n");
										logFile = fopen("result-writer.log", "a+");
										fprintf(logFile, "Trying to reconnect MySQL database\n");
										fclose(logFile);
										int try = 0;
										while (mysql_real_connect(conn, "hanau.dvs.informatik.tu-darmstadt.de", "solarhouse", "Noo9rai4", "wsn_solarhouse", 0, NULL, 0) == NULL && try < 3) {
											printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
											t = time(&t);
											strftime(timeBuf, 20, timeformat, localtime(&t));
											logFile = fopen("result-writer.log", "a+");
											fprintf(logFile, "Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
											fclose(logFile);
											try++;
											
										}
										if (try >= 3) {
											printf("Reconnect failed after 3 retries\n");
											exit(1);
										}
									}
								}
							}
							usleep(1000000);
						}
					}
					memset(rxbuf, '\0', sizeof(rxbuf));
					pos = -1;
				}
			}
			
			fflush(stdout);
		}
	}
	
	mysql_close(conn);
	
	return 0;
}

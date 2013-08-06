#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ev.h>

#define PORT_NO 3033
#define BUFFER_SIZE 1024

int total_clients = 0;

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

int main()
{
	struct ev_loop *loop = ev_default_loop(0);

	int sd;
	struct sockaddr_in addr;
	struct ev_io w_accept;

	if((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		return -1;
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_NO);
	addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{
		perror("bind error");
	}

	if(listen(sd, 2) < 0)
	{
		perror("listen error");
		return -1;
	}

	ev_io_init(&w_accept, accept_cb, sd, EV_READ);
	ev_io_start(loop, &w_accept);

	ev_loop(loop, 0);

	return 0;
}

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_sd;
	struct ev_io *w_client = (struct ev_io*)malloc(sizeof(struct ev_io));

	if(EV_ERROR & revents)
	{
		perror("got invalid event");
		return;
	}

	client_sd = accept(watcher->fd, (struct sockaddr*)&client_addr, &client_len);

	if(client_sd < 0)
	{
		perror("accept error");
		return;
	}

	total_clients++;
	printf("Successfully connected with client\n");
	printf("%d clients connected.\n", total_clients);

	ev_io_init(w_client, read_cb, client_sd, EV_READ);
	ev_io_start(loop, w_client);
}

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
	char buffer[BUFFER_SIZE];
	ssize_t read;

	if(EV_ERROR & revents)
	{
		perror("got invalid event");
		return;
	}

	read = recv(watcher->fd, buffer, BUFFER_SIZE, 0);

	if(read < 0)
	{
		perror("read error");
		return;
	}

	if(read == 0)
	{
		ev_io_stop(loop, watcher);
		close(watcher->fd);
		free(watcher);
		perror("peer might closing");
		total_clients--;
		printf("%d clients connected.\n", total_clients);
		return;
	}
	else
	{
		printf("message: %s\n", buffer);
	}

	send(watcher->fd, buffer, read, 0);
	bzero(buffer, read);
}
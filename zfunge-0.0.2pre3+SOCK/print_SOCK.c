
#include <config.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include "funge_ip.h"
#include "funge_stack.h"
#include "funge_space.h"
#include "funge_commands.h"

struct sockent {
	long id;
	struct sockaddr_in sin;
	int sock;
} **socks = NULL;

int sock_id = 0;

long sock_findid(long id) {
	int i;

	for (i = 1; i <= *(long*)socks; i++) {
		if (socks[i]->id == id)
			return i;
	}

	return -1;
}

fcmd_t(sck_socket) {
	long pf, type, protocol, sock;
	struct sockent *s;

	protocol = fst_pop(&ip->stack, 0);
	type = fst_pop(&ip->stack, 0);
	pf = fst_pop(&ip->stack, 0);

	if ((sock = socket((pf==1)?PF_UNIX:PF_INET, (type==1)?SOCK_DGRAM:SOCK_STREAM, (protocol==1)?IPPROTO_TCP:IPPROTO_UDP)) < 0) {
		std_reflect(cmd_t_args);
		return;
	}

	s = malloc(sizeof(*s));

	s->sock = sock;
	s->id = sock_id++;

	socks = realloc(socks, sizeof(*socks) * (++*(long*)socks + 1));
	socks[*(long*)socks] = s;

	fst_push(&ip->stack, 0, s->id);
}

fcmd_t(sck_setsockopt) {
	long sockid, opt, n;

	n = fst_pop(&ip->stack, 0);
	opt = fst_pop(&ip->stack, 0);
	sockid = fst_pop(&ip->stack, 0);

	if ((sockid = sock_findid(sockid)) < 0) {
		std_reflect(cmd_t_args);
		return;
	}

	if (setsockopt(socks[sockid]->sock, SOL_SOCKET, opt, &n, sizeof(n))) {
		std_reflect(cmd_t_args);
		return;
	}
}

fcmd_t(sck_bind) {
	unsigned long addr;
	long port, type, sockid;
	struct sockaddr_in saddr;

	addr = fst_pop(&ip->stack, 0);
	port = fst_pop(&ip->stack, 0);
	type = fst_pop(&ip->stack, 0);
	sockid = fst_pop(&ip->stack, 0);

	if ((sockid = sock_findid(sockid)) < 0) {
		std_reflect(cmd_t_args);
		return;
	}

	saddr.sin_family = (type==1)?AF_UNIX:AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = addr;
	if (bind(socks[sockid]->sock, (struct sockaddr*)&saddr, sizeof(saddr))) {
		std_reflect(cmd_t_args);
		return;
	}
}

fcmd_t(sck_accept) {
	long sockid, rsock;
	socklen_t raddrlen;
	struct sockaddr_in raddr;
	struct sockent *s;

	sockid = fst_pop(&ip->stack, 0);

	if ((sockid = sock_findid(sockid)) < 0) {
		std_reflect(cmd_t_args);
		return;
	}

	raddrlen = sizeof(raddr);
	if ((rsock = accept(socks[sockid]->sock, (struct sockaddr*)&raddr, &raddrlen)) < 0) {
		std_reflect(cmd_t_args);
		return;
	}

	s = malloc(sizeof(*s));

	s->sock = rsock;
	s->id = sock_id++;

	socks = realloc(socks, sizeof(*socks) * (++*(long*)socks + 1));
	socks[*(long*)socks] = s;

	fst_push(&ip->stack, 0, htons(raddr.sin_port));
	fst_push(&ip->stack, 0, raddr.sin_addr.s_addr);
	fst_push(&ip->stack, 0, s->id);
}

fcmd_t(sck_listen) {
	long backlog, sockid;

	sockid = fst_pop(&ip->stack, 0);
	backlog = fst_pop(&ip->stack, 0);

	if ((sockid = sock_findid(sockid)) < 0) {
		std_reflect(cmd_t_args);
		return;
	}

	if (listen(socks[sockid]->sock, backlog)) {
		std_reflect(cmd_t_args);
		return;
	}
}

fcmd_t(sck_connect) {
	long sockid, port, type;
	unsigned long addr;
	struct sockaddr_in saddr;

	addr = fst_pop(&ip->stack, 0);
	port = fst_pop(&ip->stack, 0);
	type = fst_pop(&ip->stack, 0);
	sockid = fst_pop(&ip->stack, 0);

	if ((sockid = sock_findid(sockid)) < 0) {
		std_reflect(cmd_t_args);
		return;
	}

	saddr.sin_family = (type==1)?AF_UNIX:AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = addr;
	if (connect(socks[sockid]->sock, (struct sockaddr*)&saddr, sizeof(saddr))) {
		std_reflect(cmd_t_args);
		return;
	}
}

fcmd_t(sck_kill) {
	long sockid;

	sockid = fst_pop(&ip->stack, 0);

	if ((sockid = sock_findid(sockid)) < 0) {
		std_reflect(cmd_t_args);
		return;
	}

	shutdown(socks[sockid]->sock, 2);
	close(socks[sockid]->sock);

	if (sockid < *(long*)socks)
		socks[sockid] = socks[*(long*)socks];

	socks = realloc(socks, sizeof(*socks) * (--*(long*)socks + 1));
}

fcmd_t(sck_read) {
	long x, y, z, len, sockid;
	long rlen, i;
	char *buff;

	sockid = fst_pop(&ip->stack, 0);
	len = fst_pop(&ip->stack, 0);
	fst_popvec(&ip->stack, 0, x, y, z);

	if (len <= 0 || (sockid = sock_findid(sockid)) < 0) {
		std_reflect(cmd_t_args);
		return;
	}

	buff = malloc(len);

	if ((rlen = read(socks[sockid]->sock, buff, len)) < 0) {
		free(buff);
		std_reflect(cmd_t_args);
		return;
	}

	for (i = 0; i < rlen; i++)
		fsp_add_cell(x+i, y, z, buff[i]);

	free(buff);

	fst_push(&ip->stack, 0, rlen);
}

fcmd_t(sck_write) {
	long x, y, z, len, sockid;
	long wlen, i;
	char *buff;

	sockid = fst_pop(&ip->stack, 0);
	len = fst_pop(&ip->stack, 0);
	fst_popvec(&ip->stack, 0, x, y, z);

	if (len <= 0 || (sockid = sock_findid(sockid)) < 0) {
		std_reflect(cmd_t_args);
		return;
	}

	buff = malloc(len);

	for (i = 0; i < len; i++)
		buff[i] = fsp_get_cell(x+i, y, z);

	wlen = write(socks[sockid]->sock, buff, len);

	free(buff);

	if (wlen < 0) {
		std_reflect(cmd_t_args);
		return;
	}

	fst_push(&ip->stack, 0, wlen);
}

fcmd_t(sck_resolve) {
	char *s;
	long addr;
	struct in_addr iaddr;
	struct hostent *hent;

	s = fst_popstring(&ip->stack, 0);

	if ((addr = inet_addr(s)) == -1) {
		if (!(hent = gethostbyname(s))) {
			free(s);
			std_reflect(cmd_t_args);
			return;
		}

		addr = 0;
		memcpy(&addr, hent->h_addr, hent->h_length);
	}

	free(s);
	fst_push(&ip->stack, 0, addr);
}

fcmd_t(loadcommand_SOCK) {
	if (!socks) {
		socks = malloc(sizeof(*socks));
		*(long*)socks = 0;
	}

	override_command(ip, 'A', sck_accept);
	override_command(ip, 'B', sck_bind);
	override_command(ip, 'C', sck_connect);
	override_command(ip, 'K', sck_kill);
	override_command(ip, 'L', sck_listen);
	override_command(ip, 'O', sck_setsockopt);
	override_command(ip, 'R', sck_read);
	override_command(ip, 'S', sck_socket);
	override_command(ip, 'V', sck_resolve);
	override_command(ip, 'W', sck_write);
}

fcmd_t(unloadcommand_SOCK) {
	delete_command(ip, 'A', sck_accept);
	delete_command(ip, 'B', sck_bind);
	delete_command(ip, 'C', sck_connect);
	delete_command(ip, 'K', sck_kill);
	delete_command(ip, 'L', sck_listen);
	delete_command(ip, 'O', sck_setsockopt);
	delete_command(ip, 'R', sck_read);
	delete_command(ip, 'S', sck_socket);
	delete_command(ip, 'V', sck_resolve);
	delete_command(ip, 'W', sck_write);
}

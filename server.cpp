/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmonbeig <fmonbeig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/09 17:50:15 by fmonbeig          #+#    #+#             */
/*   Updated: 2022/03/10 17:16:32 by fmonbeig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

//TODO bien penser a free les sockets a la fin

static Socket*	createSocket(int port)
{
	Socket *new_sock = new Socket(port);
	// fcntl(sockfd, F_SETFL, O_NONBLOCK); Socket is Nonblocking (not sleeping), I have to dig this concept
	new_sock->bindSocket();
	new_sock->listenSocket();
	return (new_sock);
}

//     int poll(struct pollfd fds[], nfds_t nfds, int timeout);
// fds is our array of information (which sockets to monitor for what), nfds is the count of elements in the array, and timeout is a timeout in milliseconds. It returns the number of elements in the array that have had an event occur.

// Let’s have a look at that struct:

//     struct pollfd {
//         int fd;         // the socket descriptor
//         short events;   // bitmap of events we're interested in
//         short revents;  // when poll() returns, bitmap of events that occurred
//     };

static struct pollfd*	createPollfd(std::vector<Socket*> & socket) //create structure for poll() to listen at every socket
{
	struct pollfd *poll = new pollfd[socket.size()];
	for(int i = 0; i < socket.size(); i++)
	{
		poll->fd = socket[i]->getSocketFd();
		poll->events = POLLIN;
	}
}

int main() // faire en sorte de pouvoir creer un nombre de socket illimités
{
	// Get all the port to listen from Paul's Parsing
	std::vector<int>	allPort;
	allPort.push_back(8080);
	allPort.push_back(8001);
	allPort.push_back(8002);

	//Create a containers of Socket pointer
	std::vector<Socket*>	socket;
	for(int i = 0; i < allPort.size(); i++)
		socket.push_back(createSocket(allPort[i]));








	int new_socket;
	int valread;

	std::string hello= "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

	// regarder du coté de select pour capter les infos de plusieurs socket
	while(1)
	{
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		if ((new_socket = accept(socket[0]->getSocketFd(), (struct sockaddr *)&socket[0]->_address, (socklen_t*)&socket[0]->_addrlen))<0)
		{
			perror("In accept");
			exit(EXIT_FAILURE);
		}

		char buffer[30000] = {0};
		valread = read( new_socket , buffer, 30000);
		printf("%s\n",buffer );
		write(new_socket , hello.c_str() , hello.size());
		printf("------------------Hello message sent-------------------");
		close(new_socket);
	}
	return 0;
}


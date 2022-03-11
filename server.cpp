/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmonbeig <fmonbeig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/09 17:50:15 by fmonbeig          #+#    #+#             */
/*   Updated: 2022/03/11 18:38:13 by fmonbeig         ###   ########.fr       */
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

static struct pollfd*	createPollfd(std::vector<Socket*> & socket) //create structure for poll() to listen at every socket
{
	struct pollfd *poll = new pollfd[socket.size()];
	for(int i = 0; i < socket.size(); i++)
	{
		poll[i].fd = socket[i]->getSocketFd();
		poll[i].events = POLLIN;
	}
	return (poll);
}

void	recv_and_send(int link)
{
	// char	buff[90000];
	// int		ret;
	// std::string hello= "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

	// memset((void*)buff, 0, 90000);
	// if ((ret = recv(link, buff, 90000, 0)) < 0)
	// {
	// 	std::perror("Recv failed:");
	// 	return ;
	// }
	// std::cout << *buff << std::endl;
	// if ((ret = send(link,  hello.c_str() , hello.size(), 0)) < 0) // send() could not send all the data at once sometimes
	// {
	// 	std::perror("Send failed:");
	// 	return ;
	// }
	// std::cout << "\n+++++++ Hello message sent ++++++++\n" << std::endl;

		int valread;
		char buffer[30000] = {0};
		std::string hello= "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
		valread = read( link , buffer, 30000);
		printf("%s\n",buffer );

		write(link , hello.c_str() , hello.size());
		printf("------------------Hello message sent-------------------");


}

static void	port_listening(std::vector<Socket*> & socket, struct pollfd* poll_fd)
{
	int	poll_ret;
	int	link;

	while (1)
	{
		std::cout << "\n+++++++ Waiting for new connection ++++++++\n" << std::endl;
		std::cout << "poll fd " << poll_fd[0].fd << std::endl;
		std::cout << "socket fd " << socket[0]->getSocketFd() << std::endl;
		std::cout << "socket size " << socket.size() << std::endl;
		poll_ret = poll(poll_fd, socket.size(), 2500); // with negative number poll never time out
		std::cout << "result of poll_ret : " << poll_ret << std::endl;

		if (poll_ret == 0)
		{
			std::cout << " Server Time out" << std::endl; // related to the timeout parameter of poll
		}
		else
		{
			for(int i = 0; poll_ret > 0; i++)
			{
				std::cout << "TEST " << std::endl;
				if (poll_fd[i].revents & POLLIN)
				{
					std::cout << "BONJOUR " << std::endl;
					poll_ret--;
					if ((link = accept(socket[i]->getSocketFd(), (struct sockaddr *)&socket[i]->_address, (socklen_t*)&socket[i]->_addrlen))<0)
						std::perror("Accept failed:");
					recv_and_send(link);
					close(link);
				}
			}
		}
	}
}



int main()
{
	// Get all the port to listen from Paul's Parsing
	std::vector<int>	allPort;
	allPort.push_back(8080);
	allPort.push_back(8001);
	allPort.push_back(9034);

	//Create a containers of Socket pointer. We initialize the bind and the listening for every Socket
	std::vector<Socket*>	socket;
	for(int i = 0; i < allPort.size(); i++)
		socket.push_back(createSocket(allPort[i]));

	//Create the struct of pollfd for function poll
	struct pollfd*	poll_fd;

	poll_fd = createPollfd(socket);
	port_listening(socket, poll_fd);
}

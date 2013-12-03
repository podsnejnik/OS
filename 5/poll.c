#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <sys/fcntl.h>

main (int argc, char *argv[])
{
  
  int    my_sock;
  struct pollfd my_poll[200];
  int    cur = 1;
  int    fl = 1;
  struct sockaddr_in   my_addr;
  int connection;
  
  my_sock = socket(AF_INET, SOCK_STREAM, 0);
  
  setsockopt(my_sock, SOL_SOCKET,  SO_REUSEADDR,
                  (char *)&fl, sizeof(fl));
  fcntl(my_sock, F_SETFL, O_NONBLOCK);

  memset(&my_addr, 0, sizeof(my_addr));
  my_addr.sin_family      = AF_INET;
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  my_addr.sin_port        = htons(1337);
  bind(my_sock, (struct sockaddr *)&my_addr, sizeof(my_addr));
  
  listen(my_sock, 10);
  memset(my_poll, 0 , sizeof(my_poll));
  my_poll[0].fd = my_sock;
  my_poll[0].events = POLLIN;
  int cur_size;
  int i;
  while (1)
  {
    poll(my_poll, cur, 1000);
    cur_size = cur;

    for (i = 0; i < cur_size; i++)
    {
      if(my_poll[i].revents == 0)
        continue;

      if (my_poll[i].fd == my_sock)
      {
        do
        {
          connection = accept(my_sock, NULL, NULL);
          my_poll[cur].fd = connection;
          my_poll[cur].events = POLLIN;
          cur++;
      	} while (connection >= 0); //не должен быть равен -1
      }
      else
      {	
      	  char   data[100];
          int get = recv(my_poll[i].fd, data, sizeof(data), 0);
          send(my_poll[i].fd, data, get, 0);
      }  
    }

  }
}

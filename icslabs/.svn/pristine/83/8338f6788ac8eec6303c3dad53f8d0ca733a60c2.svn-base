/*
 * proxy.c - ICS Web proxy
 * Name : GJX
 * ID : 521021910197
 */

#include "csapp.h"
#include <stdarg.h>
#include <sys/select.h>

/*
 * Struct as params 
*/
typedef struct connect
{
    int connfd;
    struct sockaddr_in sockaddress;
} connect_t;

/*
 * Function prototypes
 */
int parse_uri(char *uri, char *target_addr, char *path, char *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, size_t size);
void doit(connect_t *con);
ssize_t Rio_readlineb_w(rio_t *rp, void* buf, size_t maxlen);
ssize_t Rio_readnb_w(rio_t* rp, void* buf, size_t n);
ssize_t Rio_writen_w(int fd, void* buf, size_t n);
void* thread(void*);

/*
 * RIO IMP
*/

ssize_t Rio_readnb_w(rio_t* rp, void* buf, size_t n)
{
    ssize_t rc;
    if((rc = rio_readnb(rp, buf, n)) < 0)
    {
        return 0;
    }
    return rc;
}

ssize_t Rio_readlineb_w(rio_t* rp, void* buf, size_t maxlen)
{
    ssize_t rc;
    if((rc = rio_readlineb(rp, buf, maxlen)) < 0)
    {
        return 0;
    }
   return rc;
}

ssize_t Rio_writen_w(int fd, void *usrbuf, size_t n)
{
    ssize_t rc;
    if ((rc = rio_writen(fd, usrbuf, n)) != n)
    {
        return 0;
    }
    return rc;
}

/*
 * thread
*/
sem_t mutex;
void* thread(void* arg)
{
    Pthread_detach(pthread_self());
    doit((connect_t*)arg);
    Free(arg);
    return NULL;
}

/*
 * main - Main routine for the proxy program
 */
int main(int argc, char **argv)
{

    int listenfd, connfd;
    socklen_t clientlen = sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;
    connect_t *conn;
    pthread_t ptid;

    signal(SIGPIPE, SIG_IGN);
    Sem_init(&mutex, 0, 1);

    /* Check arguments */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
        exit(0);
    }

    listenfd = Open_listenfd(argv[1]);
    while(1)
    {
        clientlen = sizeof(struct sockaddr_in);
        connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
        //Getnameinfo((SA*)&clientaddr, clientlen, hostname, MAXLINE, protname, MAXLINE, 0);
        conn = (connect_t *)Malloc(sizeof(connect_t));
        conn->connfd = connfd;
        conn->sockaddress = clientaddr;
        Pthread_create(&ptid, NULL, thread, conn);
    }

    exit(0);
}

void doit(connect_t *vargp)
{
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE], log[MAXLINE];
    char hostname[MAXLINE], pathname[MAXLINE], port[MAXLINE];
    char requestLine[4 * MAXLINE];
    int clientfd, serverfd;
    int contentLength = 0, rc = 0, Len = 0;
    rio_t clientRio, serverRio;

    clientfd = vargp->connfd;

    Rio_readinitb(&clientRio, clientfd);

    if (Rio_readlineb_w(&clientRio, buf, MAXLINE) == 0)
    {
        Close(clientfd);
        return;
    }

    if(sscanf(buf, "%s %s %s", method, uri, version) != 3)
    {
        Close(clientfd);
        return;
    }

    if(parse_uri(uri, hostname, pathname, port) == -1)
    {
        Close(clientfd);
        return;
    }
    
    if((serverfd = Open_clientfd(hostname, port)) < 0)
    {
        Close(clientfd);
        return;
    }

    //requestline
    Rio_readinitb(&serverRio, serverfd);
    sprintf(requestLine, "%s /%s %s\r\n", method, pathname, version);
    Rio_writen_w(serverfd, requestLine, strlen(requestLine));

    //requestheader
    while ((rc = Rio_readlineb_w(&clientRio, buf, MAXLINE)) != 0)
    {
        if (!strncmp(buf,"Content-Length: ", 16))
        {
            sscanf(buf + 16, "%d", &contentLength);
        }
        Rio_writen_w(serverfd, buf, rc);
        if (!strcmp(buf,"\r\n"))
        {
            break;
        }
    }
    
    if(rc == 0)
    {
    	Close(clientfd);
    	return;
    }
    
    //request body
    if (strcmp(method,"GET") && contentLength)
    {
        int i = 0;
        while(i < contentLength)
        {
            i++;
            if(Rio_readnb_w(&clientRio, buf, 1) == 0)
            {
                Close(clientfd);
                return;
            }
            if(Rio_writen_w(serverfd, buf, 1) == 0)
            {
                Close(clientfd);
                return;
            }
        }
    }
    contentLength = 0;
    Len = 0;
    

    // response header
    while ((rc = Rio_readlineb_w(&serverRio, buf, MAXLINE)) != 0)
    {
        if (!strncmp(buf,"Content-Length: ", 16))
        {
            sscanf(buf + 16, "%d", &contentLength);
        }
        Rio_writen_w(clientfd, buf, rc);
        Len += rc;
        if(!strcmp(buf,"\r\n"))
        {
            break;
        }
    }
    
    if(rc == 0)
    {
    	Close(clientfd);
    	return;
    }

    //response body
    if (contentLength)
    {
        int i = 0;
        while(i < contentLength)
        {
            i++;
            if(Rio_readnb_w(&serverRio, buf, 1) == 0)
            {
                Close(clientfd);
                return;
            }
            if(Rio_writen_w(clientfd, buf, 1) == 0)
            {
                Close(clientfd);
                return;
            }
        }
        Len += contentLength;
    }

    format_log_entry(log, &vargp->sockaddress, uri, Len);

    //print
    P(&mutex);
    printf("%s\n", log);
    V(&mutex);

    // close
    Close(clientfd);
    Close(serverfd);
    return;
}


/*
 * parse_uri - URI parser
 *
 * Given a URI from an HTTP proxy GET request (i.e., a URL), extract
 * the host name, path name, and port.  The memory for hostname and
 * pathname must already be allocated and should be at least MAXLINE
 * bytes. Return -1 if there are any problems.
 */
int parse_uri(char *uri, char *hostname, char *pathname, char *port)
{
    char *hostbegin;
    char *hostend;
    char *pathbegin;
    int len;

    if (strncasecmp(uri, "http://", 7) != 0) {
        hostname[0] = '\0';
        return -1;
    }

    /* Extract the host name */
    hostbegin = uri + 7;
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    if (hostend == NULL)
        return -1;
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';

    /* Extract the port number */
    if (*hostend == ':') {
        char *p = hostend + 1;
        while (isdigit(*p))
            *port++ = *p++;
        *port = '\0';
    } else {
        strcpy(port, "80");
    }

    /* Extract the path */
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL) {
        pathname[0] = '\0';
    }
    else {
        pathbegin++;
        strcpy(pathname, pathbegin);
    }

    return 0;
}

/*
 * format_log_entry - Create a formatted log entry in logstring.
 *
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), the number of bytes
 * from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr,
                      char *uri, size_t size)
{
    time_t now;
    char time_str[MAXLINE];
    char host[INET_ADDRSTRLEN];

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    if (inet_ntop(AF_INET, &sockaddr->sin_addr, host, sizeof(host)) == NULL)
        unix_error("Convert sockaddr_in to string representation failed\n");

    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %s %s %zu", time_str, host, uri, size);
}

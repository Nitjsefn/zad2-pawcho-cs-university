#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#define BUFF_S 1024
#define DEFAULT_FILE "index.html"

/*#pragma exit myexit*/

/*__attribute__((destructor))*/ void myexit();

typedef enum
{
    GET,
} reqmethod_t;

typedef enum
{
    HTML,
    TXT,
    JS,
    JPEG,
    CSS,
    PNG,
} filetype_t;

static int sd;
static volatile int exit_code = 0;

int main(int argc, char **argv)
{
    time_t currtime;
    time(&currtime);
    printf(ctime(&currtime));
    puts("\t-> Server listening on port 8080");

    struct sigaction sigint_overload = {.sa_handler = myexit};
    sigfillset(&(sigint_overload.sa_mask));
    sigaction(SIGINT, &sigint_overload, NULL);

    struct sockaddr_in sa_in = 
        {
            .sin_addr.s_addr = INADDR_ANY,
            .sin_port = htons(8080),
            .sin_family = AF_INET,
        };
    sd = socket(AF_INET, SOCK_STREAM, 0);
    int true = 1;
    /*setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(true));*/
    int bindres = 0;
    printf
        (
            "Bind: %d\n",
            bindres = bind(sd, (struct sockaddr*)&sa_in, sizeof(struct sockaddr_in))
        );

    if(argc > 1 && strcmp(*argv, "--healthcheck"))
    {
        if(0 != bindres)
            exit_code = 0;
        else
            exit_code = 127;
        return exit_code;
    }

    printf
        (
            "Listen: %d\n",
            listen(sd, 3)
        );

    while(1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len;
        int conn_sd = accept(sd, (struct sockaddr*)&client_addr, &client_addr_len);
        if(conn_sd == -1)
        {
            puts("Cannot open descriptor for new connection");
            continue;
        }
        printf("New connection fd: %d\n", conn_sd);

        char req[BUFF_S+1] = {0};
        int avail = read(conn_sd, req, BUFF_S);
        if(avail == -1)
        {
            puts("Cannot read from connection socket");
            close(conn_sd);
            continue;
        }
        puts("Read finished");
        puts("Request:");
        puts(req);
        puts("Analyzing data");

        reqmethod_t reqmethod;
        if
        (
            req[0] == 'G' &&
            req[1] == 'E' &&
            req[2] == 'T'
        )
        {
            reqmethod = GET;
        }
        else
        {
            puts("Unsopported or unreadable request. Sending 400");
            char *res = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\nContent-Length: 11\r\n\r\nBad Request";
            write(conn_sd, res, strlen(res));
            close(conn_sd);
            continue;
        }
        printf("Request method num: %d\n", reqmethod);

        char reqpath[BUFF_S+1] = {0};
        int reqlen = avail;
        int pathpos = 0;
        int reqpos = 0;
        for(; reqpos < strlen(req); reqpos++)
        {
            if(req[reqpos] == ' ')
                break;
        }
        reqpos++;
        printf("Path Position: %d\n", reqpos);
        if(req[reqpos] == '/')
        {
            reqpath[0] = '.';
            pathpos++;
        }
        for(; pathpos < BUFF_S && reqpos < avail; pathpos++, reqpos++)
        {
            if(req[reqpos] == ' ')
                break;
            reqpath[pathpos] = req[reqpos];
        }
        printf("ReqPath: %s\n", reqpath);

        char path[BUFF_S+1] = {0};
        strcpy(path, reqpath);
        if(path[strlen(path)-1] == '/')
            strncat(path, DEFAULT_FILE, BUFF_S-strlen(reqpath));
        printf("Path: %s\n", path);

        FILE *f = fopen(path, "rb");
        if(f == NULL)
        {
            puts("Cannot open requested file. Sending 404");
            char *res = "HTTP/1.1 404 Not Found\r\n\r\n";
            write(conn_sd, res, strlen(res));
            close(conn_sd);
            continue;
        }

        int filesize;
        fseek(f, 0, SEEK_END);
        filesize = ftell(f);
        rewind(f);
        printf("File Size: %ld\n", filesize);

        char *ext;
        int pathlen = strlen(path);
        int extpos = pathlen - 1;
        for(; extpos >= 0 && path[extpos] != '.' && path[extpos] != '/'; extpos--) ;;
        extpos++;
        if(0 == extpos)
            ext = NULL;
        else if(1 == extpos && '.' != path[0])
            ext = NULL;
        else if('/' == path[extpos-1])
            ext = NULL;
        else
            ext = path + extpos;
        printf("File Ext: %s\n", ext);

        filetype_t filetype;
        if(NULL == ext)
            filetype = TXT;
        else if(0 == strcmp("html", ext))
            filetype = HTML;
        else if(0 == strcmp("htm", ext))
            filetype = HTML;
        else if(0 == strcmp("js", ext))
            filetype = JS;
        else if(0 == strcmp("jpg", ext))
            filetype = JPEG;
        else if(0 == strcmp("png", ext))
            filetype = PNG;
        else if(0 == strcmp("css", ext))
            filetype = CSS;
        else if(0 == strcmp("txt", ext))
            filetype = TXT;
        printf("File Type: %d\n", filetype);

        char *mimetype;
        switch(filetype)
        {
            case HTML: mimetype = "text/html"; break;
            case CSS: mimetype = "text/css"; break;
            case JS: mimetype = "text/javascript"; break;
            case JPEG: mimetype = "image/jpeg"; break;
            case PNG: mimetype = "image/png"; break;
            case TXT: mimetype = "text/plain"; break;
            default: mimetype = "text/plain"; break;
        }
        printf("File MIME: %s\n", mimetype);

        puts("Sending data");
        char header[BUFF_S+1];
        snprintf(header, BUFF_S, "HTTP/1.1 200 OK\r\nContent-Type:%s\r\nContent-Length:%d\r\n\r\n", mimetype, filesize);
        /*char filesize_s[16] = {0};*/
        /*snprintf(filesize_s, 15, "%d", filesize);*/
        /*char *header = strcat("HTTP/1.1 200 OK\r\nContent-Type:", mimetype);*/
        /*header = strcat(header, "\r\nContent-Length:");*/
        /*header = strcat(header, filesize_s);*/
        /*header = strcat(header, "\r\n\r\n");*/
        write(conn_sd, header, strlen(header));
        char filecontent[BUFF_S + 1] = {0};
        int size_read = 0;
        while(0 < (size_read = fread(filecontent, sizeof(char), BUFF_S, f)))
            write(conn_sd, filecontent, size_read);
        puts("Data sent");

        fclose(f);

        close(conn_sd);
    }
    close(sd);
    return 0;
}

/*__attribute__((destructor))*/ void myexit()
{
    puts("\r  ");
    puts("Exiting...");
    close(sd);
    _exit(exit_code);
}

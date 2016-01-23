#include <stdio.h>

#include <pcap.h>
#include <libnet.h>

#include "data_pirate.h"
#include "sender.h"

#define INTERFACE   "en0"



int
finish(void)
{
    hunter_finish();
    sender_finish();
    return true;
}


void
signal_handler(int signo)
{
    finish();
    printf("[!] exit by signal_handler\n\033[0m");
    exit(0);
}


int
initialize(const int argc, const char* argv[])
{
    char*   perr    = 0;

    _SET_LOG_OUT_FUN(printf);
    _SET_SEND_PACKAGE_FUN(sender_send);

    if(false == net_state_init( INTERFACE,
                                "\xd4\x33\xa3\x11\x11\x11",
                                _iptonetint32("192.168.1.109"),
                                _iptonetint32("255.255.255.0"),
                                _iptonetint32("192.168.1.1")) )
    {
        return false;
    }

#if 1
    _MESSAGE_OUT("card_name:\t%s\n", net_info->net_interface);
    _MESSAGE_OUT("ip:\t\t%s\n", _netint32toip(net_info->ip_netint32));
    _MESSAGE_OUT("mask:\t\t%s\n", _netint32toip(net_info->ip_mask));
    _MESSAGE_OUT("route:\t\t%s\n", _netint32toip(net_info->ip_route_netint32));
    _MESSAGE_OUT("max devices:\t%u\n", net_info->d_arr_max);
#endif

    signal(SIGINT, signal_handler);     // ctrl+c handler

    if(false == sender_initialize(INTERFACE, &perr))
    {
        _MESSAGE_OUT("%s\n", perr);
        return false;
    }

    return true;
}


void*
thread_reader(void* q)
{
    char    err[10000];
    for(;;)
    {
        sleep(1);
        if(queue_read_message(q, err))
        {
            printf("%s\n", err);
        }
        else
            printf("empty\n");
    }

}





int
main(const int argc, const char* argv[])
{
    int         err     = 0;
    pthread_t   hunter  = 0;
    char*       perr    = 0;


    pthread_t   pt      = 0;
    void*       q       = queue_init(10000, 0);

    if(!q)
        printf("err\n");

    pthread_create(&pt, 0, thread_reader, q);

    int j   = 0;
    for(j = 0; j<99; j++)
    {
        sleep(2);
        char str[100]   = {0};
        sprintf(str, "hello , %d", j);
        queue_write_message(q, str, strlen(str), 0);
        // if(queue_read_message(q, &err))
        // {
        //     printf("%d\n", err);
        // }
    }

    printf("write end\n");
    while(1) sleep(1);

    exit(0);





    if(false == initialize(argc, argv))
    {
        return -1;
    }

    if(false == hunter_initialize(INTERFACE, &perr))
    {
        _MESSAGE_OUT("[!] hunter_initialize failed! caused by : %s\n",         \
                        perr);
        return -1;
    }

    // pthread_t n;
    // err = pthread_create(&n, 0, cheater_test, 0);
    // if(err)
    // {
    //     _MESSAGE_OUT("[!] create err : %s", strerror(err));
    // }
    cheater_test();
    // sleep(1); exit(0);
    while(1) sleep(1);


    if(false == finish())
    {
        return -1;
    }

    return 0;
}













#include <stdio.h>
#include <string.h>
#include <stdlib.h>

class Ip
{
    private:
        enum {IPSIZE = 4};
        unsigned char* b_ip;
        unsigned char* b_mask;
        unsigned char mask;
        bool default_mask; // A mask hasn't been provided.
                           // use one based on the IP class

    public:
        Ip(unsigned int bin_ip);
        Ip(char* str_ip); 
        virtual ~Ip();
        char* GetNetwork();
        char* GetMask();
        void Print();
        char GetClass();
        unsigned char* GetNetworkAddr();
        unsigned char* GetNetworkBroadcastAddr();
        unsigned char* GetNetworkFirstAddr();
        unsigned char* GetNetworkLastAddr();

    private:
        unsigned char GetDefaultMask(char ip_class);
        unsigned char* GetMaskArray(int mask = -1);
};

Ip::Ip(unsigned int bin_ip)
{
    unsigned int read_mask = 0xFF;
    b_ip = new unsigned char[IPSIZE];
    for (int i = 0; i < IPSIZE; i++)
    {
        b_ip[i] = (read_mask & bin_ip);
        bin_ip >>= 8;
    }
    default_mask = true;
    mask = GetDefaultMask(GetClass());
    b_mask = GetMaskArray(mask);
    printf("Ip:Ctr:");
    for (int i = 0; i < IPSIZE; i++)
    {
        printf(" b_ip[%d] = 0x%X(%d)", i, b_ip[i], b_ip[i]);
    }
    printf("\n");
}

Ip::~Ip()
{
    delete [] b_ip;
    delete [] b_mask;
}

char Ip::GetClass()
{
    // Class A - 0.0.0.0   (2^8 networks)    0
    // Class B - 128.0.0.0 (2^16 networks)   10
    // Class C - 192.0.0.0 (2^24 networks)   110
    // Class D - 224.0.0.0 (multicast)       1110
    // Class E - 240.0.0.0 (reserved)        1111
    switch (b_ip[3] & 0xC0)
    {
        case 0x80: return 'B';
        case 0xC0: return 'C';
        case 0xE0: return 'D';
        case 0xF0: return 'E';
        default: return 'A';
    }
}

unsigned char Ip::GetDefaultMask(char ip_class)
{
    switch (ip_class)
    {
        case 'a':
        case 'A':
            return 8;
        case 'b':
        case 'B':
            return 16;
        case 'c':
        case 'C':
            return 24;
        default:
            return 0;
    }
}

unsigned char* Ip::GetMaskArray(int in_mask)
{
    unsigned char msk = (in_mask != -1) ? in_mask : mask;
    int nr_bytes = msk / 8;
    int extra = msk % 8;
    unsigned char* buf = new unsigned char[IPSIZE];
    for (int i = 0; i < nr_bytes; i++)
    {
        buf[IPSIZE -i -1] = 0xFF;
    }
    if (extra)
    {
        unsigned char access_mask = 0x80;
        for (int j = 0; j < extra; j++)
        {
            buf[nr_bytes] |= access_mask;
            access_mask >>= 1;
        }
    }
    return buf;
}

unsigned char* Ip::GetNetworkAddr()
{
    unsigned char* network = new unsigned char[IPSIZE];
    for (int i = 0; i < IPSIZE; i++)
    {
        network[i] = b_ip[i] & b_mask[i];
    }
    return network;
}

unsigned char* Ip::GetNetworkBroadcastAddr()
{
    unsigned char* broadcast = GetNetworkAddr();
    for (int i = 0; i < IPSIZE; i++)
    {
        broadcast[i] |= ~b_mask[i];
    }
    return broadcast;
}

unsigned char* Ip::GetNetworkFirstAddr()
{
    unsigned char* first_addr = GetNetworkAddr();
    first_addr[0]++;
    return first_addr;
}

unsigned char* Ip::GetNetworkLastAddr()
{
    unsigned char* last_addr = GetNetworkBroadcastAddr();
    last_addr[0]--;
    return last_addr;
}
void Ip::Print()
{
    // a maximum of 3 chars per byte + n-1 separators + 1 terminator
    char* ip_buf = new char[IPSIZE * 4];
    char* mask_buf = new char[IPSIZE * 4];
    char* network_buf = new char[IPSIZE * 4];
    char* broadcast_buf = new char[IPSIZE * 4];
    char* first_ip_buf  = new char[IPSIZE * 4];
    char* last_ip_buf   = new char[IPSIZE * 4];

    unsigned char* network = GetNetworkAddr();
    unsigned char* broadcast = GetNetworkBroadcastAddr();
    unsigned char* first_ip = GetNetworkFirstAddr();
    unsigned char* last_ip = GetNetworkLastAddr();
    for (int i = IPSIZE; i > 0; i--)
    {
        sprintf(ip_buf, "%s%d", ip_buf, (int)b_ip[i-1]);
        sprintf(mask_buf, "%s%d", mask_buf, (int)b_mask[i-1]);
        sprintf(network_buf, "%s%d", network_buf, (int)network[i-1]);
        sprintf(first_ip_buf, "%s%d", first_ip_buf, (int)first_ip[i-1]);
        sprintf(last_ip_buf, "%s%d", last_ip_buf, (int)last_ip[i-1]);
        sprintf(broadcast_buf, "%s%d", broadcast_buf, (int)broadcast[i-1]);
        if (i-1 != 0)
        {
            sprintf(ip_buf, "%s.", ip_buf);
            sprintf(mask_buf, "%s.", mask_buf);
            sprintf(network_buf, "%s.", network_buf);
            sprintf(first_ip_buf, "%s.", first_ip_buf);
            sprintf(last_ip_buf, "%s.", last_ip_buf);
            sprintf(broadcast_buf, "%s.", broadcast_buf);
        }
    }
    printf("%s/%d (%c) %s\n", ip_buf, mask, GetClass(), mask_buf);
    printf("Network: %s Broadcast: %s\n", network_buf, broadcast_buf);
    printf("First ip: %s Last ip: %s \n", first_ip_buf, last_ip_buf);
    delete[] ip_buf;
    delete[] mask_buf;
    delete[] network_buf;
    delete[] broadcast_buf;
    delete[] first_ip_buf;
    delete[] last_ip_buf;
    delete[] network;
    delete[] broadcast;
    delete[] first_ip;
    delete[] last_ip;
}

int main(int argc, char* argv[])
{
    Ip* ip = NULL;
    printf("main: argc = %d\n", argc);
    if (argc == 1)
    {
        // ToDo
        printf("main: Aici ar fi frumos sa afisez optiunile de rulare\n");
        return 0;
    }
    for (int i = 1; i < argc; i++)
    {
        printf("main: argv[%d] = '%s'\n", i, argv[i]);
        if (strlen(argv[i]) == 8)
        {
            // todo verificare de caractere
            unsigned int x = strtol(argv[i], NULL, 16);
            ip = new Ip(x);
        }
        else if (strlen(argv[i]) == 10 &&
                 argv[i][0] == '0' &&
                 argv[i][1] == 'x')
        {
            // todo verificare de caractere
            unsigned int x = strtol(argv[i], NULL, 16);
            ip = new Ip(x);
        }
    }
    ip->Print();
    return 0;
}

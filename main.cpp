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
        printf(" b_ip[%d] = %X", i, b_ip[i]);
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

void Ip::Print()
{
    // a maximum of 3 chars per byte + n-1 separators + 1 terminator
    char* ip_buf = new char[IPSIZE * 4];
    char* mask_buf = new char[IPSIZE * 4];
    for (int i = IPSIZE; i > 0; i--)
    {
        sprintf(ip_buf, "%s%d", ip_buf, (int)b_ip[i-1]);
        sprintf(mask_buf, "%s%d", mask_buf, (int)b_mask[i-1]);
        if (i-1 != 0)
        {
            sprintf(ip_buf, "%s.", ip_buf);
            sprintf(mask_buf, "%s.", mask_buf);
        }
    }
    printf("%s/%d (%c) %s\n", ip_buf, mask, GetClass(), mask_buf);
    delete[] ip_buf;
    delete[] mask_buf;
}

int main(int argc, char* argv[])
{
    Ip* ip = NULL;
    printf("argc = %d\n", argc);
    if (argc == 1)
    {
        // ToDo
        printf("Aici ar fi frumos sa afisez optiunile de rulare\n");
        return 0;
    }
    for (int i = 1; i < argc; i++)
    {
        printf("argv[%d] = '%s'\n", i, argv[i]);
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
    printf("main: Am facut initializarile\n");

    //Ip ip(0x00020304);
    ip->Print();
    printf("class = %c\n", ip->GetClass());
    return 0;
}

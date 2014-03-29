#include<stdio.h>

class Ip
{
    private:
        enum {IPSIZE = 4};
        unsigned char b_ip[IPSIZE];
        unsigned char b_mask[IPSIZE];
        unsigned char mask;

    public:
        Ip(unsigned int bin_ip);
        Ip(char* str_ip); 
        char* GetNetwork();
        char* GetMask();
        void Print();
        char GetClass();

    private:
        unsigned char GetDefaultMask();
        unsigned char* GetMaskArray();
};

Ip::Ip(unsigned int bin_ip)
{
    unsigned int read_mask = 0xFF;
    for (int i = 0; i < IPSIZE; i++)
    {
        //printf("%X\n", (read_mask & bin_ip));
        //b_ip[IPSIZE - i - 1] = (read_mask & bin_ip) >> 8 * i;

        //b_ip[i] = (read_mask & bin_ip) >> 8 * i;
        //bin_ip <<= 8;

        b_ip[i] = (read_mask & bin_ip);
        //b_ip[IPSIZE - 1 - i] = (read_mask & bin_ip);
        bin_ip >>= 8;
    }
    for (int i = 0; i < IPSIZE; i++)
    {
        printf("# b_ip[%d] = %X\n", i, b_ip[i]);
    }
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

void Ip::Print()
{
    // a maximum of 3 chars per byte + n-1 separators + 1 terminator
    char* buf = new char[IPSIZE * 4];
    for (int i = IPSIZE; i > 0; i--)
    {
        sprintf(buf, "%s%d", buf, (int)b_ip[i-1]);
        if (i-1 != 0)
        {
            sprintf(buf, "%s.", buf);
        }
    }
    printf("%s\n", buf);
}

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        printf("argv[%d] = '%s'\n", i, argv[i]);
    }

    Ip ip(0x00020304);
    ip.Print();
    printf("class = %c\n", ip.GetClass());
    return 0;
}

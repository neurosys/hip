#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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
        Ip(char* str_ip, char* str_mask = NULL); 
        virtual ~Ip();
        char* GetNetwork();
        char* GetMask();
        void Print();
        char GetClass();
        unsigned char* GetNetworkAddr();
        unsigned char* GetNetworkBroadcastAddr();
        unsigned char* GetNetworkFirstAddr();
        unsigned char* GetNetworkLastAddr();
        unsigned char* GetNetworkWildcard();
        static bool IsValidInt32(char* str);
        static bool IsValidCharSet(char* str, char* char_set);

    private:
        unsigned char GetDefaultMask(char ip_class);
        unsigned char* GetMaskArray(int mask = -1);
        unsigned char GetMaskNum();
        char* ExtractIpByte(char* str, unsigned char* byte);
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
    /*
    printf("Ip:Ctr:");
    for (int i = 0; i < IPSIZE; i++)
    {
        printf(" b_ip[%d] = 0x%X(%d)", i, b_ip[i], b_ip[i]);
    }
    printf("\n");
    */
}

Ip::Ip(char* str_ip, char* str_mask)
{
    if (str_ip == NULL || !*str_ip)
    {
        throw "Invalid string ip provided (NULL or Blank)";
    }
    //printf("Ip::Ip(str_ip) 1\n");
    char allowed_chars[] = "0123456789./";
    if (!Ip::IsValidCharSet(str_ip, allowed_chars))
    {
        throw "Invalid string ip provided (Illegal character)";
    }
    //printf("Ip::Ip(str_ip) 2\n");
    b_ip = new unsigned char[IPSIZE];
    char* ip_str = strdup(str_ip);
    default_mask = true;
    if (str_mask == NULL)
    {
        str_mask = index(ip_str, '/');
        // Separate mask from the rest of the string
        if (str_mask)
        {
            *str_mask = '\0';
            str_mask++;
            mask = atoi(str_mask);
            default_mask = false;
            b_mask = GetMaskArray(mask);
        }
    }
    else
    {
        default_mask = false;
        char* cursor = str_mask;
        b_mask = new unsigned char[IPSIZE];
        for (int i = 0; i < IPSIZE; i++)
        {
            unsigned char byte = 0;
            cursor = ExtractIpByte(cursor, &byte);
            if (cursor != NULL)
            {
                b_mask[IPSIZE -1 - i] = byte;
            }
        }
        mask = GetMaskNum();
    }
    char* cursor = ip_str;
    for (int i = 0; i < IPSIZE; i++)
    {
        //printf("Ip::Ip(str_ip) 5\n");
        unsigned char byte = 0;
        cursor = ExtractIpByte(cursor, &byte);
        if (cursor != NULL)
        {
            b_ip[IPSIZE -1 - i] = byte;
        }
    }
    free(ip_str);
    if (default_mask)
    {
        mask = GetDefaultMask(GetClass());
        b_mask = GetMaskArray(mask);
    }
    /*
    printf("Ip:Ctr:");
    for (int i = 0; i < IPSIZE; i++)
    {
        printf(" b_ip[%d] = 0x%X(%d)", i, b_ip[i], b_ip[i]);
    }
    printf("\n");
    printf("Ip:Ctr:");
    for (int i = 0; i < IPSIZE; i++)
    {
        printf(" b_mask[%d] = 0x%X(%d)", i, b_mask[i], b_mask[i]);
    }
    printf("\n");
    */
}

// What should I return in case of error?
// should I handle mask?
char* Ip::ExtractIpByte(char* str, unsigned char* byte)
{
    if (!str || !*str)
    {
        return NULL;
    }
    int len = strlen(str);
    for (int i = 0; i < 3 && i < len; i++)
    {
        bool is_digit = isdigit(str[i]);
        if (!is_digit && (str[i] == '.'))
        {
            // OK, we found the end of a number 
            char* str_digit = new char[i+1];
            strncpy(str_digit, str, i);
            int digit = atoi(str_digit);
            if (digit >= 0 && digit <= 255)
            {
                *byte = (unsigned char)digit;
                return str + i + 1;
            }
            else
            {
                return NULL;
            }
        }
        else if (is_digit)
        {
            /*
            printf("i + 1 (%d) == strlen(str) (%d)\n",
                    i+1, (int)strlen(str));
                    */
            //if ((i + 1) == (int)strlen(str))
            if ((i + 1) == (int)strlen(str) ||  (i + 1) == 3)
            {
                int digit = atoi(str);
                if (digit >= 0 && digit <= 255)
                {
                    *byte = (unsigned char)digit;
                    return str + i + 2;
                }
            }
            // Go on!
        }
        else
        {
            // I think this is wrong...
        }
    }
    return NULL;
}

bool Ip::IsValidCharSet(char* str, char* char_set)
{
    for (int i = 0; i < (int)strlen(str); i++)
    {
        if (index(char_set, str[i]) == NULL)
        {
            return false;
        }
    }
    return true;
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

unsigned char Ip::GetMaskNum()
{
    unsigned char rez = 0;
    for (int i = 0; i < IPSIZE; i++)
    {
        if (b_mask[IPSIZE -1 -i] == 0xFF)
        {
            rez += 8;
        }
        else if (b_mask[IPSIZE -1 -i] != 0)
        {
            unsigned char read_mask = 0x01;
            for (int j = 0; j < 8; j++)
            {
                rez += (b_mask[IPSIZE -1 -i] & read_mask) ? 1 : 0;
                read_mask <<= 1;
            }
        }
    }
    return rez;
}

unsigned char* Ip::GetMaskArray(int in_mask)
{
    unsigned char msk = (in_mask != -1) ? in_mask : mask;
    int nr_bytes = msk / 8;
    int extra = msk % 8;
    unsigned char* buf = new unsigned char[IPSIZE];
    memset(buf, 0, IPSIZE);
    for (int i = 0; i < nr_bytes; i++)
    {
        buf[IPSIZE -i -1] = 0xFF;
    }
    if (extra)
    {
        unsigned char access_mask = 0x80;
        for (int j = 0; j < extra; j++)
        {
            buf[IPSIZE -nr_bytes -1] |= access_mask;
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
    if (!(first_addr[0] & 0x01))
    {
        first_addr[0]++;
    }
    return first_addr;
}

unsigned char* Ip::GetNetworkLastAddr()
{
    unsigned char* last_addr = GetNetworkBroadcastAddr();
    if ((mask < 32) && (last_addr[0] & 0x01))
    {
        last_addr[0]--;
    }
    return last_addr;
}

unsigned char* Ip::GetNetworkWildcard()
{
    unsigned char* wildcard = new unsigned char[IPSIZE];
    for (int i = 0; i < IPSIZE; i++)
    {
        wildcard[i] = ~(b_mask[i]);
    }
    return wildcard;
}

int Pow(int base, int exponent)
{
    int rez = 1;
    for (int i = 0; i < exponent; i++)
    {
        rez *= base;
    }
    return rez;
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
    char* wildcard_buf   = new char[IPSIZE * 4];
    memset(ip_buf, 0, IPSIZE * 4);
    memset(mask_buf, 0, IPSIZE * 4);
    memset(network_buf, 0, IPSIZE * 4);
    memset(broadcast_buf, 0, IPSIZE * 4);
    memset(first_ip_buf, 0, IPSIZE * 4);
    memset(last_ip_buf, 0, IPSIZE * 4);
    memset(wildcard_buf, 0, IPSIZE * 4);

    unsigned char* network = GetNetworkAddr();
    unsigned char* broadcast = GetNetworkBroadcastAddr();
    unsigned char* first_ip = GetNetworkFirstAddr();
    unsigned char* last_ip = GetNetworkLastAddr();
    unsigned char* wildcard = GetNetworkWildcard();
    for (int i = IPSIZE; i > 0; i--)
    {
        sprintf(ip_buf, "%s%d", ip_buf, (int)b_ip[i-1]);
        sprintf(mask_buf, "%s%d", mask_buf, (int)b_mask[i-1]);
        sprintf(network_buf, "%s%d", network_buf, (int)network[i-1]);
        sprintf(first_ip_buf, "%s%d", first_ip_buf, (int)first_ip[i-1]);
        sprintf(last_ip_buf, "%s%d", last_ip_buf, (int)last_ip[i-1]);
        sprintf(broadcast_buf, "%s%d", broadcast_buf, (int)broadcast[i-1]);
        sprintf(wildcard_buf, "%s%d", wildcard_buf, (int)wildcard[i-1]);
        if (i-1 != 0)
        {
            sprintf(ip_buf, "%s.", ip_buf);
            sprintf(mask_buf, "%s.", mask_buf);
            sprintf(network_buf, "%s.", network_buf);
            sprintf(first_ip_buf, "%s.", first_ip_buf);
            sprintf(last_ip_buf, "%s.", last_ip_buf);
            sprintf(broadcast_buf, "%s.", broadcast_buf);
            sprintf(wildcard_buf, "%s.", wildcard_buf);
        }
    }
    printf("%s/%d (%c) %s (%s)\n", ip_buf, mask, GetClass(), mask_buf, wildcard_buf);
    printf("Network: %s Broadcast: %s\n", network_buf, broadcast_buf);
    printf("First ip: %s Last ip: %s Hosts available %d\n", first_ip_buf, last_ip_buf, (mask < 32) ? Pow(2, (8 * IPSIZE) - mask) - 2 : 1);
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
    delete[] wildcard_buf;
    delete[] wildcard;
}

bool Ip::IsValidInt32(char* str)
{
    if (str == NULL || !*str)
    {
        return false;
    }
    for (int i = 0; i < (int)strlen(str); i++)
    {
        if (!isxdigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[])
{
    Ip* ip = NULL;
    //printf("main: argc = %d\n", argc);
    if (argc == 1)
    {
        printf("Options:\n");
        printf("\t<program> <hex value for ip>\n");
        printf("\t<program> 0x<hex value for ip>\n");
        printf("\t<program> <ip in dot form>\n");
        printf("\t<program> <ip in dot form>/<mask nr>\n");
        printf("\t<program> <ip in dot form>> <mask in dot form>\n");
        return 0;
    }
    for (int i = 1; i < argc; i++)
    {
        //printf("main: argv[%d] = '%s'\n", i, argv[i]);
        if (strlen(argv[i]) == 8 && Ip::IsValidInt32(argv[i]))
        {
            unsigned int x = strtol(argv[i], NULL, 16);
            ip = new Ip(x);
        }
        else if (strlen(argv[i]) == 10 &&
                 argv[i][0] == '0' &&
                 argv[i][1] == 'x' &&
                 Ip::IsValidInt32(argv[i] + 2))
        {
            unsigned int x = strtol(argv[i], NULL, 16);
            ip = new Ip(x);
        }
        else if (argc == 2)
        {
            ip = new Ip(argv[i]);
        }
        else if (argc == 3)
        {
            ip = new Ip(argv[i], argv[i+1]);
            i++;
        }
    }
    if (ip)
    {
        ip->Print();
    }
    return 0;
}

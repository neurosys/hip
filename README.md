
Description:
    I needed a ip calculator that could accept an 32 bit integer as parameter and convert it to an IP address

    It's not very elegant and at the moment I consider it barely working (didn't actually made intensive testing, yet)

Requirements:
    A C++ compiler. Do you have one? :-)

Instalation:
    Just run make and then copy the binary to a place you like

Runing examples:
    ./hip C0A82A8D
    Should output something on the 



echo ======================================================================================
./hip 0xAC100065
echo ======================================================================================
./hip 10.0.0.1
echo ======================================================================================
./hip 10.0.0.1/23
echo ======================================================================================
./hip 192.168.42.1 255.255.255.0

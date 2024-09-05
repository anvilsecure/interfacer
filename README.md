# Interfacer
Tool to override the default interface to which a binary connects in linux.

The tool hooks socket() and setsockopt() and changes the bound interface to the one selected by the user.

# How to compile
gcc -shared -o interfacer.so interfacer.c -ldl -fPIC

# How to use
LD_PRELOAD=./interfacer.so INTERFACE_NAME={interface_name} {command} {arguments}

# Tests
I tested it on an Ubuntu VM which had 4 interfaces:

```
ens33: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.43.129  netmask 255.255.255.0  broadcast 192.168.43.255
        inet6 fe80::a383:5fe9:84ae:e75e  prefixlen 64  scopeid 0x20<link>
        ether 00:0c:29:ac:c8:db  txqueuelen 1000  (Ethernet)
        RX packets 412621  bytes 599850865 (599.8 MB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 34633  bytes 5951197 (5.9 MB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

ens36: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.43.154  netmask 255.255.255.0  broadcast 192.168.43.255
        inet6 fe80::f76b:ab42:fa0b:c907  prefixlen 64  scopeid 0x20<link>
        ether 00:0c:29:ac:c8:e5  txqueuelen 1000  (Ethernet)
        RX packets 92694  bytes 130152686 (130.1 MB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 4325  bytes 691388 (691.3 KB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

ens37: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.43.155  netmask 255.255.255.0  broadcast 192.168.43.255
        inet6 fe80::e91d:aeb8:71ab:cb13  prefixlen 64  scopeid 0x20<link>
        ether 00:50:56:2a:a5:12  txqueuelen 1000  (Ethernet)
        RX packets 4647  bytes 831371 (831.3 KB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 2896  bytes 270386 (270.3 KB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        inet6 ::1  prefixlen 128  scopeid 0x10<host>
        loop  txqueuelen 1000  (Local Loopback)
        RX packets 2807  bytes 406606 (406.6 KB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 2807  bytes 406606 (406.6 KB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

```

I listened on my host with `ncat -vv -l 4444`.

If a connection is requested without the library I get a connection from `192.168.43.129` belonging to `ens33`:

```
alex@Alexs-MacBook-Pro interfacer % ncat -vv -l 4444
Ncat: Version 7.94 ( https://nmap.org/ncat )
Ncat: Listening on [::]:4444
Ncat: Listening on 0.0.0.0:4444
Ncat: Connection from 192.168.43.129:58186.
```

If a connection is requested with the library I get a connection from `192.168.43.154` belonging to `ens36`:

`LD_PRELOAD=./interfacer.so INTERFACE_NAME=ens36 ncat -vv 192.168.43.1 4444`

```
alex@Alexs-MacBook-Pro interfacer % ncat -vv -l 4444
Ncat: Version 7.94 ( https://nmap.org/ncat )
Ncat: Listening on [::]:4444
Ncat: Listening on 0.0.0.0:4444
Ncat: Connection from 192.168.43.154:54082.

```
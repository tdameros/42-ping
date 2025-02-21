# ft_ping

This project is a custom implementation of the `ping` utility, commonly used to test the reachability of a host on an Internet Protocol (IP) network. The program sends ICMP Echo Request messages to a target host and waits for ICMP Echo Reply messages, measuring the round-trip time and packet loss.

## Options

| Short opt | Long opt            | Description                                      |
|:---------:|---------------------|--------------------------------------------------|
| `-c`      | `--count=NUMBER`    | Stop after sending NUMBER packets                |
| `-f`      | `--flood`           | Flood ping                                       |
| `-i`      | `--interval=NUMBER` | Wait NUMBER seconds between sending each packet  |
| `-w`      | `--timeout=NUMBER`  | Stop after NUMBER seconds                        |
| `-W`      | `--linger=NUMBER`   | Time to wait for a response in seconds           |
| `-v`      | `--verbose`         | Print detailed output                            |
| `-d`      | `--debug`           | Set the SO_DEBUG option                          |
| `-?`      | `--help`            | Give this help list                              |
|           | `--usage`           | Give a short usage message                       |


## Build Instructions
To build the ft_ping executable, use make:

```
make
```

## Examples

Send 5 ICMP packets to example.com:

```
./ft_ping -c 5 example.com
```

Send 3 ICMP packets with increased verbosity to google.com:

```
./ft_ping -v -c 3 google.com
```

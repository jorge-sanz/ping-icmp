# ping-icmp

Ping service written in C using ICMP protocol. Project developed for learning purposes in Networks and Services Architecture subject of Software Engineering degree at University of Valladolid.

## Content

- [Practice statement](ping-icmp-statement.pdf) - Ping ICMP development requirements. (Spanish)
- [miping-Sanz-Perez.c](miping-Sanz-Perez.c) - Ping ICMP implemented.

## Development

### Requirements

- [GCC](https://gcc.gnu.org), the GNU Compiler Collection.

### Installation

```bash
# Clone repository.
git clone https://github.com/jorge-sanz/ping-icmp.git
cd ping-icmp
```

### Compilation

```bash
gcc -Wall -o miping.out miping-Sanz-Perez.c
```

### Execution

```bash
./miping.out ip-address [-v]
```

## Deployment

### Remote Linux Virtual Machine via SSH

This C program should be executed in a provided Slackware Linux machine, so one option to send the source files to there is:

```bash
# Transfer file to remote machine
scp -P <port-number> miping-Sanz-Perez.c youruser@your.machine.address:/destination/folder
```

Then, you can access to your remote machine via SSH and execute it there.

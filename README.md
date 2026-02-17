# Explanations
## Socket:
A socket is file (it's unix) that does the interrucitons with the internet using `send()` and `recv()`.
Using the send and receive function is te same as `read` and `write` in a regular file, but they ensure error free comunication.
I will open a Stream Socket - `SOCK_STREAM`. this is a TCP socket.
There is another type of socket - Datagram or `SOCK_DGRAM`, this is a UDP connection.

## Byte Order
Netwerk Byte Order is Big-Endian.
There is a function to set the byte order right independently of the computer Byte-Order.
for the right function use:
* h/n - for host or network
* to - followed by a `to`
* n/h - for network or host
* s/l - short or long (2 or 4 bytes)

So, for host to nestwork short number the function is `htons()`.

## Structs
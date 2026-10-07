# Non-blocking I/O Explanation

## What changed

The server previously called `send()` directly from command handlers and channel broadcasts. Client sockets were also blocking. This could make the whole server wait when a client was slow or unable to receive data.

The implementation now uses a queued, non-blocking output design:

- The listening socket is created with `SOCK_NONBLOCK`.
- Accepted client sockets are created with `accept4(..., SOCK_NONBLOCK)`.
- Each `Client` has an output buffer.
- `Client::sendMessage()` builds the IRC response and appends it to that buffer instead of calling `send()`.
- `Server::sendMessage()` also appends data to the recipient's output buffer.
- The single central `poll()` call monitors `POLLIN` and, when necessary, `POLLOUT`.
- `POLLOUT` is enabled only for clients that have pending output.
- `sendPendingData()` sends available bytes and removes only the bytes successfully written.
- If only part of a message is written, the remaining bytes stay in the buffer for a later poll cycle.
- `POLLERR`, `POLLNVAL`, and `POLLHUP` are handled by disconnecting the affected client.

Channel broadcasts now use `Server::sendMessage()`, so they follow the same queued-output path as all other responses.

## Why this is correct

`poll()` reports when a socket is ready for a particular operation. `POLLIN` means reading will not block, while `POLLOUT` means writing can proceed without blocking.

The server now follows this sequence:

1. Queue a response when a command needs to send data.
2. Add `POLLOUT` to that client's poll events while the output buffer is non-empty.
3. Wait for the single central `poll()` call to report writable status.
4. Call `send()` only from the `POLLOUT` handling path.
5. Preserve unsent bytes when `send()` performs a partial write.
6. Stop monitoring `POLLOUT` once the buffer is empty.

This prevents a slow client from blocking command processing for other clients, handles partial TCP writes correctly, and satisfies the requirement that socket writes are coordinated through the single `poll()` loop.

The design also keeps the existing command behavior intact: command handlers still request responses in the same way, but the actual network write is deferred until the socket is ready.

## Validation

The implementation was validated with:

- A clean `make re` build using `-Wall -Wextra -Werror -std=c++98`.
- Exactly one `poll()` call in the source.
- Exactly one direct `send()` call, inside `sendPendingData()`.
- No `fcntl()` calls.
- Successful registration of multiple clients.
- Successful JOIN responses, including the no-topic response.
- Successful channel PRIVMSG delivery.
- Successful recovery after an abrupt disconnect during a partial command.
- Runtime confirmation that server and client sockets include `O_NONBLOCK`.

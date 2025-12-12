## **ft_irc**
It is a fully functional **IRC server** implementation written in C++98 (not our desiсsion, but a requirement 😅), following the RFC 1459 / 2812 specifications.
The project focuses on low-level networking, concurrent client handling, and full IRC protocol compliance without using external libraries.

 ## **RUN**
```
make
./ircserv <port> <password>
```
> [!NOTE]
> You can test with `nc localhost <port>`
>
> or install [hexChat](https://hexchat.github.io/) client and use as any other IRC server :)

## ✨**Features**

1. **Non-blocking, multi-client server using `poll()`**

2. **Full IRC registration flow:**
   - PASS, NICK, USER

3. **Channel management:**
   - JOIN (incl. `JOIN 0`)
   - PART
   - TOPIC (incl. empty topic: `TOPIC #chan :`)
   - INVITE
   - KICK

4. **Channel modes:**
   - i – invite-only  
   - t – topic protected  
   - k – channel key  
   - o – operator privileges  
   - l – user limit  

5. **Operator system**
   - Per-channel operators
   - Strict privilege checks

6. **Messaging**
   - PRIVMSG (user ↔ user, user ↔ channel)

7. **Client lifecycle handling**
   - QUIT
   - Automatic disconnection
   - Channel cleanup on last user leave

8. **HexChat compatible**

9. **All numeric replies and errors implemented (RPL_XXX / ERR_XXX)**

10. **Connection & Keep-Alive Handling**
    - Full `PING` / `PONG` support
    - Inactivity timeout & automatic cleanup
    - Handles suspended (`Ctrl+Z`) and frozen clients
    - Safe handling of partial TCP send/receive
    - Prevents ghost and half-open connections

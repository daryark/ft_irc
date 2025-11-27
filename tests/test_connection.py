from .utils.irc_client import IRCClient

def test_nick_and_user_registration(irc_server):
    client = IRCClient(irc_server["host"], irc_server["port"])

    # your server REQUIRES PASSWORD for registration
    client.send_line(f"PASS {irc_server['password']}")

    client.send_line("NICK testnick")
    client.send_line("USER user 0 * :realname")

    responses = []
    for _ in range(5):
        try:
            resp = client.recv_line(timeout=0.5)
            responses.append(resp)
        except TimeoutError:
            break

    print("SERVER RESPONSES:", responses)
    assert any("001" in resp for resp in responses)

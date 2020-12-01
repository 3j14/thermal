from thermal import Printer

with Printer("./serial-port", 9600) as p:
    p.write_bytes(b"asd\n")

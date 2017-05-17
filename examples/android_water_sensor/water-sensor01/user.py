from ulnoiot import *

def main():
    mqtt("192.168.12.144", "pipe07/wsens01")

    from ulnoiot.shield.onboardled import blue
    blue.high() # make sure it's off

    input("water",d2,"1","0",threshold=10)

    ## start to transmit every 10 seconds (or when status changed)
    run(10)

main()

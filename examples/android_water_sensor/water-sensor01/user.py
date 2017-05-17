from ulnoiot import *

def main():
    mqtt("192.168.12.1", "pipe07/wsens01")

    from ulnoiot.shield import onboardled as onboard
    onboard.blue.high() # make sure it's off

    input("water",d2,"1","0")

    ## start to transmit every 10 seconds (or when status changed)
    run(10)

main()

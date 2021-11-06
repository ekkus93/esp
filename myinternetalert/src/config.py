LED = {
    "red": {
            "pin": 33,
            "on": 0,
            "off": 1
    },
    "white": {
            "pin": 4,
            "on": 1,
            "off": 0
    }
}

INTERRUPT = {
    "ping": {
        "timer": 0,
        "interval": 5 * 1000 # 5 secs
    },
    "post": {
        "timer": 1,
        "interval": 60 * 60 * 1000 # 1 hour
    }
}

MAIN = {
    "loop": {
        "interval": 1        # 1 sec
    }
}

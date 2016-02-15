import time

import uinput, mmap, struct

def main():
    events = (
        uinput.KEY_X, # gamepad A
        uinput.KEY_Z, # gamepad B
        uinput.KEY_LEFT,
        uinput.KEY_RIGHT,
        )

    x_on = False
    z_on = False
    l_on = False
    r_on = False

    with uinput.Device(events, "uinput-mario", 3, 1, 1, 1) as device, \
        open("gamestate", "r+b") as f:
        m = mmap.mmap(f.fileno(), 0, prot=mmap.PROT_READ)
        while True:
            time.sleep(.2) 
            (marioX, marioY, luigiX, luigiY) = struct.unpack_from('iiii', m)
            print marioX, marioY, luigiX, luigiY

            if (marioX == 0) and (marioY == 0) and (luigiX == 0) and (luigiY == 0):
                continue;

            if (marioX < luigiX) and not r_on:
                if l_on:
                    device.emit(uinput.KEY_LEFT, 0, False)
                    l_on = False
                device.emit(uinput.KEY_RIGHT, 1, True)
                r_on = True

            elif (marioX > luigiX) and not l_on:
                if r_on:
                    device.emit(uinput.KEY_RIGHT, 0, False)
                    r_on = False
                device.emit(uinput.KEY_LEFT, 1, True)
                l_on = True

if __name__ == "__main__":
    main()

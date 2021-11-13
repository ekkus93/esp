import uos
import machine
import connect

connect.doConnect()
uos.mount(machine.SDCard(), "/sd")

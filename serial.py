import pyudev

context = pyudev.Context()
for device in context.list_devices(subsystem='tty', DEVTYPE='serial'):
    if 'ID_SERIAL' in device:
        print(device['ID_SERIAL'])

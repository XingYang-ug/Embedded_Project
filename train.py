from bleak import BleakScanner, BleakClient
import time
import struct
import asyncio

DEVICE_NAME = 'Elfo'
AX_UUID = '00002101-0000-1000-8000-00805f9b34fb' #check 
AY_UUID = '00002102-0000-1000-8000-00805f9b34fb'
AZ_UUID = '00002103-0000-1000-8000-00805f9b34fb'
GX_UUID = '00001102-0000-1000-8000-00805f9b34fb'
GY_UUID = '00001103-0000-1000-8000-00805f9b34fb'
GZ_UUID = '00001104-0000-1000-8000-00805f9b34fb'
SCAN_INTERVAL_SEC = 5



async def main():
    address = None
    while True:
        print('Scanning...')
        devices = await BleakScanner.discover()
        for d in devices:
            print(f'{d.name}')
            if d.name == DEVICE_NAME:
                address = str(d.address)
                print(f'Found {DEVICE_NAME}! Address: {address}')
                break
        if address is not None:
            break
        time.sleep(SCAN_INTERVAL_SEC)

    async with BleakClient(address) as client:
        while True:
            ax_bytes = await client.read_gatt_char(AX_UUID)
            ax = struct.unpack('<f', ax_bytes)[0]
            ay_bytes = await client.read_gatt_char(AY_UUID)
            ay = struct.unpack('<f', ay_bytes)[0]
            az_bytes = await client.read_gatt_char(AZ_UUID)
            az = struct.unpack('<f', az_bytes)[0]
            gx_bytes = await client.read_gatt_char(GX_UUID)
            gx = struct.unpack('<f', gx_bytes)[0]
            gy_bytes = await client.read_gatt_char(GY_UUID)
            gy = struct.unpack('<f', gy_bytes)[0]
            gz_bytes = await client.read_gatt_char(GZ_UUID)
            gz = struct.unpack('<f', gz_bytes)[0]
            with open('data.csv', 'a') as f:
                f.write(f'{time.time()},0,{ax},{ay},{az},{gx},{gy},{gz}\n') # 0 sitting, 1 standing
            

if __name__ == "__main__":
    asyncio.run(main())

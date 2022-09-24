from machine import Pin, I2C, RTC
import time

DEFAULT_ADDRESS = 0x52

# I2C registers
REG_SECONDS = 0x00
REG_STATUS = 0x0E
REG_CONTROL1 = 0x0F
REG_EEADDR = 0x25
REG_EEDATA = 0x26
REG_EECMD = 0x27

# EEPROM registers
REG_BACKUP = 0x37

# Bit positions in registers
POS_STATUS_EEBUSY = 7
POS_CONTROL1_EERD = 3
POS_BACKUP_BSM = 2
POS_BACKUP_FEDE = 4


class RV3028:
    
    def __init__(self, pin_sda, pin_scl, addr=DEFAULT_ADDRESS):
        """
        Class for interfacing with the RV3028 RTC.
        
        :param int pin_sda: GPIO pin number for I2C SDA.
        :param int pin_scl: GPIO pin number for I2C SCL.
        :param int addr: I2C address.
        """
        self.addr = addr
        self.i2c = I2C(0, sda=Pin(pin_sda), scl=Pin(pin_scl))
        self.internal_rtc = RTC()
        
        # Enable battery backup
        self.set_battery_switchover_mode(1)
    
    def encode_bcd(self, normal_int):
        """
        Encode an int to a two-digit binary-coded decimal number.
        
        :param int normal_int: Integer to encode.
        :returns int: Encoded as BCD.
        """
        lower = normal_int % 10
        upper = (normal_int // 10) % 10
        return lower + (upper << 4)
    
    def decode_bcd(self, encoded_int):
        """
        Decode a two-digit binary-coded decimal number.
        
        :param int encoded_int: Encoded two-digit BCD number.
        :returns int: Decoded as int.
        """
        lower = encoded_int & 0x0F
        upper = (encoded_int & 0xF0) >> 4
        return lower + (upper * 10)
    
    def read_datetime(self):
        """
        Read the current date and time from the RV3028.
        
        :returns tuple: MicroPython machine.RTC datetime tuple. Format is 
        (year, month, day, weekday, hours, minutes, seconds, subseconds).
        """
        # Read the following registers:
        # 0x00: seconds
        # 0x01: minutes
        # 0x02: hours
        # 0x03: weekday
        # 0x04: day
        # 0x05: month
        # 0x06: year
        data = self.i2c_read(REG_SECONDS, 7)
        
        # Decode binary-coded decimal numbers
        x = [self.decode_bcd(byte) for byte in data]
        
        # Rearrange into machine.RTC datetime tuple
        dttuple = (x[6] + 2000, x[5], x[4], x[3], x[2], x[1], x[0], 0)
        return dttuple
    
    def set_datetime(self, dttuple):
        """
        Set a new date and time on the RV3028.
        
        :param tuple dttuple: MicroPython machine.RTC datetime tuple. Format is 
        (year, month, day, weekday, hours, minutes, seconds, subseconds).
        """
        # Rearrange datetime tuple to register order
        data = [dttuple[x] for x in [6, 5, 4, 3, 2, 1, 0]]
        
        # Convert to BCD
        data = [self.encode_bcd(x) for x in data]
        
        # Convert to byte array
        data = [x.to_bytes(1, 'little') for x in data]
        data = b''.join(data)
        
        # Write the following registers:
        # 0x00: seconds
        # 0x01: minutes
        # 0x02: hours
        # 0x03: weekday
        # 0x04: day
        # 0x05: month
        # 0x06: year
        self.i2c_write(REG_SECONDS, data)
        
    def set_from_systime(self):
        """
        Set the RV3028 time to the internal system time.
        """
        dttuple = self.internal_rtc.datetime()
        self.set_datetime(dttuple)
    
    def read_to_systime(self):
        """
        Set the internal system time to the RV3028 time.
        """
        dttuple = self.read_datetime()
        self.internal_rtc.datetime(dttuple)
    
    def i2c_read(self, register, num_bytes=1):
        """
        Read byte(s) from register(s).
        
        :param int register: Register to start reading from.
        :param int num_bytes: Number of bytes to read.
        :returns bytearray: Bytes read from I2C.
        """
        return self.i2c.readfrom_mem(self.addr, register, num_bytes)
    
    def i2c_write(self, register, data):
        """
        Write byte(s) to register(s).
        
        :param int register: Register to start writing to.
        :param data: Data to write. Can be int, byte, or bytearray.
        """
        if isinstance(data, int):
            data = data.to_bytes(1, 'little')
        self.i2c.writeto_mem(self.addr, register, bytearray(data))
    
    def i2c_read_bit(self, register, position):
        """
        Read a bit from a register. Used for reading status bits.
        
        :param int register: I2C register to read.
        :param int position: Byte position to return, from 0 to 7.
        :returns bool: Value, either True or False.
        """
        reg_data = self.i2c_read(register)[0]
        return bool((reg_data >> position) & 1)
    
    def i2c_write_bit(self, register, position, value):
        """
        Write a bit to a register while keeping all other bits the same. Used
        for changing config bits.
        
        :param int register: I2C register to read.
        :param int position: Byte position to write, from 0 to 7.
        :param bool value: Value to write to the bit.
        """
        reg_data = self.i2c_read(register)[0]
        if value:
            reg_data = reg_data | (1 << position)
        else:
            reg_data = reg_data & ~(1 << position)
        self.i2c_write(register, reg_data)
        
    def eeprom_wait_busy(self):
        """
        Wait until the EEBusy bit is cleared before continuing an EEPROM
        read or write operation.
        """
        while True:
            busy = self.i2c_read_bit(REG_STATUS, POS_STATUS_EEBUSY)
            if not busy: break
        
    def eeprom_read(self, ee_address):
        """
        Read data from a memory address in the EEPROM.
        
        :param int ee_address: EEPROM address to read.
        :returns bytearray: Value of the EEPROM address.
        """
        # Before starting to read a byte in the EEPROM, the auto refresh of the
        # registers from the EEPROM has to be disabled by writing 1 into the
        # EERD control bit.
        self.i2c_write_bit(REG_CONTROL1, POS_CONTROL1_EERD, True)
        self.eeprom_wait_busy()
        
        # In order to read a single byte from the EEPROM,
        # the address to be read is entered in the EEADDR register,
        # then the command 00h is written in the EECMD register,
        # then the second command 22h is written in the EECMD register
        # and the resulting byte can be read from the EEDATA register.
        self.i2c_write(REG_EEADDR, ee_address)
        self.i2c_write(REG_EECMD, 0x00)
        self.i2c_write(REG_EECMD, 0x22)
        
        self.eeprom_wait_busy()
        ee_data = self.i2c_read(REG_EEDATA)
        
        # When the transfer is finished (EEbusy = 0), the user can enable again
        # the auto refresh of the registers by writing 0 into the EERD bit in
        # the Control 1 register.
        self.eeprom_wait_busy()
        self.i2c_write_bit(REG_CONTROL1, POS_CONTROL1_EERD, False)
        
        return ee_data
    
    def eeprom_write(self, ee_address, ee_data):
        """
        Write data to a memory address in the EEPROM.
        
        :param int ee_address: EEPROM address to write.
        :param ee_data: Byte to write to the EEPROM, can be int or byte.
        """
        # Before starting to change data stored in the EEPROM, the auto refresh
        # of the registers from the EEPROM has to be disabled by writing 1 into
        # the EERD control bit.
        self.i2c_write_bit(REG_CONTROL1, POS_CONTROL1_EERD, True)
        self.eeprom_wait_busy()
        
        # In order to write a single byte to the EEPROM,
        # the address to which the data must be written is entered in the
        # EEADDR register
        # and the data to be written is entered in the EEDATA register,
        # then the command 00h is written in the EECMD register,
        # then a second command 21h is written in the EECMD register to start
        # the EEPROM write.
        self.i2c_write(REG_EEADDR, ee_address)
        self.i2c_write(REG_EEDATA, ee_data)
        self.i2c_write(REG_EECMD, 0x00)
        self.i2c_write(REG_EECMD, 0x21)
        
        # When the transfer is finished (EEbusy = 0), the user can enable again
        # the auto refresh of the registers by writing 0 into the EERD bit in
        # the Control 1 register.
        self.eeprom_wait_busy()
        self.i2c_write_bit(REG_CONTROL1, POS_CONTROL1_EERD, False)
        
    def set_battery_switchover_mode(self, bsm):
        """
        Set battery switchover mode. 
        
        :param BSM: Battery Switchover Mode. Can supply the following values:
        * 0: Switchover disabled.
        * 1: Direct Switching Mode. Switchover when V_DD < V_BACKUP. For when
            V_DD is significantly higher than V_BACKUP (e.g. 5 V).
        * 2: Switchover disabled.
        * 3: Level Switching Mode. Switchover when V_DD < 2.0 V. For when
            V_DD is around the same as V_BACKUP (e.g. 3.5 V).
        """
        # Read the current value of the EEPROM Backup register.
        ee_data = self.eeprom_read(REG_BACKUP)[0]
        
        # Clear the BSM and FEDE (Fast Edge Detection Enable) bits.
        ee_data = ee_data & ~(
            (1 << POS_BACKUP_BSM) &
            (1 << (POS_BACKUP_BSM + 1)) &
            (1 << POS_BACKUP_FEDE)
        )
        
        # Write the BSM bits.
        ee_data = ee_data | ((bsm & 0x3) << POS_BACKUP_BSM)
        
        # Set FEDE to 1, required for automatic battery switchover.
        ee_data = ee_data | (1<<POS_BACKUP_FEDE)
        
        # Write the updated config to the EEPROM Backup register.
        self.eeprom_write(REG_BACKUP, ee_data)

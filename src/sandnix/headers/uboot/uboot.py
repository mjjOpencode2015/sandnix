#! /usr/bin/env python3
# -*- coding: utf-8 -*-

'''
    Copyright 2016,暗夜幽灵 <darknightghost.cn@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
'''

import struct
import time
import binascii
import gzip
import os

class UnsupportedArch(Exception):
    def __init__(self, arch_name):
        self,arch = arch_name

    def __str__(self):
        return "Unsupported architecture : \"%s\".\n"%(self.arch)

class uboot:
    #Offsets
    #32 bits
    HCRC_OFF = 4
    TIME_OFF = HCRC_OFF + 4
    SIZE_OFF = TIME_OFF + 4
    LOAD_OFF = SIZE_OFF + 4
    EP_OFF = LOAD_OFF + 4
    DCRC_OFF  = EP_OFF + 4
    ARCH_OFF = DCRC_OFF + 5
    TYPE_OFF = ARCH_OFF + 1

    #Architectures
    LE_ARCH = {"x86" : 3, "amd64" : 3, "arm" : 2}
    BE_ARCH = {}

    #Types
    IH_TYPE_KERNEL = 2
    IH_TYPE_RAMDISK = 3
    def __init__(self, path, arch):
        f = open(path, "rb")
        self.data = f.read()
        f.close();

        if not (arch in uboot.LE_ARCH.keys() or arch in uboot.BE_ARCH.keys()):
            raise UnsupportedArch(arch)

        self.arch = arch

        return

    def set_data_address(self, addr):
        self.write_32(uboot.LOAD_OFF, addr)
        return

    def set_type(self, header_type):
        self.write_8(uboot.TYPE_OFF, header_type)
        return

    def set_entry_point(self, addr):
        self.write_32(uboot.EP_OFF, addr)
        return

    def write_32(self, off, num):
        packed = struct.pack(">I", num)
        self.data = self.data[: off] + packed + self.data[off + 4 :]
        return
        
    def write_8(self, off, num):
        packed = struct.pack(">B", num)
        self.data = self.data[: off] + packed + self.data[off + 1 :]
        return

    def read_32(self, off):
        return struct.unpack(">I", self.data[off : off + 4])[0]

    def read_8(self, off):
        if self.arch in uboot.LE_ARCH:
            return struct.unpack("<B", self.data[off : off + 1])[0]
        else:
            return struct.unpack(">B", self.data[off : off + 1])[0]

    def save(self, image_data, f):
        #Fill uimage header
        self.write_32(uboot.TIME_OFF, int(time.time()))

        if self.arch in uboot.LE_ARCH:
            self.write_8(uboot.ARCH_OFF, uboot.LE_ARCH[self.arch])
        else:
            self.write_8(uboot.ARCH_OFF, uboot.BE_ARCH[self.arch])

        #Data size
        self.write_32(uboot.SIZE_OFF, len(image_data))

        #Data crc32
        self.write_32(uboot.DCRC_OFF, binascii.crc32(image_data))

        #Header crc32
        self.write_32(uboot.HCRC_OFF, 0)
        self.write_32(uboot.HCRC_OFF, binascii.crc32(self.data))

        f.write(self.data)
        f.write(image_data)
        return

    def __str__(self):
        return "Uboot header info:\n" \
                + "Header crc : 0x%.8X\n"%(self.read_32(self.HCRC_OFF)) \
                + "Time stamp : 0x%.8X\n"%(self.read_32(self.TIME_OFF)) \
                + "Data size : 0x%.8X\n"%(self.read_32(self.SIZE_OFF)) \
                + "Data load address : 0x%.8X\n"%(self.read_32(self.LOAD_OFF)) \
                + "Entry point : 0x%.8X\n"%(self.read_32(self.EP_OFF)) \
                + "Data crc : 0x%.8X\n"%(self.read_32(self.DCRC_OFF)) \
                + "Architecture : %s\n"%(self.arch)

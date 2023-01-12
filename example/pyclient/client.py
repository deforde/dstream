#!/usr/bin/env python3

from ctypes import c_char
from enum import Enum
from math import sin, pi
import socket
import struct
from time import sleep

SOCK_PATH = "/tmp/dstream_socket"


class Dstream_Data_Type(Enum):
    U8 = 0
    U16 = 1
    U32 = 2
    U64 = 3
    I8 = 4
    I16 = 5
    I32 = 6
    I64 = 7
    F32 = 8
    F64 = 9


def getDataSize(d_ty: Dstream_Data_Type):
    if d_ty == Dstream_Data_Type.U8 or d_ty == Dstream_Data_Type.I8:
        return 1
    elif d_ty == Dstream_Data_Type.U16 or d_ty == Dstream_Data_Type.I16:
        return 2
    elif d_ty == Dstream_Data_Type.U32 or d_ty == Dstream_Data_Type.I32 or d_ty == Dstream_Data_Type.F32:
        return 4
    elif d_ty == Dstream_Data_Type.U64 or d_ty == Dstream_Data_Type.I64 or d_ty == Dstream_Data_Type.F64:
        return 8


def getDataPackFmt(d_ty: Dstream_Data_Type):
    if d_ty == Dstream_Data_Type.U8:
        return "B"
    elif d_ty == Dstream_Data_Type.U16:
        return "H"
    elif d_ty == Dstream_Data_Type.U32:
        return "I"
    elif d_ty == Dstream_Data_Type.U64:
        return "Q"
    elif d_ty == Dstream_Data_Type.I8:
        return "b"
    elif d_ty == Dstream_Data_Type.I16:
        return "h"
    elif d_ty == Dstream_Data_Type.I32:
        return "i"
    elif d_ty == Dstream_Data_Type.I64:
        return "q"
    elif d_ty == Dstream_Data_Type.F32:
        return "f"
    elif d_ty == Dstream_Data_Type.F64:
        return "d"


def packPacket(d_ty: Dstream_Data_Type, nm: str, data: list):
    fmt = getDataPackFmt(d_ty)
    dsz = getDataSize(d_ty)

    padding = 0
    nmsz = len(nm) + 1
    if nmsz % dsz != 0:
        padding = dsz - (nmsz % dsz)
        nmsz += padding
    psz = nmsz + len(data) * dsz

    packet = [struct.pack("=iiQ", d_ty.value, 0, psz)]
    packet.append(bytes(nm, encoding="utf-8"))
    packet.append(bytes("\0" * (padding + 1), encoding="utf-8"))
    for x in data:
        packet.append(struct.pack(f"={fmt}", x))

    return packet


with socket.socket(family=socket.AF_UNIX, type=socket.SOCK_STREAM) as sock:
    sock.connect(SOCK_PATH)

    t_s = 0.0
    T_s = 0.1
    freq = 1.0

    while(True):
        data = []
        for _ in range(100):
            data.append(0.5 + 0.5 * sin(2 * pi * freq * t_s))
            t_s += T_s / 100

        packet = packPacket(Dstream_Data_Type.F32, "test", data)

        sock.sendall(b"".join(packet))

        sleep(0.1)

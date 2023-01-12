from enum import Enum
import argparse
import gdb
import json
import socket
import struct

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
    elif (
        d_ty == Dstream_Data_Type.U32
        or d_ty == Dstream_Data_Type.I32
        or d_ty == Dstream_Data_Type.F32
    ):
        return 4
    elif (
        d_ty == Dstream_Data_Type.U64
        or d_ty == Dstream_Data_Type.I64
        or d_ty == Dstream_Data_Type.F64
    ):
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


class TraceDataCommand(gdb.Command):
    def __init__(self):
        super(TraceDataCommand, self).__init__("trace_data", gdb.COMMAND_USER)
        self.parser = argparse.ArgumentParser()
        self.parser.add_argument("desc", type=str, help="print json descriptor")

    def complete(self, text, word):
        return gdb.COMPLETE_SYMBOL

    def _decay_type(self, val):
        while val.type.strip_typedefs().code == gdb.TYPE_CODE_PTR:
            val = val.dereference()
        return val

    def invoke(self, args, from_tty):
        desc = json.loads(args)
        ident = desc["id"]

        packets = []

        if "@" in ident:
            ident, arr_len = ident.split("@")
            val = gdb.parse_and_eval(ident)
            arr_len = gdb.parse_and_eval(arr_len)
            arr_len = int(arr_len)
            assert val.type.strip_typedefs().code == gdb.TYPE_CODE_PTR
            val = val.cast(
                gdb.parse_and_eval(
                    f"*({val.type.strip_typedefs().target().name}(*)[{arr_len}])&{ident}"
                ).type.strip_typedefs()
            ).dereference()
            data = [float(x) for x in str(val).strip("{} ").split(",")]
            packets.append(packPacket(Dstream_Data_Type.F32, ident, data))
        else:
            val = gdb.parse_and_eval(ident)
            val = self._decay_type(val)
            if val.type.strip_typedefs().code == gdb.TYPE_CODE_STRUCT:
                for field in val.type.strip_typedefs().fields():
                    field_val = self._decay_type(val[field.name])
                    packets.append(packPacket(Dstream_Data_Type.F32, f"{ident}.{field_name}", float(str(field_val))))
            else:
                packets.append(packPacket(Dstream_Data_Type.F32, f"{ident}", float(str(val))))

        with socket.socket(family=socket.AF_UNIX, type=socket.SOCK_STREAM) as sock:
            sock.connect(SOCK_PATH)
            sock.sendall(b"".join(packets))


TraceDataCommand()

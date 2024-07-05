

import ctypes

class RqCfg(ctypes.LittleEndianStructure):
    _fields_ = [
        ('a', ctypes.c_uint64, 4),
        ('b', ctypes.c_uint64, 4),
        ('c', ctypes.c_uint64, 8),
    ]

class RqCfgUnion(ctypes.Union):
    _fields_ = [
        ('bits', RqCfg),
        ('val', ctypes.c_uint64)
    ]

rqCfgUnion = RqCfgUnion()
rqCfgUnion.bits.a = 1
rqCfgUnion.bits.b = 2
rqCfgUnion.bits.c = 0x23

# val=0000000000002321
print("val=%016x\r\n" % rqCfgUnion.val) 

#define BIG_NUM 0x1234567890ULL


quad:
.quad BIG_NUM

num:
.8byte BIG_NUM

low:
.octa BIG_NUM & 0xFFFFFFFF


high:
.4byte BIG_NUM >> 32


end:
.word  0x12345678

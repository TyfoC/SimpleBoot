#	Build your kernel!
##	**The project is built in the `build` directory, the result is in `build/bin`!**
##	**Be sure to look at the `build/Makefile` for build information!**
###	To build the kernel, you need to specify some parameters that will make the right environment for your kernel:
`cpum` - the processor mode your kernel is running in
| CPU Mode | Base address |
| :------- | :----------: |
| 32       | 0x100000     |
| 16       | 0x6000       |

`vmt` - video mode type
| Video mode type | Description                     |
| :-------        | :-----------------------------: |
| 1               | VGA text 80x25 16 colors        |
| 2               | VGA graphics 320x200 256 colors |
| 3               | VESA graphics                   |

`vmw` - video mode width, `vmh` - video mode height, `vmbpp` - video mode bits per pixel
| Video mode type | Width | Height | BPP | Buffer offset |
| :-------        | :---: | :----: | :-: | :-----------: |
| 1               | 80    | 25     | 8   | 0xB8000       |
| 2               | 320   | 200    | 8   | 0xA0000       |
| 3               | UD    | UD     | UD  | ???           |

`UD` - user-defined

`vma` - video mode attributes (**only for VESA**) (page - 31, real page offset - 40: https://pdos.csail.mit.edu/6.828/2018/readings/hardware/vbe3.pdf)

###	Build and run examples
####	32-bit kernel:
```sh
#	cpum = 32
#	vmt (video modde type) = 3 (VESA)
#	vmw (video mode width) = 800
#	vmh (video mode height) = 600
#	vmbpp (video mode bits per pixel) = 24
#	vma (VESA video mode attributes) = 144 (COLOR & LFB)
make build-kernel-cfg cpum=32 vmt=3 vmw=800 vmh=600 vmbpp=24 vma=144 && make build-os32 && make run-os32
```
####	16-bit kernel:
```sh
#	cpum = 16
#	vmt (video modde type) = 2 (VGA)
#	vmw (video mode width) = 320
#	vmh (video mode height) = 200
#	vmbpp (video mode bits per pixel) = 8 (256 colors)
#	vma (VESA video mode attributes) = 0
make build-kernel-cfg cpum=16 vmt=2 vmw=320 vmh=200 vmbpp=8 vma=0 && make build-os16 && make run-os16
```
###	How can I clear all files that I have compiled?
####	Just do this:
```sh
make clear-os16
make clear-os32
```
###	In which folder should I write the code and where is the main file, the entry point?
####	If you want to write a kernel for real mode \[16-bit\], then all files in the `../kernel16` folder, if you want to write a kernel for protected mode \[32-bit\], then the path to the folder is `../kernel32`

###	Real Mode OS limitations
####	Due to GCC restrictions, real mode code can only be located in the first segment, so your kernel is located at 0x6000, you only have 0xA000 bytes available

###	What information does the OS get at startup?
####	Structure fields you will see in the file `system-info-block.h`
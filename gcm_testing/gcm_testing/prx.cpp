#include <cellstatus.h>
#include <sys/prx.h>


//#define HAVE_STARFIELD 1
//#define HAVE_SSCROLLER 1
//#define HAVE_PNG_BG 1

#include <cell/pad.h>
//#include <cell/rtc.h>
#include <cell/cell_fs.h>
#include <cell/sysmodule.h>
//#include <sysutil/sysutil_sysparam.h>
//#include <sysutil/sysutil_music2.h>
#include <sysutil/sysutil_common.h>
//#include <sysutil/sysutil_search.h>
//#include <sysutil/sysutil_rec.h>
#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <string.h>
#include <sys/sys_time.h>
#include <sys/time_util.h>
#include <stdarg.h>
#include <assert.h>
#include <sys/process.h>
#include <sys/memory.h>
#include <sys/timer.h>
#include <sys/return_code.h>
#include <sys/prx.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
#include <cellstatus.h>
#include <typeinfo>
#include <vector>
#include <pthread.h>
#include <locale.h>
#include <cell/error.h>
#include <sys/paths.h>
#include <time.h>
#include <net\if_dl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cell/cell_fs.h>
#include <cell/sysmodule.h>
#include <stdio.h>
#include <string.h>
#include <cell/fs/cell_fs_errno.h>
#include <cell/fs/cell_fs_file_api.h>
#include <ppu_intrinsics.h>
#include <cstdlib>

#include <cell\codec\gifdec.h>
#include <cell\codec\jpgdec.h>
#include <cell\codec\jpgenc.h>
#include <cell\codec\pngcom.h>
#include <cell\codec\pngdec.h>
#include <cell\codec\pngenc.h>
#include <sys\memory.h>
#include <sys\mempool.h>
#include <sys\moduleexport.h>
#include <sysutil\sysutil_photo_decode.h>
#include <sysutil\sysutil_photo_export.h>
#include <sysutil\sysutil_photo_import.h>
#include <cell/gcm.h>
#include <cell\fontGcm.h>  //C:\usr\local\cell\target\ppu\include\cell\fontGcm.h/
#include <cell\gcm_pm.h>   //C:\usr\local\cell\target\ppu\include\cell\gcm_pm.h/
#include <cell\font\libfontGcm.h>  //C:\usr\local\cell\target\ppu\include\cell\font\libfontGcm.h/

#include <fastmath.h>
#include <math.h>
#include <cmath>
#include <cfloat>
#include <sys/random_number.h>

#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#pragma comment(lib, "lv2dbg_stub") // for the dbg header
#pragma comment(lib, "net_stub")
#pragma comment(lib, "netctl_stub")
#pragma comment(lib, "pngdec_stub")
#pragma comment(lib, "pngenc_stub")
#pragma comment(lib, "sysutil_imejp_stub")
#pragma comment(lib, "sysutil_oskdialog_ext_stub")
#pragma comment(lib, "sysutil_photo_decode_stub")
#pragma comment(lib, "sysutil_photo_export_stub")
#pragma comment(lib, "sysutil_photo_import_stub")
#pragma comment(lib, "sysutil_photo_stub")
#pragma comment(lib, "sysutil_print_stub")
#pragma comment(lib, "sysutil_rec_stub")
#pragma comment(lib, "dbgfont_gcm")
#pragma comment(lib, "dbg_gcm_method_check")
#pragma comment(lib, "gcm_cmd")
#pragma comment(lib, "gcm_cmdasm")
#pragma comment(lib, "gcm_cmddbg")
#pragma comment(lib, "gcm_gpad_stub")
#pragma comment(lib, "gcm_pm")
//#pragma comment(lib, "gcmstub")
#pragma comment(lib, "sysmodule_stub")
#pragma comment(lib, "sysutil_stub")
#pragma comment(lib, "sysutil_np_stub") 


#pragma comment(lib, "fs_stub") 
#pragma comment(lib, "rtc_stub") 
#pragma comment(lib, "io_stub") 
#pragma comment(lib, "font_stub")
#pragma comment(lib, "gcm_sys_stub")
#pragma comment(lib, "sysutil_music_stub")
#pragma comment(lib, "sysutil_search_stub")
#pragma comment(lib, "sysutil_video_export_stub")


extern "C" void *_sys_malloc(size_t size);
extern "C" void _sys_free(void *ptr);
// misc.h

static void *getNIDfunc2(const char *sprx_file, const char *prx_name, const char *module, uint32_t fnid, int32_t offset)
{
	char filename[256], fbuf[128];
	uint32_t mod_base, hdr_size, off, phoff, loff;
	int32_t fd, r;
	uint64_t nread;
	uint64_t nbytes;

	sys_prx_id_t prx_id = sys_prx_get_module_id_by_name(prx_name, 0, NULL);
	sys_prx_module_info_t pInfo;
	sys_prx_segment_info_t segment;
	pInfo.filename = &filename[0];
	pInfo.segments = &segment;
	pInfo.filename_size = 256;
	pInfo.segments_num = 1;
	pInfo.size = sizeof(sys_prx_module_info_t);
	r = sys_prx_get_module_info(prx_id, 0, &pInfo);
	mod_base = pInfo.segments->base;

	// read program header offset
	cellFsOpen(sprx_file, CELL_FS_O_RDONLY, &fd, NULL, 0);
	nbytes = 128;
	cellFsReadWithOffset(fd, 0x90/*skip sce header*/, &fbuf[0], nbytes, &nread);

	hdr_size = *(uint32_t *)(fbuf + 0x24);
	off = *(uint32_t *)(fbuf + hdr_size + 0x0C);
	phoff = *(uint32_t *)(fbuf + hdr_size + 0x1C);
	loff = phoff - off; // logical offset

	uint32_t export_start = *(uint32_t *)(mod_base + loff + 0x24);
	uint8_t export_stru_size = (uint8_t)(*(uint32_t *)export_start >> 24);
	while (export_stru_size == 0x1C || export_stru_size == 0x28) {
		uint32_t *export_stru_ptr = (uint32_t *)export_start;
		const char *lib_name_ptr = (const char *)(*(uint32_t *)((uint8_t *)export_stru_ptr + 0x10));
		if (lib_name_ptr) {
			if (strncmp(module, lib_name_ptr, strlen(lib_name_ptr)) == 0) {
				// we got the proper export struct
				uint32_t lib_fnid_ptr = *(uint32_t *)((char *)export_stru_ptr + 0x14);
				uint32_t lib_func_ptr = *(uint32_t *)((char *)export_stru_ptr + 0x18);
				uint16_t count = *(uint16_t *)((char *)export_stru_ptr + 6); // number of exports
				for (int i = 0; i < count; i++) {
					if (fnid == *(uint32_t *)((char *)lib_fnid_ptr + i * 4)) {
						// take address from OPD
						return((void **)*((uint32_t *)(lib_func_ptr)+i) + offset);
					}
				}
			}
		}
		export_stru_size = (uint8_t)(*(uint32_t *)export_start >> 24);
		export_start += export_stru_size;
	}
	return(NULL);
}


class Vector3
{
public:

	// -------------------- Attributes -------------------- //

	// Components of the vector
	float x, y, z;

	// -------------------- Methods -------------------- //

	// Constructor
	Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

	// Constructor
	Vector3(const Vector3& vector) : x(vector.x), y(vector.y), z(vector.z) {}

	// Constructor
	~Vector3() {}

	// = operator
	Vector3& operator=(const Vector3& vector) {
		if (&vector != this) {
			x = vector.x;
			y = vector.y;
			z = vector.z;
		}
		return *this;
	}

	// + operator
	Vector3 operator+(const Vector3 &v) const {
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	// += operator
	Vector3& operator+=(const Vector3 &v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	// - operator
	Vector3 operator-(const Vector3 &v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	// -= operator
	Vector3& operator-=(const Vector3 &v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	// == operator
	bool operator==(const Vector3 &v) const {
		return x == v.x && y == v.y && z == v.z;
	}

	// != operator
	bool operator!=(const Vector3 &v) const {
		return !(*this == v);
	}

	// * operator
	Vector3 operator*(float f) const {
		return Vector3(f*x, f*y, f*z);
	}

	// *= operator
	Vector3 &operator*=(float f) {
		x *= f; y *= f; z *= f;
		return *this;
	}

	// / operator
	Vector3 operator/(float f) const {
		float inv = 1.f / f;
		return Vector3(x * inv, y * inv, z * inv);
	}

	// /= operator
	Vector3 &operator/=(float f) {
		float inv = 1.f / f;
		x *= inv; y *= inv; z *= inv;
		return *this;
	}

	// - operator
	Vector3 operator-() const {
		return Vector3(-x, -y, -z);
	}

	// [] operator
	float &operator[](int i) {
		switch (i) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		}
		return z;
	}

	// [] operator
	const float &operator[](int i) const {
		switch (i) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		}
		return z;
	}

	// Cross product operator
	Vector3 cross(const Vector3 &v) const {
		return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	// Dot product operator
	float dot(const Vector3 &v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	// Normalize the vector and return it
	Vector3 Normalize() {
		float l = length();
		if (l == 0) return;
		x /= l;
		y /= l;
		z /= l;
		return *this;
	}

	bool isNull() const {
		return(x == 0. && y == 0. && z == 0.);
	}

	// Clamp the values between 0 and 1
	Vector3 clamp01() {
		if (x > 1.f) x = 1.f;
		else if (x < 0.f) x = 0.f;
		if (y > 1.f) y = 1.f;
		else if (y < 0.f) y = 0.f;
		if (z > 1.f) z = 1.f;
		else if (z < 0.f) z = 0.f;
		return *this;
	}

	// Return the squared length of the vector
	float lengthSquared() const { return x * x + y * y + z * z; }

	// Return the length of the vector
	float length() const { return sqrt(lengthSquared()); }
};

inline Vector3 operator*(float f, const Vector3 & o) {
	return o * f;
}

typedef struct {
	float x, y, z, w;
} Vector4;


class Color
{
public:

	// -------------------- Attributes -------------------- //

	// Components of the color
	int r, g, b, a;

	// -------------------- Methods -------------------- //

	// Constructor
	Color(int r = 0, int g = 0, int b = 0, int a = 0) : r(r), g(g), b(b), a(a) {}

	// Constructor
	Color(const Color& color) : r(color.r), g(color.g), b(color.b), a(color.a) {}

	// Constructor
	~Color() {}
};

class Vector2
{
public:

	// -------------------- Attributes -------------------- //

	// Components of the vector
	float x, y;

	// -------------------- Methods -------------------- //

	// Constructor
	Vector2(float x = 0, float y = 0) : x(x), y(y) {}

	// Constructor
	Vector2(const Vector2& vector) : x(vector.x), y(vector.y) {}

	// Constructor
	~Vector2() {}

	// = operator
	Vector2& operator=(const Vector2& vector) {
		if (&vector != this) {
			x = vector.x;
			y = vector.y;
		}
		return *this;
	}

	// + operator
	Vector2 operator+(const Vector2 &v) const {
		return Vector2(x + v.x, y + v.y);
	}

	// += operator
	Vector2& operator+=(const Vector2 &v) {
		x += v.x; y += v.y;
		return *this;
	}

	// - operator
	Vector2 operator-(const Vector2 &v) const {
		return Vector2(x - v.x, y - v.y);
	}

	// -= operator
	Vector2& operator-=(const Vector2 &v) {
		x -= v.x; y -= v.y;
		return *this;
	}

	// == operator
	bool operator==(const Vector2 &v) const {
		return x == v.x && y == v.y;
	}

	// != operator
	bool operator!=(const Vector2 &v) const {
		return !(*this == v);
	}

	// * operator
	Vector2 operator*(float f) const {
		return Vector2(f*x, f*y);
	}

	// *= operator
	Vector2 &operator*=(float f) {
		x *= f; y *= f;
		return *this;
	}

	// / operator
	Vector2 operator/(float f) const {
		float inv = 1.f / f;
		return Vector2(x * inv, y * inv);
	}

	// /= operator
	Vector2 &operator/=(float f) {
		float inv = 1.f / f;
		x *= inv; y *= inv;
		return *this;
	}

	// - operator
	Vector2 operator-() const {
		return Vector2(-x, -y);
	}

	// [] operator
	float &operator[](int i) {
		switch (i) {
		case 0: return x;
		case 1: return y;
		}
		return y;
	}

	// [] operator
	const float &operator[](int i) const {
		switch (i) {
		case 0: return x;
		case 1: return y;
		}
		return y;
	}

	// Normalize the vector and return it
	Vector2 Normalize() {
		float l = length();
		if (l == 0) return;
		x /= l;
		y /= l;
		return *this;
	}

	bool isNull() const {
		return(x == 0. && y == 0.);
	}

	// Clamp the values between 0 and 1
	Vector2 clamp01() {
		if (x > 1.f) x = 1.f;
		else if (x < 0.f) x = 0.f;
		if (y > 1.f) y = 1.f;
		else if (y < 0.f) y = 0.f;
		return *this;
	}

	// Return the squared length of the vector
	float lengthSquared() const { return x * x + y * y; }

	// Return the length of the vector
	float length() const { return sqrt(lengthSquared()); }
};

inline Vector2 operator*(float f, const Vector2 & o) {
	return o * f;
}



class MenuPos
{
public:

	// -------------------- Attributes -------------------- //

	// Components of the vector
	int x, y;

	// -------------------- Methods -------------------- //

	// Constructor
	MenuPos(int x = 0, int y = 0) : x(x), y(y) {}

	// Constructor
	MenuPos(const MenuPos& vector) : x(vector.x), y(vector.y) {}

	// Constructor
	~MenuPos() {}

	// = operator
	MenuPos& operator=(const MenuPos& vector) {
		if (&vector != this) {
			x = vector.x;
			y = vector.y;
		}
		return *this;
	}

	// + operator
	MenuPos operator+(const MenuPos &v) const {
		return MenuPos(x + v.x, y + v.y);
	}

	// += operator
	MenuPos& operator+=(const MenuPos &v) {
		x += v.x; y += v.y;
		return *this;
	}

	// - operator
	MenuPos operator-(const MenuPos &v) const {
		return MenuPos(x - v.x, y - v.y);
	}

	// -= operator
	MenuPos& operator-=(const MenuPos &v) {
		x -= v.x; y -= v.y;
		return *this;
	}

	// == operator
	bool operator==(const MenuPos &v) const {
		return x == v.x && y == v.y;
	}

	// != operator
	bool operator!=(const MenuPos &v) const {
		return !(*this == v);
	}

	// * operator
	MenuPos operator*(float f) const {
		return MenuPos(f*x, f*y);
	}

	// *= operator
	MenuPos &operator*=(float f) {
		x *= f; y *= f;
		return *this;
	}

	// / operator
	MenuPos operator/(float f) const {
		float inv = 1.f / f;
		return MenuPos(x * inv, y * inv);
	}

	// /= operator
	MenuPos &operator/=(float f) {
		float inv = 1.f / f;
		x *= inv; y *= inv;
		return *this;
	}

	// - operator
	MenuPos operator-() const {
		return MenuPos(-x, -y);
	}

	bool isNull() const {
		return(x == 0. && y == 0.);
	}
};




// redefinition of pad bit flags

#define    PAD_SELECT    (1<<0)
#define    PAD_L3        (1<<1)
#define    PAD_R3        (1<<2)
#define    PAD_START     (1<<3)
#define    PAD_UP        (1<<4)
#define    PAD_RIGHT     (1<<5)
#define    PAD_DOWN      (1<<6)
#define    PAD_LEFT      (1<<7)
#define    PAD_L2        (1<<8)
#define    PAD_R2        (1<<9)
#define    PAD_L1        (1<<10)
#define    PAD_R1        (1<<11)
#define    PAD_TRIANGLE  (1<<12)
#define    PAD_CIRCLE    (1<<13)
#define    PAD_CROSS     (1<<14)
#define    PAD_SQUARE    (1<<15)

#define _ES32(v)((uint32_t)(((((uint32_t)v) & 0xFF000000) >> 24) | \
                            ((((uint32_t)v) & 0x00FF0000) >> 8 ) | \
                            ((((uint32_t)v) & 0x0000FF00) << 8 ) | \
                            ((((uint32_t)v) & 0x000000FF) << 24)))


int32_t rsx_fifo_pause(uint32_t proc_ctx, uint8_t pause);
void buzzer(uint8_t mode);
void get_temperature(uint32_t _dev, uint32_t *_temp);
void read_temperature(uint32_t *t1, uint32_t *t2);

void read_meminfo(char *data);








// misc.c

int32_t sys_rsx_context_attribute(uint32_t rsx_ctx_id, uint32_t pkg_id, uint64_t arg_1, uint64_t arg_2, uint64_t arg_3, uint64_t arg_4) {
	system_call_6(674, (uint64_t)rsx_ctx_id, (uint64_t)pkg_id, arg_1, (uint64_t)arg_2, arg_3, arg_4);
	return_to_user_prog(int32_t);
}

/***********************************************************************
* pause/continue rsx fifo
*
* uint32_t ctx = vsh RSX context (0), game RSX context (1)
* uint8_t pause    = pause fifo (1), continue fifo (0)
***********************************************************************/
int32_t rsx_fifo_pause(uint32_t ctx, uint8_t pause)
{
	// lv2 sys_rsx_context_attribute()
	return sys_rsx_context_attribute(0x55555555ULL ^ ctx, (pause ? 2 : 3), 0, 0, 0, 0);
}

/***********************************************************************
* ring buzzer
***********************************************************************/
void buzzer(uint8_t mode)
{
	uint16_t param = 0;

	switch (mode)
	{
	case 1: param = 0x0006; break;    // single beep
	case 2: param = 0x0036; break;    // double beep
	case 3: param = 0x01B6; break;    // triple beep
	case 4: param = 0x0FFF; break;    // continuous beep, gruesome!!!
	}

	system_call_3(392, 0x1007, 0xA, param);
}

/***********************************************************************
* #define SC_GET_TEMPERATURE                (383)
***********************************************************************/
void get_temperature(uint32_t _dev, uint32_t *_temp)
{
	system_call_2(383, (uint64_t)(uint32_t)_dev, (uint64_t)(uint32_t)_temp);
}

/***********************************************************************
* a wrapper to read CPU and RSX temperature data
***********************************************************************/
void read_temperature(uint32_t *t1, uint32_t *t2)
{
	get_temperature(0, t1); // 3E030000 -> 3E.03°C -> 62.(03/256)°C
	get_temperature(1, t2);
	*t1 >>= 24, *t2 >>= 24;
}

/***********************************************************************
* a wrapper to read sys_memory data and compose text string from
***********************************************************************/
void read_meminfo(char *data)
{
	struct {
		uint32_t total;
		uint32_t avail;
	} meminfo;
	system_call_1(352, (uint64_t)(uint32_t)&meminfo);

	sprintf(data, "freemem:%i/%ikb", meminfo.avail / 1024, meminfo.total / 1024);
}
















// mem.h

#define MB(x)      ((x)*1024*1024)


int32_t create_heap(int32_t size);
void destroy_heap(void);
void *mem_alloc(uint32_t size);








// mem.c



static sys_memory_container_t mc_app = (sys_memory_container_t)-1;
static sys_addr_t heap_mem = 0;
static uint32_t prx_heap = 0;



/***********************************************************************
* create prx heap from memory container 1("app")
***********************************************************************/
int32_t create_heap(int32_t size)
{
	int32_t ret;

	ret = sys_memory_allocate(MB(size), SYS_MEMORY_PAGE_SIZE_1M, &heap_mem);
	if (ret != 0) {
		// memory allocate error
		heap_mem = 0;
		return(-1);
	}

	prx_heap = (uint32_t)heap_mem;
	return(0);
}

/***********************************************************************
*
***********************************************************************/
void destroy_heap(void)
{
	if (heap_mem) {
		sys_memory_free((sys_addr_t)heap_mem);
	}
	heap_mem = 0;
	prx_heap = 0;
	mc_app = (sys_memory_container_t)-1;
}

/***********************************************************************
*
***********************************************************************/
void *mem_alloc(uint32_t size)
{
	uint32_t add = prx_heap;
	prx_heap += size;
	return (void*)add;
}












// bitting.h





#ifdef HAVE_PNG_BG
#define PNG_BG_PATH "/dev_hdd0/tmp/background.png" // use external background.png
#endif

// font constants

#define PNG_FONT_PATH "/dev_hdd0/tmp/font.png" // use external font.png
#define FONT_PNG_W     512                 // width of font png file in pixel
#define FONT_PNG_H     514                 // height of font png file in pixel
#define FONT_W         18                  // font width in pixel
#define FONT_H         22                  // font height in pixel

// common
#define FONT_D         1                   // distance to next char
#define BORD_D         4                   // distance from canvas border
// additional png bitmaps
#define PNG_MAX        4

// canvas constants
// the values for canvas width and height can be changed for make a smaller or larger
// menu. They must be smaller than the max resulution of 1920*1080 and additional
// code is necessary to manage the other possible resolutions.
// Due to the fact that we read and write 64bit values(2 pixel) at once to the framebuffer,
// CANVAS_W must be a multiple of 2.
#define BASE          0xC0000000UL      // local memory base ea
#define CANVAS_W      300               // canvas width in pixel
#define CANVAS_H      400               // canvas height in pixel

// get pixel offset into framebuffer by x/y coordinates
#define OFFSET(x, y) ((((uint32_t)offset) + ((((int16_t)x) + \
                     (((int16_t)y) * (((uint32_t)pitch) / \
                     ((int32_t)4)))) * ((int32_t)4))) + (BASE))

// compose ARGB color by components
#define ARGB(a, r, g, b) ( \
          (((a) &0xFF) <<24) | (((r) &0xFF) <<16) | \
          (((g) &0xFF) << 8) | (((b) &0xFF) << 0))

// extract single component form ARGB color
#define GET_A(color) ((color >>24) &0xFF)
#define GET_R(color) ((color >>16) &0xFF)
#define GET_G(color) ((color >> 8) &0xFF)
#define GET_B(color) ((color >> 0) &0xFF)



// graphic buffers
typedef struct _Buffer {
	uint32_t *addr;        // buffer address
	uint16_t  w;           // buffer width
	uint16_t  h;           // buffer height
} Buffer
__attribute__((aligned(8)));

// drawing context
typedef struct _DrawCtx {
	uint32_t *canvas;      // addr of canvas
	uint32_t *bg;          // addr of background backup
	uint32_t bg_color;     // background color
	uint32_t fg_color;     // foreground color
	uint32_t *font;        // addr of decoded png font
	Buffer   png[PNG_MAX]; // bitmaps
} DrawCtx
__attribute__((aligned(16)));


int32_t init_graphic(void);
void flip_frame(void);
void pause_RSX_rendering(void);

void set_background_color(uint32_t color);
void set_foreground_color(uint32_t color);
void draw_background(void);

int32_t load_png_bitmap(const int32_t idx, const char *path);
void draw_png(const int32_t idx, const int32_t c_x, const int32_t c_y, const int32_t p_x, const int32_t p_y, const int32_t w, const int32_t h);

// text
#define LEFT      0   // useless
#define RIGHT     1
#define CENTER    2
uint16_t get_aligned_x(const char *str, const uint8_t alignment);
void print_text(int32_t x, int32_t y, const char *str);

// primitives
void draw_pixel(int32_t x, int32_t y);
void draw_line(int32_t x, int32_t y, int32_t x2, int32_t y2);
void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void draw_circle(int32_t x_c, int32_t y_c, int32_t r);

#ifdef HAVE_STARFIELD
void draw_stars(void);
#endif













// starfield.h

typedef struct _STAR {
	float     xpos;
	float     ypos;
	uint8_t   color;
	uint16_t  zpos;
	uint16_t  speed;
} STAR
__attribute__((aligned(16)));

void init_star(STAR *star, const uint16_t i);
void init_once(void);
void move_star(uint32_t *canvas);












// starfield.c


#include <sys/random_number.h>
#define RAND_MAXN 0xffffffff

#define NUMBER_OF_STARS 256*4     // max 2^16 for uint16_t


static STAR stars[NUMBER_OF_STARS];

void init_star(STAR *star, const uint16_t i)
{
	/* randomly init stars, generate them around the center of the screen */
	uint32_t rand;
	sys_get_random_number(&rand, sizeof(rand));
	star->xpos = -10.0 + (20.0 * (rand / (RAND_MAXN + 1.0)));
	sys_get_random_number(&rand, sizeof(rand));
	star->ypos = -10.0 + (20.0 * (rand / (RAND_MAXN + 1.0)));

	/* change viewpoint */
	star->xpos *= 3141;
	star->ypos *= 3141;

	star->zpos = i;
	sys_get_random_number(&rand, sizeof(rand));
	star->speed = 2 + (int)(2.0 * (rand / (RAND_MAXN + 1.0)));

	/* the closer to the viewer the brighter */
	star->color = i >> 2;
}

void init_once(/* stars, first run */)
{
	uint16_t i;
	for (i = 0; i < NUMBER_OF_STARS; i++)
		init_star(stars + i, i + 1);
}

void move_star(uint32_t *canvas)
{
	int16_t tx, ty;
	uint16_t i;

	for (i = 0; i < NUMBER_OF_STARS; i++)
	{
		stars[i].zpos -= stars[i].speed;

		if (stars[i].zpos <= 0) init_star(stars + i, i + 1);

		/* compute 3D position */
		tx = (stars[i].xpos / stars[i].zpos) + (CANVAS_W >> 1);
		ty = (stars[i].ypos / stars[i].zpos) + (CANVAS_H >> 1);

		/* check if a star leaves the screen */
		if (tx < 0 || tx > CANVAS_W - 1
			|| ty < 0 || ty > CANVAS_H - 1)
		{
			init_star(stars + i, i + 1);
			continue;
		}

		canvas[tx + ty * CANVAS_W] = ARGB(0xff, stars[i].color, stars[i].color, stars[i].color);
	}
}





int write_process(void* destination, const void* source, size_t size)
{
	system_call_4(905, (uint64_t)sys_process_getpid(), (uint64_t)destination, size, (uint64_t)source);
	return_to_user_prog(int);
}
void PatchInJump(uint64_t Address, int Destination, bool Linked)
{
	int FuncBytes[4];													// Use this data to copy over the address.
	Destination = *(int*)Destination;									// Get the actual destination address (pointer to void).
	FuncBytes[0] = 0x3D600000 + ((Destination >> 16) & 0xFFFF);			// lis %r11, dest>>16
	if (Destination & 0x8000)											// if bit 16 is 1...
		FuncBytes[0] += 1;
	FuncBytes[1] = 0x396B0000 + (Destination & 0xFFFF);					// addi %r11, %r11, dest&OxFFFF
	FuncBytes[2] = 0x7D6903A6;											// mtctr %r11
	FuncBytes[3] = 0x4E800420;											// bctr
	if (Linked)
		FuncBytes[3] += 1;												// bctrl
	write_process((void*)Address, FuncBytes, 4 * 4);
	//memcpy((void*)Address, FuncBytes, 4 * 4);
}




// scroller.h


void draw_text(const int y);
void move_text(void);







// scroller.c

// testing f_sinf() export
// masterzorag, 2015

#define STEP_X  -4            // horizontal displacement
#define S_LEN   64 *4         // we save 2 strlen(), also it's 256!


static int32_t sx = CANVAS_W;

/* 256 characters, 64 * 4 lines or switch from uint8_t i below */
static const char ss[S_LEN + 1] __attribute__((aligned(16))) = "\
    Sinusscroller scrolling text. Scrolling text Scrolling text \
Scrolling text Scrolling text Scrolling text Scrolling text     \
Scrolling text Scrolling text Scrolling text Scrolling text     \
                      * end of scrolling text *               ";
/*-------------------------------------------------------------*/

/***********************************************************************
* Draw a string of chars, amplifing y position by sin(x)
***********************************************************************/
void draw_text(const int y)
{
	uint8_t i;                // max 64*4 characters message !
	float_t amp;
	int16_t x = sx;           // every call sets the initial x
	char   c[2];
	c[1] = '\0';              // print_text() deals with '\0' terminated

	for (i = 0; i < S_LEN - 1; i++)
	{
		if (x > 0 && x < CANVAS_W - FONT_W)
		{
			amp = f_sinf(x    // testing f_sinf() export
				* 0.02)         // it turns out in num of bends
				* 20;           // +/- vertical bounds over y

			c[0] = ss[i];     // split text into single characters

			set_foreground_color(0xFFFFFFFF);
			print_text(x, y + amp, c);
		}
		x += FONT_W;
	}
}


/***********************************************************************
* Move string by defined step
***********************************************************************/
void move_text(void)
{
	sx += STEP_X;

	if (sx < -(uint16_t)(S_LEN * FONT_W))  // horizontal bound, then loop
		sx = CANVAS_W + FONT_W;
}




































// png_dec.h


// memory callback's
typedef struct {
	uint32_t mallocCallCounts;
	uint32_t freeCallCounts;
} Cb_Arg;

// decoder context
typedef struct {
	CellPngDecMainHandle main_h;             // decoder
	CellPngDecSubHandle sub_h;               // stream
	Cb_Arg cb_arg;                           // callback arg
} png_dec_info;



Buffer load_png(const char *file_path);
















// png_dec.c



static int32_t png_w = 0, png_h = 0;


static int32_t create_decoder(png_dec_info *dec_ctx);
static int32_t open_png(png_dec_info *dec_ctx, const char *file_path);
static int32_t set_dec_param(png_dec_info	*dec_ctx);
static int32_t decode_png_stream(png_dec_info	*dec_ctx, void *buf);
static void* cb_malloc(uint32_t size, void *cb_malloc_arg);
static int32_t cb_free(void *ptr, void *cb_free_arg);


/***********************************************************************
* create png decoder
***********************************************************************/
static int32_t create_decoder(png_dec_info *dec_ctx)
{
	uint32_t ret = 0;
	CellPngDecThreadInParam   in;
	CellPngDecThreadOutParam  out;

	// set params
	dec_ctx->cb_arg.mallocCallCounts = 0;
	dec_ctx->cb_arg.freeCallCounts = 0;

	in.spuThreadEnable = CELL_PNGDEC_SPU_THREAD_DISABLE;   // ppu only
	in.ppuThreadPriority = 512;
	in.spuThreadPriority = 200;
	in.cbCtrlMallocFunc = cb_malloc;
	in.cbCtrlMallocArg = &dec_ctx->cb_arg;
	in.cbCtrlFreeFunc = cb_free;
	in.cbCtrlFreeArg = &dec_ctx->cb_arg;

	// create png decoder
	ret = cellPngDecCreate(&dec_ctx->main_h, &in, &out);
	return ret;
}

/***********************************************************************
* open png stream
***********************************************************************/
static int32_t open_png(png_dec_info *dec_ctx, const char *file_path)
{
	uint32_t ret = 0;
	CellPngDecSrc      src;
	CellPngDecOpnInfo  info;

	// set stream source
	src.srcSelect = CELL_PNGDEC_FILE;        // source is file
	src.fileName = file_path;               // path to source file
	src.fileOffset = 0;
	src.fileSize = 0;
	src.streamPtr = NULL;
	src.streamSize = 0;

	// spu thread disable
	src.spuThreadEnable = CELL_PNGDEC_SPU_THREAD_DISABLE;

	// open stream 
	ret = cellPngDecOpen(dec_ctx->main_h, &dec_ctx->sub_h, &src, &info);
	return ret;
}

/***********************************************************************
* set decode parameter
***********************************************************************/
static int32_t set_dec_param(png_dec_info	*dec_ctx)
{
	uint32_t ret = 0;
	CellPngDecInfo      info;
	CellPngDecInParam   in;
	CellPngDecOutParam  out;

	// read png header 
	cellPngDecReadHeader(dec_ctx->main_h, dec_ctx->sub_h, &info);
	png_w = info.imageWidth;
	png_h = info.imageHeight;


	/*
	printf("\nimageWidth:       %d\n"
	"imageHeight:      %d\n"
	"numComponents:    0x%08X\n"
	"colorSpace:       0x%08X\n"
	"bitDepth:         0x%08X\n"
	"interlaceMethod:  0x%08X\n"
	"chunkInformation: 0x%08X\n\n",
	info.imageWidth, info.imageHeight, info.numComponents,
	info.colorSpace, info.bitDepth, info.interlaceMethod, info.chunkInformation);
	*/



	// set decoder parameter
	in.commandPtr = NULL;
	in.outputMode = CELL_PNGDEC_TOP_TO_BOTTOM;
	in.outputColorSpace = CELL_PNGDEC_ARGB;        // ps3 framebuffer is ARGB
	in.outputBitDepth = 8;
	in.outputPackFlag = CELL_PNGDEC_1BYTE_PER_1PIXEL;

	if ((info.colorSpace == CELL_PNGDEC_GRAYSCALE_ALPHA) ||
		(info.colorSpace == CELL_PNGDEC_RGBA) ||
		(info.chunkInformation & 0x10))
	{
		in.outputAlphaSelect = CELL_PNGDEC_STREAM_ALPHA;
	}
	else
	{
		in.outputAlphaSelect = CELL_PNGDEC_FIX_ALPHA;
	}
	in.outputColorAlpha = 0xFF;

	ret = cellPngDecSetParameter(dec_ctx->main_h, dec_ctx->sub_h, &in, &out);
	return ret;
}

/***********************************************************************
* decode png stream
***********************************************************************/
static int32_t decode_png_stream(png_dec_info	*dec_ctx, void *buf)
{
	uint32_t ret = 0;
	uint8_t *out;
	CellPngDecDataCtrlParam  param;
	CellPngDecDataOutInfo    info;


	param.outputBytesPerLine = png_w * 4;
	out = (uint8_t*)buf;

	memset(out, 0, (png_w * png_h * 4));

	// decode png...
	ret = cellPngDecDecodeData(dec_ctx->main_h, dec_ctx->sub_h, out, &param, &info);

	return ret;
}

/***********************************************************************
* malloc callback
***********************************************************************/
static void *cb_malloc(uint32_t size, void *cb_malloc_arg)
{
	Cb_Arg *arg;
	arg = (Cb_Arg *)cb_malloc_arg;
	arg->mallocCallCounts++;
	return _sys_malloc(size);
}

/***********************************************************************
* free callback
***********************************************************************/
static int32_t cb_free(void *ptr, void *cb_free_arg)
{
	Cb_Arg *arg;
	arg = (Cb_Arg *)cb_free_arg;
	arg->freeCallCounts++;
	_sys_free(ptr);
	return 0;
}


/***********************************************************************
* decode png file
* const char *file_path  =  path to png file e.g. "/dev_hdd0/test.png"
***********************************************************************/
Buffer load_png(const char *file_path)
{
	Buffer tmp;
	png_dec_info dec_ctx;          // decryption handles
	void *buf_addr = NULL;         // buffer for decoded png data


	// create png decoder 
	create_decoder(&dec_ctx);

	// open png stream
	open_png(&dec_ctx, file_path);

	// set decode parameter
	set_dec_param(&dec_ctx);

	// alloc target buffer
	buf_addr = mem_alloc(png_w * png_h * 4);

	// decode png stream, into target buffer
	decode_png_stream(&dec_ctx, buf_addr);

	cellPngDecClose(dec_ctx.main_h, dec_ctx.sub_h);
	cellPngDecDestroy(dec_ctx.main_h);

	// store png values
	tmp.addr = (uint32_t*)buf_addr;
	tmp.w = png_w;
	tmp.h = png_h;

	return tmp;
}

















// bitting.c




// graphic buffers and drawing context
static DrawCtx ctx __attribute__((aligned(16)));    // drawing context

// display values
static uint32_t offset = 0, pitch = 0;
static uint16_t h = 0, w = 0, canvas_x = 0, canvas_y = 0;


/***********************************************************************
* pause rsx fifo
***********************************************************************/
void pause_RSX_rendering()
{
	// is a flip occurred ? (the moment RSX has finished the rendering of a new frame and flip him on screen)
	while (1)
		if (*(uint32_t*)0x60201100 == 0x80000000) break;

	rsx_fifo_pause(0, 1);       // pause rsx fifo (no new frames...)
}


/***********************************************************************
* alpha blending (ARGB)
*
* uint32_t bg = background color
* uint32_t fg = foreground color
***********************************************************************/
static uint32_t mix_color(const uint32_t bg, const uint32_t fg)
{
	uint32_t a = fg >> 24;
	if (a == 0) return bg;

	uint32_t rb = (((fg & 0x00FF00FF) * a) + ((bg & 0x00FF00FF) * (255 - a))) & 0xFF00FF00;
	uint32_t g = (((fg & 0x0000FF00) * a) + ((bg & 0x0000FF00) * (255 - a))) & 0x00FF0000;
	uint32_t Fg = a + ((bg >> 24) * (255 - a) / 255);

	return (Fg << 24) | ((rb | g) >> 8);
}


/***********************************************************************
* dump background
***********************************************************************/
static void dump_bg(void)
{
	uint16_t i, k;
	uint64_t *bg = (uint64_t*)ctx.bg;

	for (i = 0; i < CANVAS_H; i++)
		for (k = 0; k < CANVAS_W / 2; k++)
			bg[k + i * CANVAS_W / 2] = *(uint64_t*)(OFFSET(canvas_x + (k * 2), canvas_y + (i)));
}

int32_t init_graphic(void)
{
	memset(&ctx, 0, sizeof(DrawCtx));

	// set drawing context
	ctx.canvas = (uint32_t *)mem_alloc(CANVAS_W * CANVAS_H * sizeof(uint32_t));    // canvas buffer
	ctx.bg = (uint32_t *)mem_alloc(CANVAS_W * CANVAS_H * sizeof(uint32_t));    // background buffer
	ctx.bg_color = 0xFF000000;          // black, opaque
	ctx.fg_color = 0xFFFFFFFF;          // white, opaque
	if (!ctx.canvas || !ctx.bg) {
		return(-1);
	}
	Buffer font = load_png(PNG_FONT_PATH);  // load font png
	ctx.font = font.addr;
	if (!font.addr) {
		return(-1);
	}
	const CellGcmDisplayInfo *info = cellGcmGetDisplayInfo();
	uint8_t id;
	cellGcmGetCurrentDisplayBufferId(&id);
	offset = info[id].offset;
	if (!offset) {
		return(-1);
	}
	pitch = info[id].pitch;
	h = info[id].height;
	w = info[id].width;

	// get x/y start coordinates for our canvas, always center
	canvas_x = (w - CANVAS_W) / 2;
	canvas_y = (h - CANVAS_H) / 2;

	// dump background, for alpha blending
	dump_bg();

	// init first frame with background dump
	memcpy((uint32_t*)ctx.canvas, (uint32_t*)ctx.bg, CANVAS_W * CANVAS_H * sizeof(uint32_t));
	return(0);
}

/***********************************************************************
* flip finished frame into paused ps3-framebuffer
***********************************************************************/
void flip_frame()
{
	uint16_t i, k;
	uint64_t *canvas = (uint64_t*)ctx.canvas;

	for (i = 0; i < CANVAS_H; i++)
		for (k = 0; k < CANVAS_W / 2; k++)
			*(uint64_t*)(OFFSET(canvas_x + (k * 2), canvas_y + (i))) = canvas[k + i * CANVAS_W / 2];

	// after flip, clear frame buffer with background
	memcpy((uint32_t*)ctx.canvas, (uint32_t*)ctx.bg, CANVAS_W * CANVAS_H * sizeof(uint32_t));
}


/***********************************************************************
* set background color
***********************************************************************/
void set_background_color(uint32_t color)
{
	ctx.bg_color = color;
}


/***********************************************************************
* set foreground color
***********************************************************************/
void set_foreground_color(uint32_t color)
{
	ctx.fg_color = color;
}


/***********************************************************************
* draw background, with current background color
***********************************************************************/
void draw_background()
{
	uint16_t tmp_x = 0, tmp_y = 0;
	uint32_t i;

	for (i = 0; i < CANVAS_W * CANVAS_H; i++)
	{
		ctx.canvas[i] = mix_color(ctx.bg[i], ctx.bg_color);

		tmp_x++;
		if (tmp_x == CANVAS_W)
		{
			tmp_x = 0, tmp_y++;
		}
	}
}


/***********************************************************************
* compute x to align text into canvas
*
* const char *str = referring string
* uint8_t align   = RIGHT / CENTER (1/2)
***********************************************************************/
uint16_t get_aligned_x(const char *str, const uint8_t alignment)
{
	return (uint16_t)((CANVAS_W - (strlen(str) * FONT_W)) / alignment);
}

/***********************************************************************
* print text, with data from font.png
*
* int32_t x       = start x coordinate into canvas
* int32_t y       = start y coordinate into canvas
* const char *str = string to print
***********************************************************************/
void print_text(int32_t x, int32_t y, const char *str)
{
	int32_t c_x = x, c_y = y;
	int32_t i = 0, char_w = 0, p_x = 0, p_y = 0;
	int32_t tmp_x = 0, tmp_y = 0;
	uint8_t c = 0;

	while (*str != '\0')
	{
		c = *str;

		if (c > 127)
		{
			str++;
			c = (*str & 0x3F) | 0x80;
		}

		if (c == '\n')
		{
			c_x = x;
			c_y += FONT_H;
		}
		else
		{
			p_y = (c >> 4) * FONT_H * FONT_PNG_W;
			p_x = (c & 0x0F) * FONT_W;

			char_w = ctx.font[p_x + p_y];

			for (i = 0; i < FONT_H * char_w; i++)
			{
				if ((ctx.font[(p_x + tmp_x) + (p_y + tmp_y * FONT_PNG_W)]) != 0)
				{
					ctx.canvas[(c_y + tmp_y) * CANVAS_W + c_x + tmp_x] =
						mix_color(ctx.canvas[(c_y + tmp_y) * CANVAS_W + c_x + tmp_x],
						(ctx.font[(p_x + tmp_x) + (p_y + tmp_y * FONT_PNG_W)] & 0xFF000000) |
							(ctx.fg_color & 0x00FFFFFF));
				}

				tmp_x++;
				if (tmp_x == char_w)
				{
					tmp_x = 0, tmp_y++;
				}
			}
			tmp_y = 0;
			c_x += char_w;
		}
		str++;
	}
}

/***********************************************************************
* load a png file
*
* int32_t idx      = index of png, max 4 (0 - 3)
* const char *path = path to png file
***********************************************************************/
int32_t load_png_bitmap(const int32_t idx, const char *path)
{
	if (idx > PNG_MAX) return -1;
	ctx.png[idx] = load_png(path);
	return 0;
}


/***********************************************************************
* draw png part into frame.
*
* int32_t idx      =  index of loaded png
* int32_t can_x    =  start x coordinate into canvas
* int32_t can_y    =  start y coordinate into canvas
* int32_t png_x    =  start x coordinate into png
* int32_t png_y    =  start y coordinate into png
* int32_t w        =  width of png part to blit
* int32_t h        =  height of png part to blit
***********************************************************************/
void draw_png(const int32_t idx, const int32_t c_x, const int32_t c_y, const int32_t p_x, const int32_t p_y, const int32_t w, const int32_t h)
{
	int32_t i, k;
	uint32_t *px = NULL;

	for (i = 0; i < h; i++)
		for (k = 0; k < w; k++)
			if ((c_x + k < CANVAS_W)
				&& (c_y + i < CANVAS_H))
			{
				px = &ctx.canvas[(c_y + i) * CANVAS_W + c_x + k];

				*px = mix_color(*px,
					ctx.png[idx].addr[(p_x + p_y * ctx.png[idx].w) + (k + i * ctx.png[idx].w)]);
			}
}

#ifdef HAVE_STARFIELD
void draw_stars()
{
	move_star((uint32_t*)ctx.canvas);
}
#endif


// some primitives...
/***********************************************************************
* draw a single pixel,
*
* int32_t x   = start x coordinate into frame
* int32_t y   = start y coordinate into frame
**********************************************************************/
void draw_pixel(int32_t x, int32_t y)
{
	if ((x < CANVAS_W) && (y < CANVAS_H))
		ctx.canvas[x + y * CANVAS_W] = ctx.fg_color;
}

/***********************************************************************
* draw a line,
*
* int32_t x   = line start x coordinate into frame
* int32_t y   = line start y coordinate into frame
* int32_t x2  = line end x coordinate into frame
* int32_t y2  = line end y coordinate into frame
**********************************************************************/
void draw_line(int32_t x, int32_t y, int32_t x2, int32_t y2)
{
	int32_t i = 0, dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
	int32_t w = x2 - x;
	int32_t h = y2 - y;


	if (w < 0) dx1 = -1; else if (w > 0) dx1 = 1;
	if (h < 0) dy1 = -1; else if (h > 0) dy1 = 1;
	if (w < 0) dx2 = -1; else if (w > 0) dx2 = 1;

	int32_t l = abs(w);
	int32_t s = abs(h);

	if (!(l > s))
	{
		l = abs(h);
		s = abs(w);

		if (h < 0) dy2 = -1; else if (h > 0) dy2 = 1;

		dx2 = 0;
	}

	int32_t num = l >> 1;

	for (i = 0; i <= l; i++)
	{
		//ctx.canvas[x + y * CANVAS_W] = ctx.fg_color;
		draw_pixel(x, y);
		num += s;

		if (!(num < l))
		{
			num -= l;
			x += dx1;
			y += dy1;
		}
		else
		{
			x += dx2;
			y += dy2;
		}
	}
}

/***********************************************************************
* draw a rectangle,
*
* int32_t x   = rectangle start x coordinate into frame
* int32_t y   = rectangle start y coordinate into frame
* int32_t w   = width of rectangle
* int32_t h   = height of rectangle
**********************************************************************/
void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
	draw_line(x, y, x + w, y);
	draw_line(x + w, y, x + w, y + h);
	draw_line(x + w, y + h, x, y + h);
	draw_line(x, y + h, x, y);
}

/***********************************************************************
* circle helper function
*
* int32_t x_c = circle center x coordinate into frame
* int32_t y_c = circle center y coordinate into frame
* int32_t x   = circle point x coordinate into frame
* int32_t y   = circle point y coordinate into frame
**********************************************************************/
static void circle_points(int32_t x_c, int32_t y_c, int32_t x, int32_t y)
{
	draw_pixel(x_c + x, y_c + y);
	draw_pixel(x_c - x, y_c + y);
	draw_pixel(x_c + x, y_c - y);
	draw_pixel(x_c - x, y_c - y);
	draw_pixel(x_c + y, y_c + x);
	draw_pixel(x_c - y, y_c + x);
	draw_pixel(x_c + y, y_c - x);
	draw_pixel(x_c - y, y_c - x);
}

/***********************************************************************
* draw a circle,
*
* int32_t x_c = circle center x coordinate into frame
* int32_t y_c = circle center y coordinate into frame
* int32_t r   = circle radius
**********************************************************************/
void draw_circle(int32_t x_c, int32_t y_c, int32_t r)
{
	int32_t x = 0;
	int32_t y = r;
	int32_t p = 1 - r;

	circle_points(x_c, y_c, x, y);

	while (x < y)
	{
		x++;

		if (p < 0)
		{
			p += 2 * x + 1;
		}
		else
		{
			y--;
			p += 2 * (x - y) + 1;
		}

		circle_points(x_c, y_c, x, y);
	}
}









// pad_hook_c.c



CellPadData mypdata; // from game_menu.c
bool menu_running; // from game_menu.c

//Detour<int32_t> *detour_cellPadGetData;


// game_menu.c

// game menu specific methods
static int32_t load_modules(void);
static int32_t unload_modules(void);

// game menu data
static uint8_t plugin_running;  // pluign running


/***********************************************************************
* stop game menu
***********************************************************************/
static void stop_GAME_Menu(void)
{
	//menu_running = 0;     // menu off
	destroy_heap();       // free heap memory
	rsx_fifo_pause(1, 0); // continue rsx rendering
}


/***********************************************************************
* start game menu
***********************************************************************/
static int32_t start_GAME_Menu(void)
{
	int32_t ret, mem_size;

	rsx_fifo_pause(1, 1);
	mem_size = (((CANVAS_W * CANVAS_H * sizeof(uint32_t)) * 2)/*fg & bg*/ + MB(4)/*leave some head room*/) / MB(1)/*convert to multiple of page size 1MB*/; //  MB(2)/*leave some head room*/)   used ot be 2 MB but changed it to 4 becuase it was crashing when I tried to load the png background. (I check and debugger and it looks like there wasn't enough memory for the png, it was out of range. All I saw was just DDDDDDDD DDDDDDDD DDDDDDDD DDDDDDDD DDDDDDDD DDDDDDDD DDDDDDDD)
	ret = create_heap(mem_size);
	if (ret != 0) {
		return(-1);
	}
	ret = init_graphic();                                             // initialize game menu graphic
	if (ret != 0) {
		return(-1);
	}
#ifdef HAVE_PNG_BG
	ret = load_png_bitmap(0, PNG_BG_PATH);                            // load a png background if you like
	if (ret != 0) {
		printf("Max amount of PNG's has been reached\n");
		return(-1);
	}
#endif
	//menu_running = 1;                                                 // set menu_running on
	return(0);
}





class Button
{
public:
	static uint32_t pad;

	static void SnycPad(uint32_t sync_pad)
	{
		pad = sync_pad;
	}
	static bool Pressed(uint32_t button)
	{
		return pad & button;
	}


}; uint32_t Button::pad = 0;

#define MaxSubmenuLevels 20

typedef void(*Function)();
typedef void(*PaletteHandler)(int);
typedef void(*KeyboardHandler)(char*);

class Menu
{
private:
	Function mainMenu;
	Function currentMenu;
	Function lastSubmenu[MaxSubmenuLevels];
	int submenuLevel;

	char* tipText;
	int lastOption[MaxSubmenuLevels];

	bool optionPress,
		leftPress,
		rightPress,
		leftHold,
		rightHold,
		upPress,
		downPress,
		squarePress;

	template <typename T>
	struct scrollData
	{
		T* var;
		T min;
		T max;
		int decimals;
	};
	static scrollData<int> intScrollData;
	static scrollData<float> floatScrollData;

	enum Alignment;


	bool fastScrolling;

public:
	bool open,
		sounds,
		instructions,
		rainbow;

	char* title;

	int currentOption;
	int optionCount;
	int maxOptions;

	Vector2 position;

	Menu();
	Menu(Menu& menu);
	Menu(Function mainSubmenu);

	enum Alignment
	{
		text_alignment_left,
		text_alignment_center,
		text_alignment_right
	};


	void positionMenuText(char* text, uint32_t xPos);
	void drawText(char* text, MenuPos pos, uint32_t scale, uint32_t color);

	void monitorButtons();
	void on_tick();
	void run();

	void changeSubmenu(Function submenu);

	void banner(char* text);

	bool hovered();
	bool pressed();

	Menu& option(char* text);

	Menu& submenu(Function sub);
	template <typename F>
	Menu& call(F func)
	{
		if (pressed())
		{
			func();
		}
		return *this;
	}
	template <typename F, typename T1>
	Menu& call(F func, T1 P1)
	{
		if (pressed())
		{
			func(P1);
		}
		return *this;
	}
	template <typename F, typename T1, typename T2>
	Menu& call(F func, T1 P1, T2 P2)
	{
		if (pressed())
		{
			func(P1, P2);
		}
		return *this;
	}
	template <typename F, typename T1, typename T2, typename T3>
	Menu& call(F func, T1 P1, T2 P2, T3 P3)
	{
		if (pressed())
		{
			func(P1, P2, P3);
		}
		return *this;
	}
	template <typename F, typename T1, typename T2, typename T3, typename T4>
	Menu& call(F func, T1 P1, T2 P2, T3 P3, T4 P4)
	{
		if (pressed())
		{
			func(P1, P2, P3, P4);
		}
		return *this;
	}
	template <typename F, typename T1, typename T2, typename T3, typename T4, typename T5>
	Menu& call(F func, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5)
	{
		if (pressed())
		{
			func(P1, P2, P3, P4, P5);
		}
		return *this;
	}
	template <typename F, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	Menu& call(F func, T1 P1, T2 P2, T3 P3, T4 P4, T5 P5, T6 P6)
	{
		if (pressed())
		{
			func(P1, P2, P3, P4, P5, P6);
		}
		return *this;
	}
};

//#include <cell\pad\pad_codes.h>

Menu::Menu() {}
Menu::Menu(Menu& menu) {}
Menu::Menu(Function mainSubmenu)
{
	mainMenu = mainSubmenu;
	open = false;

	// Settings
	sounds = true;
	instructions = true;
	rainbow = false;

	maxOptions = 15;

	position = Vector2(0.825f, 0.100000f);
}

uint32_t oldpad = 0, curpad = 0;

void Menu::monitorButtons()
{
	if (open) {

		uint32_t type;
		cellPadGetDataExtra(0, &type, &mypdata);                          // use alternate get pad method
		if (mypdata.len > 0)                                               // if pad data
		{
			curpad = (mypdata.button[2] | (mypdata.button[3] << 8));        // merge pad data
			memset(&mypdata, 0, sizeof(CellPadData));
			if (curpad != oldpad)
			{
				Button::SnycPad(curpad);

				optionPress = leftPress = rightPress = leftHold = rightHold = upPress = downPress = squarePress = false;

				if (Button::Pressed(PAD_CIRCLE)) {
					if (currentMenu == mainMenu) {
						open = false;
						stop_GAME_Menu();
						sys_timer_usleep(100000);
					}
					else {
						submenuLevel--;
						currentMenu = lastSubmenu[submenuLevel];
						currentOption = lastOption[submenuLevel];
					}
				}
				else if (Button::Pressed(PAD_UP)) {
					upPress = true;
					currentOption--;
					if (currentOption < 1) {
						currentOption = optionCount;
					}
				}
				else if (Button::Pressed(PAD_DOWN)) {
					downPress = true;
					currentOption++;
					if (currentOption > optionCount) {
						currentOption = 1;
					}
				}
				else if (Button::Pressed(PAD_RIGHT)) {
					rightPress = true;
				}
				else if (Button::Pressed(PAD_LEFT)) {
					leftPress = true;
				}
				else if (Button::Pressed(PAD_RIGHT)) {
					rightHold = true;
				}
				else if (Button::Pressed(PAD_LEFT)) {
					leftHold = true;
				}
				else if (Button::Pressed(PAD_SQUARE)) {
					squarePress = true;
				}
				else if (Button::Pressed(PAD_CROSS)) {
					optionPress = true;
				}
			}
			oldpad = curpad;
		}
		else
		{
			oldpad = 0;
		}
		//sys_timer_usleep(200);
	}
	else {

		if (mypdata.len > 0)                                                 // if pad data
		{
			curpad = (mypdata.button[2] | (mypdata.button[3] << 8));          // merge pad data
			memset(&mypdata, 0, sizeof(CellPadData));
			if (curpad != oldpad)
			{
				Button::SnycPad(curpad);

				if (Button::Pressed(PAD_R3) && Button::Pressed(PAD_LEFT)) {

					int ret = start_GAME_Menu();
					if (ret != 0)
					{
						stop_GAME_Menu();                                         // failed to init graphics, try pressing select again
					}

					open = true;
					optionPress = false;
					submenuLevel = 0;
					currentMenu = mainMenu;
					currentOption = 1;
				}
			}
			oldpad = curpad;
		}
		else
		{
			oldpad = 0;
		}

		sys_timer_usleep(70000);                                          // game sync
	}
}


void Menu::drawText(char* text, MenuPos pos, uint32_t scale, uint32_t color) {
	set_foreground_color(color);
	print_text(pos.x, pos.y, text);
}
void Menu::positionMenuText(char* text, uint32_t xPos) {
	uint32_t optionColor = hovered() ? 0xFFFF0000 : 0xFFFFFFFF;

	int optionIndex = 0;
	if (currentOption <= maxOptions && optionCount <= maxOptions) {
		optionIndex = optionCount;
	}
	else if ((optionCount > (currentOption - maxOptions)) && optionCount <= currentOption) {
		optionIndex = optionCount - (currentOption - maxOptions);
	}
	if (optionIndex != 0) {
		drawText(text, MenuPos(xPos, 10 + (FONT_H * (optionIndex + 1))), 5, optionColor);
	}
}
void Menu::run() {
	optionCount = 0;
	tipText = NULL;


	// all 32bit colors are ARGB, the framebuffer format
	set_background_color(0x96000000);     // black, opaque
	set_foreground_color(0x96FFFFFF);     // white, opaque

	// fill background with background color
	draw_background();

#ifdef HAVE_STARFIELD
	draw_stars();       // to keep them under text lines
#endif

#ifdef HAVE_PNG_BG
	// draw background from png
	draw_png(0, 0, 0, 0, 0, 720, 400);
#endif

	currentMenu();

#ifdef HAVE_SSCROLLER
	// testing sine in a scroller
	draw_text(330);
	move_text();
#endif

	// Draw banner top
	drawText(title, MenuPos(100, 8), 70, 0xFFFFFFFF);

}

void Menu::changeSubmenu(Function submenu) {
	lastSubmenu[submenuLevel] = currentMenu;
	lastOption[submenuLevel] = currentOption;
	currentOption = 1;
	optionPress = false;
	currentMenu = submenu;
	submenuLevel++;
}
Menu& Menu::submenu(Function sub) {
	if (pressed()) {
		changeSubmenu(sub);
	}
	return *this;
}
void Menu::banner(char* text) {
	title = text;
}
bool Menu::hovered() {
	return currentOption == optionCount;
}
bool Menu::pressed() {
	if (hovered()) {
		if (optionPress) {
			return true;
		}
	}
	return false;
}
Menu& Menu::option(char* text) {
	optionCount++;
	positionMenuText(text, 10);
	return *this;
}


bool init = false;
Menu menu;

void Menu::on_tick()
{

}


void worldOptions() {
	menu.banner("World Options");
	menu.option("Option 1");
	menu.option("Option 2");
	menu.option("Option 3");
}

void combatOptions() {
	menu.banner("Combat Options");
	menu.option("Option 1");
	menu.option("Option 2");
	menu.option("Option 3");
}

void movmentOptions() {
	menu.banner("Movment Options");
	menu.option("Option 1");
	menu.option("Option 2");
	menu.option("Option 3");
}

void playerOptions() {
	menu.banner("Player Options");
	menu.option("Render");
	menu.option("Auto");
	menu.option("Fun");
	menu.option("Other Mode");
	menu.option("Radar");
	menu.option("Text Rader");
}

void mainMenu() {
	menu.banner("Main Menu");
	menu.option("Player").submenu(playerOptions);
	menu.option("Movment").submenu(movmentOptions);
	menu.option("Combat").submenu(combatOptions);
	menu.option("World").submenu(worldOptions);
}

int32_t MY_cellPadGetData(uint32_t port_no, CellPadData *data)
{
	uint32_t type;
	cellPadGetDataExtra(port_no, &type, data);
	memcpy(&mypdata, data, sizeof(CellPadData));
	if (menu.open) {
		memset(data, 0, sizeof(CellPadData));
	}

	//detour_cellPadGetData->GetOriginalIE(port_no, data); // does it need call original?
}

/***********************************************************************
* game menu main
***********************************************************************/
int32_t game_menu(void)
{
	int32_t ret = load_modules();
	if (ret < 0) {
		return(ret);
	}

#ifdef HAVE_STARFIELD
	init_once(/* stars */);
#endif
	uint32_t call_addr;
	int32_t(*_cellPadGetData)(uint32_t port_no, CellPadData *data);
	_cellPadGetData = (int32_t(*)(uint32_t, CellPadData*))getNIDfunc2("/dev_flash/sys/external/libio.sprx", "sys_io_library", "sys_io", 0x8b72cda1, 0);
	if (!(_cellPadGetData)) {
		return(-1);
	}
	call_addr = **(uint32_t **)&_cellPadGetData;

	// they hook to do the same thing they do above :)
	//detour_cellPadGetData = new Detour<int32_t>;
	//detour_cellPadGetData->HookIAT("sys_io", 0x8B72CDA1, MY_cellPadGetData);

	PatchInJump(call_addr, (int)MY_cellPadGetData, false);

	plugin_running = 1;
	while (plugin_running)
	{
		if (!init) {
			menu = Menu(mainMenu);
			init = true;
		}

		menu.monitorButtons();
		menu.on_tick();

		if (menu.open) {
			menu.run();
			flip_frame();
			sys_timer_usleep(200);                                              // short menu frame delay
		}
	}

	// unloading plugin...
	unload_modules();
	return(0);
}

////////////////////////////////////////////////////////////////////////
// GAME PLUGIN SPECIFIC METHODS
/***********************************************************************
* load system modules
***********************************************************************/
static int32_t load_modules(void)
{
	int32_t ret;

	// load necessary png and font modules for blitting
	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_PNGDEC);
	if (ret != CELL_OK) {
		return(ret);
	}
	/*
	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_FONT);
	if (ret != CELL_OK) {
		return(ret);
	}
	*/
	/*
	// module for setting screenshot date
	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_RTC);
	if (ret != CELL_OK) {
		return(ret);
	}

	// modules for in game music
	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_SEARCH);
	if (ret != CELL_OK) {
		return(ret);
	}
	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_MUSIC2);
	if (ret != CELL_OK) {
		return(ret);
	}

	// initialize search and music modules
	ret = cellSearchInitialize(CELL_SEARCH_MODE_NORMAL, SYS_MEMORY_CONTAINER_ID_INVALID, cb_searchutil, NULL);
	if (ret != CELL_MUSIC2_OK) {
		return(ret);
	}
	ret = cellMusicInitialize2(CELL_MUSIC2_PLAYER_MODE_NORMAL, 250, cb_musicutil, NULL);
	if (ret != CELL_MUSIC2_OK) {
		return(ret);
	}

	// initialize recording module
	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_REC);
	if (ret != CELL_OK) {
		return(ret);
	}
	*/
	return(0);
}

/***********************************************************************
* unload system modules
***********************************************************************/
static int32_t unload_modules(void)
{
	int32_t ret;

	/*ret = cellMusicFinalize2();
	if ( ret != CELL_OK ) {
	return(ret);
	}*/
	/*ret = cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_MUSIC2);
	if (ret != CELL_OK) {
		return(ret);
	}
	ret = cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_SEARCH);
	if (ret != CELL_OK) {
		return(ret);
	}
	ret = cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_REC);
	if (ret != CELL_OK) {
		return(ret);
	}
	ret = cellSysmoduleUnloadModule(CELL_SYSMODULE_VIDEO_EXPORT);
	if (ret != CELL_OK) {
		return(ret);
	}*/
	ret = cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_GAME);
	if (ret != CELL_OK) {
		return(ret);
	}

	// leave some modules loaded in case they are used by the game
	/*ret = cellSysmoduleUnloadModule(CELL_SYSMODULE_RTC);
	if (ret != CELL_OK) {
	return(ret);
	}
	ret = cellSysmoduleUnloadModule(CELL_SYSMODULE_FONT);
	if (ret != CELL_OK) {
	return(ret);
	}
	ret = cellSysmoduleUnloadModule(CELL_SYSMODULE_PNGDEC);
	if (ret != CELL_OK) {
	return(ret);
	}*/
	return(0);
}











#define THREAD_NAME         "ps3_menu_thread"
#define STOP_THREAD_NAME    "ps3_menu_stop_thread"

static sys_ppu_thread_t ps3_menu_tid = -1;
static bool done = 0;

/***********************************************************************
* sys_ppu_thread_exit, direct over syscall
***********************************************************************/
static inline void _sys_ppu_thread_exit2(uint64_t val)
{
	system_call_1(41, val);
}

/***********************************************************************
*
***********************************************************************/
static inline sys_prx_id_t prx_get_module_id_by_address(void *addr)
{
	system_call_1(461, (uint64_t)(uint32_t)addr);
	return (int32_t)p1;
}

/***********************************************************************
* plugin main ppu thread
***********************************************************************/
static void ps3_menu_thread(uint64_t arg) {
	printf("ps3_menu_thread loading...\n");
	game_menu();
	//PatchInJump();
	sys_ppu_thread_exit(0);
}
extern "C" int _gcm_stuff_prx_entry()
{
	sys_ppu_thread_t ps3_menu_thread_2;
	sys_ppu_thread_create(&ps3_menu_tid, ps3_menu_thread, 0, 3000, 0x4000, 1, THREAD_NAME);
}
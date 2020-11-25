/* Copyright 2020 0x7ff
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "libdimentio.h"
#include <compression.h>
#include <mach-o/fat.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define LZSS_F (18)
#define LZSS_N (4096)
#define LZSS_THRESHOLD (2)
#define IPC_ENTRY_SZ (0x18)
#define OS_STRING_LEN_OFF (0xC)
#define KCOMP_HDR_PAD_SZ (0x16C)
#define OS_STRING_STRING_OFF (0x10)
#define IPC_SPACE_IS_TABLE_OFF (0x20)
#define IPC_ENTRY_IE_OBJECT_OFF (0x0)
#define PROC_P_LIST_LE_PREV_OFF (0x8)
#define OS_DICTIONARY_COUNT_OFF (0x14)
#define IPC_PORT_IP_KOBJECT_OFF (0x68)
#define PROC_P_LIST_LH_FIRST_OFF (0x0)
#define PREBOOT_PATH "/private/preboot/"
#define IPC_SPACE_IS_TABLE_SZ_OFF (0x14)
#define OS_DICTIONARY_DICT_ENTRY_OFF (0x20)
#define OS_STRING_LEN(a) extract32(a, 14, 18)
#define LOADED_KEXT_SUMMARY_HDR_NAME_OFF (0x10)
#define LOADED_KEXT_SUMMARY_HDR_ADDR_OFF (0x60)
#define APPLE_MOBILE_AP_NONCE_CLEAR_NONCE_SEL (0xC9)
#define APPLE_MOBILE_AP_NONCE_GENERATE_NONCE_SEL (0xC8)
#define kCFCoreFoundationVersionNumber_iOS_10_0_b5 (1348)
#define kCFCoreFoundationVersionNumber_iOS_13_0_b2 (1656)
#define kCFCoreFoundationVersionNumber_iOS_14_0_b1 (1740)
#define kCFCoreFoundationVersionNumber_iOS_11_0_b1 (1429.15)
#define kCFCoreFoundationVersionNumber_iOS_12_0_b1 (1535.13)
#define kCFCoreFoundationVersionNumber_iOS_13_0_b1 (1652.20)
#define kCFCoreFoundationVersionNumber_iOS_14_3_b1 (1770.300)
#define BOOT_PATH "/System/Library/Caches/com.apple.kernelcaches/kernelcache"

#define DER_INT (0x2U)
#define DER_SEQ (0x30U)
#define DER_IA5_STR (0x16U)
#define DER_OCTET_STR (0x4U)
#define RD(a) extract32(a, 0, 5)
#define RN(a) extract32(a, 5, 5)
#define VM_KERN_MEMORY_OSKEXT (5)
#define KCOMP_HDR_MAGIC (0x636F6D70U)
#define ADRP_ADDR(a) ((a) & ~0xFFFULL)
#define ADRP_IMM(a) (ADR_IMM(a) << 12U)
#define IO_OBJECT_NULL ((io_object_t)0)
#define ADD_X_IMM(a) extract32(a, 10, 12)
#define kIODeviceTreePlane "IODeviceTree"
#define KCOMP_HDR_TYPE_LZSS (0x6C7A7373U)
#define LDR_X_IMM(a) (sextract64(a, 5, 19) << 2U)
#define kOSBundleLoadAddressKey "OSBundleLoadAddress"
#define IS_ADR(a) (((a) & 0x9F000000U) == 0x10000000U)
#define IS_ADRP(a) (((a) & 0x9F000000U) == 0x90000000U)
#define IS_LDR_X(a) (((a) & 0xFF000000U) == 0x58000000U)
#define IS_ADD_X(a) (((a) & 0xFFC00000U) == 0x91000000U)
#define LDR_W_UNSIGNED_IMM(a) (extract32(a, 10, 12) << 2U)
#define LDR_X_UNSIGNED_IMM(a) (extract32(a, 10, 12) << 3U)
#define kBootNoncePropertyKey "com.apple.System.boot-nonce"
#define kIONVRAMDeletePropertyKey "IONVRAM-DELETE-PROPERTY"
#define IS_LDR_W_UNSIGNED_IMM(a) (((a) & 0xFFC00000U) == 0xB9400000U)
#define IS_LDR_X_UNSIGNED_IMM(a) (((a) & 0xFFC00000U) == 0xF9400000U)
#define ADR_IMM(a) ((sextract64(a, 5, 19) << 2U) | extract32(a, 29, 2))
#define kIONVRAMForceSyncNowPropertyKey "IONVRAM-FORCESYNCNOW-PROPERTY"

#ifndef SECT_CSTRING
#	define SECT_CSTRING "__cstring"
#endif

#ifndef SEG_TEXT_EXEC
#	define SEG_TEXT_EXEC "__TEXT_EXEC"
#endif

typedef char io_string_t[512];
typedef mach_port_t io_object_t;
typedef uint32_t IOOptionBits, ipc_entry_num_t;
typedef io_object_t io_service_t, io_connect_t, io_registry_entry_t;

typedef struct {
	struct section_64 s64;
	const char *data;
} sec_64_t;

typedef struct {
	struct symtab_command cmd_symtab;
	sec_64_t sec_text, sec_cstring;
	kaddr_t base, kslide;
	const char *kernel;
	size_t kernel_sz;
	char *data;
} pfinder_t;

kern_return_t
IOServiceClose(io_connect_t);

kern_return_t
IOObjectRelease(io_object_t);

CFMutableDictionaryRef
IOServiceMatching(const char *);

CFDictionaryRef
OSKextCopyLoadedKextInfo(CFArrayRef, CFArrayRef);

io_registry_entry_t
IORegistryEntryFromPath(mach_port_t, const io_string_t);

io_service_t
IOServiceGetMatchingService(mach_port_t, CFDictionaryRef);

kern_return_t
IOServiceOpen(io_service_t, task_port_t, uint32_t, io_connect_t *);

kern_return_t
IORegistryEntrySetCFProperty(io_registry_entry_t, CFStringRef, CFTypeRef);

kern_return_t
mach_vm_write(vm_map_t, mach_vm_address_t, vm_offset_t, mach_msg_type_number_t);

kern_return_t
IOConnectCallStructMethod(io_connect_t, uint32_t, const void *, size_t, void *, size_t *);

CFTypeRef
IORegistryEntryCreateCFProperty(io_registry_entry_t, CFStringRef, CFAllocatorRef, IOOptionBits);

kern_return_t
mach_vm_read_overwrite(vm_map_t, mach_vm_address_t, mach_vm_size_t, mach_vm_address_t, mach_vm_size_t *);

kern_return_t
mach_vm_machine_attribute(vm_map_t, mach_vm_address_t, mach_vm_size_t, vm_machine_attribute_t, vm_machine_attribute_val_t *);

kern_return_t
mach_vm_region(vm_map_t, mach_vm_address_t *, mach_vm_size_t *, vm_region_flavor_t, vm_region_info_t, mach_msg_type_number_t *, mach_port_t *);

extern const mach_port_t kIOMasterPortDefault;

static kread_func_t kread_buf;
task_t tfp0 = TASK_NULL;
static kwrite_func_t kwrite_buf;
kaddr_t kbase, kslide, this_proc, our_task, allproc;
static kaddr_t kernproc;
static size_t proc_task_off, proc_p_pid_off, task_itk_space_off, io_dt_nvram_of_dict_off;

static uint32_t
extract32(uint32_t val, unsigned start, unsigned len) {
	return (val >> start) & (~0U >> (32U - len));
}

static uint64_t
sextract64(uint64_t val, unsigned start, unsigned len) {
	return (uint64_t)((int64_t)(val << (64U - len - start)) >> (64U - len));
}

static size_t
decompress_lzss(const uint8_t *src, size_t src_len, uint8_t *dst, size_t dst_len) {
	const uint8_t *src_end = src + src_len, *dst_start = dst, *dst_end = dst + dst_len;
	uint16_t i, r = LZSS_N - LZSS_F, flags = 0;
	uint8_t text_buf[LZSS_N + LZSS_F - 1], j;

	memset(text_buf, ' ', r);
	while(src != src_end && dst != dst_end) {
		if(((flags >>= 1U) & 0x100U) == 0) {
			flags = *src++ | 0xFF00U;
			if(src == src_end) {
				break;
			}
		}
		if((flags & 1U) != 0) {
			text_buf[r++] = *dst++ = *src++;
			r &= LZSS_N - 1U;
		} else {
			i = *src++;
			if(src == src_end) {
				break;
			}
			j = *src++;
			i |= (j & 0xF0U) << 4U;
			j = (j & 0xFU) + LZSS_THRESHOLD;
			do {
				*dst++ = text_buf[r++] = text_buf[i++ & (LZSS_N - 1U)];
				r &= LZSS_N - 1U;
			} while(j-- != 0 && dst != dst_end);
		}
	}
	return (size_t)(dst - dst_start);
}

static const uint8_t *
der_decode(uint8_t tag, const uint8_t *der, const uint8_t *der_end, size_t *out_len) {
	size_t der_len;

	if(der_end - der > 2 && tag == *der++) {
		if(((der_len = *der++) & 0x80U) != 0) {
			*out_len = 0;
			if((der_len &= 0x7FU) <= sizeof(*out_len) && (size_t)(der_end - der) >= der_len) {
				while(der_len-- != 0) {
					*out_len = (*out_len << 8U) | *der++;
				}
			}
		} else {
			*out_len = der_len;
		}
		if(*out_len != 0 && (size_t)(der_end - der) >= *out_len) {
			return der;
		}
	}
	return NULL;
}

static const uint8_t *
der_decode_seq(const uint8_t *der, const uint8_t *der_end, const uint8_t **seq_end) {
	size_t der_len;

	if((der = der_decode(DER_SEQ, der, der_end, &der_len)) != NULL) {
		*seq_end = der + der_len;
	}
	return der;
}

static const uint8_t *
der_decode_uint64(const uint8_t *der, const uint8_t *der_end, uint64_t *r) {
	size_t der_len;

	if((der = der_decode(DER_INT, der, der_end, &der_len)) != NULL && (*der & 0x80U) == 0 && (der_len <= sizeof(*r) || (--der_len == sizeof(*r) && *der++ == 0))) {
		*r = 0;
		while(der_len-- != 0) {
			*r = (*r << 8U) | *der++;
		}
		return der;
	}
	return NULL;
}

static void *
kdecompress(const void *src, size_t src_len, size_t *dst_len) {
	const uint8_t *der, *octet, *der_end, *src_end = (const uint8_t *)src + src_len;
	struct {
		uint32_t magic, type, adler32, uncomp_sz, comp_sz;
		uint8_t pad[KCOMP_HDR_PAD_SZ];
	} kcomp_hdr;
	size_t der_len;
	uint64_t r;
	void *dst;

	if((der = der_decode_seq(src, src_end, &der_end)) != NULL && (der = der_decode(DER_IA5_STR, der, der_end, &der_len)) != NULL && der_len == 4 && (memcmp(der, "IMG4", der_len) != 0 || ((der = der_decode_seq(der + der_len, src_end, &der_end)) != NULL && (der = der_decode(DER_IA5_STR, der, der_end, &der_len)) != NULL && der_len == 4)) && memcmp(der, "IM4P", der_len) == 0 && (der = der_decode(DER_IA5_STR, der + der_len, der_end, &der_len)) != NULL && der_len == 4 && memcmp(der, "krnl", der_len) == 0 && (der = der_decode(DER_IA5_STR, der + der_len, der_end, &der_len)) != NULL && (der = der_decode(DER_OCTET_STR, der + der_len, der_end, &der_len)) != NULL && der_len > sizeof(kcomp_hdr)) {
		octet = der;
		memcpy(&kcomp_hdr, octet, sizeof(kcomp_hdr));
		if(kcomp_hdr.magic == __builtin_bswap32(KCOMP_HDR_MAGIC)) {
			if(kcomp_hdr.type == __builtin_bswap32(KCOMP_HDR_TYPE_LZSS) && (kcomp_hdr.comp_sz = __builtin_bswap32(kcomp_hdr.comp_sz)) <= der_len - sizeof(kcomp_hdr) && (kcomp_hdr.uncomp_sz = __builtin_bswap32(kcomp_hdr.uncomp_sz)) != 0 && (dst = malloc(kcomp_hdr.uncomp_sz)) != NULL) {
				if(decompress_lzss(octet + sizeof(kcomp_hdr), kcomp_hdr.comp_sz, dst, kcomp_hdr.uncomp_sz) == kcomp_hdr.uncomp_sz) {
					*dst_len = kcomp_hdr.uncomp_sz;
					return dst;
				}
				free(dst);
			}
		} else if((der = der_decode_seq(der + der_len, src_end, &der_end)) != NULL && (der = der_decode_uint64(der, der_end, &r)) != NULL && r == 1 && der_decode_uint64(der, der_end, &r) != NULL && r != 0 && (dst = malloc(r)) != NULL) {
			if(compression_decode_buffer(dst, r, octet, der_len, NULL, COMPRESSION_LZFSE) == r) {
				*dst_len = r;
				return dst;
			}
			free(dst);
		}
	}
	return NULL;
}

kern_return_t
init_tfp0(void) {
	kern_return_t ret = task_for_pid(mach_task_self(), 0, &tfp0);
	mach_port_t host;

	if(ret != KERN_SUCCESS) {
		host = mach_host_self();
		if(MACH_PORT_VALID(host)) {
			printf("host: 0x%" PRIX32 "\n", host);
			ret = host_get_special_port(host, HOST_LOCAL_NODE, 4, &tfp0);
			mach_port_deallocate(mach_task_self(), host);
		}
	}
	if(ret == KERN_SUCCESS && MACH_PORT_VALID(tfp0)) {
		return ret;
	}
	return KERN_FAILURE;
}

kern_return_t
kread_buf_tfp0(kaddr_t addr, void *buf, mach_vm_size_t sz) {
	mach_vm_address_t p = (mach_vm_address_t)buf;
	mach_vm_size_t read_sz, out_sz = 0;

	while(sz != 0) {
		read_sz = MIN(sz, vm_kernel_page_size - (addr & vm_kernel_page_mask));
		if(mach_vm_read_overwrite(tfp0, addr, read_sz, p, &out_sz) != KERN_SUCCESS || out_sz != read_sz) {
			return KERN_FAILURE;
		}
		p += read_sz;
		sz -= read_sz;
		addr += read_sz;
	}
	return KERN_SUCCESS;
}

kern_return_t
kread_addr(kaddr_t addr, kaddr_t *val) {
	return kread_buf(addr, val, sizeof(*val));
}

kern_return_t
kwrite_buf_tfp0(kaddr_t addr, const void *buf, mach_msg_type_number_t sz) {
	vm_machine_attribute_val_t mattr_val = MATTR_VAL_CACHE_FLUSH;
	mach_vm_address_t p = (mach_vm_address_t)buf;
	mach_msg_type_number_t write_sz;

	while(sz != 0) {
		write_sz = (mach_msg_type_number_t)MIN(sz, vm_kernel_page_size - (addr & vm_kernel_page_mask));
		if(mach_vm_write(tfp0, addr, p, write_sz) != KERN_SUCCESS || mach_vm_machine_attribute(tfp0, addr, write_sz, MATTR_CACHE, &mattr_val) != KERN_SUCCESS) {
			return KERN_FAILURE;
		}
		p += write_sz;
		sz -= write_sz;
		addr += write_sz;
	}
	return KERN_SUCCESS;
}

static kern_return_t
find_section(const char *p, struct segment_command_64 sg64, const char *sect_name, struct section_64 *sp) {
	for(; sg64.nsects-- != 0; p += sizeof(*sp)) {
		memcpy(sp, p, sizeof(*sp));
		if((sp->flags & SECTION_TYPE) != S_ZEROFILL) {
			if(sp->offset < sg64.fileoff || sp->size > sg64.filesize || sp->offset - sg64.fileoff > sg64.filesize - sp->size) {
				break;
			}
			if(sp->size != 0 && strncmp(sp->segname, sg64.segname, sizeof(sp->segname)) == 0 && strncmp(sp->sectname, sect_name, sizeof(sp->sectname)) == 0) {
				return KERN_SUCCESS;
			}
		}
	}
	return KERN_FAILURE;
}

static void
sec_reset(sec_64_t *sec) {
	memset(&sec->s64, '\0', sizeof(sec->s64));
	sec->data = NULL;
}

static kern_return_t
sec_read_buf(sec_64_t sec, kaddr_t addr, void *buf, size_t sz) {
	size_t off;

	if(addr < sec.s64.addr || sz > sec.s64.size || (off = addr - sec.s64.addr) > sec.s64.size - sz) {
		return KERN_FAILURE;
	}
	memcpy(buf, sec.data + off, sz);
	return KERN_SUCCESS;
}

static void
pfinder_reset(pfinder_t *pfinder) {
	pfinder->base = 0;
	pfinder->kslide = 0;
	pfinder->data = NULL;
	pfinder->kernel = NULL;
	pfinder->kernel_sz = 0;
	sec_reset(&pfinder->sec_text);
	sec_reset(&pfinder->sec_cstring);
	memset(&pfinder->cmd_symtab, '\0', sizeof(pfinder->cmd_symtab));
}

static void
pfinder_term(pfinder_t *pfinder) {
	free(pfinder->data);
	pfinder_reset(pfinder);
}

static kern_return_t
pfinder_init_file(pfinder_t *pfinder, const char *filename) {
	struct symtab_command cmd_symtab;
	kern_return_t ret = KERN_FAILURE;
	struct segment_command_64 sg64;
	struct mach_header_64 mh64;
	struct load_command lc;
	struct section_64 s64;
	struct fat_header fh;
	struct stat stat_buf;
	struct fat_arch fa;
	const char *p, *e;
	size_t len;
	void *m;
	int fd;

	pfinder_reset(pfinder);
	if((fd = open(filename, O_RDONLY | O_CLOEXEC)) != -1) {
		if(fstat(fd, &stat_buf) != -1 && S_ISREG(stat_buf.st_mode) && stat_buf.st_size > 0) {
			len = (size_t)stat_buf.st_size;
			if((m = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0)) != MAP_FAILED) {
				if((pfinder->data = kdecompress(m, len, &pfinder->kernel_sz)) != NULL && pfinder->kernel_sz > sizeof(fh) + sizeof(mh64)) {
					pfinder->kernel = pfinder->data;
					memcpy(&fh, pfinder->kernel, sizeof(fh));
					if(fh.magic == __builtin_bswap32(FAT_MAGIC) && (fh.nfat_arch = __builtin_bswap32(fh.nfat_arch)) < (pfinder->kernel_sz - sizeof(fh)) / sizeof(fa)) {
						for(p = pfinder->kernel + sizeof(fh); fh.nfat_arch-- != 0; p += sizeof(fa)) {
							memcpy(&fa, p, sizeof(fa));
							if(fa.cputype == (cpu_type_t)__builtin_bswap32(CPU_TYPE_ARM64) && (fa.offset = __builtin_bswap32(fa.offset)) < pfinder->kernel_sz && (fa.size = __builtin_bswap32(fa.size)) <= pfinder->kernel_sz - fa.offset && fa.size > sizeof(mh64)) {
								pfinder->kernel_sz = fa.size;
								pfinder->kernel += fa.offset;
								break;
							}
						}
					}
					memcpy(&mh64, pfinder->kernel, sizeof(mh64));
					if(mh64.magic == MH_MAGIC_64 && mh64.cputype == CPU_TYPE_ARM64 && mh64.filetype == MH_EXECUTE && mh64.sizeofcmds < pfinder->kernel_sz - sizeof(mh64)) {
						for(p = pfinder->kernel + sizeof(mh64), e = p + mh64.sizeofcmds; mh64.ncmds-- != 0 && (size_t)(e - p) >= sizeof(lc); p += lc.cmdsize) {
							memcpy(&lc, p, sizeof(lc));
							if(lc.cmdsize < sizeof(lc) || (size_t)(e - p) < lc.cmdsize) {
								break;
							}
							if(lc.cmd == LC_SEGMENT_64) {
								if(lc.cmdsize < sizeof(sg64)) {
									break;
								}
								memcpy(&sg64, p, sizeof(sg64));
								if(sg64.vmsize == 0) {
									continue;
								}
								if(sg64.nsects != (lc.cmdsize - sizeof(sg64)) / sizeof(s64) || sg64.fileoff > pfinder->kernel_sz || sg64.filesize > pfinder->kernel_sz - sg64.fileoff) {
									break;
								}
								if(sg64.fileoff == 0 && sg64.filesize != 0) {
									if(pfinder->base != 0) {
										break;
									}
									pfinder->base = sg64.vmaddr;
									printf("base: " KADDR_FMT "\n", sg64.vmaddr);
								}
								if(strncmp(sg64.segname, SEG_TEXT_EXEC, sizeof(sg64.segname)) == 0) {
									if(find_section(p + sizeof(sg64), sg64, SECT_TEXT, &s64) != KERN_SUCCESS) {
										break;
									}
									pfinder->sec_text.s64 = s64;
									pfinder->sec_text.data = pfinder->kernel + s64.offset;
									printf("sec_text_addr: " KADDR_FMT ", sec_text_off: 0x%" PRIX32 ", sec_text_sz: 0x%" PRIX64 "\n", s64.addr, s64.offset, s64.size);
								} else if(strncmp(sg64.segname, SEG_TEXT, sizeof(sg64.segname)) == 0) {
									if(find_section(p + sizeof(sg64), sg64, SECT_CSTRING, &s64) != KERN_SUCCESS || pfinder->kernel[s64.offset + s64.size - 1] != '\0') {
										break;
									}
									pfinder->sec_cstring.s64 = s64;
									pfinder->sec_cstring.data = pfinder->kernel + s64.offset;
									printf("sec_cstring_addr: " KADDR_FMT ", sec_cstring_off: 0x%" PRIX32 ", sec_cstring_sz: 0x%" PRIX64 "\n", s64.addr, s64.offset, s64.size);
								}
							} else if(lc.cmd == LC_SYMTAB) {
								if(lc.cmdsize != sizeof(cmd_symtab)) {
									break;
								}
								memcpy(&cmd_symtab, p, sizeof(cmd_symtab));
								printf("cmd_symtab_symoff: 0x%" PRIX32 ", cmd_symtab_nsyms: 0x%" PRIX32 ", cmd_symtab_stroff: 0x%" PRIX32 "\n", cmd_symtab.symoff, cmd_symtab.nsyms, cmd_symtab.stroff);
								if(cmd_symtab.nsyms != 0 && (cmd_symtab.symoff > pfinder->kernel_sz || cmd_symtab.nsyms > (pfinder->kernel_sz - cmd_symtab.symoff) / sizeof(struct nlist_64) || cmd_symtab.stroff > pfinder->kernel_sz || cmd_symtab.strsize > pfinder->kernel_sz - cmd_symtab.stroff || cmd_symtab.strsize == 0 || pfinder->kernel[cmd_symtab.stroff + cmd_symtab.strsize - 1] != '\0')) {
									break;
								}
								pfinder->cmd_symtab = cmd_symtab;
							}
							if(pfinder->base != 0 && pfinder->sec_text.s64.size != 0 && pfinder->sec_cstring.s64.size != 0 && pfinder->cmd_symtab.cmdsize != 0) {
								ret = KERN_SUCCESS;
								break;
							}
						}
					}
				}
				munmap(m, len);
			}
		}
		close(fd);
	}
	if(ret != KERN_SUCCESS) {
		pfinder_term(pfinder);
	}
	return ret;
}

static kaddr_t
pfinder_xref_rd(pfinder_t pfinder, uint32_t rd, kaddr_t start, kaddr_t to) {
	kaddr_t x[32] = { 0 };
	uint32_t insn;

	for(; sec_read_buf(pfinder.sec_text, start, &insn, sizeof(insn)) == KERN_SUCCESS; start += sizeof(insn)) {
		if(IS_LDR_X(insn)) {
			x[RD(insn)] = start + LDR_X_IMM(insn);
		} else if(IS_ADR(insn)) {
			x[RD(insn)] = start + ADR_IMM(insn);
		} else if(IS_ADD_X(insn)) {
			x[RD(insn)] = x[RN(insn)] + ADD_X_IMM(insn);
		} else if(IS_LDR_W_UNSIGNED_IMM(insn)) {
			x[RD(insn)] = x[RN(insn)] + LDR_W_UNSIGNED_IMM(insn);
		} else if(IS_LDR_X_UNSIGNED_IMM(insn)) {
			x[RD(insn)] = x[RN(insn)] + LDR_X_UNSIGNED_IMM(insn);
		} else {
			if(IS_ADRP(insn)) {
				x[RD(insn)] = ADRP_ADDR(start) + ADRP_IMM(insn);
			}
			continue;
		}
		if(RD(insn) == rd) {
			if(to == 0) {
				if(x[rd] < pfinder.base) {
					break;
				}
				return x[rd];
			}
			if(x[rd] == to) {
				return start;
			}
		}
	}
	return 0;
}

static kaddr_t
pfinder_xref_str(pfinder_t pfinder, const char *str, uint32_t rd) {
	const char *p, *e;
	size_t len;

	for(p = pfinder.sec_cstring.data, e = p + pfinder.sec_cstring.s64.size; p != e; p += len) {
		len = strlen(p) + 1;
		if(strncmp(str, p, len) == 0) {
			return pfinder_xref_rd(pfinder, rd, pfinder.sec_text.s64.addr, pfinder.sec_cstring.s64.addr + (kaddr_t)(p - pfinder.sec_cstring.data));
		}
	}
	return 0;
}

static kaddr_t
pfinder_sym(pfinder_t pfinder, const char *sym) {
	const char *p, *strtab = pfinder.kernel + pfinder.cmd_symtab.stroff;
	struct nlist_64 nl64;

	for(p = pfinder.kernel + pfinder.cmd_symtab.symoff; pfinder.cmd_symtab.nsyms-- != 0; p += sizeof(nl64)) {
		memcpy(&nl64, p, sizeof(nl64));
		if(nl64.n_un.n_strx != 0 && nl64.n_un.n_strx < pfinder.cmd_symtab.strsize && (nl64.n_type & (N_STAB | N_TYPE)) == N_SECT && nl64.n_value >= pfinder.base && strcmp(strtab + nl64.n_un.n_strx, sym) == 0) {
			return nl64.n_value + pfinder.kslide;
		}
	}
	return 0;
}

static kaddr_t
pfinder_kernproc(pfinder_t pfinder) {
	kaddr_t ref = pfinder_sym(pfinder, "_kernproc");
	uint32_t insns[2];

	if(ref != 0) {
		return ref;
	}
	for(ref = pfinder_xref_str(pfinder, "\"Should never have an EVFILT_READ except for reg or fifo.\"", 0); sec_read_buf(pfinder.sec_text, ref, insns, sizeof(insns)) == KERN_SUCCESS; ref -= sizeof(*insns)) {
		if(IS_ADRP(insns[0]) && IS_LDR_X_UNSIGNED_IMM(insns[1]) && RD(insns[1]) == 3) {
			return pfinder_xref_rd(pfinder, RD(insns[1]), ref, 0);
		}
	}
	return 0;
}

static kaddr_t
pfinder_allproc(pfinder_t pfinder) {
	kaddr_t ref = pfinder_xref_str(pfinder, "shutdownwait", 2);

    if(ref == 0) {
        ref = pfinder_xref_str(pfinder, "shutdownwait", 3);                                                                                                                 /* msleep */
    }
    return pfinder_xref_rd(pfinder, 8, ref, 0);
}

static kaddr_t
pfinder_init_kbase(pfinder_t *pfinder) {
	mach_msg_type_number_t cnt = TASK_DYLD_INFO_COUNT;
	vm_region_extended_info_data_t extended_info;
	kaddr_t addr, kext_addr, kext_addr_slid;
	CFDictionaryRef kexts_info, kext_info;
	task_dyld_info_data_t dyld_info;
	char kext_name[KMOD_MAX_NAME];
	struct mach_header_64 mh64;
	CFStringRef kext_name_cf;
	CFNumberRef kext_addr_cf;
	mach_port_t object_name;
	CFArrayRef kext_names;
	mach_vm_size_t sz;

	if(pfinder->kslide == 0) {
		if(task_info(tfp0, TASK_DYLD_INFO, (task_info_t)&dyld_info, &cnt) == KERN_SUCCESS) {
			pfinder->kslide = dyld_info.all_image_info_size;
		}
		if(pfinder->kslide == 0) {
			cnt = VM_REGION_EXTENDED_INFO_COUNT;
			for(addr = 0; mach_vm_region(tfp0, &addr, &sz, VM_REGION_EXTENDED_INFO, (vm_region_info_t)&extended_info, &cnt, &object_name) == KERN_SUCCESS; addr += sz) {
				mach_port_deallocate(mach_task_self(), object_name);
				if(extended_info.protection == VM_PROT_READ && extended_info.user_tag == VM_KERN_MEMORY_OSKEXT) {
					if(kread_buf(addr + LOADED_KEXT_SUMMARY_HDR_NAME_OFF, kext_name, sizeof(kext_name)) == KERN_SUCCESS) {
						printf("kext_name: %s\n", kext_name);
						if(kread_addr(addr + LOADED_KEXT_SUMMARY_HDR_ADDR_OFF, &kext_addr_slid) == KERN_SUCCESS) {
							printf("kext_addr_slid: " KADDR_FMT "\n", kext_addr_slid);
							if((kext_name_cf = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, kext_name, kCFStringEncodingUTF8, kCFAllocatorNull)) != NULL) {
								if((kext_names = CFArrayCreate(kCFAllocatorDefault, (const void **)&kext_name_cf, 1, &kCFTypeArrayCallBacks)) != NULL) {
									if((kexts_info = OSKextCopyLoadedKextInfo(kext_names, NULL)) != NULL) {
										if(CFGetTypeID(kexts_info) == CFDictionaryGetTypeID() && CFDictionaryGetCount(kexts_info) == 1 && (kext_info = CFDictionaryGetValue(kexts_info, kext_name_cf)) != NULL && CFGetTypeID(kext_info) == CFDictionaryGetTypeID() && (kext_addr_cf = CFDictionaryGetValue(kext_info, CFSTR(kOSBundleLoadAddressKey))) != NULL && CFGetTypeID(kext_addr_cf) == CFNumberGetTypeID() && CFNumberGetValue(kext_addr_cf, kCFNumberSInt64Type, &kext_addr) && kext_addr_slid > kext_addr) {
											pfinder->kslide = kext_addr_slid - kext_addr;
										}
										CFRelease(kexts_info);
									}
									CFRelease(kext_names);
								}
								CFRelease(kext_name_cf);
							}
						}
					}
					break;
				}
			}
		}
	}
	if(pfinder->base + pfinder->kslide > pfinder->base && kread_buf(pfinder->base + pfinder->kslide, &mh64, sizeof(mh64)) == KERN_SUCCESS && mh64.magic == MH_MAGIC_64 && mh64.cputype == CPU_TYPE_ARM64 && mh64.filetype == MH_EXECUTE) {
		pfinder->sec_text.s64.addr += pfinder->kslide;
		pfinder->sec_cstring.s64.addr += pfinder->kslide;
		printf("kbase: " KADDR_FMT ", kslide: " KADDR_FMT "\n", pfinder->base + pfinder->kslide, pfinder->kslide);
		kbase = pfinder->base + pfinder->kslide;
		kslide = pfinder->kslide;
		return KERN_SUCCESS;
	}
	return KERN_FAILURE;
}

static char *
get_boot_path(void) {
	size_t hash_len, path_len = sizeof(BOOT_PATH);
	io_registry_entry_t chosen;
	struct stat stat_buf;
	const uint8_t *hash;
	CFDataRef hash_cf;
	char *path = NULL;

	if(stat(PREBOOT_PATH, &stat_buf) != -1 && S_ISDIR(stat_buf.st_mode) && (chosen = IORegistryEntryFromPath(kIOMasterPortDefault, kIODeviceTreePlane ":/chosen")) != IO_OBJECT_NULL) {
		if((hash_cf = IORegistryEntryCreateCFProperty(chosen, CFSTR("boot-manifest-hash"), kCFAllocatorDefault, kNilOptions)) != NULL) {
			if(CFGetTypeID(hash_cf) == CFDataGetTypeID() && (hash_len = (size_t)CFDataGetLength(hash_cf) << 1U) != 0) {
				path_len += strlen(PREBOOT_PATH) + hash_len;
				if((path = malloc(path_len)) != NULL) {
					memcpy(path, PREBOOT_PATH, strlen(PREBOOT_PATH));
					for(hash = CFDataGetBytePtr(hash_cf); hash_len-- != 0; ) {
						path[strlen(PREBOOT_PATH) + hash_len] = "0123456789ABCDEF"[(hash[hash_len >> 1U] >> ((~hash_len & 1U) << 2U)) & 0xFU];
					}
				}
			}
			CFRelease(hash_cf);
		}
		IOObjectRelease(chosen);
	}
	if(path == NULL) {
		path_len = sizeof(BOOT_PATH);
		path = malloc(path_len);
	}
	if(path != NULL) {
		memcpy(path + (path_len - sizeof(BOOT_PATH)), BOOT_PATH, sizeof(BOOT_PATH));
	}
	return path;
}

kern_return_t
pfinder_init_offsets(void) {
	kern_return_t ret = KERN_FAILURE;
	pfinder_t pfinder;
	char *boot_path;

	proc_task_off = 0x18;
	proc_p_pid_off = 0x10;
	task_itk_space_off = 0x290;
	io_dt_nvram_of_dict_off = 0xC0;
	if(kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_10_0_b5) {
		task_itk_space_off = 0x300;
		if(kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_11_0_b1) {
			task_itk_space_off = 0x308;
			if(kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_12_0_b1) {
				proc_task_off = 0x10;
				proc_p_pid_off = 0x60;
				task_itk_space_off = 0x300;
				if(kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_13_0_b1) {
					task_itk_space_off = 0x320;
					if(kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_13_0_b2) {
						proc_p_pid_off = 0x68;
						if(kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_14_0_b1) {
							task_itk_space_off = 0x330;
							io_dt_nvram_of_dict_off = 0xB8;
							if(kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_14_3_b1) {
								io_dt_nvram_of_dict_off = 0xC0;
							}
						}
					}
				}
			}
		}
	}
	if((boot_path = get_boot_path()) != NULL) {
		printf("boot_path: %s\n", boot_path);
		if(pfinder_init_file(&pfinder, boot_path) == KERN_SUCCESS) {
			pfinder.kslide = kslide;
			if(pfinder_init_kbase(&pfinder) == KERN_SUCCESS && (kernproc = pfinder_kernproc(pfinder)) != 0) {
				printf("kernproc: " KADDR_FMT "\n", kernproc);
				if((allproc = pfinder_allproc(pfinder)) != 0) {
					printf("allproc: " KADDR_FMT "\n", allproc);
					ret = KERN_SUCCESS;
				}
			}
			pfinder_term(&pfinder);
		}
		free(boot_path);
	}
	return ret;
}

kern_return_t
find_task(pid_t pid, kaddr_t *task) {
	pid_t cur_pid;
	kaddr_t proc;

	if(kread_addr(kernproc + PROC_P_LIST_LH_FIRST_OFF, &proc) == KERN_SUCCESS) {
		while(proc != 0 && kread_buf(proc + proc_p_pid_off, &cur_pid, sizeof(cur_pid)) == KERN_SUCCESS) {
			if(cur_pid == pid) {
				this_proc = proc;
				return kread_addr(proc + proc_task_off, task);
			}
			if(pid == 0 || kread_addr(proc + PROC_P_LIST_LE_PREV_OFF, &proc) != KERN_SUCCESS) {
				break;
			}
		}
	}
	return KERN_FAILURE;
}

static kern_return_t
lookup_ipc_port(mach_port_name_t port_name, kaddr_t *ipc_port) {
	ipc_entry_num_t port_idx, is_table_sz;
	kaddr_t itk_space, is_table;

	if(MACH_PORT_VALID(port_name) && kread_addr(our_task + task_itk_space_off, &itk_space) == KERN_SUCCESS) {
		printf("itk_space: " KADDR_FMT "\n", itk_space);
		if(kread_buf(itk_space + IPC_SPACE_IS_TABLE_SZ_OFF, &is_table_sz, sizeof(is_table_sz)) == KERN_SUCCESS) {
			printf("is_table_sz: 0x%" PRIX32 "\n", is_table_sz);
			if((port_idx = MACH_PORT_INDEX(port_name)) < is_table_sz && kread_addr(itk_space + IPC_SPACE_IS_TABLE_OFF, &is_table) == KERN_SUCCESS) {
				printf("is_table: " KADDR_FMT "\n", is_table);
				return kread_addr(is_table + port_idx * IPC_ENTRY_SZ + IPC_ENTRY_IE_OBJECT_OFF, ipc_port);
			}
		}
	}
	return KERN_FAILURE;
}

static kern_return_t
lookup_io_object(io_object_t object, kaddr_t *ip_kobject) {
	kaddr_t ipc_port;

	if(lookup_ipc_port(object, &ipc_port) == KERN_SUCCESS) {
		printf("ipc_port: " KADDR_FMT "\n", ipc_port);
		return kread_addr(ipc_port + IPC_PORT_IP_KOBJECT_OFF, ip_kobject);
	}
	return KERN_FAILURE;
}

static kern_return_t
nonce_generate(io_service_t nonce_serv) {
	uint8_t nonce_d[CC_SHA384_DIGEST_LENGTH];
	kern_return_t ret = KERN_FAILURE;
	io_connect_t nonce_conn;
	size_t nonce_d_sz;

	if(IOServiceOpen(nonce_serv, mach_task_self(), 0, &nonce_conn) == KERN_SUCCESS) {
		printf("nonce_conn: 0x%" PRIX32 "\n", nonce_conn);
		if(IOConnectCallStructMethod(nonce_conn, APPLE_MOBILE_AP_NONCE_CLEAR_NONCE_SEL, NULL, 0, NULL, NULL) == KERN_SUCCESS) {
			nonce_d_sz = sizeof(nonce_d);
			ret = IOConnectCallStructMethod(nonce_conn, APPLE_MOBILE_AP_NONCE_GENERATE_NONCE_SEL, NULL, 0, nonce_d, &nonce_d_sz);
		}
		IOServiceClose(nonce_conn);
	}
	return ret;
}

static kern_return_t
get_of_dict(io_service_t nvram_serv, kaddr_t *of_dict) {
	kaddr_t nvram_object;

	if(lookup_io_object(nvram_serv, &nvram_object) == KERN_SUCCESS) {
		printf("nvram_object: " KADDR_FMT "\n", nvram_object);
		return kread_addr(nvram_object + io_dt_nvram_of_dict_off, of_dict);
	}
	return KERN_FAILURE;
}

static kaddr_t
lookup_key_in_os_dict(kaddr_t os_dict, const char *key) {
	kaddr_t os_dict_entry_ptr, string_ptr, val = 0;
	uint32_t os_dict_cnt, cur_key_len;
	size_t key_len = strlen(key) + 1;
	struct {
		kaddr_t key, val;
	} os_dict_entry;
	char *cur_key;

	if((cur_key = malloc(key_len)) != NULL) {
		if(kread_addr(os_dict + OS_DICTIONARY_DICT_ENTRY_OFF, &os_dict_entry_ptr) == KERN_SUCCESS && os_dict_entry_ptr != 0) {
			printf("os_dict_entry_ptr: " KADDR_FMT "\n", os_dict_entry_ptr);
			if(kread_buf(os_dict + OS_DICTIONARY_COUNT_OFF, &os_dict_cnt, sizeof(os_dict_cnt)) == KERN_SUCCESS) {
				printf("os_dict_cnt: 0x%" PRIX32 "\n", os_dict_cnt);
				while(os_dict_cnt-- != 0 && kread_buf(os_dict_entry_ptr + os_dict_cnt * sizeof(os_dict_entry), &os_dict_entry, sizeof(os_dict_entry)) == KERN_SUCCESS) {
					printf("key: " KADDR_FMT ", val: " KADDR_FMT "\n", os_dict_entry.key, os_dict_entry.val);
					if(kread_buf(os_dict_entry.key + OS_STRING_LEN_OFF, &cur_key_len, sizeof(cur_key_len)) != KERN_SUCCESS) {
						break;
					}
					cur_key_len = OS_STRING_LEN(cur_key_len);
					printf("cur_key_len: 0x%" PRIX32 "\n", cur_key_len);
					if(cur_key_len == key_len) {
						if(kread_addr(os_dict_entry.key + OS_STRING_STRING_OFF, &string_ptr) != KERN_SUCCESS || string_ptr == 0) {
							break;
						}
						printf("string_ptr: " KADDR_FMT "\n", string_ptr);
						if(kread_buf(string_ptr, cur_key, key_len) != KERN_SUCCESS) {
							break;
						}
						if(memcmp(cur_key, key, key_len) == 0) {
							val = os_dict_entry.val;
							break;
						}
					}
				}
			}
		}
		free(cur_key);
	}
	return val;
}

static kern_return_t
set_nvram_prop(io_service_t nvram_serv, const char *key, const char *val) {
	CFStringRef cf_key = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, key, kCFStringEncodingUTF8, kCFAllocatorNull), cf_val;
	kern_return_t ret = KERN_FAILURE;

	if(cf_key != NULL) {
		if((cf_val = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, val, kCFStringEncodingUTF8, kCFAllocatorNull)) != NULL) {
			ret = IORegistryEntrySetCFProperty(nvram_serv, cf_key, cf_val);
			CFRelease(cf_val);
		}
		CFRelease(cf_key);
	}
	return ret;
}

static kern_return_t
sync_nonce(io_service_t nvram_serv) {
	if(set_nvram_prop(nvram_serv, "temp_key", "temp_val") == KERN_SUCCESS && set_nvram_prop(nvram_serv, kIONVRAMDeletePropertyKey, "temp_key") == KERN_SUCCESS) {
		return set_nvram_prop(nvram_serv, kIONVRAMForceSyncNowPropertyKey, kBootNoncePropertyKey);
	}
	return KERN_FAILURE;
}

static bool
entangle_nonce(uint64_t nonce, uint8_t entangled_nonce[CC_SHA384_DIGEST_LENGTH]) {
	bool ret = false;
#ifdef __arm64e__
#	define IO_AES_ACCELERATOR_SPECIAL_KEYS_OFF (0xD0)
#	define IO_AES_ACCELERATOR_SPECIAL_KEY_CNT_OFF (0xD8)
	io_service_t aes_serv = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("IOAESAccelerator"));
	struct {
		uint32_t generated, key_id, key_sz, val[4], key[4], zero, pad;
	} key;
	uint64_t buf[] = { 0, nonce };
	kaddr_t aes_object, keys_ptr;
	uint32_t key_cnt;
	size_t out_sz;

	if(aes_serv != IO_OBJECT_NULL) {
		printf("aes_serv: 0x%" PRIX32 "\n", aes_serv);
		if(lookup_io_object(aes_serv, &aes_object) == KERN_SUCCESS) {
			printf("aes_object: " KADDR_FMT "\n", aes_object);
			if(kread_addr(aes_object + IO_AES_ACCELERATOR_SPECIAL_KEYS_OFF, &keys_ptr) == KERN_SUCCESS) {
				printf("keys_ptr: " KADDR_FMT "\n", keys_ptr);
				if(kread_buf(aes_object + IO_AES_ACCELERATOR_SPECIAL_KEY_CNT_OFF, &key_cnt, sizeof(key_cnt)) == KERN_SUCCESS) {
					printf("key_cnt: 0x%" PRIX32 "\n", key_cnt);
					for(; key_cnt-- != 0 && kread_buf(keys_ptr, &key, sizeof(key)) == KERN_SUCCESS; keys_ptr += sizeof(key)) {
						printf("generated: 0x%" PRIX32 ", key_id: 0x%" PRIX32 ", key_sz: 0x%" PRIX32 "\n", key.generated, key.key_id, key.key_sz);
						if(key.generated == 1 && key.key_id == 0x8A3 && key.key_sz == 8 * kCCKeySizeAES128) {
							if(CCCrypt(kCCEncrypt, kCCAlgorithmAES128, 0, key.val, kCCKeySizeAES128, NULL, buf, sizeof(buf), buf, sizeof(buf), &out_sz) == kCCSuccess && out_sz == sizeof(buf)) {
								CC_SHA384(buf, sizeof(buf), entangled_nonce);
								ret = true;
							}
							break;
						}
					}
				}
			}
		}
		IOObjectRelease(aes_serv);
	}
#else
	(void)nonce;
	(void)entangled_nonce;
#endif
	return ret;
}

void
dimentio_term(void) {
	if(tfp0 != TASK_NULL) {
		mach_port_deallocate(mach_task_self(), tfp0);
	}
	setpriority(PRIO_PROCESS, 0, 0);
}

kern_return_t
dimentio_init(kaddr_t _kslide, kread_func_t _kread_buf, kwrite_func_t _kwrite_buf) {
	kslide = _kslide;
	if(_kread_buf != NULL && _kwrite_buf != NULL) {
		kread_buf = _kread_buf;
		kwrite_buf = _kwrite_buf;
	} else if(init_tfp0() == KERN_SUCCESS) {
		printf("tfp0: 0x%" PRIX32 "\n", tfp0);
		kread_buf = _kread_buf != NULL ? _kread_buf : kread_buf_tfp0;
		kwrite_buf = _kwrite_buf != NULL ? _kwrite_buf : kwrite_buf_tfp0;
	}
	if(setpriority(PRIO_PROCESS, 0, PRIO_MIN) != -1 && pfinder_init_offsets() == KERN_SUCCESS) {
		return KERN_SUCCESS;
	}
	dimentio_term();
	return KERN_FAILURE;
}

kern_return_t
dementia(uint64_t *nonce, uint8_t entangled_nonce[CC_SHA384_DIGEST_LENGTH], bool *entangled) {
	io_service_t nvram_serv = IORegistryEntryFromPath(kIOMasterPortDefault, kIODeviceTreePlane ":/options");
	char nonce_hex[sizeof("0x") + 2 * sizeof(*nonce)];
	kaddr_t of_dict, os_string, string_ptr;
	kern_return_t ret = KERN_FAILURE;

	if(nvram_serv != IO_OBJECT_NULL) {
		printf("nvram_serv: 0x%" PRIX32 "\n", nvram_serv);
		if(find_task(getpid(), &our_task) == KERN_SUCCESS) {
			printf("our_task: " KADDR_FMT "\n", our_task);
			if(get_of_dict(nvram_serv, &of_dict) == KERN_SUCCESS && of_dict != 0) {
				printf("of_dict: " KADDR_FMT "\n", of_dict);
				if((os_string = lookup_key_in_os_dict(of_dict, kBootNoncePropertyKey)) != 0) {
					printf("os_string: " KADDR_FMT "\n", os_string);
					if(kread_addr(os_string + OS_STRING_STRING_OFF, &string_ptr) == KERN_SUCCESS && string_ptr != 0) {
						printf("string_ptr: " KADDR_FMT "\n", string_ptr);
						if(kread_buf(string_ptr, nonce_hex, sizeof(nonce_hex)) == KERN_SUCCESS && sscanf(nonce_hex, "0x%016" PRIx64, nonce) == 1) {
							ret = KERN_SUCCESS;
							*entangled = entangle_nonce(*nonce, entangled_nonce);
						}
					}
				}
			}
		}
		IOObjectRelease(nvram_serv);
	}
	return ret;
}

kern_return_t
dimentio(uint64_t nonce, uint8_t entangled_nonce[CC_SHA384_DIGEST_LENGTH], bool *entangled) {
	io_service_t nonce_serv, nvram_serv = IORegistryEntryFromPath(kIOMasterPortDefault, kIODeviceTreePlane ":/options");
	char nonce_hex[sizeof("0x") + 2 * sizeof(nonce)];
	kaddr_t of_dict, os_string, string_ptr;
	kern_return_t ret = KERN_FAILURE;

	if(nvram_serv != IO_OBJECT_NULL) {
		printf("nvram_serv: 0x%" PRIX32 "\n", nvram_serv);
		if(find_task(getpid(), &our_task) == KERN_SUCCESS) {
			printf("our_task: " KADDR_FMT "\n", our_task);
			if((nonce_serv = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleMobileApNonce"))) != IO_OBJECT_NULL) {
				printf("nonce_serv: 0x%" PRIX32 "\n", nonce_serv);
				if(nonce_generate(nonce_serv) == KERN_SUCCESS && get_of_dict(nvram_serv, &of_dict) == KERN_SUCCESS && of_dict != 0) {
					printf("of_dict: " KADDR_FMT "\n", of_dict);
					if((os_string = lookup_key_in_os_dict(of_dict, kBootNoncePropertyKey)) != 0) {
						printf("os_string: " KADDR_FMT "\n", os_string);
						if(kread_addr(os_string + OS_STRING_STRING_OFF, &string_ptr) == KERN_SUCCESS && string_ptr != 0) {
							printf("string_ptr: " KADDR_FMT "\n", string_ptr);
							snprintf(nonce_hex, sizeof(nonce_hex), "0x%016" PRIx64, nonce);
							if(kwrite_buf(string_ptr, nonce_hex, sizeof(nonce_hex)) == KERN_SUCCESS && sync_nonce(nvram_serv) == KERN_SUCCESS) {
								ret = KERN_SUCCESS;
								*entangled = entangle_nonce(nonce, entangled_nonce);
							}
						}
					}
				}
				IOObjectRelease(nonce_serv);
			}
		}
		IOObjectRelease(nvram_serv);
	}
	return ret;
}

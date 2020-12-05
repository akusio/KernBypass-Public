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
#ifndef LIBDIMENTIO_H
#	define LIBDIMENTIO_H
#	include <CommonCrypto/CommonCrypto.h>
#	include <CoreFoundation/CoreFoundation.h>
#	include <mach/mach.h>
#	define KADDR_FMT "0x%" PRIX64
#	ifndef MIN
#		define MIN(a, b) ((a) < (b) ? (a) : (b))
#	endif
typedef uint64_t kaddr_t;
typedef kern_return_t (*kread_func_t)(kaddr_t, void *, mach_vm_size_t), (*kwrite_func_t)(kaddr_t, const void *, mach_msg_type_number_t);
static kread_func_t kread_buf;
static kwrite_func_t kwrite_buf;
static size_t proc_p_pid_off;
static size_t proc_task_off;
task_t tfp0;
kaddr_t kbase, kslide, this_proc, our_task, allproc;

void
dimentio_term(void);

kern_return_t
dimentio_init(kaddr_t, kread_func_t, kwrite_func_t);

kern_return_t
dimentio(uint64_t, uint8_t[CC_SHA384_DIGEST_LENGTH], bool *);

kern_return_t
dementia(uint64_t *, uint8_t[CC_SHA384_DIGEST_LENGTH], bool *);

kern_return_t
init_tfp0(void);

kern_return_t
kread_addr(kaddr_t, kaddr_t *);

kern_return_t
find_task(pid_t, kaddr_t *);

kern_return_t
pfinder_init_offsets(void);

kern_return_t
kread_buf_tfp0(kaddr_t, void *, mach_vm_size_t);

kern_return_t
kwrite_buf_tfp0(kaddr_t, const void *, mach_msg_type_number_t);
#endif

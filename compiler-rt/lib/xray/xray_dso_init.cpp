//===-- xray_init.cpp -------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is a part of XRay, a dynamic runtime instrumentation system.
//
// XRay initialisation logic for DSOs.
//===----------------------------------------------------------------------===//

#include <stdio.h>

//#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_atomic.h"
#include "xray_defs.h"
#include "xray_flags.h"
#include "xray_interface_internal.h"

using namespace __sanitizer;

extern "C" {
extern const XRaySledEntry __start_xray_instr_map[] __attribute__((weak)) __attribute__((visibility("hidden")));
extern const XRaySledEntry __stop_xray_instr_map[] __attribute__((weak)) __attribute__((visibility("hidden")));
extern const XRayFunctionSledIndex __start_xray_fn_idx[] __attribute__((weak)) __attribute__((visibility("hidden")));
extern const XRayFunctionSledIndex __stop_xray_fn_idx[] __attribute__((weak)) __attribute__((visibility("hidden")));

#if SANITIZER_MAC
// HACK: This is a temporary workaround to make XRay build on
// Darwin, but it will probably not work at runtime.
const XRaySledEntry __start_xray_instr_map[] = {};
extern const XRaySledEntry __stop_xray_instr_map[] = {};
extern const XRayFunctionSledIndex __start_xray_fn_idx[] = {};
extern const XRayFunctionSledIndex __stop_xray_fn_idx[] = {};
#endif
}

// Handler functions to call in the patched entry/exit sled.
extern atomic_uintptr_t XRayPatchedFunction;
extern atomic_uintptr_t XRayArgLogger;
extern atomic_uintptr_t XRayPatchedCustomEvent;
extern atomic_uintptr_t XRayPatchedTypedEvent;

void __xray_init_dso() XRAY_NEVER_INSTRUMENT {

  // Register sleds in main XRay runtime.
  __xray_register_dso(__start_xray_instr_map, __stop_xray_instr_map, __start_xray_fn_idx, __stop_xray_fn_idx, {});

}

// .preinit_array initialization does not work for DSOs
__attribute__ ((constructor (0)))
static void __local_xray_init_dso() {
  __xray_init_dso();
}
//#endif

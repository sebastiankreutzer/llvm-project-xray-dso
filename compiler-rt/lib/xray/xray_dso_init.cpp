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
extern const XRaySledEntry __start_xray_instr_map[] __attribute__((weak));
extern const XRaySledEntry __stop_xray_instr_map[] __attribute__((weak));
extern const XRayFunctionSledIndex __start_xray_fn_idx[] __attribute__((weak));
extern const XRayFunctionSledIndex __stop_xray_fn_idx[] __attribute__((weak));

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
//  if (__start_xray_instr_map == nullptr) {
//    // TODO: How to link to include sanitizer functionality?
//    //if (Verbosity())
//    //  Report("XRay instrumentation map missing from DSO.\n");
//    return;
//  }
//

}

// TODO: Not needed here, can be removed
//namespace __xray {
//// This is the function to call when we encounter the entry or exit sleds.
//extern atomic_uintptr_t XRayPatchedFunction;
//
//// This is the function to call from the arg1-enabled sleds/trampolines.
//extern atomic_uintptr_t XRayArgLogger;
//
//// This is the function to call when we encounter a custom event log call.
//extern atomic_uintptr_t XRayPatchedCustomEvent;
//
//// This is the function to call when we encounter a typed event log call.
//extern atomic_uintptr_t XRayPatchedTypedEvent;
//}

extern "C" {

//__attribute((visibility("default"))) void handleXRayEventDSO(int32_t id, XRayEntryType type)  XRAY_NEVER_INSTRUMENT {
//  printf("DSO local handler function: %p\n", &handleXRayEventDSO);
//  printf("Handling XRay event of function %d with type %d\n", id, (int)type);
//  reinterpret_cast<void (*)(int32_t, XRayEntryType)>(atomic_load(&__xray::XRayPatchedFunction, memory_order_acquire))(id, type);
//}
}


// FIXME: Make check-xray tests work on FreeBSD without
// SANITIZER_CAN_USE_PREINIT_ARRAY.
// See sanitizer_internal_defs.h where the macro is defined.
// Calling unresolved PLT functions in .preinit_array can lead to deadlock on
// FreeBSD but here it seems benign.
//#if !defined(XRAY_NO_PREINIT) &&                                               \
//    (SANITIZER_CAN_USE_PREINIT_ARRAY || SANITIZER_FREEBSD)
//// Only add the preinit array initialization if the sanitizers can.
//__attribute__((section(".preinit_array"),
//               used)) void (*__local_xray_preinit_dso)(void) = __xray_init_dso;
//#else
//// If we cannot use the .preinit_array section, we should instead use dynamic
//// initialisation.
__attribute__ ((constructor (0)))
static void __local_xray_init_dso() {
  __xray_init_dso();
}
//#endif

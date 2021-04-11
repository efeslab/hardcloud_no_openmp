//===--- opae_generic_app.cpp - AFU Link Interface Implementation - C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
//
// Generic AFU Link Interface implementation
//
//===----------------------------------------------------------------------===//

#include "hardcloud_app.h"
#include <stdio.h>

HardcloudApp::HardcloudApp() {
    init();
}

HardcloudApp::HardcloudApp(bool _opsim) : opsim(_opsim) {
    init();
    if (opsim) {
        opinit();
    }
}

int HardcloudApp::opinit() {
    csr_offset = 0x1000;
    uint64_t afu_id_lo = fpga->mmioRead64(0x8);
    uint64_t afu_id_hi = fpga->mmioRead64(0x10);

    printf("afu_id_lo: %lx\n", afu_id_lo);
    printf("afu_id_hi: %lx\n", afu_id_hi);

    // AFU reset
    fpga->mmioWrite64(0x18, ~0);
    usleep(10);
    fpga->mmioWrite64(0x18, 0);
    usleep(10);

    // AFU offset array
    fpga->mmioWrite64(0x30, 0);
    fpga->mmioWrite64(0x38, 0);
    fpga->mmioWrite64(0x48, 0);
    fpga->mmioWrite64(0x48, 0);

    return 0;
}

HardcloudApp::~HardcloudApp() {}

int HardcloudApp::init() {
  uint64_t ioAddress;

  printf("using vai\n");

  fpga = new VAI_SVC_WRAPPER();

  fpga->reset();

  // Device Status Memory (DSM)
  dsm.size = getpagesize();
  dsm.virt = (volatile uint64_t*)fpga->allocBuffer(getpagesize(), &ioAddress);
  dsm.phys = ioAddress;

  csr_offset = 0;

  return 0;
}

void* HardcloudApp::alloc_buffer(uint64_t size) {
  Buffer buffer;
  uint64_t ioAddress;

  buffer.size = size;
  buffer.virt = (volatile uint64_t*) fpga->allocBuffer(size, &ioAddress);
  buffer.phys = ioAddress;

  buffers.push_front(buffer);

  return (void*) buffer.virt;
}

void HardcloudApp::delete_buffer(void *tgt_ptr) {
  for (uint32_t i = 0; buffers.size(); i++) {
    if (buffers[i].virt == tgt_ptr) {
      fpga->freeBuffer((void*)buffers[i].virt);

      buffers.erase(buffers.begin() + i);

      if (buffers.empty())
        this->finish();

      return;
    }
  }
}

int HardcloudApp::finish() {
  fpga->freeBuffer((void*)dsm.virt);

  delete fpga;

  return 0;
}

int HardcloudApp::run() {

  if (opsim) {
    fpga->mmioWrite64(0x18, ~0);
    usleep(10);
    fpga->mmioWrite64(0x18, 0);
  } else {
    fpga->reset();
  }

  // Clear the DSM
  dsm.virt[0] = 0;

  // Initiate DSM Reset
  // Set DSM base, high then low
  fpga->mmioWrite64(csr_offset + CSR_AFU_DSM_BASEL, intptr_t(dsm.virt));

  // Assert AFU reset
  fpga->mmioWrite64(csr_offset + CSR_CTL, CTL_ASSERT_RST);

  // De-Assert AFU reset
  fpga->mmioWrite64(csr_offset + CSR_CTL, CTL_DEASSERT_RST);

  for (int i = 0; i < buffers.size(); i++) {
    int pos = buffers.size() - i - 1;

    fpga->mmioWrite64(csr_offset + CSR_BASE_BUFFER + 0x10*i, intptr_t(buffers[pos].virt));
    fpga->mmioWrite64(csr_offset + CSR_BASE_BUFFER + 0x10*i + 0x08, buffers[pos].size/CL(1));
  }

  // Start the test
  fpga->mmioWrite64(csr_offset + CSR_CTL, CTL_START);

  // Wait for test completion
  while (0 == dsm.virt[0]) {
  };

  // Stop the device
  fpga->mmioWrite64(csr_offset + CSR_CTL, CTL_STOP);

  return 0;
}


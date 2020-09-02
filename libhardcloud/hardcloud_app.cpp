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

  fpga->reset();

  // Clear the DSM
  dsm.virt[0] = 0;

  // Initiate DSM Reset
  // Set DSM base, high then low
  fpga->mmioWrite64(CSR_AFU_DSM_BASEL, intptr_t(dsm.virt));

  // Assert AFU reset
  fpga->mmioWrite64(CSR_CTL, CTL_ASSERT_RST);

  // De-Assert AFU reset
  fpga->mmioWrite64(CSR_CTL, CTL_DEASSERT_RST);

  for (int i = 0; i < buffers.size(); i++) {
    int pos = buffers.size() - i - 1;

    fpga->mmioWrite64(CSR_BASE_BUFFER + 0x10*i, intptr_t(buffers[pos].virt));
    fpga->mmioWrite64(CSR_BASE_BUFFER + 0x10*i + 0x08, buffers[pos].size/CL(1));
  }

  // Start the test
  fpga->mmioWrite64(CSR_CTL, CTL_START);

  // Wait for test completion
  while (0 == dsm.virt[0]) {
  };

  // Stop the device
  fpga->mmioWrite64(CSR_CTL, CTL_STOP);

  return 0;
}


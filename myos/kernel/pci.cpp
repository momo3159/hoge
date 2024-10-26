#include "pci.hpp"
#include "asmfunc.h"

// 無名名前空間→内部の変数や関数などは他のファイルからは参照できない
namespace {
  using namespace pci;

  uint32_t MakeAddress(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg_addr) {
    auto shl = [](uint32_t x, unsigned int bits) {
      return x << bits;
    };

    return shl(1, 31) | shl(bus, 16) | shl(device, 11) | shl(function, 8) | (reg_addr & 0xfcu);
  }

  Error AddDevice(uint8_t bus, uint8_t device, uint8_t function, uint8_t header_type) {
    if (num_device == devices.size()) return MAKE_ERROR(Error::kFull);

    devices[num_device] = Device{bus, device, function, header_type};
    num_device++;
    return MAKE_ERROR(Error::kSuccess);
  }

  bool IsDisabledFunction(uint16_t vendor_id) {
    return vendor_id == 0xffffu;
  }

  bool IsStandardPCI_PCIBridge(ClassCode class_code) {
    return class_code.base == 0x06u && class_code.sub == 0x04u;
  }

  Error ScanBus(uint8_t bus);
  Error ScanFunction(uint8_t bus, uint8_t device, uint8_t function) {
    auto header_type = ReadHeaderType(bus, device, function);
    if (auto err = AddDevice(bus, device, function, header_type)) {
      return err;
    }

    auto class_code = ReadClassCode(bus, device, function);
    if (IsStandardPCI_PCIBridge(class_code)) {
      auto bus_numbers = ReadBusNumbers(bus, device, function);
      uint8_t secondary_bus = (bus_numbers >> 8) & 0xffu;
      return ScanBus(secondary_bus);
    }

    return MAKE_ERROR(Error::kSuccess);
  }

  Error ScanDevice(uint8_t bus, uint8_t device) {
    if (auto err = ScanFunction(bus, device, 0)) {
      return err;
    }
    if (IsSingleFunctionDevice(ReadHeaderType(bus, device, 0))) {
      return MAKE_ERROR(Error::kSuccess);
    }


    for (uint8_t function=1;function<=7;function++) {
      auto vendor_id = ReadVendorId(Device{bus, device, function});
      if (IsDisabledFunction(vendor_id)) continue;

      if (auto err = ScanFunction(bus, device, function)) {
        return err;
      }
    }

    return MAKE_ERROR(Error::kSuccess);
  }

  Error ScanBus(uint8_t bus) {
    for (uint8_t device=0;device<32;device++) {
      auto vendor_id = ReadVendorId(Device{bus, device, 0});
      if (IsDisabledFunction(vendor_id)) continue; // 必ずファンクション0は持つ. 0が無効であればスキップして良い．
      if (auto err = ScanDevice(bus, device)) return err;
    }

    return MAKE_ERROR(Error::kSuccess);
  }
}


namespace pci {
void WriteAddress(uint32_t address) {
    IoOut32(kConfigAddress, address);
  }

  void WriteData(uint32_t data) {
    IoOut32(kConfigData, data);
  }

  uint32_t ReadData() {
    return IoIn32(kConfigData);
  }

  uint16_t ReadVendorId(const Device& dev) {
    WriteAddress(MakeAddress(dev.bus, dev.device, dev.function, 0x00));
    return ReadData() & 0xffffu;
  }

  uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function) {
    WriteAddress(MakeAddress(bus, device, function, 0x0c));
    return (ReadData() >> 16) & 0xffu;
  }

  ClassCode ReadClassCode(uint8_t bus, uint8_t device, uint8_t function) {
    WriteAddress(MakeAddress(bus, device, function, 0x08));
    auto class_code = ReadData();
    ClassCode cc;
    cc.base =(class_code >> 24) & 0xffu;
    cc.sub =(class_code >> 16) & 0xffu;
    cc.interface =(class_code >> 8) & 0xffu;

    return cc;
  }

  uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function) {
    WriteAddress(MakeAddress(bus, device, function, 0x18));
    return ReadData();
  }


  uint32_t ReadConfReg(const Device& dev, uint8_t reg_addr) {
    WriteAddress(MakeAddress(dev.bus, dev.device, dev.function, reg_addr));
    return ReadData();
  }

  WithError<uint64_t> ReadBar(Device& dev, unsigned int bar_index) {
    if (bar_index >= 6) {
      return {0, MAKE_ERROR(Error::kIndexOutOfRange)};
    }

    const auto addr = CalcBarAddress(bar_index);
    const auto bar = ReadConfReg(dev, addr);

    if ((bar & 4u) == 0) {
      // 32bit
      return {bar, MAKE_ERROR(Error::kSuccess)};
    }

    if (bar_index >= 5) {
      return {0, MAKE_ERROR(Error::kIndexOutOfRange)};
    }

    const auto bar_upper = ReadConfReg(dev, addr + 4);
    return {
      bar | (static_cast<uint64_t>(bar_upper) << 32),
      MAKE_ERROR(Error::kSuccess)
    };
  }

  bool IsSingleFunctionDevice(uint8_t header_type) {
    return (header_type & 0x80u) == 0;
  }

  Error ScanAllBus() {
    num_device = 0;

    /*
    ホストブリッジ: ホスト側とPCIバス側を橋渡しする部品．CPUとPCIデバイスの通信は必ずここを通過する．
    ホストブリッジのファンクションN は PCIバスN を担当する．
    */
    auto header_type = ReadHeaderType(0, 0, 0);
    if (IsSingleFunctionDevice(header_type)) return ScanBus(0);

    for (uint8_t function=1;function<8;function++) {
      auto vendor_id = ReadVendorId(Device{0, 0, function});
      if (IsDisabledFunction(vendor_id)) continue;

      if (auto err = ScanBus(function)) {
        return err;
      }
    }

    return MAKE_ERROR(Error::kSuccess);
  }
}
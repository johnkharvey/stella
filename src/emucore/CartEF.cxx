//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2017 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#include "System.hxx"
#include "CartEF.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CartridgeEF::CartridgeEF(const BytePtr& image, uInt32 size,
                         const Settings& settings)
  : Cartridge(settings),
    myBankOffset(0)
{
  // Copy the ROM image into my buffer
  memcpy(myImage, image.get(), std::min(65536u, size));
  createCodeAccessBase(65536);

  // Remember startup bank
  myStartBank = 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CartridgeEF::reset()
{
  // Upon reset we switch to the startup bank
  bank(myStartBank);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CartridgeEF::install(System& system)
{
  mySystem = &system;

  // Install pages for the startup bank
  bank(myStartBank);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt8 CartridgeEF::peek(uInt16 address)
{
  address &= 0x0FFF;

  // Switch banks if necessary
  if((address >= 0x0FE0) && (address <= 0x0FEF))
    bank(address - 0x0FE0);

  return myImage[myBankOffset + address];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartridgeEF::poke(uInt16 address, uInt8)
{
  address &= 0x0FFF;

  // Switch banks if necessary
  if((address >= 0x0FE0) && (address <= 0x0FEF))
    bank(address - 0x0FE0);

  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartridgeEF::bank(uInt16 bank)
{
  if(bankLocked()) return false;

  // Remember what bank we're in
  myBankOffset = bank << 12;

  System::PageAccess access(this, System::PA_READ);

  // Set the page accessing methods for the hot spots
  for(uInt32 i = (0x1FE0 & ~System::PAGE_MASK); i < 0x2000;
      i += (1 << System::PAGE_SHIFT))
  {
    access.codeAccessBase = &myCodeAccessBase[myBankOffset + (i & 0x0FFF)];
    mySystem->setPageAccess(i >> System::PAGE_SHIFT, access);
  }

  // Setup the page access methods for the current bank
  for(uInt32 address = 0x1000; address < (0x1FE0U & ~System::PAGE_MASK);
      address += (1 << System::PAGE_SHIFT))
  {
    access.directPeekBase = &myImage[myBankOffset + (address & 0x0FFF)];
    access.codeAccessBase = &myCodeAccessBase[myBankOffset + (address & 0x0FFF)];
    mySystem->setPageAccess(address >> System::PAGE_SHIFT, access);
  }
  return myBankChanged = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt16 CartridgeEF::getBank() const
{
  return myBankOffset >> 12;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt16 CartridgeEF::bankCount() const
{
  return 16;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartridgeEF::patch(uInt16 address, uInt8 value)
{
  myImage[myBankOffset + (address & 0x0FFF)] = value;
  return myBankChanged = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const uInt8* CartridgeEF::getImage(int& size) const
{
  size = 65536;
  return myImage;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartridgeEF::save(Serializer& out) const
{
  try
  {
    out.putString(name());
    out.putShort(myBankOffset);
  }
  catch(...)
  {
    cerr << "ERROR: CartridgeEF::save" << endl;
    return false;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool CartridgeEF::load(Serializer& in)
{
  try
  {
    if(in.getString() != name())
      return false;

    myBankOffset = in.getShort();
  }
  catch(...)
  {
    cerr << "ERROR: CartridgeEF::load" << endl;
    return false;
  }

  // Remember what bank we were in
  bank(myBankOffset >> 12);

  return true;
}

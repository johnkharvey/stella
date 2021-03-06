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
// Copyright (c) 1995-2020 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#ifndef CARTRIDGEUA_WIDGET_HXX
#define CARTRIDGEUA_WIDGET_HXX

class CartridgeUA;
class PopUpWidget;

#include "CartDebugWidget.hxx"

class CartridgeUAWidget : public CartDebugWidget
{
  public:
    CartridgeUAWidget(GuiObject* boss, const GUI::Font& lfont,
                      const GUI::Font& nfont,
                      int x, int y, int w, int h,
                      CartridgeUA& cart, bool swapHotspots);
    virtual ~CartridgeUAWidget() = default;

  private:
    CartridgeUA& myCart;
    PopUpWidget* myBank{nullptr};

    bool mySwappedHotspots;

    enum { kBankChanged = 'bkCH' };

  private:
    void loadConfig() override;
    void handleCommand(CommandSender* sender, int cmd, int data, int id) override;

    string bankState() override;

    // Following constructors and assignment operators not supported
    CartridgeUAWidget() = delete;
    CartridgeUAWidget(const CartridgeUAWidget&) = delete;
    CartridgeUAWidget(CartridgeUAWidget&&) = delete;
    CartridgeUAWidget& operator=(const CartridgeUAWidget&) = delete;
    CartridgeUAWidget& operator=(CartridgeUAWidget&&) = delete;
};

#endif

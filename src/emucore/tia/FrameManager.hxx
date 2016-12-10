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
// Copyright (c) 1995-2016 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id$
//============================================================================

#ifndef TIA_6502TS_CORE_FRAME_MANAGER
#define TIA_6502TS_CORE_FRAME_MANAGER

#include <functional>

#include "Serializable.hxx"
#include "Types.hxx"
#include "bspf.hxx"

class FrameManager : public Serializable
{
  public:

    using callback = std::function<void()>;

  public:

    FrameManager();

  public:
    void setHandlers(callback frameStartCallback, callback frameCompletionCallback);

    void reset();

    void nextLine();

    void setVblank(bool vblank);

    void setVsync(bool vsync);

    bool isRendering() const;

    TvMode tvMode() const;

    bool vblank() const;

    uInt32 height() const;

    uInt32 currentLine() const;

    uInt32 scanlines() const;

    /**
      Serializable methods (see that class for more information).
    */
    bool save(Serializer& out) const override;
    bool load(Serializer& in) override;
    string name() const override { return "TIA_FrameManager"; }

  private:

    enum State {
      waitForVsyncStart,
      waitForVsyncEnd,
      waitForFrameStart,
      frame,
      overscan
    };

  private:

    void setTvMode(TvMode mode);

    void setState(State state);

    void finalizeFrame();

  private:

    callback myOnFrameStart;
    callback myOnFrameComplete;

    TvMode myMode;
    State myState;
    bool myWaitForVsync;
    uInt32 myLineInState;
    uInt32 myLinesWithoutVsync;
    uInt32 myCurrentFrameTotalLines;
    uInt32 myCurrentFrameFinalLines;

    bool myVsync;
    bool myVblank;

    uInt32 myVblankLines;
    uInt32 myKernelLines;
    uInt32 myOverscanLines;
    uInt32 myFrameLines;
    uInt32 myMaxLinesWithoutVsync;

  private:
    FrameManager(const FrameManager&) = delete;
    FrameManager(FrameManager&&) = delete;
    FrameManager& operator=(const FrameManager&) = delete;
    FrameManager& operator=(FrameManager&&) = delete;
};

#endif // TIA_6502TS_CORE_FRAME_MANAGER

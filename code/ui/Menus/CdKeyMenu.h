#pragma once

#include "../ui_local.h"
#include "../../common_game/String.h"
#include "MenuBase.h"

namespace UI::Menus
{
  enum ECdKeyEventID
  {
    Cdkey = 10,
    Accept,
    AcceptLater
  };

  class CdKeyMenu : public MenuBase
  {
  public:
    void OnDraw() override;
    sfxHandle_t OnKey(int32_t Key) override;

  private:
    menufield_s m_CdKey;
    menubitmap_s m_Accept;
    menubitmap_s m_AcceptLater;
    int32_t m_KeyResult;
    qhandle_t m_Halfround;
    int32_t m_FromMenu;

    int32_t PreValidateKey(const Common::String& Key);
    void Event(void* Ptr, int32_t Event);
    void DrawKey();
    void Graphics();
  };
}

#include "SpinControl.h"
#include "../Menus/MenuBase.h"
#include "../../common_game/FlagSupport.h"

using namespace Common;
using namespace BG;

namespace UI::MenuItems
{
  SpinControl::SpinControl(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const String& Name, Size Size, const Color& Clr, const Color& ColorHighlight, TextInfo Text) : MenuItem(EMenuItemType::SpinControl, ID, Position, Flags, Parent, Name)
    , m_OldValue{ 0 }
    , m_CurrentValue{ 0 }
    , m_Top{ 0 }
    , m_Size(Size)
    , m_Columns{ 0 }
    , m_Separation{ 0 }
    , m_Color(Clr)
    , m_ColorHighlight(ColorHighlight)
    , m_Text(Text)
    , m_Updated{ false }
    , m_ListPosition{ 0,0 }
    , m_ListFlags{ 0 }
    , m_ListColor{ 0 }
    , m_MaxChars{ 0 }
    , m_IgnoreList{ false }
    , m_DrawRect{ 0,0,0,0 }
  {}

  void SpinControl::DrawItem(int32_t BoxWidth, bool Alternate, String Text)
  {
    if(Text.find('\n') == String::npos)
    {
      if(m_MaxChars && Text.length() > m_MaxChars)
        Text = Text.substr(0, m_MaxChars) + "...";

      const auto Color = !m_ListColor ? EColors::White : m_ListColor;

      if(!m_ListPosition.X() && !m_ListPosition.Y())
      {
        const MenuPosition Pos{ X() + MENU_BUTTON_MED_HEIGHT + BoxWidth - 8 + MENU_BUTTON_MED_HEIGHT + 2, Alternate ? Y() + 5 : Y() + m_Text.Y() };
        DrawProportionalString(Pos, Text, UI_SMALLFONT | m_ListFlags, Color);

      }
      else
      {
        const MenuPosition Pos{ X() + m_ListPosition.X(), Alternate ? Y() + 5 : Y() + m_ListPosition.Y() };
        DrawProportionalString(Pos, Text, UI_SMALLFONT | m_ListFlags, Color);
      }
    }
  }

  void SpinControl::Draw()
  {
    const auto BoxWidth = m_Size.m_Width ? m_Size.m_Width : MENU_BUTTON_MED_WIDTH;
    Color ButtonColor;
    Color ButtonTextColor;

    if(IsSet(m_Flags, QMF_GRAYED))
    {
      ButtonColor = EColors::DarkGrey;
      ButtonTextColor = m_Text.Color();
    }
    else if(m_Parent->ItemAtCursor() == this)
    {
      auto Text2 = m_Text.GetText2();
      if(!Text2.empty())
        DrawProportionalString(m_Parent->GetDescriptionPosition(), Text2, UI_LEFT | UI_TINYFONT, EColors::Black);

      ButtonColor = m_ColorHighlight;
      ButtonTextColor = m_Text.ColorHightlight();
    }
    else
    {
      ButtonColor = m_Color;
      ButtonTextColor = m_Text.Color();
    }

    if(m_Updated)
    {
      SetColor(EColors::LightGold1);
      DrawPicture({ X() - 10, Y() + 6, 8, 8 }, uis.graphicCircle);
    }

    if(!m_ListShaders.empty())
    {
      DrawPicture({ X(), Y(), m_Size.m_Width, m_Size.m_Height }, m_ListShaders[m_CurrentValue]);
    }
    else
    {
      const RectF Position{ X(), Y(), BoxWidth, PROP_BIG_HEIGHT };

      SetColor(ButtonColor);
      if(IsSet(m_Flags, QMF_ALTERNATE))
        DrawPicture(Position, uis.whiteShader);
      else if(IsSet(m_Flags, QMF_ALTERNATE2))
        DrawPicture(Position, uis.graphicButtonLeft);
      else
      {
        DrawPicture({ X(), Y(), MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT }, uis.graphicButtonLeftEnd);
        DrawPicture({ X() + BoxWidth + MENU_BUTTON_MED_HEIGHT - 16, Y(), -MENU_BUTTON_MED_HEIGHT, MENU_BUTTON_MED_HEIGHT }, uis.graphicButtonLeftEnd);
        DrawPicture({ X() + MENU_BUTTON_MED_HEIGHT - 8, Y(), BoxWidth, MENU_BUTTON_MED_HEIGHT }, uis.whiteShader);
      }

      const auto Alternate = IsSet(m_Flags, QMF_ALTERNATE) || IsSet(m_Flags, QMF_ALTERNATE2);
      if(m_Text)
      {
        const MenuPosition TextPos{ X() + m_Text.X(), Y() + m_Text.Y() };
        const auto TextFlags = (Alternate ? UI_TINYFONT : UI_SMALLFONT) | m_Text.Flags();
        DrawProportionalString(TextPos, m_Text, TextFlags, ButtonTextColor);
      }

      if(!m_ListNames.empty())
      {
        const String Text = menu_normal_text[m_ListNames[m_CurrentValue]];
        DrawItem(BoxWidth, Alternate, Text);
      }

      if(!m_ItemNames.empty())
      {
        const auto Text = m_ItemNames[m_CurrentValue];
        DrawItem(BoxWidth, Alternate, Text);
      }
    }
  }

  void SpinControl::Init()
  {
    if(m_ListNames.empty() && m_ItemNames.empty())
      return;

    m_Rect.Left() = X();
    m_Rect.Top() = Y();

    if(!m_FocusSize.m_Width && !m_FocusSize.m_Height)
    {
      m_Rect.Right() = X() + MENU_BUTTON_MED_HEIGHT + MENU_BUTTON_MED_WIDTH + MENU_BUTTON_MED_HEIGHT;
      m_Rect.Bottom() = Y() + MENU_BUTTON_MED_HEIGHT;
    }
    else
    {
      m_Rect.Right() = X() + m_FocusSize.m_Width;
      m_Rect.Bottom() = Y() + m_FocusSize.m_Height;
    }
  }

  sfxHandle_t SpinControl::Key(int32_t& Key)
  {
    auto Sound = 0;
    auto Callback = false;

    switch(Key)
    {
    case K_MOUSE1:
    case K_ENTER:
    case K_KP_ENTER:
      {
        if(m_ListShaders.empty() && !m_IgnoreList)
        {
          Sound = InitListRender();
          if(m_Parent->DisplaySpinList() != nullptr)
            Callback = true;
        }
        else
        {
          m_CurrentValue++;
          if(m_CurrentValue >= ItemCount())
            m_CurrentValue = 0;

          Sound = menu_move_sound;
          Callback = true;
        }
        break;
      }
    case K_KP_LEFTARROW:
    case K_LEFTARROW:
      {
        if(m_CurrentValue > 0)
          m_CurrentValue--;
        else
          m_CurrentValue = ItemCount() - 1;

        Sound = menu_move_sound;
        Callback = true;
        break;
      }
    case K_KP_RIGHTARROW:
    case K_RIGHTARROW:
      {
        if(m_CurrentValue < ItemCount() - 1)
          m_CurrentValue++;
        else
          m_CurrentValue = 0;

        Sound = menu_move_sound;
        Callback = true;
        break;
      }
      default: 
      break;;
    }

    if(Sound && OnCallback && Callback)
      OnCallback(this, QM_ACTIVATED);

    return Sound;
  }

  int32_t SpinControl::ItemCount() const
  {
    if(!m_ListNames.empty())
      return static_cast<int32_t>(m_ListNames.size());
    if(!m_ItemNames.empty())
      return static_cast<int32_t>(m_ItemNames.size());

    return 0;
  }

  void SpinControl::SetTextFlags(uint32_t Flags)
  {
    m_Text.SetFlags(Flags);
  }

  uint32_t SpinControl::GetTextFlags() const
  {
    return m_Text.Flags();
  }

  void SpinControl::SetTextColor(const Color& Color)
  {
    m_Text.SetColor(Color);
  }

  void SpinControl::SetTextColorHighlight(const Color& Color)
  {
    m_Text.SetColorHighlight(Color);
  }

  void SpinControl::SetListPositionX(MenuPosition::value_type X)
  {
    m_ListPosition.X() = X;
  }

  void SpinControl::SetListPositionY(MenuPosition::value_type Y)
  {
    m_ListPosition.Y() = Y;
  }

  void SpinControl::AddItemNames(String&& ItemName)
  {
    m_ItemNames.emplace_back(std::forward<String>(ItemName));
  }

  void SpinControl::AddItemNames(StringID&& ListName)
  {
    m_ListNames.emplace_back(std::forward<StringID>(ListName));
  }

  void SpinControl::AddListShaders(qhandle_t&& Handle)
  {
    m_ListShaders.emplace_back(std::forward<qhandle_t>(Handle));
  }

  sfxHandle_t SpinControl::InitListRender()
  {
    if(m_Parent->DisplaySpinList())
    {
      if(IsSet(m_Flags, QMF_HASMOUSEFOCUS))
      {
        m_DrawRect = { 0,0,0,0 };
        m_DrawOffsets = { 0,0 };
        auto BestWidth = 0;
        int32_t WidthOffset;
        int32_t HeightOffset;

        if(!m_ListNames.empty())
          for(const auto& Name : m_ListNames)
          {
            if(ProportionalStringWidth(menu_normal_text[Name], UI_SMALLFONT) > BestWidth)
              BestWidth = ProportionalStringWidth(menu_normal_text[Name], UI_SMALLFONT);
          }
        else
          for(const auto& Name : m_ItemNames)
            if(ProportionalStringWidth(Name, UI_SMALLFONT) > BestWidth)
              BestWidth = ProportionalStringWidth(Name, UI_SMALLFONT);

        if(!m_ListPosition.X() && !m_ListPosition.Y())
        {
          WidthOffset = m_Size.m_Width ? m_Size.m_Width : MENU_BUTTON_MED_WIDTH;
          WidthOffset = MENU_BUTTON_MED_HEIGHT + WidthOffset - 8 + MENU_BUTTON_MED_HEIGHT + 2;
          HeightOffset = m_Text.X();
        }
        else
        {
          WidthOffset = m_ListPosition.X();
          HeightOffset = m_ListPosition.Y();
        }

        m_DrawRect.Left() = X() + WidthOffset - 2;
        m_DrawRect.Top() = Y() + HeightOffset - 2;
        m_DrawRect.Right() = m_DrawRect.Left() + BestWidth + 4;
        m_DrawRect.Bottom() = m_DrawRect.Top() + SMALLCHAR_HEIGHT * ItemCount() + 3;

        if(IsSet(m_Text.Flags(), UI_CENTER))
        {
          const auto Half = (m_DrawRect.Right() - m_DrawRect.Left()) >> 1;
          m_DrawRect.Left() -= Half;
          m_DrawRect.Bottom() -= Half;
        }
        // TODO: Add handling for UI_RIGHT?

        HeightOffset = static_cast<float>(m_DrawRect.Bottom() - m_DrawRect.Top()) * 0.33f;
        m_DrawRect.Top() -= HeightOffset;
        m_DrawRect.Bottom() -= HeightOffset;

        if(m_DrawRect.Right() > SCREEN_WIDTH)
        {
          m_DrawOffsets.m_Width = m_DrawRect.Right() - SCREEN_WIDTH + 6;
          m_DrawRect.Left() -= m_DrawOffsets.m_Width;
          m_DrawRect.Right() -= m_DrawOffsets.m_Width;
        }

        if(m_DrawRect.Bottom() > SCREEN_HEIGHT)
        {
          m_DrawOffsets.m_Height = m_DrawRect.Bottom() - SCREEN_HEIGHT + 6;
          m_DrawRect.Top() -= m_DrawOffsets.m_Height;
          m_DrawRect.Bottom() -= m_DrawOffsets.m_Height;
        }

        if(m_DrawRect.Top() < 0)
        {
          m_DrawOffsets.m_Height = 0 - m_DrawRect.Top();
          m_DrawRect.Top() += m_DrawOffsets.m_Height;
          m_DrawRect.Bottom() += m_DrawOffsets.m_Height;
        }

        if(m_DrawRect.Left() < 0)
        {
          m_DrawOffsets.m_Width = 0 - m_DrawRect.Left();
          m_DrawRect.Left() += m_DrawOffsets.m_Width;
          m_DrawRect.Right() += m_DrawOffsets.m_Width;
        }

        m_Parent->SetDisplaySpinList(this);
        m_Parent->SetNoNewSelecting(true);

        return menu_move_sound;
      }
    }
    else
    {
      if(CursorInRect(m_DrawRect) || ItemCount() >= MAX_LIST && CursorInRect({ m_DrawRect.Left(), m_DrawRect.Top(), m_DrawRect.Width() * 2, m_DrawRect.Height() }))
      {
        auto SelectedNum = (uis.cursory - m_DrawRect.Top() + 1) / SMALLCHAR_HEIGHT;

        if(ItemCount() > MAX_LIST && uis.cursorx > m_DrawRect.Right() && SelectedNum + MAX_LIST < ItemCount())
          SelectedNum += MAX_LIST;

        if(SelectedNum < 0)
          SelectedNum = 0;
        else if(SelectedNum > ItemCount() - 1)
          SelectedNum = ItemCount() - 1;

        m_CurrentValue = SelectedNum;

        m_Parent->SetDisplaySpinList(nullptr);
        m_Parent->SetNoNewSelecting(false);

        return menu_out_sound;
      }
      
      m_Parent->SetDisplaySpinList(nullptr);
      m_Parent->SetNoNewSelecting(false);

      return menu_out_sound;
    }

    return menu_move_sound;
  }
}

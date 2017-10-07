#pragma once
#include "MenuItem.h"
#include "../../base_game/q_shared.h"
#include "TextInfo.h"
#include "../../common_game/Size.h"
#include "../../base_game/Color.h"

namespace UI::MenuItems
{
  class SpinControl : public MenuItem
  {
    friend class Menus::MenuBase;
  public:
    SpinControl(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const Common::String& Name, Common::Size Size, const BG::Color& Clr, const BG::Color& ColorHighlight, TextInfo Text);
    void DrawItem(int32_t BoxWidth, bool Alternate, Common::String Text);

    void Draw() override;
    void Init() override;
    sfxHandle_t Key(int32_t& Key) override;

    int32_t ItemCount() const;

    void SetCurrentValue(int32_t Index) { m_CurrentValue = Index; }
    int32_t CurrentValue() const { return m_CurrentValue; }

    void SetColumns(int32_t Columns) { m_Columns = Columns; }
    void SetColor(const BG::Color& Color) { m_Color = Color; }
    void SetColorHighlight(const BG::Color&  Color) { m_ColorHighlight = Color; }

    void SetTextFlags(uint32_t Flags);
    uint32_t GetTextFlags() const;

    void SetTextColor(const BG::Color& Color);
    void SetTextColorHighlight(const BG::Color& Color);
    void SetFocusSize(const Common::Size& Size) { m_FocusSize = Size; }
    void SetFocusWidth(Common::Size::value_type Width) { m_FocusSize.m_Width = Width; }
    void SetFocusHeight(Common::Size::value_type Height) { m_FocusSize.m_Height = Height; }
    void SetListPosition(const MenuPosition& Position) { m_ListPosition = Position; }
    void SetListPositionX(MenuPosition::value_type X);
    void SetListPositionY(MenuPosition::value_type Y);

    void SetListFlags(uint32_t Flags) { m_ListFlags = Flags; }
    uint32_t GetListFlags() const { return m_ListFlags; }

    void SetMaxChars(int32_t Max) { m_MaxChars = Max; }
    void SetIgnoreList(bool Value = true) { m_IgnoreList = Value; }

    void SetItemNames(const std::vector<Common::String>& ItemNames) { m_ItemNames = ItemNames; }
    void SetItemNames(std::vector<Common::String>&& ItemNames) { m_ItemNames = std::move(ItemNames); }
    const std::vector<Common::String>& ItemNames() const { return m_ItemNames; }

    template<typename... ArgsT>
    void AddItemNames(Common::String&& ItemName, ArgsT&&... Args)
    {
      AddItemNames(std::forward<Common::String>(ItemName));
      AddItemNames(std::forward<ArgsT>(Args)...);
    }

    void AddItemNames(Common::String&& ItemName);

    void SetListNames(const std::vector<StringID>& ListNames) { m_ListNames = ListNames; }
    void SetListNames(std::vector<StringID>&& ListNames) { m_ListNames = std::move(ListNames); }
    const std::vector<StringID>& ListNames() const { return m_ListNames; }

    template<typename... ArgsT>
    void AddListNames(StringID&& ListName, ArgsT&&... Args)
    {
      AddItemNames(std::forward<StringID>(ListName));
      AddItemNames(std::forward<ArgsT>(Args)...);
    }

    void AddItemNames(StringID&& ListName);

    void SetListShaders(const std::vector<qhandle_t>& ListShaders) { m_ListShaders = ListShaders; }
    void SetListShaders(std::vector<qhandle_t>&& ListShaders) { m_ListShaders = std::move(ListShaders); }

    template<typename... ArgsT>
    void AddListShaders(qhandle_t&& Handle, ArgsT&&... Args)
    {
      AddListShaders(std::forward<qhandle_t>(Handle));
      AddListShaders(std::forward<ArgsT>(Args)...);
    }

    void AddListShaders(qhandle_t&& Handle);

  protected:
    int32_t m_OldValue;
    int32_t m_CurrentValue;
    int32_t m_Top;
    Common::Size m_Size;
    int32_t m_Columns;
    int32_t m_Separation;
    BG::Color m_Color;
    BG::Color m_ColorHighlight;
    TextInfo m_Text;
    bool m_Updated;
    Common::Size m_FocusSize;
    MenuPosition m_ListPosition;
    uint32_t m_ListFlags;
    BG::Color m_ListColor;
    int32_t m_MaxChars;
    bool m_IgnoreList;
    Common::Rect m_DrawRect;
    Common::Size m_DrawOffsets;

    std::vector<Common::String> m_ItemNames;
    std::vector<StringID> m_ListNames;
    std::vector<qhandle_t> m_ListShaders;

    sfxHandle_t InitListRender();
  };
}

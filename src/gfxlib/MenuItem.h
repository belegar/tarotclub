/*=============================================================================
 * TarotClub - BidsForm.h
 *=============================================================================
 * Graphical menu to choose the bids
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef MENU_ITEM_H_
#define MENU_ITEM_H_

// Qt includes
#include <QGraphicsSvgItem>
#include <QtGui>

// Game includes
#include "TextBox.h"
#include "Common.h"
#include "CheckBoxItem.h"
#include "CustomTypes.h"
#include "ButtonItem.h"
#include "RectBase.h"

/*****************************************************************************/
class MenuItem : public RectBase
{
public:
    MenuItem(IButtonEvent *event);

    /**
     * @brief The MenuWidget enum gather all the widgets managed by this menu
     *
     * Just add a new enum line to manage this widget
     */
    static const std::uint8_t PASS_BUTTON              = 0U;
    static const std::uint8_t TAKE_BUTTON              = 1U;
    static const std::uint8_t GUARD_BUTTON             = 2U;
    static const std::uint8_t GUARD_WITHOUT_BUTTON     = 3U;
    static const std::uint8_t GUARD_AGAINST_BUTTON     = 4U;
    static const std::uint8_t DECLARE_HANDLE_BUTTON    = 5U;
    static const std::uint8_t ACCEPT_DISCARD_BUTTON    = 6U;
    static const std::uint8_t START_SINGLE_PLAYER      = 7U;

    static const std::uint8_t NO_MENU       = 0U;
    static const std::uint8_t BIDS_MENU     = 1U;
    static const std::uint8_t HANDLE_MENU   = 2U;
    static const std::uint8_t DISCARD_MENU  = 3U;
    static const std::uint8_t MAIN_MENU     = 4U;

    enum { Type = UserType + MENU_TYPE_ITEM };

    // Virtual methods
    int type() const;

    // Helpers
    void DisplayMenu(std::uint8_t menu);
    void DisplayMenu(Contract minContract);
    void Initialize();
    void ClearSlamOption();

    // Getters
    bool GetSlamOption();

private:
    QPointF GetButtonPosition(std::uint32_t pos);

    std::vector<ButtonItem *> buttons; // menu type, button
    CheckBoxItem checkBox;
};

#endif // MENU_ITEM_H_

//=============================================================================
// End of file MenuItem.h
//=============================================================================

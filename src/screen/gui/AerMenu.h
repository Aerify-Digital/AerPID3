/* =============================================================================
 * Copyright (c) 2015-2024 AerifyDigital LLC.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * =============================================================================
 */

#ifndef _AER_MENU_H
#define _AER_MENU_H

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <Arduino.h>

class AerMenu
{
public:
    AerMenu() { AerMenu(0); };
    AerMenu(uint16_t index, const uint16_t parent, std::vector<uint16_t> children)
    {
        this->_parent = parent;
        this->_menuIndex = index;
        children.insert(children.begin(), parent);
        this->children = children;
        this->levels = children.size();
        this->selected_level = 0;
        this->selected = false;
    };
    ~AerMenu() { dispose(); };

    std::vector<uint16_t> getChildren() { return children; }
    void addChild(uint16_t child) { children.push_back(child); }

    uint16_t getIndex() { return _menuIndex; }
    void setIndex(uint16_t ind) { _menuIndex = ind; };
    uint16_t getParent() { return _parent; }
    uint8_t getLevels() { return levels; }
    uint16_t getSelected() { return selected_level; }
    bool isSelected() { return selected; }

    void select(uint16_t level, bool select)
    {
        if (level <= levels)
        {
            if (select)
            {
                this->selected = true;
                this->selected_level = level;
            }
            else
            {
                this->selected = false;
                this->selected_level = 0;
            }
        }
    }

protected:
    AerMenu(uint16_t index)
    {
        this->_parent = 0;
        this->_menuIndex = index;
        this->children = {};
        this->levels = 0;
        this->selected_level = 0;
        this->selected = false;
    };

private:
    uint16_t _menuIndex;
    uint16_t _parent;

    uint8_t levels;
    uint16_t selected_level;

    bool selected;

    std::vector<uint16_t> children;

    void dispose()
    {
        this->children.clear();
        this->children.shrink_to_fit();
    }
};

#endif
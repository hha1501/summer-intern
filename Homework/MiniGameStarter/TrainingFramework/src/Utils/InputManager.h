#pragma once

#include "KeyCode.h"

#include "GameConfig.h"

#include <stdint.h>
#include <stdio.h>

class InputManager
{
public:
    InputManager() : m_keyHoldingBitSet{}, m_keyDownBitSet{}, m_keyUpBitSet{}
    {
    }

    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    void OnKeyEvent(unsigned char key, bool pressed)
    {
        constexpr uint32_t one = (uint32_t)1;
        const int keyShiftAmount = (int)KeyValueToCode(key);
        const uint32_t mask = one << keyShiftAmount;

        //if (pressed)
        //{
        //	// ____1____
        //	m_keyDownBitSet |= mask;

        //	//		____ 1 ____ ^
        //	//		0000 k 0000
        //	// =	____~k ____
        //	m_keyDownBitSet ^= (m_keyHoldingBitSet & mask);

        //	// Set holding state
        //	m_keyHoldingBitSet |= mask;

        //	// Clear up state
        //	m_keyUpBitSet &= ~mask;
        //}
        //else
        //{
        //	// Clear holding state
        //	m_keyHoldingBitSet &= ~mask;

        //	// Clear down state
        //	m_keyDownBitSet &= ~mask;

        //	// Set up state
        //	m_keyUpBitSet |= mask;
        //}

        if (pressed)
        {
            // Set holding state
            m_keyHoldingBitSet |= mask;

            // Set down state
            m_keyDownBitSet |= mask;

            // Clear up state
            m_keyUpBitSet &= ~mask;
        }
        else
        {
            // Clear holding state
            m_keyHoldingBitSet &= ~mask;

            // Clear down state
            m_keyDownBitSet &= ~mask;

            // Set up state
            m_keyUpBitSet |= mask;
        }

    }

    void OnUpdate()
    {
        m_keyDownBitSet = 0;
        m_keyUpBitSet = 0;
    }

    bool Key(KeyCode keyCode) const
    {
        constexpr uint32_t one = (uint32_t)1;
        const int keyShiftAmount = (int)keyCode;
        const uint32_t mask = one << keyShiftAmount;

        return (m_keyHoldingBitSet & mask) != 0;
    }

    bool KeyDown(KeyCode keyCode) const
    {
        constexpr uint32_t one = (uint32_t)1;
        const int keyShiftAmount = (int)keyCode;
        const uint32_t mask = one << keyShiftAmount;

        return (m_keyDownBitSet & mask) != 0;
    }

    bool KeyUp(KeyCode keyCode) const
    {
        constexpr uint32_t one = (uint32_t)1;
        const int keyShiftAmount = (int)keyCode;
        const uint32_t mask = one << keyShiftAmount;

        return (m_keyUpBitSet & mask) != 0;
    }

private:
    constexpr KeyCode KeyValueToCode(unsigned char key)
    {
        switch (key)
        {
        case VK_LEFT:
            return KeyCode::LEFT;
        case VK_RIGHT:
            return KeyCode::RIGHT;
        case VK_UP:
            return KeyCode::UP;
        case VK_DOWN:
            return KeyCode::DOWN;
        case 'A':
            return KeyCode::A;
        case 'W':
            return KeyCode::W;
        case 'D':
            return KeyCode::D;
        case 'S':
            return KeyCode::S;
        default:
            return KeyCode::UNKNOWN;
        }
    }

private:
    uint32_t m_keyHoldingBitSet;
    uint32_t m_keyDownBitSet;
    uint32_t m_keyUpBitSet;
};

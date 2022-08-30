#pragma once

enum class FilterValue
{
    Node = 1,
    Manipulator = 2,
    NM = (Node | Manipulator), //Node and Manipulator
};

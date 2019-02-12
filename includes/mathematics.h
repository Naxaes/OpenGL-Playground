//
// Created by Ted Klein Bergman on 2019-01-11.
//

#pragma once

template <typename T>
class Array2D
{
    std::vector<T> content;
public:
    const unsigned rows;
    const unsigned columns;

    Array2D(unsigned rows, unsigned columns) : content(rows * columns), rows(rows), columns(columns) {}

    T& operator () (unsigned row, unsigned column)
    {
        return content[row * columns + column];
    }

    std::vector<T> data()
    {
        return content;
    }
};


float lerp(float start, float stop, float value)
{
    return (1-value) * start + value * stop;
}

float map(float start, float stop, float value)
{
    value = value / stop;
    return (1-value) * start + value * stop;
}
/*
    Copyright (c) 2008-10 Qtrac Ltd. All rights reserved.
    This program or module is free software: you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 2 of
    the License, or version 3 of the License, or (at your option) any
    later version. This program is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    This is based on Brian M. Clapper's Python 2 implementation
    at http://www.clapper.org/bmc/ itself based on the implementation at
    http://www.public.iastate.edu/~ddoty/HungarianAlgorithm.html
*/

#include "kuhn_munkres.hpp"
#include <utility>


KuhnMunkres::Indexes KuhnMunkres::calculate(const Grid &grid)
{
    grid_ = grid;
    const Dimensions dimensions = ensure_grid_is_square();
    size = static_cast<int>(grid_.size());
#ifdef USE_STL
    row_covered.resize(size, false);
    column_covered.resize(size, false);
#else
    row_covered.fill(false, size);
    column_covered.fill(false, size);
#endif
    z0_row = 0;
    z0_column = 0;
    path = make_grid(size * 2, static_cast<int>(ZERO));
    marked = make_grid(size, static_cast<int>(ZERO));

    int step = 1;
    while (step) {
        switch (step) {
            case 1: step = step1(); break;
            case 2: step = step2(); break;
            case 3: step = step3(); break;
            case 4: step = step4(); break;
            case 5: step = step5(); break;
            case 6: step = step6(); break;
            default: break;
        }
    }

    Indexes indexes;
    for (int row = 0; row < size; ++row) {
        for (int column = 0; column < size; ++column) {
            if ((row < dimensions.first) &&
                (column < dimensions.second) &&
                marked.at(row).at(column) == STAR)
#ifdef USE_STL
                indexes.push_back(std::make_pair(row, column));
#else
                indexes.push_back(qMakePair(row, column));
#endif
        }
    }
    return indexes;
}


KuhnMunkres::Dimensions KuhnMunkres::ensure_grid_is_square()
{
    int max_columns = 0;
    int total_rows = grid_.size();
    for (int row = 0; row < total_rows; ++row)
        max_columns = std::max(max_columns,
                               static_cast<int>(grid_.at(row).size()));
    total_rows = std::max(total_rows, max_columns);
    const int rows_used = grid_.size();
    const int columns_used = max_columns;

    Grid new_grid;
    for (int row = 0; row < static_cast<int>(grid_.size()); ++row) {
        Row new_row(grid_.at(row));
#ifdef USE_STL
        new_row.resize(total_rows, DBL_MIN);
#else
        // QVector::fill() is _not_ the same as std::vector::resize()!
        new_row.reserve(total_rows);
        while (new_row.size() < total_rows)
            new_row.push_back(DBL_MIN);
#endif
        new_grid.push_back(new_row);
    }

    while (static_cast<int>(new_grid.size()) < total_rows)
        new_grid.push_back(Row(total_rows, DBL_MIN));

    grid_ = new_grid;
#ifdef USE_STL
    return std::make_pair(rows_used, columns_used);
#else
    return qMakePair(rows_used, columns_used);
#endif
}


template<typename T>
#ifdef USE_STL
std::vector<std::vector<T> >
#else
QVector<QVector<T> >
#endif
KuhnMunkres::make_grid(int size, T value)
{
#ifdef USE_STL
    std::vector<std::vector<T> > grid;
    grid.resize(size, std::vector<T>());
#else
    QVector<QVector<T> > grid;
    grid.fill(QVector<T>(), size);
#endif
    for (int row = 0; row < size; ++row)
#ifdef USE_STL
        grid[row].resize(size, value);
#else
        grid[row].fill(value, size);
#endif
    return grid;
}


int KuhnMunkres::step1()
{
    for (int row = 0; row < size; ++row) {
        double minimum = grid_.at(row).at(0);
        for (int column = 1; column < size; ++column)
            minimum = std::min(minimum, grid_.at(row).at(column));
        for (int column = 0; column < size; ++column)
            grid_[row][column] -= minimum;
    }
    return 2;
}


int KuhnMunkres::step2()
{
    for (int row = 0; row < size; ++row) {
        for (int column = 0; column < size; ++column) {
            if (is_zero(grid_.at(row).at(column)) &&
                (!row_covered.at(row)) && (!column_covered.at(column))) {
                marked[row][column] = STAR;
                row_covered[row] = true;
                column_covered[column] = true;
            }
        }
    }
    clear_covers();
    return 3;
}


int KuhnMunkres::step3()
{
    int count = 0;
    for (int row = 0; row < size; ++row) {
        for (int column = 0; column < size; ++column) {
            if (marked.at(row).at(column) == STAR) {
                column_covered[column] = true;
                ++count;
            }
        }
    }
    return (count >= size) ? 0 : 4;
}


int KuhnMunkres::step4()
{
    int step = 0;
    bool done = false;
    int row = -1;
    int column = -1;
    int star_column = -1;
    while (!done) {
        Index index = find_a_zero();
        row = index.first;
        column = index.second;
        if (row < 0) {
            done = true;
            step = 6;
        }
        else {
            marked[row][column] = PRIME;
            star_column = find_star_in_row(row);
            if (star_column >= 0) {
                column = star_column;
                row_covered[row] = true;
                column_covered[column] = false;
            }
            else {
                done = true;
                z0_row = row;
                z0_column = column;
                step = 5;
            }
        }
    }
    return step;
}


int KuhnMunkres::step5()
{
    int count = 0;
    path[count][0] = z0_row;
    path[count][1] = z0_column;
    bool done = false;
    while (!done) {
        int row = find_star_in_column(path.at(count).at(1));
        if (row >= 0) {
            ++count;
            path[count][0] = row;
            path[count][1] = path.at(count - 1).at(1);
        }
        else
            done = true;
        if (!done) {
            int column = find_prime_in_row(path.at(count).at(0));
            ++count;
            path[count][0] = path.at(count - 1).at(0);
            path[count][1] = column;
        }
    }
    convert_path(count);
    clear_covers();
    erase_primes();
    return 3;
}


int KuhnMunkres::step6()
{
    double minimum = find_smallest();
    for (int row = 0; row < size; ++row) {
        for (int column = 0; column < size; ++column) {
            if (row_covered.at(row))
                grid_[row][column] += minimum;
            if (!column_covered.at(column))
                grid_[row][column] -= minimum;
        }
    }
    return 4;
}


void KuhnMunkres::clear_covers()
{
#ifdef USE_STL
    row_covered.assign(size, false);
    column_covered.assign(size, false);
#else
    row_covered.fill(false, size);
    column_covered.fill(false, size);
#endif
}


KuhnMunkres::Index KuhnMunkres::find_a_zero()
{
    int row = -1;
    int column = -1;
    int i = 0;
    bool done = false;
    while (!done) {
        int j = 0;
        while (true) {
            if (is_zero(grid_.at(i).at(j)) &&
                (!row_covered.at(i)) && (!column_covered.at(j))) {
                row = i;
                column = j;
                done = true;
            }
            ++j;
            if (j >= size)
                break;
        }
        ++i;
        if (i >= size)
            done = true;
    }
#ifdef USE_STL
    return std::make_pair(row, column);
#else
    return qMakePair(row, column);
#endif
}


int KuhnMunkres::find_star_in_row(int row)
{
    int column = -1;
    for (int j = 0; j < size; ++j)
        if (marked.at(row).at(j) == STAR) {
            column = j;
            break;
        }
    return column;
}


int KuhnMunkres::find_star_in_column(int column)
{
    int row = -1;
    for (int i = 0; i < size; ++i)
        if (marked.at(i).at(column) == STAR) {
            row = i;
            break;
        }
    return row;
}


int KuhnMunkres::find_prime_in_row(int row)
{
    int column = -1;
    for (int j = 0; j < size; ++j)
        if (marked.at(row).at(j) == PRIME) {
            column = j;
            break;
        }
    return column;
}


void KuhnMunkres::convert_path(int count)
{
    for (int i = 0; i <= count; ++i) {
        int &x = marked[path.at(i).at(0)][path.at(i).at(1)];
        x = (x == STAR ? ZERO : STAR);
    }
}


void KuhnMunkres::erase_primes()
{
    for (int row = 0; row < size; ++row) {
        for (int column = 0; column < size; ++column) {
            int &x = marked[row][column];
            if (x == PRIME)
                x = ZERO;
        }

    }
}


double KuhnMunkres::find_smallest()
{
    double minimum = DBL_MAX;
    for (int row = 0; row < size; ++row) {
        for (int column = 0; column < size; ++column)
            if ((!row_covered.at(row)) && (!column_covered.at(column))) {
                if (minimum > grid_.at(row).at(column))
                    minimum = grid_[row][column];
            }
    }
    return minimum;
}

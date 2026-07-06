#pragma once

#include <iostream>
#include <cstring>

bool IsSeparator(char *separators, char c)
{
    for (size_t i = 0; separators[i] != '\0'; ++i)
    {
        if (separators[i] == c)
            return 1;
    }
    return 0;
}

void WordsReplacing(char *str, size_t id1, int max_size, size_t id2, int min_size)
{
    char s[300];
    size_t i = 0;

    size_t first_start = (id1 < id2) ? id1 : id2;
    size_t second_start = (id1 < id2) ? id2 : id1;
    size_t first_len = (id1 < id2) ? max_size : min_size;
    size_t second_len = (id1 < id2) ? min_size : max_size;

    for (; i < first_start; ++i)
    {
        s[i] = str[i];
    }

    for (size_t j = 0; j < second_len; ++j, ++i)
    {
        s[i] = str[second_start + j];
    }

    for (size_t j = first_start + first_len; j < second_start; ++j, ++i)
    {
        s[i] = str[j];
    }

    for (size_t j = 0; j < first_len; ++j, ++i)
    {
        s[i] = str[first_start + j];
    }

    size_t second_end = second_start + second_len;
    while (str[second_end] != '\0')
    {
        s[i] = str[second_end];
        ++i;
        ++second_end;
    }
    s[i] = '\0';

    i = 0;
    while (s[i] != '\0')
    {
        str[i] = s[i];
        ++i;
    }
    str[i] = '\0';
}

char *Laba7(char *input_str)
{
    char separators[] = " ,.!?;:\n\t";

    int min_len{301}, max_len{-1};
    size_t min_id, max_id;
    bool found = false;

    size_t i = 0;
    while (input_str[i] != '\0')
    {
        while (input_str[i] != '\0' && IsSeparator(separators, input_str[i]))
            ++i;
        if (input_str[i] == '\0')
            break;
        size_t start = i;
        int len = 0;
        while (input_str[i] != '\0' && !IsSeparator(separators, input_str[i]))
        {
            ++len;
            ++i;
        }
        if (len > 0)
        {
            found = true;
            if (len <= min_len)
            {
                min_len = len;
                min_id = start;
            }
            if (len > max_len)
            {
                max_len = len;
                max_id = start;
            }
        }
    }
    if (!found)
    {
        return "No words found";
    }

    if (min_id == max_id)
    {
        return "Only one word (shortest == longest). Nothing to swap";
    }
    WordsReplacing(input_str, max_id, max_len, min_id, min_len);
    return input_str;
}
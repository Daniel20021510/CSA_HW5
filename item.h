//
// Created by Даниил Лесь on 15.01.2022.
//

#ifndef CSA_HW5_ITEM_H
#define CSA_HW5_ITEM_H

// структура описывающая товар
struct item {
    bool first_section;
    int count;
    item() {
        first_section = rand() % 2;
        count = (rand() % 10) + 1;
    }
};

#endif //CSA_HW5_ITEM_H

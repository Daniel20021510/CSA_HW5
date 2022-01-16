//
// Created by Даниил Лесь on 15.01.2022.
//

#ifndef CSA_HW5_CONSUMER_H
#define CSA_HW5_CONSUMER_H

// структура описывающая покупателя
// для упрощения программы было приянто решение реализовать укладку товаров в списке
// в шахматном порядке, в целом это не повлеяло на концепцию многопоточности
// такое решение было приянто из-за проблем со средой
struct consumer {
    std::queue<item> shopping_list;
    std::string name;
    explicit consumer(std::string name) {
        this->name = std::move(name);
        for (int i = 0; i < rand() % 3 + 3; ++i) {
            shopping_list.push(item());
        }
    }
    consumer() = default;

    bool isFirstItemForFirstSeller() {
        return shopping_list.front().first_section;
    }
};

#endif //CSA_HW5_CONSUMER_H

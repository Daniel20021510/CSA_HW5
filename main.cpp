#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <thread>
#include <queue>
#include <chrono>
#include <utility>
#include "item.h"
#include "consumer.h"

/*
 * Вообще я пытался организовать прогрумму с ООП подходом, но у меня cloin сошел с ума.
 * Подход был такой: создать отдельно класс покупателя, продовца и магазина.
 * магазин хранил бы очереди двух продавцов, однако при импорте в класс магазина класс покупателя все ломалось(
 */

// общее кол-во покупателей
int n;
// тригерр конца покупателей
bool end = true;

// Очереди покупателей
std::queue<consumer> first_departament_queue;
std::queue<consumer> second_departament_queue;

// Мьютекс - изменение очередей
pthread_mutex_t m_changing_queues;

// Мьютекс засыпания первого продавца
pthread_mutex_t m_seller_1_sleep;

// Мьютекс засыпания второго продовцы
pthread_mutex_t m_seller_2_sleep;

// Мьютекс печати
pthread_mutex_t m_cout;

// Разбудить первого продавца
pthread_cond_t weak_seller_1;
// Разбудить второго продавца
pthread_cond_t weak_seller_2;

/*
 * поток обработки работы продовца.
 * в нем известны сразу две очереди (т.е. ведем работу с обоими прродовцами)
 */
void *Sellers(void *params) {
    bool is_first_departament = ((int*)params);
    pthread_mutex_t *this_mutex = &m_seller_1_sleep;
    pthread_cond_t *this_cond = &weak_seller_1;
    pthread_cond_t *other_cond = &weak_seller_2;
    std::queue<consumer> *this_queue = &first_departament_queue;
    std::queue<consumer> *other_queue = &second_departament_queue;
    int this_seller_id = 2;
    int other_seller_id = 1;

    if (!is_first_departament) {
        this_queue = &second_departament_queue;
        other_queue = &first_departament_queue;
        this_mutex = &m_seller_2_sleep;
        this_cond = &weak_seller_2;
        other_cond = &weak_seller_1;
        other_seller_id = 1;
        this_seller_id = 2;
    }

    pthread_cond_wait(this_cond, this_mutex);

    while (end) {
        if (this_queue->empty()) {
            pthread_mutex_lock(&m_cout);
            std::cout << "Seller " << this_seller_id << " is gone to sleep\n";
            pthread_mutex_unlock(&m_cout);

            pthread_cond_wait(this_cond, this_mutex);

            pthread_mutex_lock(&m_cout);
            std::cout << "Seller " << this_seller_id << " woken up\n";
            pthread_mutex_unlock(&m_cout);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            pthread_mutex_lock(&m_changing_queues);
            this_queue->front().shopping_list.pop();
            pthread_mutex_lock(&m_cout);
            if (this_queue->front().shopping_list.empty()) {
                std::cout << this_queue->front().name << " go out of the shop\n";
            } else {
                std::cout << this_queue->front().name << " go to seller " << other_seller_id << "\n";
                other_queue->push(this_queue->front());
                pthread_cond_signal(other_cond);
            }
            pthread_mutex_unlock(&m_cout);
            this_queue->pop();
            pthread_mutex_unlock(&m_changing_queues);
        }
    }
}

// Добавляем новых покупателей в потоке
void *AddConsumer(void *params) {
    consumer new_consumer;
    std::queue<consumer> *this_queue;
    pthread_cond_t *this_condition;
    std::string this_departament_name;
    int customer_count = 1;

    while (customer_count != n + 1) {
        new_consumer = consumer("Customer " + std::to_string(customer_count));
        if (new_consumer.isFirstItemForFirstSeller()) {
            this_queue = &first_departament_queue;
            this_condition = &weak_seller_1;
            this_departament_name = "1";
        } else {
            this_queue = &second_departament_queue;
            this_condition = &weak_seller_2;
            this_departament_name = "2";
        }

        pthread_mutex_lock(&m_changing_queues);
        this_queue->push(new_consumer);
        pthread_mutex_unlock(&m_changing_queues);

        pthread_mutex_lock(&m_cout);
        std::cout << "New customer " << new_consumer.name << " joined queue in section " << this_departament_name << "\n";
        customer_count++;
        pthread_mutex_unlock(&m_cout);

        // Будим продовца
        pthread_cond_signal(this_condition);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000 + rand() % 10));
    }
    end = false;
}

int main() {
    std::cout << ("Enter number of consumers > 0: ");
    std::cin >> n;
    if (n < 1 or n > 1000){
        std::cout << ("Number of consumers must be > 0");
        return 1;
    }
    std::srand(std::time(nullptr));
    pthread_mutex_init(&m_changing_queues, nullptr);
    pthread_mutex_init(&m_seller_1_sleep, nullptr);
    pthread_mutex_init(&m_seller_2_sleep, nullptr);
    pthread_mutex_init(&m_cout, nullptr);
    pthread_cond_init(&weak_seller_1, nullptr);
    pthread_cond_init(&weak_seller_2, nullptr);
    pthread_t threads[3];
    bool true_bool = true, false_bool = false;

    pthread_create(&threads[0], nullptr, Sellers, (void *) (true_bool));
    pthread_create(&threads[1], nullptr, Sellers, (void *) (false_bool));
    pthread_create(&threads[2], nullptr, AddConsumer, (nullptr));
    pthread_join(threads[0], nullptr);
    pthread_join(threads[1], nullptr);
    pthread_join(threads[2], nullptr);

    pthread_mutex_destroy(&m_seller_1_sleep);
    pthread_mutex_destroy(&m_seller_2_sleep);
    pthread_mutex_destroy(&m_changing_queues);
    pthread_mutex_destroy(&m_cout);
    pthread_cond_destroy(&weak_seller_1);
    pthread_cond_destroy(&weak_seller_2);

    std::cout << "Shop is closed";
    return 0;
}

#ifndef ASCII_PROGRESS_BAR_H
#define ASCII_PROGRESS_BAR_H

typedef uint64_t u64;

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>

const int DEFAULT_COLS = 80;

class ascii_progress_bar {
private:
    const unsigned num_cols;

    u64 max_value;
    u64 current_value;
    u64 step;
    u64 next_draw;

    void update_step() {
        step = max_value / num_cols;
    }

    void draw() {
        const u64 percentage = 100 * current_value / max_value;
        const u64 middle = num_cols * current_value / max_value;

        std::cout << "\r";
        std::cout << std::setw(3) << percentage << "% ";
        std::cout << "[";
        for (u64 i = 0; i < middle; ++i) {
            std::cout << "=";
        }
        for (u64 i = middle; i < num_cols; ++i) {
            std::cout << " ";
        }
        std::cout << "]";
        std::cout << std::flush;
    }

public:
    ascii_progress_bar() :
            num_cols(DEFAULT_COLS),
            max_value(100),
            current_value(0),
            step(max_value / num_cols),
            next_draw(0) {
        draw();
    }

    explicit ascii_progress_bar(const u64 max_value, const unsigned cols = DEFAULT_COLS) :
            num_cols(cols),
            max_value(max_value),
            current_value(0),
            step(max_value / num_cols),
            next_draw(0) {
        draw();
    }

    void reset(const u64 max_value = 100) {
        this->max_value = max_value;
        current_value = 0;
        next_draw = 0;
        update_step();
        draw();
    }

    void set_max_value(const u64 max_value) {
        this->max_value = max_value;
        update_step();
    }

    void set_current_value(const u64 current_value) {
        this->current_value = std::min<u64>(current_value, max_value);
    }

    void progress(const u64 increment = 1) {
        current_value = std::min<u64>(current_value + increment, max_value);
        next_draw++;
        if (next_draw == step) {
            next_draw = 0;
            draw();
        }
    }

    void stop() {
        draw();
        std::cout << "\n";
    }

    void finish() {
        current_value = max_value;
        draw();
        std::cout << "\n";
    }
};

#endif

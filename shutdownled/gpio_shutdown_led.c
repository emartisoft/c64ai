#include <gpiod.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define CHIP_NAME   "gpiochip0"
#define LED_LINE    15   // active-LOW LED
#define BTN_LINE    14   // button, pull-up, active-LOW

int main(void)
{
    struct gpiod_chip *chip;
    struct gpiod_line *led;
    struct gpiod_line *btn;

    chip = gpiod_chip_open_by_name(CHIP_NAME);
    if (!chip) return 1;

    led = gpiod_chip_get_line(chip, LED_LINE);
    btn = gpiod_chip_get_line(chip, BTN_LINE);

    if (!led || !btn) return 1;

    // LED: output, default OFF (HIGH)
    gpiod_line_request_output(led, "shutdown_led", 1);

    // Button: input with pull-up
    gpiod_line_request_input_flags(
        btn,
        "shutdown_button",
        GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP
    );

    // Sistem çalışıyor → LED ON
    gpiod_line_set_value(led, 0);

    while (1) {
        if (gpiod_line_get_value(btn) == 0) {
            // Debounce
            usleep(200000);

            if (gpiod_line_get_value(btn) == 0) {
                // Kapanıyor → LED OFF
                gpiod_line_set_value(led, 1);

                system("shutdown -h now");
                break;
            }
        }
        usleep(50000);
    }

    gpiod_chip_close(chip);
    return 0;
}

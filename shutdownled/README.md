# requirements
sudo apt update  
sudo apt install -y libgpiod-dev gpiod

# build 
gcc gpio_shutdown_led.c -o gpio_shutdown_led -lgpiod
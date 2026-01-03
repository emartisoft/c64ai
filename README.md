![Peripheral](./doc/Peripheral-1.png "Peripheral")![Welcome Screen](./doc/03.png "Welcome Screen")

# C64AI
**C64AI** brings the spirit of modern artificial intelligence to the legendary **Commodore 64** — without sacrificing the authentic retro experience. While chatting with an AI on real 8-bit hardware, you can simultaneously enjoy classic **SID music**, just like the golden days of home computing.

Chat interaction and music playback run side by side, smoothly and continuously, proving that the C64 still has plenty of magic left under the hood.

With built-in support for the **Commodore 1351 mouse**, navigating menus and interacting with the interface feels natural and responsive. Prefer the keyboard, just like back in the day? No problem — the application offers full keyboard control, with all essential functions accessible through carefully designed key bindings.

Whether you are running:

* Original Commodore 64 hardware
* Your favorite emulator
* A setup with or without a mouse

**C64AI** adapts seamlessly and delivers a true retro computing experience — enhanced with a modern twist. Power up your C64, turn up the SID, and experience what 8-bit computing can still achieve.

The project supports **three different usage scenarios**, covering both emulation and real hardware setups.

In the first scenario, **C64AI** runs on **Windows and Linux** systems via the **VICE emulator**, offering an easy and accessible way to get started.

The second scenario uses a real **Commodore 64 or Commodore 128** connected through custom hardware integrated with a **Raspberry Pi Zero 2**, combining classic 8-bit systems with modern embedded technology.

In the third scenario, a real **Commodore 64 or Commodore 128** operates alongside **any 64-bit computer** with an installed operating system and **USB serial port support**, enabling seamless communication between vintage and modern platforms.

## Hardware Setup
The **C64AI** project enables serial communication between **Commodore 64 and Commodore 128** systems and an external computer via the **User Port interface**, using an **FT232RL-based USB–TTL converter**. This architecture supports **bidirectional data transfer**, allowing real-time interaction between classic 8-bit hardware and modern systems.

The **Raspberry Pi Zero 2** is used as the reference platform due to its **compact form factor**, **low power consumption**, and **Linux-based operating system support**. The required hardware connections are clearly illustrated in the schematic provided below.

Alternatively, the same communication infrastructure can be established by connecting the **FT232RL module** via a standard USB interface to any computer running **Windows, Linux, or macOS**, making the system flexible across a wide range of modern platforms.

![Hardware Setup](./doc/c64ai.png "Hardware Setup")

The figure illustrates the **hardware connection architecture** of the **C64AI** system in detail. The **Commodore 64 User Port interface** is directly connected to an **FT232RL-based USB–TTL converter module**. Within this setup, the **A pin** of the User Port is used as the **ground (GND)** reference, while the **B and C pins** are configured as **transmit (TX) data lines** connected to the FT232RL module. The **receive (RX) data line** is routed from the corresponding User Port pin to the **RX input** of the FT232RL module. This configuration enables reliable serial communication between the Commodore 64 and the external system.

The **FT232RL module** is connected to a **Raspberry Pi Zero 2** via a **USB OTG cable**, providing the serial data link. Power for the Raspberry Pi Zero 2 is supplied independently through its secondary USB port, separate from the FT232RL connection. This separation of **power and data paths** improves overall system stability. The Raspberry Pi Zero 2 operates from a **microSD card containing the C64AI software** and serves as the primary control unit of the system.

The **GPIO pins** on the **Raspberry Pi Zero 2** are configured for **optional user interaction and system control**. One designated GPIO pin is connected to a **push-button input**. When triggered, the button initiates a **safe operating system–level shutdown**, ensuring that the system powers down cleanly.

This mechanism is specifically intended to **prevent microSD card data corruption and file system errors**, allowing the system to shut down safely without risking data integrity.

In addition, another GPIO pin is connected to an **LED through a series resistor**, providing **visual feedback on the system’s operational status**. A **67 kΩ current-limiting resistor** is used in the LED circuit to ensure safe operation.

Together, this hardware and control architecture delivers a **reliable, bidirectional serial communication infrastructure** between the **Commodore 64** and the **Raspberry Pi Zero 2**, while also incorporating practical safeguards to maintain **system stability and data integrity**.

For use on any 64-bit computer running Windows, Linux, or macOS via a USB connection, refer to the figure below.

![Hardware Setup](./doc/c64ai-x64-con.png "Hardware Setup")

The FT232RL module is widely supported and is typically recognized without issues by modern operating systems, requiring little to no manual configuration.

|  Operating System |  Driver Support |  Detected Port |
|---|---|---|
|  Windows 10/11 |  Automatic |  COMx |
|  Linux |  Built-in kernel driver |  /dev/ttyUSBx |
|  macOS |  Automatic |  /dev/tty.usbserial-* |

Thanks to this broad native support, the FT232RL provides a simple and reliable bridge between vintage Commodore hardware and modern desktop systems, keeping setup time minimal and the retro experience uninterrupted.

### Running C64AI Using a Virtual Serial Port
If working with real hardware is not desired, **C64AI** can be executed using **virtual serial ports** on **Windows or Linux**, in combination with the **VICE emulator**.

##### Required files (server directory)
The following three files are required:

* `.env`
* `c64aiserver` (GCC is required to build)
* `ask2ai` (Deno is required to build: [https://deno.com/](https://deno.com/))

To power up the AI core correctly, you **must configure the `.env` file** with your Gemini settings.

This file defines which Gemini model is used, how responses are generated, and the language of interaction. Without it, the system won’t know _who it is_ or _how to talk_.

Make sure your `.env` file includes the following entries:
```toml
GEMINI_API_KEY=your_api_key_here
GEMINI_MODEL=gemini-2.5-flash
GEMINI_INITIAL_INSTRUCTION=Provide text-based responses. Keep the conversation context in mind. Responses should be neither too short nor too long.
LANGUAGE=ENGLISH
```

* **GEMINI\_API\_KEY** 
Your personal key to access the Gemini API
* **GEMINI\_MODEL** 
The selected Gemini model (fast, lightweight, and perfect for retro systems)
* **GEMINI\_INITIAL\_INSTRUCTION** 
Defines the AI’s response style and context awareness
* **LANGUAGE** 
Sets the conversation language

To obtain your **API key** and select a **Gemini model**, visit the official Gemini dashboard at [https://aistudio.google.com/apps](https://aistudio.google.com/apps). This site lets you register, manage access credentials, and choose the model that best suits your needs before adding them to your `.env` configuration.

#### Windows
A guide for creating virtual serial port drivers on Windows is available at:
[https://vovsoft.com/blog/how-to-sniff-serial-port-communication](https://vovsoft.com/blog/how-to-sniff-serial-port-communication)

After creating the virtual ports, open **VICE Settings → Peripheral Devices → RS232** and enable:

* **Enable userport RS232 emulation**
* Set **Serial1** and **Serial2** to **2400 BAUD**
* Enter the COM port names created in **Windows Device Manager** for Serial1 and Serial2

![VICE Settings](./doc/vicesettings.png "VICE Settings")

_VICE Settings_

##### Running the server
```bash
.\c64aiserver.exe
Usage: c64aiserver.exe <port_name>
Windows: c64aiserver.exe COM3
Linux  : c64aiserver.exe /dev/ttyUSB0
```
If the virtual serial port is **COM6**, run:
```bash
.\c64aiserver.exe com6
com6 port opened (2400 baud).
C64AI Server started. Waiting for commands from C64…
```
#### Linux
On Linux, virtual serial ports can be created easily using **socat** and used with **VICE**.

Create paired virtual serial ports:
```bash
socat -d -d pty,link=/tmp/ttyS10,raw,echo=0 pty,link=/tmp/ttyS11,raw,echo=0
```
Example output:
```bash
2025/12/07 16:56:59 socat[12620] N PTY is /dev/pts/5
2025/12/07 16:56:59 socat[12620] N PTY is /dev/pts/7
2025/12/07 16:56:59 socat[12620] N starting data transfer loop with FDs [5,5] and [7,7]
```
Run the C64AI server:
```bash
sudo ./c64aiserver /dev/pts/7
/dev/pts/7 port opened (2400 baud).
C64AI Server started. Waiting for commands from C64...
```
##### VICE Settings (Linux)

In **VICE → Userport RS232 Settings**:

* **Enable userport RS232 emulation**
* **Device**: Serial1
* **Baud rate**: 2400
* **Serial1 device**: `/dev/pts/5` (or `/dev/ttyS11`)

This setup allows **C64AI** to run entirely in an emulated environment, delivering a smooth **retro experience** without requiring physical Commodore hardware—perfect for development, testing, and experimentation.

## Quick Start
Once all **hardware connections** have been completed, the system should be connected to the **C64/C128 User Port**. When using a **Raspberry Pi Zero 2**, the prepared **system image** must first be written to a **microSD card** and inserted into the device. (See the `SD Card Installation Guide.md` document)

The **Raspberry Pi Zero 2** should be powered on using an **external power source before starting the Commodore 64**. After the Raspberry Pi is fully operational, the **Commodore 64** can be powered on.

If the system is being started for the **first time**, the **C64AISETUP** application must be loaded and executed on the C64 side to perform the initial configuration.
```bash
LOAD "C64AISETUP",8,1
```
![C64AISETUP](./doc/00.png "C64AISETUP")

_C64AISETUP_

Once the application is launched, on-screen guidance and explanatory prompts will be displayed.

If the operating system running on the **Raspberry Pi Zero 2** is unable to connect to a predefined **Wi-Fi SSID**, it automatically starts a **built-in web server**. In this case, a client device should connect to the wireless network named **`C64AI-Setup`** using the password **`12345678`**.

After connecting to the network, the configuration interface can be accessed either by scanning the **QR code** displayed on the screen or by entering the following address into a web browser:
```bash
http://10.42.0.1
```
![Configuration interface](./doc/webserver.png)

_Configuration interface_

This setup process allows the system to be configured quickly and intuitively, keeping the classic C64 experience intact while leveraging modern connectivity.

This process is designed to complete the system’s **network configuration** in a **secure and controlled manner**. During the same stage, the **API key required for accessing the Gemini AI service**, along with the **configuration parameters of the selected model**, are defined.

To enable the **Raspberry Pi Zero 2** to access the internet over the local network and activate the **AI-driven chat functionality**, the corresponding **Wi-Fi network credentials** must be entered **accurately and completely**.

When the **Save & Connect** button is pressed, the **Raspberry Pi Zero 2** system is automatically **rebooted**. Once this process is complete, the **Commodore 64** should be powered off and then turned on again.

Next, the **C64CHAT** application must be loaded using the command below. Due to potential compatibility issues with certain **cartridge hardware**, it is recommended that **no cartridge be inserted** while running the application. Although the loading process may appear to start, the application may fail to operate correctly if a cartridge is present.
```bash
LOAD "C64CHAT",8,1
```
![C64CHAT loading command](./doc/01.png "C64CHAT loading command")

_C64CHAT loading command_

When using a **floppy disk** or an **SD2IEC device**, the file **`c64-UP2400.ser`** (https://github.com/nanoflite/c64-up2400-cc65) (See the `where is c64-up2400.ser.md` document) must be present in the corresponding directory. This file is a **mandatory component** required to establish serial communication with the correct parameters. In addition, the file **`HELP.PRG`** must also be available.

![Welcome Screen](./doc/03.png "Welcome Screen")

_Welcome Screen_

![Chat Textbox](doc/04.png "Chat Textbox")

_Entering a question in the chat textbox_

After entering the text, press **RETURN** or click the **UP ARROW** button with the mouse. **CURSOR LEFT** and **CURSOR RIGHT** can be used to navigate between words inside the textbox.

![Thinking](./doc/05.png "Thinking")

_The question is being sent to the AI_

![Receiving data](./doc/06.png "Receiving data")

_Receiving the AI response_

![AI Response](./doc/07.png "AI Response")

_First page of the AI response_

Page navigation is performed using **CURSOR UP** or **CURSOR DOWN**. Page navigation is not limited to keys only — pages can also be switched by clicking the **<** and **>** symbols next to the page numbers using a 1351 mouse.

![SID file list](doc\10.png "SID file list")

_Accessing the SID file list by clicking the music note icon_

The **F2** key or entering the command **`/sidfiles`** in the textbox provides the same functionality.

![Available devices](./doc/11.png "Available devices")

_Switching between available disk devices_

By clicking the **Device** field or pressing **F4**, available disk drives are cycled and SID files are listed if present. Page navigation is done using **CURSOR UP** or **CURSOR DOWN**.

![Loading](./doc/12.png "Loading")

_Loading and playing a SID file_

To load and play a file, simply click on the desired SID filename. Alternatively, the command **`/load sidfilename.sid`** can be used.

![SID Tune Info](./doc/13.png "SID Tune Info")

_Viewing SID file information_

Press **F3** to display SID details. If the SID contains multiple tracks, **F5** is used to switch between songs.

![Help](./doc/15.png "Help")

_Help_

The **`/help`** command displays the help screen and can also be accessed by pressing the **F1** key or clicking the **?** icon. The content of the **`HELP.PRG`** file located on the same drive as the application is displayed.

To start a **new conversation** and clear the chat history, use the **`/new`** command or click the icon located in the **top-right corner** of the screen.

If you want to receive **AI responses in a specific language**, use the **`/lang`** command. Regardless of the language used for text input, the AI response will be generated in the language specified by this command.

Examples:
```bash
/lang=english
/lang=german
/lang=turkce
/lang=<write your own language>
```
To change the **page color**, press the **F7** key or click the icon located below the **New Chat** symbol.

SID playback can be controlled using the **PLAY** and **PAUSE** icons, or by entering the **`/play`** or **`/pause`** commands.

To **re-display the most recent AI response**, press **F6**. In cases where a response cannot be received due to connection interruptions or temporary AI unavailability, press **F8** to resend the last prompt instead of retyping it.

## SID Relocation
To ensure **seamless SID music playback** within the application, certain **technical constraints** must be satisfied. Specifically, the SID file’s **load address** is expected to be `$1000`, and the **data offset** must fall within the `$1000–$3000` range. This corresponds to an approximate file size of **8 KB**, or **32 disk blocks**.

SID files that do not meet these requirements should be made compatible using the **`sidreloc`** tool to perform **memory relocation**. Failure to comply with these constraints may prevent the application from functioning correctly.

#### Example on Windows
As an example, consider a SID file selected from the **HVSC archive**:

`MUSICIANS/C/Cadaver/Metal\_Warrior\_2.sid`

When inspecting the SID tune properties using **Sidplay2/w (Win32 version 2.6)**, the **data size** is reported as **7812 bytes**. For **C64AI**, the maximum supported size is **8192 bytes**, meaning this file **meets the size requirement**.

However, the **load range** of the SID file is `$8100–$9F83`. If the load address had started at `$1000`, the file could have been used **without relocation**. In its current form, the SID file must be **relocated** before it can be used with C64AI.

![SID Tune Properties](./doc/sidinfo.png "SID Tune Properties")

_SID Tune Properties_

Now, to **relocate this SID file to address `$1000`**, use the **`sidreloc`** tool from the terminal. The **first argument** must be the **original SID file**, and the **second argument** must be the **name of the relocated SID file**.

The relocated SID filename **must not exceed 16 characters** and **must use the `.SID` file extension**.
```bash
sidreloc.exe .\Metal_Warrior_2.sid .\MW2.sid
```
After this step, the SID file can be **played seamlessly within C64AI** without any compatibility issues.

![Relocation](./doc/reloc.png "Relocation")

_Relocated SID file_


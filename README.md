
🕵️‍♂️ Steganography — LSB Image Encoding (C Project)

A disciplined step into the world of secure messaging — this Steganography project hides secret text inside BMP images using the Least Significant Bit (LSB) technique. The encoded image looks unchanged to the human eye, yet carries covert information beneath its pixels.

🎯 Project Vision

“Security that whispers, not shouts。”

This system enables silent, undetectable communication by embedding text behind image pixels — enhancing confidentiality without raising suspicion.

✨ Key Features

Hide secret text data inside 24-bit BMP images

Extract the hidden message with precise bit-manipulation

No visible distortion to the image

Modular and scalable C code architecture

Safe file operations and input validations

Clear console-based UI

🧠 Technology Stack
Component	Description
Language	C Programming
Algorithm	Least Significant Bit (LSB)
File Type	BMP (24-bit bitmap)
Compilation	GCC or any standard C compiler


🔐 LSB Encoding Concept

Each pixel contains 3 bytes → R, G, B

Only the lowest bit (LSB) of each channel is modified

That tiny modification stores the message without visible change

Example:

Pixel:   11001001 → 11001000  (LSB changed to store a '0')

🏁 How to Run

1️⃣ Compile

gcc stego.c -o stego


2️⃣ Encode

./stego -e input.bmp secret.txt output.bmp

<img width="664" height="389" alt="Encode" src="https://github.com/user-attachments/assets/77223e57-3cba-4acd-a6ee-7d0cf70c3db7" />


3️⃣ Decode
./stego -d output.bmp decoded.txt

<img width="634" height="208" alt="decode" src="https://github.com/user-attachments/assets/5e5d086f-1153-4ddd-be94-613a84eecc84" />

📌 Folder Structure
📦 Steganography-Project
 ┣ 📂 src
 ┃ ┣ stego.c
 ┃ ┗ encoder_decoder.h
 ┣ 📂 assets
 ┃ ┗ sample.bmp
 ┣ secret.txt
 ┣ README.md
 ┗ Makefile

🚀 Future Enhancements

🔐 Password protection for decode mode

🔑 AES encryption + Steganography combo

🖼 PNG and JPEG support

💻 GUI version

📱 Android app deployment

🧩 Learning Outcomes

Mastery of bit manipulation

Understanding of BMP file structure

Real-world security & data-hiding concept

Modular C programming experience

👤 Author

Parth Pawar
Embedded Systems Enthusiast • Future Innovator ✨
